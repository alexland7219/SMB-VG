#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include "Block.h"
#include <vector>

// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.


class TileMap
{

private:
	TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, bool isbg);

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, bool isbg);

	~TileMap();

	void render() const;
	void update(int deltatime);
	void free();
	
	int getTileSize() const { return tileSize; }

	bool collisionMoveLeft(const glm::vec2 &pos, const glm::ivec2 &size, bool koopaBreak);
	bool collisionMoveRight(const glm::vec2 &pos, const glm::ivec2 &size, bool koopaBreak, bool* flagpole, int* flagX);
	bool collisionMoveDown(const glm::vec2 &pos, const glm::ivec2 &size, float *posY, bool* flagpole, int* flagX);
	bool collisionMoveUp(const glm::vec2 &pos, const glm::ivec2 &size, float *posY, bool bigMario);

	void breakBlock(const glm::ivec2 pos);
	int getTotalCoins();
	// New item aux function for Scene.cpp
	void flushNewItemQueue();
	char getNewItemChar();
	glm::vec2 getNewItemPos();

private:
	bool loadLevel(const string &levelFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);
	bool isInsideScreen(int x, int y);

private:

	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	ShaderProgram texProgram;
	int nTiles;
	glm::ivec2 position, mapSize, tilesheetSize;
	int tileSize, blockSize;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	int *map;
	int nCoins;
	Block ** blockMatrix;

	vector<float> blocksBroken;

	// New item (Mushroom / Star) after bump
	char newItem; // Either 'N' for None, 'M' for Mushroom, 'S' for star, 'C' for coin
	glm::vec2 posNewItem; // If newItem != 'N'

	bool isBackground;
};


#endif // _TILE_MAP_INCLUDE


