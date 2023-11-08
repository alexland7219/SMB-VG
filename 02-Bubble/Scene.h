#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "Texture.h"
#include "Item.h"

// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init(int lvl);
	void update(int deltaTime);
	void render();
	bool isOver();
	bool hasWon();
	void removePlayer();

private:
	void initShaders();
	void initGlyphTextures();
	void renderText(string& text, glm::vec2 pos);

	void initFloatTextures();
	void renderFloating(int type, glm::vec2 pos);

private:
	glm::vec4 camera; // Left, Right, Bottom, Up

	TileMap* map;
	TileMap* bgmap;
	TileMap* fgmap;
	Player* player;
	vector<Item*> enemies;
	vector<Item*> items;

	Item* goomba;
	ShaderProgram texProgram;

	Texture textSpriteSheet;
	Sprite* textSprite;

	Texture floatSpriteSheet;
	Sprite* floatSprite;

	// First int -> type. Second -> Time to live. Third and Fourth -> position
	vector<glm::vec4> floatsToRender;

	glm::mat4 projection;

	//sf::Music defaultMus, goombaMus, koopaMus, mushroomMus, winMus, zeldaMus;
	// Plays Main theme (7), Goomba (9), Koopa (10), Mushroom (4), Win (6) and Zelda (8) songs
	bool playerDeathStarted, playerFlagpoleStarted, defPaused, bowserplayed;

	int points, level, pointStreak, resetStreakCounter;
	float remTime;

	bool gameOver, won;
};


#endif // _SCENE_INCLUDE

