#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Item.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 69
#define FALL_STEP 4
#define ITEM_HEIGHT 16
#define ITEM_WIDTH 16

#define MIN_WALK_SPEED 0.4
#define MAX_WALK_SPEED 1.5
#define MAX_RUN_SPEED 2.1
#define WALK_ACELERATION 0.15
#define RUN_ACELERATION 0.3
#define RELEASE_DECELERATION 0.08

#define SHIFT_KEY 112
#define SPACE_KEY 32

enum ItemAnims
{
	WALK
};


void Item::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int type)
{
	bJumping = false;
	bFalling = false;
	vel = glm::vec2(0.f, 0.f);

	// TYPE 0: Goomba
	spritesheet.loadFromFile("images/enemies-small.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(1);

	sprite->setAnimationSpeed(WALK, 8);
	sprite->addKeyframe(WALK, glm::vec2(0.f, 0.f));
	sprite->addKeyframe(WALK, glm::vec2(0.25f, 0.f));

	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
}

void Item::update(int deltaTime)
{
	sprite->update(deltaTime);


	posItem.x += 4;


	// Check for collisions left-right
	if (map->collisionMoveRight(posItem, glm::ivec2(ITEM_WIDTH, ITEM_HEIGHT)) ||
		map->collisionMoveLeft(posItem, glm::ivec2(ITEM_WIDTH, ITEM_HEIGHT))) {
		posItem.x -= vel.x;
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
}

void Item::changeAnimation(int animId) {
	sprite->changeAnimation(animId);
}

void Item::render()
{
	sprite->render(false);
}

void Item::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Item::setPosition(const glm::vec2& pos)
{
	posItem = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
}




