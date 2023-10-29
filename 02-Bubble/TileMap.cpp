#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"

using namespace std;

enum BlockTypes
{
    T_COIN = 2,
    T_BREAKABLE = 1,
    T_QUESTION_COIN = 5,
    T_QUESTION_MUSH = 6,
    T_QUESTION_STAR = 7,
    T_UNBREAKABLE = 10,
	T_FLAGPOLE = 14,
	T_UPPERPOLE = 13
};
/* We use negative numbers to represent dynamic objects*/

TileMap *TileMap::createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, bool isbg)
{
	TileMap *map = new TileMap(levelFile, minCoords, program, isbg);
	
	return map;
}


TileMap::TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program, bool isbg)
{
	texProgram = program;
	isBackground = isbg;
	nCoins = 0;
	newItem = 'N';
	loadLevel(levelFile);
	prepareArrays(minCoords, program);

}

TileMap::~TileMap()
{
	if(map != NULL)
		delete map;
}


void TileMap::render() const
{
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);	
	glDrawArrays(GL_TRIANGLES, 0, 6 * nTiles);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_STENCIL_TEST);

	if (!isBackground)
		for(int j=0; j<mapSize.y; j++)
			for(int i=0; i<mapSize.x; i++)
				if (map[j*mapSize.x+i] < 0)
					blockMatrix[j*mapSize.x+i]->render();

}

void TileMap::update(int deltaTime){
	if (!isBackground)
		for(int j=0; j<mapSize.y; j++)
			for(int i=0; i<mapSize.x; i++)
				if (map[j*mapSize.x+i] < 0)
					blockMatrix[j*mapSize.x+i]->update(deltaTime);

}

void TileMap::free()
{
	glDeleteBuffers(1, &vbo);
}

bool TileMap::loadLevel(const string &levelFile)
{
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;
	
	fin.open(levelFile.c_str());
	if(!fin.is_open())
		return false;
	getline(fin, line);
	if(line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);
	
	map = new int[mapSize.x * mapSize.y];
	if (!isBackground) blockMatrix = new Block*[mapSize.x * mapSize.y];


	for(int j=0; j<mapSize.y; j++)
	{
		for(int i=0; i<mapSize.x; i++)
		{
			fin.get(tile);

			if (!isBackground){
				switch(tile) {
					case '-':
						// No block
						map[j*mapSize.x+i] = 0;
						blockMatrix[j*mapSize.x+i] = NULL;
						break;
					case '1': // Breakable
					case '2': // Coin
					case '5': // Question block with coin
					case '6': // Question block with mushroom
					case '7': // Question block with star
						// Dynamic block (breakable, question, coin)
						map[j*mapSize.x+i] = int('0') - tile; // Negative to mark dynamic block
						blockMatrix[j*mapSize.x+i] = new Block();
						blockMatrix[j*mapSize.x+i]->init(glm::vec2(i*tileSize, j*tileSize), texProgram, tile - int('0'));
						break;
					default:
						// Static block
						blockMatrix[j*mapSize.x+i] = NULL;
						map[j*mapSize.x+i] = tile - int('0');
				}
			}
			else if(tile == '-') // If its a blank background
				map[j*mapSize.x+i] = 0;
			else				// Background with tile
				map[j*mapSize.x+i] = tile - int('0');
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();
	return true;
}

void TileMap::prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program)
{
	int tile;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;
	
	nTiles = 0;
	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());
	for(int j=0; j<mapSize.y; j++)
	{
		for(int i=0; i<mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];
			if(tile > 0)
			{
				// Non-empty tile
				nTiles++;
				posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
				texCoordTile[0] = glm::vec2(float((tile-1)%tilesheetSize.x) / tilesheetSize.x, float((tile-1)/tilesheetSize.x) / tilesheetSize.y);
				texCoordTile[1] = texCoordTile[0] + tileTexSize;
				//texCoordTile[0] += halfTexel;
				texCoordTile[1] -= halfTexel;
				// First triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				// Second triangle
				vertices.push_back(posTile.x); vertices.push_back(posTile.y);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
				vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
				vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
				vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
			}
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4*sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4*sizeof(float), (void *)(2*sizeof(float)));
}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below.

bool TileMap::collisionMoveLeft(const glm::vec2 &pos, const glm::ivec2 &size, bool koopaBreak)
{
	int x, y0, y1;
	
	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for(int y=y0; y<=y1; y++)
	{
		if (map[y*mapSize.x + x] == -T_COIN && !blockMatrix[y * mapSize.x + x]->isBroken()){
			// Coin
			++nCoins;
			blockMatrix[y * mapSize.x + x]->collectCoin(true);
			continue;
		}
		//else if (map[y*mapSize.x+x] > 0) return true;
		//else if (blockMatrix[y*mapSize.x + x] != NULL) 
		//	if (map[y*mapSize.x+x] < 0 && !blockMatrix[y * mapSize.x + x]->isBroken())
		//		return true;
		else if (map[y*mapSize.x+x] > 0){

			return true;
		}
		else if (blockMatrix[y*mapSize.x + x] != NULL){
			if (map[y*mapSize.x+x] < 0 && !blockMatrix[y * mapSize.x + x]->isBroken()){
				if (map[y * mapSize.x + x] == -T_BREAKABLE && koopaBreak)
					blockMatrix[y * mapSize.x + x]->breakBlock();

				return true;
			}
		}
	}
	
	return false;
}

bool TileMap::collisionMoveRight(const glm::vec2 &pos, const glm::ivec2 &size, bool koopaBreak, bool& flagpole)
{
	int x, y0, y1;
	
	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for(int y=y0; y<=y1; y++)
	{		
		if (map[y*mapSize.x + x] == -T_COIN && !blockMatrix[y * mapSize.x + x]->isBroken()){
			// Coin
			++nCoins;
			blockMatrix[y * mapSize.x + x]->collectCoin(true);

			continue;
		}
		//else if (map[y*mapSize.x+x] > 0) return true;
		//else if (blockMatrix[y*mapSize.x + x] != NULL) 
		//	if (map[y*mapSize.x+x] < 0 && !blockMatrix[y * mapSize.x + x]->isBroken())
		//		return true;
		else if (map[y*mapSize.x+x] > 0){
			if (map[y*mapSize.x + x] == T_FLAGPOLE){
				if (pos.x + size.x >= x*tileSize + 2)
					flagpole = true;
				return false;

			} else if (map[y*mapSize.x+x] == T_UPPERPOLE){
				if (pos.x + size.x >= x*tileSize + 2 && pos.y + size.y >= y + 3)
					flagpole = true;
			}

			return true;
		}
		else if (blockMatrix[y*mapSize.x + x] != NULL){
			if (map[y*mapSize.x+x] < 0 && !blockMatrix[y * mapSize.x + x]->isBroken()){
				if (map[y * mapSize.x + x] == -T_BREAKABLE && koopaBreak)
					blockMatrix[y * mapSize.x + x]->breakBlock();

				return true;
			}
		}
	}
	
	return false;
}

bool TileMap::collisionMoveUp(const glm::vec2 &pos, const glm::ivec2 &size, float *posY, bool bigMario)
{
	int x0, x1, y;
	
	x0 = (pos.x + 3) / tileSize;
	x1 = (pos.x + size.x - 3) / tileSize;
	y = pos.y / tileSize;
	bool addNewItem;

	for(int x=x0; x<=x1; x++)
	{	
		if (map[y*mapSize.x + x] == -T_COIN && !blockMatrix[y * mapSize.x + x]->isBroken()){
			// Coin
			++nCoins;
			blockMatrix[y * mapSize.x + x]->collectCoin(true);

			continue;
		}
		else if (map[y*mapSize.x+x] > 0){

			return true;

		}
		else if (blockMatrix[y*mapSize.x + x] != NULL){
			if (map[y*mapSize.x+x] < 0 && !blockMatrix[y * mapSize.x + x]->isBroken()){
				if (map[y * mapSize.x + x] == -T_BREAKABLE && bigMario) {
					blockMatrix[y * mapSize.x + x]->breakBlock();
				} 
				else if (map[y * mapSize.x + x] == -T_BREAKABLE) blockMatrix[y * mapSize.x + x]->bumpBlock();
				else if (map[y * mapSize.x + x] == -T_QUESTION_COIN){
					// Animate getting coin
					addNewItem = blockMatrix[y * mapSize.x + x]->bumpBlock();
					if (addNewItem){
						blockMatrix[y * mapSize.x + x]->collectCoin(false);
						newItem = 'C';
						posNewItem = glm::vec2(x, y - 1);
						++nCoins;
					}
				}
				else if (map[y * mapSize.x + x] == -T_QUESTION_MUSH){
					addNewItem = blockMatrix[y * mapSize.x + x]->bumpBlock();
					// Create mushroom item
					if (addNewItem){
						newItem = 'M';
						posNewItem = glm::vec2(x, y - 1);
					}
				}

				return true;
			}
		}

	}
	
	return false;

}


bool TileMap::collisionMoveDown(const glm::vec2 &pos, const glm::ivec2 &size, float *posY, bool& flagpole)
{
	int x0, x1, y;
	
	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;
	y = (pos.y + size.y - 1) / tileSize;

	if (pos.y + size.y >= 240) return false;

	for(int x=x0; x<=x1; x++)
	{
		if (map[y*mapSize.x + x] == -2 && !blockMatrix[y * mapSize.x + x]->isBroken()){
			// Coin
			++nCoins;
			blockMatrix[y * mapSize.x + x]->collectCoin(true);
			continue;
		}
		else if (map[y*mapSize.x+x] > 0) {
			if (map[y*mapSize.x + x] == T_FLAGPOLE) {
				flagpole = true;
				return false;
			} else if (map[y*mapSize.x + x] == T_UPPERPOLE) return false;

			if(*posY - tileSize * y + size.y <= 4)
			{
				*posY = tileSize * y - size.y;
				return true;
			}
		} else if (blockMatrix[y * mapSize.x + x] != NULL){
			if (map[y*mapSize.x + x] < 0 && !blockMatrix[y * mapSize.x + x]->isBroken()){
				if(*posY - tileSize * y + size.y <= 4)
				{
					*posY = tileSize * y - size.y;
					return true;
				}
			}
		}

	}
	
	return false;
}

int TileMap::getTotalCoins(){ return nCoins; }
void TileMap::flushNewItemQueue(){ newItem = 'N'; }
char TileMap::getNewItemChar(){ return newItem; }
glm::vec2 TileMap::getNewItemPos(){ return posNewItem; }
