# client/Makefile — Build the GTK4 Chat Client
#
# Usage:
#   make          — build chatbot_client.exe
#   make clean    — remove object files and the executable
#
# Requires:
#   GTK4 development headers and pkg-config installed.
#   On MSYS2/MinGW: pacman -S mingw-w64-x86_64-gtk4

CC      := gcc
CFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -O2 \
           -I./include -I../shared \
           $(shell pkg-config --cflags gtk4)
LDFLAGS := -lws2_32 $(shell pkg-config --libs gtk4)

TARGET  := chatbot_client.exe
SRCS    := src/main.c \
           src/network_client.c \
           src/ui_display.c \
           src/ui_callbacks.c \
           src/ui_layout.c

OBJS    := $(SRCS:src/%.c=build/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "[Client] Build complete → $(TARGET)"

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET)
	@echo "[Client] Clean complete."
