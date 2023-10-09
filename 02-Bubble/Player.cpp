#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4
#define MARIO_HEIGHT (bigMario ? 32 : 16)
#define MARIO_WIDTH 16

#define MIN_WALK_SPEED 0.4
#define MAX_WALK_SPEED 1.5
#define MAX_RUN_SPEED 2.1
#define WALK_ACELERATION 0.15
#define RUN_ACELERATION 0.3
#define RELEASE_DECELERATION 0.08

#define SHIFT_KEY 112

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, SKID_RIGHT, SKID_LEFT, JUMP_RIGHT, JUMP_LEFT
};


void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	bJumping = false;
	bigMario = false;
	bFalling = false;
	vel = glm::vec2(0.f, 0.f);

	spritesheet.loadFromFile("images/mario128.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.125, 0.125), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(8);

	sprite->setAnimationSpeed(STAND_LEFT, 8);
	sprite->addKeyframe(STAND_LEFT, glm::vec2(1.0 - 0.125, 0.f));

	sprite->setAnimationSpeed(STAND_RIGHT, 8);
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.f));

	sprite->setAnimationSpeed(MOVE_LEFT, 8);
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.625f, 0.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.f));

	sprite->setAnimationSpeed(MOVE_RIGHT, 8);
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125f, 0.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.f));

	sprite->setAnimationSpeed(SKID_RIGHT, 8);
	sprite->addKeyframe(SKID_RIGHT, glm::vec2(0.625f, 0.125f));

	sprite->setAnimationSpeed(SKID_LEFT, 8);
	sprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.125f));

	sprite->setAnimationSpeed(JUMP_RIGHT, 8);
	sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.125f));

	sprite->setAnimationSpeed(JUMP_LEFT, 8);
	sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.5f, 0.125f));


	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

}

void Player::update(int deltaTime)
{
	sprite->update(deltaTime);

	// Increment Velocities
	if (vel.x >= 0 and Game::instance().getSpecialKey(GLUT_KEY_RIGHT)){
		if (Game::instance().getSpecialKey(SHIFT_KEY)){
			// Running
			vel.x += RUN_ACELERATION;

			if (vel.x < MIN_WALK_SPEED) vel.x = MIN_WALK_SPEED;
			else if (vel.x > MAX_RUN_SPEED) vel.x = MAX_RUN_SPEED;
		} else {
			// Walking
			vel.x += WALK_ACELERATION;

			if (vel.x < MIN_WALK_SPEED) vel.x = MIN_WALK_SPEED;
			else if (vel.x > MAX_WALK_SPEED) vel.x = MAX_WALK_SPEED;

		}
	}
	else if (vel.x <= 0 and Game::instance().getSpecialKey(GLUT_KEY_LEFT)){

		if (not Game::instance().getSpecialKey(SHIFT_KEY)){
			// Walking acceleration
			vel.x -= WALK_ACELERATION;
			
			if (vel.x > -MIN_WALK_SPEED) vel.x = -MIN_WALK_SPEED;
			else if (vel.x < -MAX_WALK_SPEED) vel.x = -MAX_WALK_SPEED;

		} else {
			// Running acceleration
			vel.x -= RUN_ACELERATION;
			
			if (vel.x > -MIN_WALK_SPEED) vel.x = -MIN_WALK_SPEED;
			else if (vel.x < -MAX_RUN_SPEED) vel.x = -MAX_RUN_SPEED;

		}
	}
	else if (vel.x > 0){
		// Release deceleration

		vel.x -= RELEASE_DECELERATION;

		if (vel.x < MIN_WALK_SPEED){
			vel.x = 0;
			if (not bJumping) sprite->changeAnimation(STAND_RIGHT);
		}

		// Check for skidding
		if (Game::instance().getSpecialKey(GLUT_KEY_LEFT) and not bJumping and not bFalling){
			sprite->changeAnimation(SKID_RIGHT);
		}
	}
	else if (vel.x < 0){
		// Release deceleration

		vel.x += RELEASE_DECELERATION;

		if (vel.x > -MIN_WALK_SPEED){
			vel.x = 0;
			if (not bJumping) sprite->changeAnimation(STAND_LEFT);
		}

		if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT) and not bJumping and not bFalling){
			sprite->changeAnimation(SKID_LEFT);
		}
	}


	if (vel.x > 0 and not bJumping and not bFalling)
	{
		if (sprite->animation() != MOVE_RIGHT and sprite->animation() != SKID_RIGHT) sprite->changeAnimation(MOVE_RIGHT);

	} else if (vel.x < 0 and not bJumping and not bFalling)
	{
		if (sprite->animation() != MOVE_LEFT and sprite->animation() != SKID_LEFT) sprite->changeAnimation(MOVE_LEFT);

	}

	/*
	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		if (sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posPlayer.x -= 2;
		if (map->collisionMoveLeft(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT)))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{
		if (sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		posPlayer.x += 2;
		if (map->collisionMoveRight(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT)))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else
	{
		if (sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if (sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}
	*/
	if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;

		if (jumpAngle == 180)
		{
			bJumping = false;
			bFalling = true;
			posPlayer.y = startY;

			sprite->changeAnimation(vel.x >= 0 ? STAND_RIGHT : STAND_LEFT);
		}
		else
		{
			posPlayer.y = int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));

			if (map->collisionMoveUp(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT), &posPlayer.y) or jumpAngle > 90){
				// UP COLLISION
				bJumping = false;
				bFalling = true;
			}

		}
	}
	else
	{
		posPlayer.y += FALL_STEP;

		if (map->collisionMoveDown(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT), &posPlayer.y))
		{
			if (bFalling){
				bFalling = false;
				sprite->changeAnimation(vel.x >= 0 ? STAND_RIGHT : STAND_LEFT);
			}

			if (Game::instance().getSpecialKey(GLUT_KEY_UP))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
				sprite->changeAnimation(vel.x >= 0 ? JUMP_RIGHT : JUMP_LEFT);
			}
		}
	}

	posPlayer.x += vel.x;

	// Check for collisions left-right
	if (map->collisionMoveRight(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT)) or 
		map->collisionMoveLeft(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT))){
		posPlayer.x -= vel.x;
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	// Always invert
	sprite->render(false);
}

void Player::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2& pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}




