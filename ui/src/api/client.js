import axios from 'axios'

const api = axios.create({ baseURL: '/api', timeout: 8000 })

export const sendCommand = (command) =>
  api.post('/command', { command }).then(r => r.data)

export const fetchAll = () =>
  api.get('/all').then(r => r.data)

export const fetchFiles = () =>
  api.get('/files').then(r => r.data)

export const fetchProcesses = () =>
  api.get('/processes').then(r => r.data)

export const fetchMemory = () =>
  api.get('/memory').then(r => r.data)

export const fetchStatus = () =>
  api.get('/status').then(r => r.data)

export const checkHealth = () =>
  api.get('/health').then(r => r.data)
