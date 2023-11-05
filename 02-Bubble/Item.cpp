#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Item.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 3
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
	WALK, DEATH, WALK_RIGHT, WALK_LEFT, IN_SHELL, STAR_DEATH
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
	starDeathAnim = false;
	koopaShell = false;
	typeItem = type;
	vel = glm::vec2((type == STAR ? 1.0f : 0.5f), 0.f);

	if (typeItem == GOOMBA || typeItem == KOOPA) vel.x = -0.5f;

	// TYPE 0: Goomba
	switch (type){
		case GOOMBA:
			spritesheet.loadFromFile("images/enemies-small.png", TEXTURE_PIXEL_FORMAT_RGBA);
			sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
			sprite->setNumberAnimations(6);

			sprite->setAnimationSpeed(WALK, 4);
			sprite->addKeyframe(WALK, glm::vec2(0.f, 0.f));
			sprite->addKeyframe(WALK, glm::vec2(0.25f, 0.f));

			sprite->setAnimationSpeed(DEATH, 1);
			sprite->addKeyframe(DEATH, glm::vec2(0.5f, 0.f));

			sprite->setAnimationSpeed(STAR_DEATH, 1);
			sprite->addKeyframe(STAR_DEATH, glm::vec2(0.f, 0.75f));

			sprite->changeAnimation(WALK, false);
			tileMapDispl = tileMapPos;
			sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));

			spawned = false;
			break;

		case KOOPA:
			spritesheet.loadFromFile("images/koopa.png", TEXTURE_PIXEL_FORMAT_RGBA);
			sprite = Sprite::createSprite(glm::ivec2(16, 24), glm::vec2(0.25, 0.5), &spritesheet, &shaderProgram);
			sprite->setNumberAnimations(6);

			sprite->setAnimationSpeed(WALK_RIGHT, 4);
			sprite->addKeyframe(WALK_RIGHT, glm::vec2(0.f, 0.5f));
			sprite->addKeyframe(WALK_RIGHT, glm::vec2(0.25f, 0.5f));
			
			sprite->setAnimationSpeed(WALK_LEFT, 4);
			sprite->addKeyframe(WALK_LEFT, glm::vec2(0.f, 0.f));
			sprite->addKeyframe(WALK_LEFT, glm::vec2(0.25f, 0.f));

			sprite->setAnimationSpeed(IN_SHELL, 1);
			sprite->addKeyframe(IN_SHELL, glm::vec2(0.5f, 0.f));

			sprite->setAnimationSpeed(DEATH, 1);
			sprite->addKeyframe(DEATH, glm::vec2(0.5f, 0.5f));

			sprite->setAnimationSpeed(STAR_DEATH, 1);
			sprite->addKeyframe(STAR_DEATH, glm::vec2(0.5f, 0.5f));

			sprite->changeAnimation(WALK_LEFT, false);
			tileMapDispl = tileMapPos;
			sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));

			spawned = false;
			break;
		case MUSHROOM:
			spritesheet.loadFromFile("images/enemies-small.png", TEXTURE_PIXEL_FORMAT_RGBA);
			sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
			sprite->setNumberAnimations(1);

			sprite->setAnimationSpeed(WALK, 1);
			sprite->addKeyframe(WALK, glm::vec2(0.f, 0.25f));

			sprite->changeAnimation(WALK, false);
			tileMapDispl = tileMapPos;
			sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
			spawned = true;
			break;
		case STAR:
			spritesheet.loadFromFile("images/enemies-small.png", TEXTURE_PIXEL_FORMAT_RGBA);
			sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);
			sprite->setNumberAnimations(1);

			sprite->setAnimationSpeed(WALK, 1);
			sprite->addKeyframe(WALK, glm::vec2(0.25f, 0.25f));

			sprite->changeAnimation(WALK, false);
			tileMapDispl = tileMapPos;
			sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
			spawned = true;
			break;

	}
}

void Item::update(int deltaTime)
{
	if (deadAnimStart){
		deadAnimCounter -= deltaTime;
		if (deadAnimCounter < 0) itemKO = true; 

		if (starDeathAnim){
			// 800 is the initial deadAnimCounter timer

			posItem.x = deathPos.x - 0.05*(800 - deadAnimCounter);
			posItem.y = deathPos.y - 0.05*(800 - deadAnimCounter) + 0.0002*(800 - deadAnimCounter)*(800 - deadAnimCounter);// + 0.05*(deadAnimCounter - 800)*(deadAnimCounter - 800);


			sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
		}

		return;
	}

	sprite->update(deltaTime);


	// Check for collisions left-right
	int off = (koopaShell ? 10 : 0);
	int off2 = (typeItem==STAR ? 2 : 0);
	bool koopaBreak = koopaShell && vel.x != 0;

	if (typeItem != STAR) {
		if (map->collisionMoveRight(glm::vec2(posItem.x, posItem.y + off), glm::ivec2(ITEM_WIDTH, ITEM_HEIGHT - off), koopaBreak, NULL, NULL) ||
			map->collisionMoveLeft(glm::vec2(posItem.x, posItem.y + off), glm::ivec2(ITEM_WIDTH, ITEM_HEIGHT - off), koopaBreak)) {
			posItem.x -= vel.x;
			vel.x = -vel.x;

			if (vel.x < 0 && typeItem == KOOPA && !koopaShell) changeAnimation(WALK_LEFT);
			else if (typeItem == KOOPA && !koopaShell) changeAnimation(WALK_RIGHT);
		}
		posItem.y += FALL_STEP;
	}
	else {
		if (map->collisionMoveRight(glm::vec2(posItem.x, posItem.y + off + off2), glm::ivec2(ITEM_WIDTH, ITEM_HEIGHT - off - off2), koopaBreak, NULL, NULL)) {
			if (vel.x > 0) {
				if (typeItem == KOOPA) sprite->changeAnimation(WALK_LEFT, false);
				posItem.x -= vel.x;
				vel.x = -vel.x;
			}
		}
		else if (map->collisionMoveLeft(glm::vec2(posItem.x, posItem.y + off + off2), glm::ivec2(ITEM_WIDTH, ITEM_HEIGHT - off - off2), koopaBreak)) {
			if (vel.x < 0) {
				if (typeItem == KOOPA) sprite->changeAnimation(WALK_RIGHT, false);
				posItem.x -= vel.x;
				vel.x = -vel.x;
			}
		}
	}

	if (map->collisionMoveDown(posItem, glm::ivec2(ITEM_WIDTH, ITEM_HEIGHT+off2), &posItem.y, NULL, NULL)){
		if (typeItem == STAR) jump();
	}

	if (typeItem == STAR)
	{
		jumpAngle += JUMP_ANGLE_STEP;

		if (jumpAngle <= 180) vel.y = -2*(cos(3.14159f * jumpAngle / 180.f));
		
		if (map->collisionMoveUp(posItem+glm::vec2(0,-off2), glm::ivec2(16, 16), &posItem.y, false) && bJumping) {
			// UP COLLISION
			bJumping = false;
			jumpAngle = 180-jumpAngle;
		}
	}

	vel.y = glm::clamp(vel.y, -2.f, 2.f);

	posItem += vel;

	if (posItem.x < 0){
		posItem.x = 0;
		vel.x = -vel.x;	
	}
	if (posItem.y >= 240) itemKO = true; 

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
}

void Item::changeAnimation(int animId) {
	sprite->changeAnimation(animId, false);
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
	// Plays the star death animation
	if (typeItem == MUSHROOM || typeItem == STAR){
		itemKO = true;
		return;
	}

	deathPos = posItem;
	deadAnimStart = true;
	deadAnimCounter = 800;
	starDeathAnim = true;

	sprite->changeAnimation(STAR_DEATH, false);
}

void Item::stomp(const glm::ivec2& pos){
	switch (typeItem){
		case GOOMBA:
		deadAnimStart = true;
		deadAnimCounter = 400;
		sprite->changeAnimation(DEATH, false);
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

void Item::jump() {
	bJumping = true;
	jumpAngle = 0;
	startY = posItem.y;
}

bool Item::isDead(){ return itemKO; }
bool Item::hasDeathAnimStarted(){ return deadAnimStart || itemKO; }
bool Item::killsEnemies() { return typeItem == KOOPA && koopaShell && vel.x != 0; }
bool Item::isSpawned(){ return spawned; }

void Item::setSpawned(bool sp){ spawned = sp; }
glm::vec2 Item::getPosition(){ return posItem; }
int Item::getType(){ return typeItem; }

glm::vec2 Item::getSize(){ return glm::vec2(ITEM_WIDTH, ITEM_HEIGHT); }
