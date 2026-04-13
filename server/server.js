/**
 * server.js  —  MiniOS Node.js Bridge
 *
 * Spawns the compiled C binary (../miniOS) as a persistent child process.
 * Commands are serialized through a queue so stdin/stdout are never
 * interleaved.  Every response from the C program ends with __END__\n
 * which is used as the framing delimiter.
 *
 * API:
 *   POST /api/command        { command: "ls" }  → { output: "..." }
 *   GET  /api/status         → { files, procs, memFree, memTotal, memUsed }
 *   GET  /api/files          → { files: [{ name, size }] }
 *   GET  /api/processes      → { processes: [{ pid, name, state, burst, remaining, priority }] }
 *   GET  /api/memory         → { used, free, total }
 */

const express    = require('express');
const cors       = require('cors');
const { spawn }  = require('child_process');
const path       = require('path');

const app    = express();
const PORT   = 3001;
const BINARY = path.join(__dirname, '..', 'miniOS');
const END    = '__END__\n';

app.use(cors());
app.use(express.json());

/* ── Child process lifecycle ──────────────────────── */
let child        = null;
let outputBuf    = '';
let ready        = false;
let readyCb      = null;
let pendingCb    = null;
let cmdTimeout   = null;
let cmdQueue     = [];
let processing   = false;

function startChild() {
    child     = spawn(BINARY, [], { stdio: ['pipe', 'pipe', 'pipe'] });
    outputBuf = '';
    ready     = false;

    child.stdout.on('data', chunk => {
        outputBuf += chunk.toString();
        drainBuffer();
    });

    child.stderr.on('data', d => process.stderr.write('[miniOS] ' + d));

    child.on('close', code => {
        console.log(`miniOS exited (code ${code}), restarting…`);
        ready = false;
        setTimeout(startChild, 500);
    });
}

function drainBuffer() {
    while (outputBuf.includes(END)) {
        const idx   = outputBuf.indexOf(END);
        const chunk = outputBuf.slice(0, idx);
        outputBuf   = outputBuf.slice(idx + END.length);

        if (!ready) {
            ready = true;
            if (readyCb) { readyCb(); readyCb = null; }
        } else if (pendingCb) {
            clearTimeout(cmdTimeout);
            const cb = pendingCb;
            pendingCb   = null;
            processing  = false;
            cb(null, chunk.trim());
            runNext();
        }
    }
}

function runNext() {
    if (processing || cmdQueue.length === 0) return;
    const { cmd, resolve, reject } = cmdQueue.shift();
    processing = true;

    pendingCb  = (err, result) => err ? reject(err) : resolve(result);
    cmdTimeout = setTimeout(() => {
        pendingCb  = null;
        processing = false;
        reject(new Error('Command timed out'));
        runNext();
    }, 6000);

    child.stdin.write(cmd + '\n');
}

function exec(cmd) {
    return new Promise((resolve, reject) => {
        cmdQueue.push({ cmd, resolve, reject });
        runNext();
    });
}

function waitReady() {
    return new Promise(resolve => {
        if (ready) return resolve();
        readyCb = resolve;
    });
}

startChild();

/* ── Parsers for structured data ──────────────────── */

/**
 * Parse "ls" output: "name (N B)\n" lines → [{ name, size }]
 */
function parseFiles(raw) {
    if (!raw || raw === '(no files)') return [];
    return raw.split('\n').filter(Boolean).map(line => {
        const m = line.match(/^(.+?)\s+\((\d+)\s*B\)$/);
        return m ? { name: m[1].trim(), size: parseInt(m[2], 10) } : null;
    }).filter(Boolean);
}

/**
 * Parse "ps" output:
 *   "PID:1 NAME:chrome STATE:READY BURST:10 REMAIN:8 PRI:2\n"
 */
function parseProcesses(raw) {
    if (!raw || raw.includes('(no active')) return [];
    return raw.split('\n').filter(Boolean).map(line => {
        const get = key => {
            const m = line.match(new RegExp(key + ':(\\S+)'));
            return m ? m[1] : '';
        };
        return {
            pid:       parseInt(get('PID'),    10) || 0,
            name:      get('NAME'),
            state:     get('STATE'),
            burst:     parseInt(get('BURST'),  10) || 0,
            remaining: parseInt(get('REMAIN'), 10) || 0,
            priority:  parseInt(get('PRI'),    10) || 0
        };
    }).filter(p => p.pid > 0);
}

/**
 * Parse "status" output → { files, procs, memTotal, memUsed, memFree }
 */
function parseStatus(raw) {
    const num = (key) => {
        const m = raw.match(new RegExp(key + '\\s*:\\s*(\\d+)'));
        return m ? parseInt(m[1], 10) : 0;
    };
    return {
        files:    num('Files'),
        procs:    num('Processes'),
        memTotal: num('VRAM Total'),
        memUsed:  num('VRAM Used'),
        memFree:  num('VRAM Free')
    };
}

/* ── Routes ───────────────────────────────────────── */

app.post('/api/command', async (req, res) => {
    const { command } = req.body;
    if (!command) return res.status(400).json({ error: 'No command provided' });
    try {
        await waitReady();
        const output = await exec(command.trim());
        res.json({ output });
    } catch (e) {
        res.status(500).json({ error: e.message });
    }
});

app.get('/api/files', async (req, res) => {
    try {
        await waitReady();
        const raw = await exec('ls');
        res.json({ files: parseFiles(raw) });
    } catch (e) { res.status(500).json({ error: e.message }); }
});

app.get('/api/processes', async (req, res) => {
    try {
        await waitReady();
        const raw = await exec('ps');
        res.json({ processes: parseProcesses(raw) });
    } catch (e) { res.status(500).json({ error: e.message }); }
});

app.get('/api/memory', async (req, res) => {
    try {
        await waitReady();
        const raw    = await exec('status');
        const parsed = parseStatus(raw);
        res.json({ used: parsed.memUsed, free: parsed.memFree, total: parsed.memTotal });
    } catch (e) { res.status(500).json({ error: e.message }); }
});

app.get('/api/status', async (req, res) => {
    try {
        await waitReady();
        const raw = await exec('status');
        res.json(parseStatus(raw));
    } catch (e) { res.status(500).json({ error: e.message }); }
});

/* Aggregate endpoint polled by the side-panels (serial, not parallel) */
app.get('/api/all', async (req, res) => {
    try {
        await waitReady();
        const statusRaw = await exec('status');
        const filesRaw  = await exec('ls');
        const procsRaw  = await exec('ps');
        const status    = parseStatus(statusRaw);
        res.json({
            status,
            files:     parseFiles(filesRaw),
            processes: parseProcesses(procsRaw)
        });
    } catch (e) { res.status(500).json({ error: e.message }); }
});

app.get('/api/health', (_req, res) => res.json({ ready }));

app.listen(PORT, () =>
    console.log(`MiniOS bridge running → http://localhost:${PORT}`)
);
