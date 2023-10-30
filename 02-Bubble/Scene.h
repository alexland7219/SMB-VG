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
#include <SFML/Audio.hpp>

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

	sf::Music defaultMus, goombaMus, koopaMus, mushroomMus, winMus, zeldaMus;
	bool playerDeathStarted;

	int points, level, pointStreak;
	float remTime;

	bool gameOver;
};


#endif // _SCENE_INCLUDE

