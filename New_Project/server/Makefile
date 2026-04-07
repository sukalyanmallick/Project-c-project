# server/Makefile — Build the AI Chatbot Server
#
# Usage:
#   make          — build chatbot_server.exe
#   make clean    — remove object files and the executable

CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -O2 \
           -I./include -I../shared
LDFLAGS := -lws2_32

TARGET  := chatbot_server.exe
SRCS    := src/main.c \
           src/network_server.c \
           src/ai_engine.c \
           src/ai_keyword.c \
           src/ai_time.c \
           src/ai_random.c

OBJS    := $(SRCS:src/%.c=build/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "[Server] Build complete → $(TARGET)"

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET)
	@echo "[Server] Clean complete."
