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

	void init();
	void update(int deltaTime);
	void render();

private:
	void initShaders();
	void initGlyphTextures();
	void renderText(string& text, glm::vec2 pos);

private:
	glm::vec4 camera; // Left, Right, Bottom, Up

	TileMap* map;
	TileMap* bgmap;
	Player* player;
	vector<Item*> enemies;

	Item* goomba;
	ShaderProgram texProgram;

	Texture textSpriteSheet;
	Sprite* textSprite;
	glm::mat4 projection;

	sf::Music defaultMus;
	bool playerDeathStarted;

	int points, level;
	float remTime;
};


#endif // _SCENE_INCLUDE

