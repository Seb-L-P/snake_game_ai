# --- Variables ---
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# --- Targets ---
# 'make all' will build both versions
all: ai human

# Build the AI version
ai: src/Snake.cpp src/SnakeGame.cpp src/main_ai.cpp
	$(CXX) $(CXXFLAGS) $^ -o snake_ai

# Build the human (keyboard) version
human: src/Snake.cpp src/SnakeGame.cpp src/main.cpp
	$(CXX) $(CXXFLAGS) $^ -o snake_human

# Remove compiled executables
clean:
	rm -f snake_ai snake_human
