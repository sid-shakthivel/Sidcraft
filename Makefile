CC = clang++
CFLAGS = -std=c++17 -fdiagnostics-color=always -Wall
INCLUDE = /Users/siddharth/Code/C++/GameEngine/dependencies/include

PROJECT_PATH = /Users/siddharth/Code/C++/GameEngine
DEPENDENCIES_PATH = $(PROJECT_PATH)/dependencies

LIBS = $(DEPENDENCIES_PATH)/lib $(DEPENDENCIES_PATH)/lib/libglfw.3.3.dylib  $(DEPENDENCIES_PATH)/lib/libassimp.5.2.4.dylib

TARGET = GameEngine

FILES = $(PROJECT_PATH)/src/*.cpp $(PROJECT_PATH)/src/glad.c

all: $(FILES)
	$(CC) $(CFLAGS) -g -I $(INCLUDE) -L $(LIBS) $(FILES) -o $(TARGET) -framework OpenGL -Wno-deprecated
	./$(TARGET)
