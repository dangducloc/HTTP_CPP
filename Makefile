# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./utils

# Sources
SRC = main.cpp \
      $(wildcard utils/extra/*.cpp) \
      $(wildcard utils/model/*.cpp)

# Output
TARGET = server

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Clean up
clean:
	-rm -f $(TARGET) server.exe
