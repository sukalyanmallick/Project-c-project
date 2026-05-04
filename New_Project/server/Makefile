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
             src/controller/network_server.cpp \
             src/model/ai_engine.cpp     \
             src/model/ai_keyword.cpp    \
             src/model/ai_time.cpp       \
             src/model/ai_random.cpp     \
             src/view/ServerLogger.cpp

OBJS     := $(SRCS:src/%.cpp=build/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	@echo "[Server] Build complete → $(TARGET)"

build/%.o: src/%.cpp
	if not exist $(subst /,\,$(dir $@)) mkdir $(subst /,\,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	if not exist build mkdir build
	if not exist build\model mkdir build\model
	if not exist build\view mkdir build\view
	if not exist build\controller mkdir build\controller

clean:
	if exist build rmdir /s /q build
	if exist $(TARGET) del /q $(TARGET)
	@echo "[Server] Clean complete."
