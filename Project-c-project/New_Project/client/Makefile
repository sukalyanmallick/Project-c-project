# client/Makefile — Build the GTK4 Chat Client (C++17 / SOLID)
#
# Usage:
#   make          — build chatbot_client.exe
#   make clean    — remove object files and the executable
#
# Requires:
#   GTK4 development headers and pkg-config installed.
#   On MSYS2/MinGW: pacman -S mingw-w64-x86_64-gtk4

CXX      := g++
CXXFLAGS := -Wall -Wextra -Wpedantic -std=c++17 -O2 \
            -I./include -I../shared
GTK_CFLAGS := $(shell pkg-config --cflags gtk4 2>nul)
GTK_LDFLAGS := -lws2_32 $(shell pkg-config --libs gtk4 2>nul)

TARGET   := chatbot_client.exe
CONSOLE_TARGET := chatbot_console.exe
SRCS     := src/main.cpp          \
             src/network_client.cpp \
             src/ui_display.cpp    \
             src/ui_callbacks.cpp  \
             src/ui_layout.cpp

CONSOLE_SRCS := src/console_main.cpp \
                 src/network_client.cpp \
                 src/model/ChatModel.cpp \
                 src/view/ConsoleView.cpp \
                 src/controller/ChatController.cpp

OBJS     := $(SRCS:src/%.cpp=build/%.o)
CONSOLE_OBJS := $(CONSOLE_SRCS:src/%.cpp=build/%.o)

.PHONY: all clean console

all: $(CONSOLE_TARGET)

console: $(CONSOLE_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(GTK_LDFLAGS)
	@echo "[Client] GUI Build complete → $(TARGET)"

$(CONSOLE_TARGET): $(CONSOLE_OBJS)
	$(CXX) $(CONSOLE_OBJS) -o $@ -lws2_32
	@echo "[Client] Console Build complete → $(CONSOLE_TARGET)"

build/%.o: src/%.cpp
	if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
	$(CXX) $(CXXFLAGS) $(GTK_CFLAGS) -c $< -o $@

clean:
	if exist build rmdir /s /q build
	if exist $(TARGET) del /q $(TARGET)
	if exist $(CONSOLE_TARGET) del /q $(CONSOLE_TARGET)
	@echo "[Client] Clean complete."
