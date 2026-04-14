CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = miniOS

SRCS = src/main.c     \
       src/shell.c    \
       src/parser.c   \
       src/math.c     \
       src/string.c   \
       src/screen.c   \
       src/keyboard.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

run: all
	./$(TARGET)