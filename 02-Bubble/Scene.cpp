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
	initGlyphTextures();
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

	camera = glm::vec4(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	projection = glm::ortho(camera.x, camera.y, camera.z, camera.w);
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

	map->update(deltaTime);

	if (player->isDead()){
		// Game Over
		exit(0);
	}
	
	glm::ivec2 playerPosAnt = player->getPosition();

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


	if (playerPos.x <= camera.x && playerPos.x - playerPosAnt.x < 0){
		player->setPosition(glm::vec2(camera.x, playerPos.y));
	}

	// Scroll to the right (bottom and up stay the same)
	if (playerPos.x > camera.x + 2*SCREEN_WIDTH / 3 && playerPos.x - playerPosAnt.x > 0){
		camera.x += (playerPos.x - playerPosAnt.x);
		camera.y += (playerPos.x - playerPosAnt.x);
		projection = glm::ortho(camera.x, camera.y, camera.z, camera.w);
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

	// Render text
	const char * level = "LEVEL 1";
	renderText(level, glm::vec2(5, 5));

	const char * coins = "# 5";
	renderText(coins, glm::vec2(128, 5));

}

enum Glyphs {
	ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, XZERO, XONE, XTWO, XTHREE, XFOUR, XFIVE, XSIX, XSEVEN, XEIGHT, XNINE,
	COIN, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 
	POINT, COMMA, DASH, EXCLAMATION, EQUALS, TWODOTS, APOSTROPHE, DOUBLEAPOSTROPHE
};

void Scene::initGlyphTextures(){
	textSpriteSheet.loadFromFile("images/letters.png", TEXTURE_PIXEL_FORMAT_RGBA);
	textSprite = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(0.0625, 0.0625), &textSpriteSheet, &texProgram);

	textSprite->setNumberAnimations(64);

	// Numbers
	for (int x = ZERO; x <= XNINE; ++x){
		textSprite->setAnimationSpeed(x, 0);
		textSprite->addKeyframe(x, glm::vec2(0.0625*(x % 10), (x > NINE ? 0.0625 : 0)));
	}

	textSprite->setAnimationSpeed(COIN, 0);
	textSprite->addKeyframe(COIN, glm::vec2(0.625f, 0.f));

	// Letters
	for (int x = A; x <= Z; ++x){
		int offset = x - A;
		textSprite->setAnimationSpeed(x, 0);
		textSprite->addKeyframe(x, glm::vec2(0.0625*(offset % (Q-A)), (x > P ? 0.1875f : 0.125f)));
	}

	return;
}

void Scene::renderText(const char * text, glm::vec2 pos){
	// Position top-left
	const char * c;
	int i = 0;

	for (c = text; *c != '\0'; ++c){
		// Rendering char *c
		if (*c == ' ') {
			++i;
			continue;
		} else if (*c == '#'){
			// Coin
			textSprite->changeAnimation(COIN);
		}
		else if (*c >= int('0') && *c <= int('9')) textSprite->changeAnimation(*c - int('0'));
		else if (*c >= int('A') && *c <= int('Z')) textSprite->changeAnimation(*c - int('A') + A);
		
		textSprite->setPosition( glm::vec2(pos.x + 8*i + camera.x, pos.y));
		textSprite->render(false);

		++i;
	}
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



