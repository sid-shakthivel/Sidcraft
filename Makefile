CC = g++
CFLAGS = -std=c++17 -fdiagnostics-color=always -Wall
INCLUDE = /Users/siddharth/Code/C++/Sidcraft/dependencies/include

PROJECT_PATH = /Users/siddharth/Code/C++/Sidcraft
DEPENDENCIES_PATH = $(PROJECT_PATH)/dependencies

LIBS = $(DEPENDENCIES_PATH)/lib $(DEPENDENCIES_PATH)/lib/libglfw.3.3.dylib

TARGET = Sidcraft

OBJ := ObjectFiles

C++_SOURCES := $(wildcard $(PROJECT_PATH)/src/*.cpp)
C++_OBJECTS := $(patsubst %.cpp, %.o, $(C++_SOURCES))

all: $(TARGET)

$(TARGET): $(C++_OBJECTS) $(GLAD)
	$(CC) -L $(LIBS) -o $@ $(OBJ)/* $(PROJECT_PATH)/glad.o -framework  -Wdeprecated-declarations -Wno-deprecated && ./$(TARGET)

GLAD: $(PROJECT_PATH)/src/glad.c
	gcc -c $(patsubst %.o, %.c, $<) -o 

$(C++_OBJECTS): $(C++_SOURCES)
	$(CC) -g -I $(INCLUDE) $(CFLAGS) -c $(patsubst %.o, %.cpp, $@) -o $@ &&\
	mv $@ $(OBJ)

clean:
	rm -rf $(PROJECT_PATH)/$(OBJ)/*.o
