#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
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

private:
	glm::vec4 camera; // Left, Right, Bottom, Up

	TileMap* map;
	TileMap* bgmap;
	Player* player;
	Item* goomba;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	sf::Music defaultMus;
	bool playerDeathStarted;
};


#endif // _SCENE_INCLUDE

