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

	// Level, coins, points, remainig time
	level = 1;
	points = 0;
	remTime = 400.f;

	// Enemies initialization
	enemies.clear();
	enemies.resize(4);
	for (int e = 0; e < 4; ++e){
		enemies[e] = new Item();
		if (e != 3) enemies[e]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		else enemies[e]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[e]->setPosition(glm::vec2(10 * map->getTileSize() + e*40, 10 * map->getTileSize()));
		enemies[e]->setTileMap(map);
	}

	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);

	camera = glm::vec4(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	projection = glm::ortho(camera.x, camera.y, camera.z, camera.w);

	defaultMus.openFromFile("audio/track1.ogg");
	defaultMus.setLoop(true);
	defaultMus.setVolume(30);
	defaultMus.play();

	playerDeathStarted = false;

}

void Scene::update(int deltaTime)
{
	remTime -= deltaTime / 1000.f;
	map->update(deltaTime);

	if (player->isDead()){
		// Game Over
		exit(0);
	}
	
	glm::ivec2 playerPosAnt = player->getPosition();

	// Update player
	player->update(deltaTime);
	glm::ivec2 playerPos = player->getPosition();
	glm::ivec2 playerSize = player->getSize();

	// Update enemies
	for (int e = 0; e < enemies.size(); ++e){
		if (enemies[e]->isDead()) continue;

		enemies[e]->update(deltaTime);
		glm::ivec2 enemyPos = enemies[e]->getPosition();
		glm::ivec2 enemySize = enemies[e]->getSize();

		// First check for collision between enemies
		for (int ee = 0; ee < enemies.size(); ++ee){
			if (ee == e || enemies[ee]->isDead()) continue;

			if (enemies[ee]->collisionKill(enemyPos, enemySize)){
				// Collision
				enemies[ee]->invertXVelocity();
				enemies[e]->invertXVelocity();
			}
		}


		if (enemies[e]->collisionStomped(playerPos, playerSize) && !playerDeathStarted){
			enemies[e]->stomp(playerPos);
			player->jump(30);
		} else if (enemies[e]->collisionKill(playerPos, playerSize)){
			player->die();
			defaultMus.stop();
			playerDeathStarted = true;
		}
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

	// Enemies
	for (int e = 0; e < enemies.size(); ++e){
		if (!enemies[e]->isDead()) enemies[e]->render();
	}

	// Render text
	string pointString = std::to_string(points);
	pointString.insert(pointString.begin(), 6 - pointString.length(), '0');
	renderText(pointString, glm::vec2(5, 5));

	string levelStr = "WORLD";
	renderText(levelStr, glm::vec2(75, 5));

	levelStr = "1-" + std::to_string(level);
	renderText(levelStr, glm::vec2(85, 15));

	string coins = "# ";
	coins += std::to_string(map->getTotalCoins());
	renderText(coins, glm::vec2(140, 5));

	string timeStr = "TIME";
	renderText(timeStr, glm::vec2(200, 5));

	timeStr = std::to_string((int) remTime);
	renderText(timeStr, glm::vec2(208, 15));

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

	textSprite->setAnimationSpeed(POINT, 0);
	textSprite->addKeyframe(POINT, glm::vec2(0.f, 0.25f));
	textSprite->setAnimationSpeed(COMMA, 0);
	textSprite->addKeyframe(COMMA, glm::vec2(0.0625f, 0.25f));
	textSprite->setAnimationSpeed(DASH, 0);
	textSprite->addKeyframe(DASH, glm::vec2(0.125f, 0.25f));
	textSprite->setAnimationSpeed(EXCLAMATION, 0);
	textSprite->addKeyframe(EXCLAMATION, glm::vec2(0.1875f, 0.25f));
	textSprite->setAnimationSpeed(EQUALS, 0);
	textSprite->addKeyframe(EQUALS, glm::vec2(0.25f, 0.25f));
	textSprite->setAnimationSpeed(TWODOTS, 0);
	textSprite->addKeyframe(TWODOTS, glm::vec2(0.3125f, 0.25f));
	textSprite->setAnimationSpeed(APOSTROPHE, 0);
	textSprite->addKeyframe(APOSTROPHE, glm::vec2(0.375f, 0.25f));
	textSprite->setAnimationSpeed(DOUBLEAPOSTROPHE, 0);
	textSprite->addKeyframe(DOUBLEAPOSTROPHE, glm::vec2(0.4375f, 0.25f));

	return;
}

void Scene::renderText(string& text, glm::vec2 pos){
	// Position top-left
	int i = 0;

	for (char c : text){
		// Rendering char c
		switch (c){
			case ' ':
				++i;
				continue;
			case '#':
				textSprite->changeAnimation(COIN);
				break;
			case '.':
				textSprite->changeAnimation(POINT);
				break;
			case ',':
				textSprite->changeAnimation(COMMA);
				break;
			case '-':
				textSprite->changeAnimation(DASH);
				break;
			case '!':
				textSprite->changeAnimation(EXCLAMATION);
				break;
			case ':':
				textSprite->changeAnimation(TWODOTS);
				break;
			case '=':
				textSprite->changeAnimation(EQUALS);
				break;
			case '\'':
				textSprite->changeAnimation(APOSTROPHE);
				break;
			case '\"':
				textSprite->changeAnimation(DOUBLEAPOSTROPHE);
				break;
			default:
				if (c >= int('0') && c <= int('9')) textSprite->changeAnimation(c - int('0'));
				else if (c >= int('A') && c <= int('Z')) textSprite->changeAnimation(c - int('A') + A);
		}
		
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



