# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./utils

# Sources
SRC = demo.cpp \
      utils/read_env.cpp \
      utils/read_file.cpp \
      utils/request.class.cpp \
      utils/server.class.cpp \
      utils/init_get.cpp\
      utils/extra.cpp \

# Output
TARGET = demo.exe

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) -lws2_32

# Clean up (Windows)
clean:
	del /Q $(TARGET)
