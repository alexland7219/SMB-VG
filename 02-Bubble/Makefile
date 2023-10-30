# Compiler
CC = g++

# Compiler flags
CFLAGS = -g -std=c++11 -Wall -I/usr/include -I/usr/local/include

# Linker flags
LDFLAGS = -lGL -lGLEW -lglut -lSOIL -lGLU -lsfml-audio -lsfml-system

# Source files and target executable
SOURCES = main.cpp Game.cpp Player.cpp Scene.cpp Shader.cpp ShaderProgram.cpp Sprite.cpp Texture.cpp TileMap.cpp Item.cpp Block.cpp Screen.cpp
HEADERS  = Game.h Player.h Scene.h Shader.h ShaderProgram.h Sprite.h Texture.h TileMap.h Item.h Block.h Screen.h
EXECUTABLE = game

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

