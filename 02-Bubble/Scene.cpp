#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include <SFML/Audio.hpp>

#define SCREEN_X 0
#define SCREEN_Y 0

#define INIT_PLAYER_X_TILES 4
#define INIT_PLAYER_Y_TILES 4


Scene::Scene()
{
	map = NULL;
	bgmap = NULL;
	player = NULL;
	goomba = NULL;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
}


void Scene::init()
{
	initShaders();
	map = TileMap::createTileMap("levels/level01.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram, false);
	bgmap = TileMap::createTileMap("levels/level01-bg.txt", glm::vec2(0, 0), texProgram, true);

	goomba = new Item();
	goomba->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
	goomba->setPosition(glm::vec2(10 * map->getTileSize(), 10 * map->getTileSize()));
	goomba->setTileMap(map);


	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);


	projection = glm::ortho(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	currentTime = 0.0f;

	defaultMus.openFromFile("audio/track1.ogg");
	defaultMus.setLoop(true);
	defaultMus.setVolume(30);
	defaultMus.play();


	playerDeathStarted = false;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	if (player->isDead()){
		// Game Over
		exit(0);
	}

	player->update(deltaTime);
	if (!goomba->isDead()) goomba->update(deltaTime);

	glm::ivec2 playerPos = player->getPosition();
	glm::ivec2 playerSize = player->getSize();

	if (goomba->collisionStomped(playerPos, playerSize) && !playerDeathStarted){
		goomba->die();
	} else if (goomba->collisionKill(playerPos, playerSize)){
		// Death
		player->die();
		playerDeathStarted = true;
	}
}

void Scene::render()
{
	glm::mat4 modelview;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	texProgram.setUniformInt("invertX", 0); // False

	glClearColor(.57f, 0.57f, 1.0f, 1.0f);

	bgmap->render();
	map->render();
	player->render();
	if (!goomba->isDead()) goomba->render();
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}



