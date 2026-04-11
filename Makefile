CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = miniOS

SRCS = src/shell.c src/math.c src/string.c src/memory.c src/screen.c src/keyboard.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run