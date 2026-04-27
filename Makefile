CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = miniOS
FRONTEND_PORT ?= 8000
FRONTEND_HOST ?= 127.0.0.1
FRONTEND_PID_FILE ?= .frontend.pid
FRONTEND_LOG_FILE ?= .frontend.log

SRCS = src/main.c     \
       src/shell.c    \
       src/parser.c   \
       src/math.c     \
       src/string.c   \
       src/screen.c   \
       src/keyboard.c \
       src/memory.c   \
       src/filesystem.c \
       src/scheduler.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean run frontend frontend-stop

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET) $(FRONTEND_PID_FILE) $(FRONTEND_LOG_FILE)

run: all
	./$(TARGET)

frontend:
	@set -e; \
	if [ -f "$(FRONTEND_PID_FILE)" ] && kill -0 "$$(cat "$(FRONTEND_PID_FILE)")" 2>/dev/null; then \
		echo "Frontend already running at http://$(FRONTEND_HOST):$(FRONTEND_PORT)/index.html"; \
		exit 0; \
	fi; \
	rm -f "$(FRONTEND_PID_FILE)"; \
	python3 -m http.server "$(FRONTEND_PORT)" --bind "$(FRONTEND_HOST)" --directory . >"$(FRONTEND_LOG_FILE)" 2>&1 & \
	echo $$! > "$(FRONTEND_PID_FILE)"; \
	echo "Frontend running at http://$(FRONTEND_HOST):$(FRONTEND_PORT)/index.html"

frontend-stop:
	@set -e; \
	if [ ! -f "$(FRONTEND_PID_FILE)" ]; then \
		echo "No frontend pid file found ($(FRONTEND_PID_FILE))."; \
		exit 0; \
	fi; \
	PID="$$(cat "$(FRONTEND_PID_FILE)")"; \
	if kill -0 "$$PID" 2>/dev/null; then \
		kill "$$PID"; \
		echo "Stopped frontend (pid $$PID)."; \
	else \
		echo "Frontend not running (stale pid $$PID)."; \
	fi; \
	rm -f "$(FRONTEND_PID_FILE)"
