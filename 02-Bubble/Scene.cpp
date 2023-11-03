#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include <SFML/Audio.hpp>
#include <string> 

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

bool Scene::isOver(){ return gameOver; }
bool Scene::hasWon(){ return won; }


void Scene::init(int lvl)
{
	initShaders();
	initGlyphTextures();
	initFloatTextures();

	string levelX = "level0" + std::to_string(lvl);

	map = TileMap::createTileMap("levels/" + levelX + ".txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram, false);
	bgmap = TileMap::createTileMap("levels/" + levelX + "-bg.txt", glm::vec2(0, 0), texProgram, true);

	// Level, coins, points, remainig time, gameOver?
	level = lvl;
	points = 0;
	pointStreak = 0;
	remTime = 400.f;
	resetStreakCounter = 5000;
	gameOver = false;
	won = false;

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

	// Items initialization
	items.clear();

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

	zeldaMus.openFromFile("audio/zelda.ogg");
	zeldaMus.setLoop(true);
	zeldaMus.setVolume(60);

	goombaMus.openFromFile("audio/goomba.ogg");
	koopaMus.openFromFile("audio/koopa.ogg");
	mushroomMus.openFromFile("audio/Mushroom.ogg");
	mushroomMus.setVolume(15);

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

void Scene::update(int deltaTime)
{
	if (remTime < 0 && !playerFlagpoleStarted) gameOver = true;

	if (!playerFlagpoleStarted) remTime -= deltaTime / 1000.f;
	else if (remTime > 0){
		remTime -= deltaTime / 10.f;
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
		/*
		items.push_back(new Item());
		items[items.size()-1]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 2);
		items[items.size()-1]->setPosition(glm::vec2(16*mushPos.x, 16*mushPos.y));
		items[items.size()-1]->setTileMap(map);*/
		map->flushNewItemQueue();
		break;
		case 'S':
		// New star
		mushPos = map->getNewItemPos();
		floatsToRender.push_back(glm::vec4(STAR_BUMP, 600, 16*mushPos.x, 17*mushPos.y));

		/*items.push_back(new Item());
		items[items.size()-1]->init(glm::vec2(SCREEN_X, SCREEN_Y), texProgram, 3);
		items[items.size()-1]->setPosition(glm::vec2(16*mushPos.x, 16*mushPos.y));
		items[items.size()-1]->setTileMap(map);*/
		map->flushNewItemQueue();
		break;
		case 'C':
		mushPos = map->getNewItemPos();
		floatsToRender.push_back(glm::vec4(COIN_BUMP, 400, 16*mushPos.x, 16*mushPos.y));
		map->flushNewItemQueue();

	}

	if (player->isDead()){
		// Game Over
		gameOver = true;
		return;
	} 
	else if (player->hasDeathAnimStarted()) defaultMus.stop();
	
	glm::ivec2 playerPosAnt = player->getPosition();

	// Update player
	player->update(deltaTime);
	glm::ivec2 playerPos = player->getPosition();
	glm::ivec2 playerSize = player->getSize();

	// Check if the player has touched flagpole
	if (player->hasWinningAnimStarted() && !playerFlagpoleStarted){
		playerFlagpoleStarted = true;

		int inc = getPtsFromFlagpole(playerPos.y);
		points += inc;
		floatsToRender.push_back(glm::vec4(getFloatByPoints(inc), 400, playerPos.x - 5, playerPos.y - 16));
		//std::cout << int(perc) << std::end;
	}

	// Update enemies
	for (int e = 0; e < enemies.size(); ++e){
		if (enemies[e]->isDead()) continue;

		enemies[e]->update(deltaTime);
		glm::ivec2 enemyPos = enemies[e]->getPosition();
		glm::ivec2 enemySize = enemies[e]->getSize();

		// First check for collision between enemies
		for (int ee = 0; ee < enemies.size(); ++ee){
			if (ee == e || enemies[ee]->hasDeathAnimStarted() || enemies[e]->hasDeathAnimStarted()) continue;

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


		if (enemies[e]->collisionStomped(playerPos, playerSize) && !playerDeathStarted){
			int enemyType = enemies[e]->getType();

			int inc = getPtsByStreak(pointStreak);
			int spriteThatWillRender = getFloatByPoints(inc);

			switch (enemyType){
				case 0: // GOOMBA
				floatsToRender.push_back(glm::vec4(spriteThatWillRender, 400, playerPos.x, playerPos.y));
				points += getPtsByStreak(pointStreak);
				goombaMus.play();
				goombaMus.setPlayingOffset(sf::Time::Zero);
				break;
				case 1: // KOOPA TROOPA
				floatsToRender.push_back(glm::vec4(spriteThatWillRender, 400, playerPos.x, playerPos.y));
				points += getPtsByStreak(pointStreak);
				koopaMus.play();
				koopaMus.setPlayingOffset(sf::Time::Zero);
				break;

			}

			++pointStreak;

			enemies[e]->stomp(playerPos);
			player->jump(30);
		} else if (enemies[e]->collisionKill(playerPos, playerSize)){
			pointStreak = 0;
			player->die();
			playerDeathStarted = player->hasDeathAnimStarted();
			if (playerDeathStarted) defaultMus.stop();

		}
	}

	// Update Items
	for (int i = 0; i < items.size(); ++i){
		if (items[i]->isDead()) continue;

		items[i]->update(deltaTime);
		glm::ivec2 itemPos = items[i]->getPosition();
		glm::ivec2 itemSize = items[i]->getSize();

		if (items[i]->collisionStomped(playerPos, playerSize) || items[i]->collisionKill(playerPos, playerSize)){
			points += 1000;
			floatsToRender.push_back(glm::vec4(ONE_TH, 400, playerPos.x, playerPos.y - 16));
			player->mushroom();
			mushroomMus.play();
			mushroomMus.setPlayingOffset(sf::Time::Zero);
			items[i]->die();
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

	// Secret :)
	if (playerPos.x > 800 && defaultMus.getStatus() == 2){
		defaultMus.stop();
		zeldaMus.play();
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
	floatSprite->changeAnimation(type);
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



