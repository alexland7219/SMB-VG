#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Item.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 69
#define FALL_STEP 3
#define ITEM_HEIGHT (typeItem == KOOPA ? 24 : 16)
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
	WALK, DEATH, WALK_RIGHT, WALK_LEFT, IN_SHELL
};

enum ItemTypes
{
	GOOMBA, KOOPA, MUSHROOM, STAR
};

void Item::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int type)
{
	bJumping = false;
	bFalling = false;
	itemKO = false;
	koopaShell = false;
	typeItem = type;
	vel = glm::vec2(0.5f, 0.f);

	// TYPE 0: Goomba
	switch (type){
		case GOOMBA:
			spritesheet.loadFromFile("images/enemies-small.png", TEXTURE_PIXEL_FORMAT_RGBA);
			sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
			sprite->setNumberAnimations(2);

			sprite->setAnimationSpeed(WALK, 4);
			sprite->addKeyframe(WALK, glm::vec2(0.f, 0.f));
			sprite->addKeyframe(WALK, glm::vec2(0.25f, 0.f));

			sprite->setAnimationSpeed(DEATH, 1);
			sprite->addKeyframe(DEATH, glm::vec2(0.5f, 0.f));

			sprite->changeAnimation(0);
			tileMapDispl = tileMapPos;
			sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
			break;

		case KOOPA:
			spritesheet.loadFromFile("images/koopa.png", TEXTURE_PIXEL_FORMAT_RGBA);
			sprite = Sprite::createSprite(glm::ivec2(16, 24), glm::vec2(0.25, 0.5), &spritesheet, &shaderProgram);
			sprite->setNumberAnimations(5);

			sprite->setAnimationSpeed(WALK_RIGHT, 4);
			sprite->addKeyframe(WALK_RIGHT, glm::vec2(0.f, 0.5f));
			sprite->addKeyframe(WALK_RIGHT, glm::vec2(0.25f, 0.5f));
			
			sprite->setAnimationSpeed(WALK_LEFT, 4);
			sprite->addKeyframe(WALK_LEFT, glm::vec2(0.f, 0.f));
			sprite->addKeyframe(WALK_LEFT, glm::vec2(0.25f, 0.f));

			sprite->setAnimationSpeed(IN_SHELL, 1);
			sprite->addKeyframe(IN_SHELL, glm::vec2(0.5f, 0.f));

			sprite->changeAnimation(WALK_RIGHT);
			tileMapDispl = tileMapPos;
			sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
			break;
		case MUSHROOM:
			spritesheet.loadFromFile("images/enemies-small.png", TEXTURE_PIXEL_FORMAT_RGBA);
			sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
			sprite->setNumberAnimations(1);

			sprite->setAnimationSpeed(WALK, 1);
			sprite->addKeyframe(WALK, glm::vec2(0.f, 0.25f));

			sprite->changeAnimation(WALK);
			tileMapDispl = tileMapPos;
			sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
			break;
	}
}

void Item::update(int deltaTime)
{
	if (deadAnimStart){
		deadAnimCounter -= deltaTime;
		if (deadAnimCounter < 0) itemKO = true; 
		return;
	}

	sprite->update(deltaTime);

	posItem.x += vel.x;


	// Check for collisions left-right
	int off = (koopaShell ? 10 : 0);
	bool koopaBreak = koopaShell && vel.x != 0;

	if (map->collisionMoveRight(glm::vec2(posItem.x, posItem.y + off), glm::ivec2(ITEM_WIDTH, ITEM_HEIGHT - off), koopaBreak, NULL, NULL) ||
		map->collisionMoveLeft(glm::vec2(posItem.x, posItem.y + off), glm::ivec2(ITEM_WIDTH, ITEM_HEIGHT - off), koopaBreak)) {
		posItem.x -= vel.x;
		vel.x = -vel.x;

		if (vel.x < 0 && typeItem == KOOPA && !koopaShell) changeAnimation(WALK_LEFT);
		else if (typeItem == KOOPA && !koopaShell) changeAnimation(WALK_RIGHT);
	}

	posItem.y += FALL_STEP;

	if (map->collisionMoveDown(posItem, glm::ivec2(ITEM_WIDTH, ITEM_HEIGHT), &posItem.y, NULL, NULL)){

	}

	if (posItem.x < 0) posItem.x = 0;
	if (posItem.y >= 240) itemKO = true; 

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

void Item::invertXVelocity(){
	vel.x = -vel.x;
	if (vel.x < 0 && typeItem == KOOPA && !koopaShell) changeAnimation(WALK_LEFT);
	else if (typeItem == KOOPA && !koopaShell) changeAnimation(WALK_RIGHT);

}

void Item::setPosition(const glm::vec2& pos)
{
	posItem = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
}

bool Item::collisionStomped(const glm::ivec2& pos, const glm::ivec2 size){
	if (deadAnimStart) return false;

	int off = (typeItem == KOOPA && koopaShell ? 10 : 0);

	//bool above = (pos.y + (size.y - 16) < posItem.y + off && pos.y + (size.y - 16) > posItem.y - ITEM_HEIGHT + off);
	bool above = (pos.y + size.y >= posItem.y + off && pos.y + size.y < posItem.y + off + 5);
	bool intersect = ((pos.x > posItem.x && pos.x < posItem.x + ITEM_WIDTH) || (pos.x + size.x > posItem.x && pos.x + size.x < posItem.x + ITEM_WIDTH));

	return above && intersect;
}

bool Item::collisionKill(const glm::ivec2& pos, const glm::ivec2 size){
	if (deadAnimStart) return false;

	int off = (typeItem == KOOPA && koopaShell ? 10 : 0);

	bool intersect = ((pos.x > posItem.x && pos.x < posItem.x + ITEM_WIDTH) || (pos.x + size.x > posItem.x && pos.x + size.x < posItem.x + ITEM_WIDTH));
	bool y_collide = (pos.y + size.y >= posItem.y + off + 3 && pos.y <= posItem.y + off + ITEM_HEIGHT);

	if (typeItem == KOOPA && koopaShell && vel.x == 0 && intersect && y_collide){
		stomp(pos);
		return false;
	}


	return y_collide && intersect;
}

void Item::die() {
	if (typeItem == MUSHROOM){
		itemKO = true;
		return;
	}

	deadAnimStart = true;
	deadAnimCounter = 400;

	sprite->changeAnimation(DEATH);
}

void Item::stomp(const glm::ivec2& pos){
	switch (typeItem){
		case GOOMBA:
		deadAnimStart = true;
		deadAnimCounter = 400;
		sprite->changeAnimation(DEATH);
		break;

		case KOOPA:
		if (!koopaShell){
			vel.x = 0;
			posItem.y += 0;
			changeAnimation(IN_SHELL);
			koopaShell = true;
		}
		else if (koopaShell) {
			if (vel.x == 0) {
				if (pos.x > posItem.x) vel.x = -3.5f;
				else vel.x = +3.5f;
			}
			else vel.x = 0;
		}
	}

}

bool Item::isDead(){ return itemKO; }
bool Item::killsEnemies() { return typeItem == KOOPA && koopaShell && vel.x != 0; }

glm::vec2 Item::getPosition(){ return posItem; }
int Item::getType(){ return typeItem; }

glm::vec2 Item::getSize(){ return glm::vec2(ITEM_WIDTH, ITEM_HEIGHT); }
