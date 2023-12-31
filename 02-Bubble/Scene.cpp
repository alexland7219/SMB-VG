#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "Sound.h"
#include <string> 

#define SCREEN_X 0
#define SCREEN_Y 0

#define INIT_PLAYER_X_TILES 0
#define INIT_PLAYER_Y_TILES (lvl==1 ? 13 : 11)
#define SPAWN_DISTANCE 200

Scene::Scene()
{
	map = NULL;
	bgmap = NULL;
	fgmap = NULL;
	player = NULL;
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
}

bool Scene::isOver(){ return gameOver; }
bool Scene::hasWon(){ return won; }


void Scene::init(int lvl)
{
	initShaders();
	initGlyphTextures();
	initFloatTextures();

	string levelX = "level0" + std::to_string(lvl);

	map = TileMap::createTileMap("levels/" + levelX + ".txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram, false);
	fgmap = TileMap::createTileMap("levels/"+levelX + "-fg.txt", glm::vec2(0,0), texProgram, true);
	bgmap = TileMap::createTileMap("levels/" + levelX + "-bg.txt", glm::vec2(0, 0), texProgram, true);

	// Level, coins, points, remainig time, gameOver?
	level = lvl;
	points = 0;
	pointStreak = 0;
	remTime = 400.f;
	resetStreakCounter = 5000;
	gameOver = false;
	won = false;
	defPaused = false;
	bowserplayed = false;

	// Enemies initialization

	if (lvl == 1){
		// Ubicacions dels enemics en el nivell 1
		enemies.clear();
		enemies.resize(16);

		enemies[0] = new Item();
		enemies[0]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[0]->setPosition(glm::vec2(7 * map->getTileSize(), 13 * map->getTileSize()));
		enemies[0]->setTileMap(map);

		enemies[1] = new Item();
		enemies[1]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[1]->setPosition(glm::vec2(10 * map->getTileSize(), 6 * map->getTileSize()));
		enemies[1]->setTileMap(map);

		enemies[2] = new Item();
		enemies[2]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[2]->setPosition(glm::vec2(40 * map->getTileSize(), 12 * map->getTileSize()));
		enemies[2]->setTileMap(map);

		enemies[3] = new Item();
		enemies[3]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[3]->setPosition(glm::vec2(45 * map->getTileSize(), 11 * map->getTileSize()));
		enemies[3]->setTileMap(map);

		enemies[4] = new Item();
		enemies[4]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[4]->setPosition(glm::vec2(72 * map->getTileSize(), 6 * map->getTileSize()));
		enemies[4]->setTileMap(map);

		enemies[5] = new Item();
		enemies[5]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[5]->setPosition(glm::vec2(74 * map->getTileSize(), 8 * map->getTileSize()));
		enemies[5]->setTileMap(map);

		enemies[6] = new Item();
		enemies[6]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[6]->setPosition(glm::vec2(75 * map->getTileSize(), 12 * map->getTileSize()));
		enemies[6]->setTileMap(map);

		enemies[7] = new Item();
		enemies[7]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[7]->setPosition(glm::vec2(82 * map->getTileSize(), 11 * map->getTileSize()));
		enemies[7]->setTileMap(map);

		enemies[8] = new Item();
		enemies[8]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[8]->setPosition(glm::vec2(85 * map->getTileSize(), 6 * map->getTileSize()));
		enemies[8]->setTileMap(map);

		enemies[11] = new Item();
		enemies[11]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[11]->setPosition(glm::vec2(85 * map->getTileSize(), 8 * map->getTileSize()));
		enemies[11]->setTileMap(map);

		enemies[9] = new Item();
		enemies[9]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[9]->setPosition(glm::vec2(90 * map->getTileSize(), 8 * map->getTileSize()));
		enemies[9]->setTileMap(map);

		enemies[10] = new Item();
		enemies[10]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[10]->setPosition(glm::vec2(100 * map->getTileSize(), 12 * map->getTileSize()));
		enemies[10]->setTileMap(map);

		enemies[12] = new Item();
		enemies[12]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[12]->setPosition(glm::vec2(87 * map->getTileSize(), 3 * map->getTileSize()));
		enemies[12]->setTileMap(map);

		enemies[13] = new Item();
		enemies[13]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[13]->setPosition(glm::vec2(57 * map->getTileSize(), 10 * map->getTileSize()));
		enemies[13]->setTileMap(map);

		enemies[14] = new Item();
		enemies[14]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[14]->setPosition(glm::vec2(55 * map->getTileSize(), 10 * map->getTileSize()));
		enemies[14]->setTileMap(map);

		enemies[15] = new Item();
		enemies[15]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[15]->setPosition(glm::vec2(59 * map->getTileSize(), 10 * map->getTileSize()));
		enemies[15]->setTileMap(map);

	}
	else if (lvl == 2)
	{
		// Ubicacions dels enemics en el nivell 1
		enemies.clear();
		enemies.resize(13);

		enemies[0] = new Item();
		enemies[0]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[0]->setPosition(glm::vec2(6 * map->getTileSize(), 11 * map->getTileSize()));
		enemies[0]->setTileMap(map);

		enemies[1] = new Item();
		enemies[1]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[1]->setPosition(glm::vec2(9 * map->getTileSize(), 11 * map->getTileSize()));
		enemies[1]->setTileMap(map);

		enemies[2] = new Item();
		enemies[2]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[2]->setPosition(glm::vec2(24 * map->getTileSize(), 11 * map->getTileSize()));
		enemies[2]->setTileMap(map);

		enemies[3] = new Item();
		enemies[3]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[3]->setPosition(glm::vec2(26 * map->getTileSize(), 12 * map->getTileSize()));
		enemies[3]->setTileMap(map);

		enemies[4] = new Item();
		enemies[4]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[4]->setPosition(glm::vec2(31 * map->getTileSize(), 12 * map->getTileSize()));
		enemies[4]->setTileMap(map);

		enemies[5] = new Item();
		enemies[5]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[5]->setPosition(glm::vec2(46 * map->getTileSize(), 10 * map->getTileSize()));
		enemies[5]->setTileMap(map);

		enemies[6] = new Item();
		enemies[6]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[6]->setPosition(glm::vec2(49 * map->getTileSize(), 11 * map->getTileSize()));
		enemies[6]->setTileMap(map);

		enemies[7] = new Item();
		enemies[7]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 1);
		enemies[7]->setPosition(glm::vec2(54 * map->getTileSize(), 10 * map->getTileSize()));
		enemies[7]->setTileMap(map);

		enemies[8] = new Item();
		enemies[8]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[8]->setPosition(glm::vec2(57 * map->getTileSize(), 11 * map->getTileSize()));
		enemies[8]->setTileMap(map);

		enemies[9] = new Item();
		enemies[9]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[9]->setPosition(glm::vec2(69 * map->getTileSize(), 8 * map->getTileSize()));
		enemies[9]->setTileMap(map);

		enemies[10] = new Item();
		enemies[10]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[10]->setPosition(glm::vec2(71 * map->getTileSize(), 8 * map->getTileSize()));
		enemies[10]->setTileMap(map);

		enemies[11] = new Item();
		enemies[11]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[11]->setPosition(glm::vec2(73 * map->getTileSize(), 8 * map->getTileSize()));
		enemies[11]->setTileMap(map);

		enemies[12] = new Item();
		enemies[12]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 0);
		enemies[12]->setPosition(glm::vec2(90 * map->getTileSize(), 13 * map->getTileSize()));
		enemies[12]->setTileMap(map);
	}

	// Items initialization
	items.clear();

	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);

	camera = glm::vec4(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	projection = glm::ortho(camera.x, camera.y, camera.z, camera.w);
	
	Sound::instance().stopAll();
	if (level == 1) {
		Sound::instance().stop(15);
		Sound::instance().play(7); // Play Main Theme
	}
	else {
		Sound::instance().stop(7);
		Sound::instance().play(15); //  Play second level theme
	}

	playerDeathStarted = false;
	playerFlagpoleStarted = false;
	floatsToRender.clear();
}

void Scene::removePlayer(){
	delete player;
}

enum FloatingBlocks {
	ONE_H, TWO_H, FOUR_H, EIGHT_H, ONE_TH, TWO_TH, FOUR_TH, EIGHT_TH, COIN_BUMP, MUSH_BUMP, STAR_BUMP
};

int getPtsByStreak(int streak){
	switch (streak){
		case 0:
		return 400;
		case 1:
		return 800;
		case 2:
		return 1000;
		case 3:
		return 2000;
		case 4:
		return 4000;
		default:
		return 8000;
	}
}

int getPtsFromFlagpole(int height){
	float perc = 147.5 - 0.709*height;

	if (perc < 12.5) return 100;
	else if (perc < 25) return 200;
	else if (perc < 37.5) return 400;
	else if (perc < 50) return 800;
	else if (perc < 62.5) return 1000;
	else if (perc < 75) return 2000;
	else if (perc < 87.5) return 4000;
	else return 8000;

}


int getFloatByPoints(int pts){
	switch (pts){
		case 100:
		return ONE_H;
		case 200:
		return TWO_H;
		case 400:
		return FOUR_H;
		case 800:
		return EIGHT_H;
		case 1000:
		return ONE_TH;
		case 2000:
		return TWO_TH;
		case 4000:
		return FOUR_TH;
		default:
		return EIGHT_TH;
	}
}

void Scene::update(int deltaTime){
	// Star
	if (player->getStar() && !defPaused) {
		if (level == 1) Sound::instance().stop(7); // Stop main theme
		else Sound::instance().stop(15); // Stop second level theme
		defPaused = true;
	}
	else if (!player->getStar() && defPaused && !playerFlagpoleStarted) {
		if (level == 1) Sound::instance().play(7); // Start main theme
		else Sound::instance().play(15); // Start second level theme theme 
		defPaused = false;
	}

	// Remaining time update
	if (remTime < 0 && !playerFlagpoleStarted) gameOver = true;

	if (!playerFlagpoleStarted) remTime -= deltaTime / 1000.f;
	else if (remTime > 0){
		remTime -= deltaTime / 10.f;
		if (remTime < 0) remTime = 0;

		points += (deltaTime);
	}

	resetStreakCounter -= deltaTime;
	if (resetStreakCounter < 0){
		pointStreak = 0;
		resetStreakCounter = 5000;
	}


	map->update(deltaTime);

	// Update floating blocks
	for (int i = 0; i < floatsToRender.size(); ++i){
		if (floatsToRender[i].y <= 0) continue;

		floatsToRender[i].y -= deltaTime;

		if (floatsToRender[i].y <= 0 && (floatsToRender[i].x >= MUSH_BUMP)){
			// Need to add the new item
			items.push_back(new Item());
			items[items.size()-1]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, floatsToRender[i].x - EIGHT_TH);
			items[items.size()-1]->setPosition(glm::vec2(floatsToRender[i].z, floatsToRender[i].w));
			items[items.size()-1]->setTileMap(map);
		}

		if (floatsToRender[i].x >= COIN_BUMP){
			floatsToRender[i].w -= .3f; // Raise
		}
	}

	// handle possible new items
	char newItem = map->getNewItemChar();
	glm::vec2 mushPos;

	switch (newItem){
		case 'M':
		// New mushroom
		mushPos = map->getNewItemPos();
		floatsToRender.push_back(glm::vec4(MUSH_BUMP, 600, 16*mushPos.x, 17*mushPos.y));
		map->flushNewItemQueue();
		break;
		case 'S':
		// New star
		mushPos = map->getNewItemPos();
		floatsToRender.push_back(glm::vec4(STAR_BUMP, 600, 16*mushPos.x, 17*mushPos.y));
		map->flushNewItemQueue();
		break;
		case 'C':
		mushPos = map->getNewItemPos();
		floatsToRender.push_back(glm::vec4(COIN_BUMP, 400, 16*mushPos.x, 16*mushPos.y));
		map->flushNewItemQueue();

	}

	if (player->isDead()) {
		// Game Over
		bowserplayed = false;
		gameOver = true;
		return;
	}
	else if (player->hasDeathAnimStarted()) {
		Sound::instance().stop(7); // Stop main theme
		Sound::instance().stop(15); // Stop second level theme	//	AIXO NO SE SI FA ALGO
	}
	glm::ivec2 playerPosAnt = player->getPosition();

	// Update player
	player->update(deltaTime);
	glm::ivec2 playerPos = player->getPosition();
	glm::ivec2 playerSize = player->getSize();

	// Check if the player has touched flagpole
	if (player->hasWinningAnimStarted() && !playerFlagpoleStarted){
		playerFlagpoleStarted = true;

		Sound::instance().stopAll(); // Stop all songs
		Sound::instance().play(6);  // Play win theme

		int inc = getPtsFromFlagpole(playerPos.y);
		points += inc;
		floatsToRender.push_back(glm::vec4(getFloatByPoints(inc), 400, playerPos.x - 5, playerPos.y - 16));
	}

	// Update enemies
	for (int e = 0; e < enemies.size(); ++e){
		if (enemies[e]->isDead()) continue;

		if (!enemies[e]->isSpawned() && enemies[e]->getPosition().x - playerPos.x <= SPAWN_DISTANCE) enemies[e]->setSpawned(true);
		else if (!enemies[e]->isSpawned()) continue;

		enemies[e]->update(deltaTime);
		glm::ivec2 enemyPos = enemies[e]->getPosition();
		glm::ivec2 enemySize = enemies[e]->getSize();

		// First check for collision between enemies
		for (int ee = 0; ee < enemies.size(); ++ee){
			if (ee == e || enemies[ee]->hasDeathAnimStarted() || enemies[e]->hasDeathAnimStarted()) continue;
			if (!enemies[ee]->isSpawned()) continue;

			if (enemies[ee]->collisionKill(enemyPos, enemySize)){
				// Collision
				int inc = getPtsByStreak(pointStreak);

				if (enemies[ee]->killsEnemies()){
					enemies[e]->die();
					points += inc;
					++pointStreak;
					floatsToRender.push_back(glm::vec4(getFloatByPoints(inc), 400, enemyPos.x, enemyPos.y - 16));
				}
				else if (enemies[e]->killsEnemies()){
					enemies[ee]->die();
					points += getPtsByStreak(pointStreak);
					++pointStreak;
					glm::vec2 otherEnemyPos = enemies[ee]->getPosition();
					floatsToRender.push_back(glm::vec4(getFloatByPoints(inc), 400, otherEnemyPos.x, otherEnemyPos.y - 16));
				} 
				else {
					enemies[ee]->invertXVelocity();
					enemies[e]->invertXVelocity();
				}
			}
		}

		// Collisions enemy - player
		if (enemies[e]->collisionStomped(playerPos, playerSize) && !playerDeathStarted && !player->getStar()){
			int enemyType = enemies[e]->getType();

			int inc = getPtsByStreak(pointStreak);
			int spriteThatWillRender = getFloatByPoints(inc);

			switch (enemyType){
				case 0: // GOOMBA
				floatsToRender.push_back(glm::vec4(spriteThatWillRender, 400, playerPos.x, playerPos.y));
				points += getPtsByStreak(pointStreak);

				Sound::instance().play(9);
				break;
				case 1: // KOOPA TROOPA
				floatsToRender.push_back(glm::vec4(spriteThatWillRender, 400, playerPos.x, playerPos.y));
				points += getPtsByStreak(pointStreak);

				Sound::instance().play(10);
				break;

			}

			++pointStreak;

			enemies[e]->stomp(playerPos);
			player->jump(30);
		} else if (enemies[e]->collisionKill(playerPos, playerSize) && !player->getStar() && !player->hasWinningAnimStarted()){
			pointStreak = 0;
			player->die();
			playerDeathStarted = player->hasDeathAnimStarted();

			if (playerDeathStarted){
				// Stop overworld songs
				Sound::instance().stop(7);
				Sound::instance().stop(15);
				Sound::instance().stop(8);
				Sound::instance().stop(16);
			} 

		} else if ((enemies[e]->collisionKill(playerPos, playerSize) || enemies[e]->collisionStomped(playerPos, playerSize)) && player->getStar()){
			// Player has a star!

			int enemyType = enemies[e]->getType();
			int inc = getPtsByStreak(pointStreak);
			int spriteThatWillRender = getFloatByPoints(inc);

			switch (enemyType){
				case 0: // GOOMBA
				floatsToRender.push_back(glm::vec4(spriteThatWillRender, 400, playerPos.x, playerPos.y));
				points += getPtsByStreak(pointStreak);

				Sound::instance().play(9);
				break;
				case 1: // KOOPA TROOPA
				floatsToRender.push_back(glm::vec4(spriteThatWillRender, 400, playerPos.x, playerPos.y));
				points += getPtsByStreak(pointStreak);

				Sound::instance().play(9);
				break;

			}

			++pointStreak;

			enemies[e]->die();
		}
	}

	// Update Items
	for (int i = 0; i < items.size(); ++i){
		if (items[i]->isDead()) continue;

		items[i]->update(deltaTime);
		glm::ivec2 itemPos = items[i]->getPosition();
		glm::ivec2 itemSize = items[i]->getSize();

		if (items[i]->collisionStomped(playerPos, playerSize) || items[i]->collisionKill(playerPos, playerSize)){
			// MUSHROOM
			if (items[i]->getType() == 2){
				points += 1000;
				floatsToRender.push_back(glm::vec4(ONE_TH, 400, playerPos.x, playerPos.y - 16));
				player->mushroom();

				Sound::instance().play(4);
				items[i]->die();

			} else { // STAR
				points += 2000;
				floatsToRender.push_back(glm::vec4(TWO_TH, 400, playerPos.x, playerPos.y - 16));
				player->star();
				items[i]->die();
			}
		}

	}

	if (playerPos.x <= camera.x && playerPos.x - playerPosAnt.x < 0){
		player->setPosition(glm::vec2(camera.x, playerPos.y));
	}

	// Scroll to the right (bottom and up stay the same)
	if (playerPos.x > camera.x + 2*SCREEN_WIDTH / 3 && playerPos.x - playerPosAnt.x > 0 && !player->hasWinningAnimStarted()){
		camera.x += (playerPos.x - playerPosAnt.x);
		camera.y += (playerPos.x - playerPosAnt.x);
		projection = glm::ortho(camera.x, camera.y, camera.z, camera.w);
	}

	// WIN
	if (player->won()){
		won = true;
	}

	// DONT OVERFLOW OR ELSE SIGFAULT
	if (points > 999999) points = 999999;

	// Secret :)
	if (playerPos.x > 750 && Sound::instance().getStatus(7) == 2 && !playerFlagpoleStarted && level == 1){
		Sound::instance().stop(7);
		Sound::instance().play(8);
	}

	cout << (level == 2) << " " << (playerPos.x > 448) << " " << !bowserplayed << endl;
	if (level == 2 && playerPos.x > 448 && !bowserplayed) {
		Sound::instance().play(13);
		bowserplayed = true;
	}
	if (level == 2) {
		if (playerPos.x > 1424 && Sound::instance().getStatus(15) == 2) {
			Sound::instance().stop(15);
			Sound::instance().play(16);
		}
		else if (playerPos.x <= 1424 && Sound::instance().getStatus(16) == 2) {
			Sound::instance().stop(16);
			Sound::instance().play(15);
		}
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

	if (level == 1 || player->getPosition().x > 1424) glClearColor(0.57f, 0.57f, 1.0f, 1.0f);
	else glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	bgmap->render();
	map->render();
	player->render();

	// Render floating blocks
	for (int i = 0; i < floatsToRender.size(); ++i){
		if (floatsToRender[i].y <= 0) continue;

		renderFloating(floatsToRender[i].x, glm::vec2(floatsToRender[i].z, floatsToRender[i].w));
	}


	// Enemies
	for (int e = 0; e < enemies.size(); ++e){
		if (!enemies[e]->isDead()) enemies[e]->render();
	}

	// Items
	for (int i = 0; i < items.size(); ++i){
		if (!items[i]->isDead()) items[i]->render();
	}

	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	texProgram.setUniformInt("invertX", 0); // False


	// Render foreground
	fgmap->render();

	// Render text
	string pointString = std::to_string(points + map->getTotalCoins()*100);
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
				textSprite->changeAnimation(COIN,false);
				break;
			case '.':
				textSprite->changeAnimation(POINT, false);
				break;
			case ',':
				textSprite->changeAnimation(COMMA, false);
				break;
			case '-':
				textSprite->changeAnimation(DASH, false);
				break;
			case '!':
				textSprite->changeAnimation(EXCLAMATION, false);
				break;
			case ':':
				textSprite->changeAnimation(TWODOTS, false);
				break;
			case '=':
				textSprite->changeAnimation(EQUALS, false);
				break;
			case '\'':
				textSprite->changeAnimation(APOSTROPHE, false);
				break;
			case '\"':
				textSprite->changeAnimation(DOUBLEAPOSTROPHE, false);
				break;
			default:
				if (c >= int('0') && c <= int('9')) textSprite->changeAnimation(c - int('0'), false);
				else if (c >= int('A') && c <= int('Z')) textSprite->changeAnimation(c - int('A') + A, false);
		}
		
		textSprite->setPosition( glm::vec2(pos.x + 8*i + camera.x, pos.y));
		textSprite->render(false);

		++i;
	}
}


void Scene::initFloatTextures(){
	floatSpriteSheet.loadFromFile("images/letters.png", TEXTURE_PIXEL_FORMAT_RGBA);
	floatSprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.125, 0.125), &floatSpriteSheet, &texProgram);

	floatSprite->setNumberAnimations(11);

	floatSprite->setAnimationSpeed(ONE_H, 0);
	floatSprite->addKeyframe(ONE_H, glm::vec2(0.f, 0.3125f));
	floatSprite->setAnimationSpeed(TWO_H, 0);
	floatSprite->addKeyframe(TWO_H, glm::vec2(0.125f, 0.3125f));
	floatSprite->setAnimationSpeed(FOUR_H, 0);
	floatSprite->addKeyframe(FOUR_H, glm::vec2(0.25f, 0.3125f));
	floatSprite->setAnimationSpeed(EIGHT_H, 0);
	floatSprite->addKeyframe(EIGHT_H, glm::vec2(0.375f, 0.3125f));

	floatSprite->setAnimationSpeed(ONE_TH, 0);
	floatSprite->addKeyframe(ONE_TH, glm::vec2(0.5f, 0.3125f));
	floatSprite->setAnimationSpeed(TWO_TH, 0);
	floatSprite->addKeyframe(TWO_TH, glm::vec2(0.625f, 0.3125f));
	floatSprite->setAnimationSpeed(FOUR_TH, 0);
	floatSprite->addKeyframe(FOUR_TH, glm::vec2(0.75f, 0.3125f));
	floatSprite->setAnimationSpeed(EIGHT_TH, 0);
	floatSprite->addKeyframe(EIGHT_TH, glm::vec2(0.875f, 0.3125f));

	floatSprite->setAnimationSpeed(COIN_BUMP, 4);
	floatSprite->addKeyframe(COIN_BUMP, glm::vec2(0.f, 0.4375f));
	floatSprite->addKeyframe(COIN_BUMP, glm::vec2(0.375f, 0.4375f));
	floatSprite->addKeyframe(COIN_BUMP, glm::vec2(0.5f, 0.4375f));
	floatSprite->setAnimationSpeed(MUSH_BUMP, 0);
	floatSprite->addKeyframe(MUSH_BUMP, glm::vec2(0.125f, 0.4375f));
	floatSprite->setAnimationSpeed(STAR_BUMP, 0);
	floatSprite->addKeyframe(STAR_BUMP, glm::vec2(0.25f, 0.4375f));

	return;

}

void Scene::renderFloating(int type, glm::vec2 pos){
	floatSprite->changeAnimation(type, false);
	floatSprite->setPosition( glm::vec2(pos.x, pos.y));
	floatSprite->render(false);
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



