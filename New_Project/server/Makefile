# server/Makefile — Build the AI Chatbot Server (C++17 / SOLID)
#
# Usage:
#   make          — build chatbot_server.exe
#   make clean    — remove object files and the executable

CXX      := g++
CXXFLAGS := -Wall -Wextra -Wpedantic -std=c++17 -O2 \
            -I./include -I../shared
LDFLAGS  := -lws2_32

TARGET   := chatbot_server.exe
SRCS     := src/main.cpp          \
             src/network_server.cpp \
             src/ai_engine.cpp     \
             src/ai_keyword.cpp    \
             src/ai_time.cpp       \
             src/ai_random.cpp

OBJS     := $(SRCS:src/%.cpp=build/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	@echo "[Server] Build complete → $(TARGET)"

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	if not exist build mkdir build

clean:
	if exist build rmdir /s /q build
	if exist $(TARGET) del /q $(TARGET)
	@echo "[Server] Clean complete."
