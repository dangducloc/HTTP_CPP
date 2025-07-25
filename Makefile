# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./utils

# Sources
SRC = main.cpp \
      utils/extra/read_env.cpp \
      utils/extra/read_file.cpp \
      utils/extra/form_data.cpp \
      utils/extra/file_upload.cpp \
      utils/model/request.class.cpp \
      utils/model/logger.class.cpp \
      utils/model/server.class.cpp \
      utils/model/init_get.cpp \
      utils/model/init_post.cpp \
      utils/extra/extra.cpp

# Output
TARGET = server

# Default target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) -lstdc++fs

# Clean up (Windows)
clean:
	del /Q $(TARGET)
