#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 69
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
#define SPACE_KEY 32

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, SKID_RIGHT, SKID_LEFT, JUMP_RIGHT, JUMP_LEFT
};


void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	jumpMus.openFromFile("audio/jump.ogg");

	bJumping = false;
	bigMario = false;
	bFalling = false;
	allowChangeTimer = 0;
	vel = glm::vec2(0.f, 0.f);

	// Small Mario
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


	// Big Mario
	spritesheetBig.loadFromFile("images/big-mario.png", TEXTURE_PIXEL_FORMAT_RGBA);
	bigSprite = Sprite::createSprite(glm::ivec2(16, 32), glm::vec2(0.125, 0.25), &spritesheetBig, &shaderProgram);
	bigSprite->setNumberAnimations(8);

	bigSprite->setAnimationSpeed(STAND_LEFT, 8);
	bigSprite->addKeyframe(STAND_LEFT, glm::vec2(1.0 - 0.125, 0.f));

	bigSprite->setAnimationSpeed(STAND_RIGHT, 8);
	bigSprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.f));

	bigSprite->setAnimationSpeed(MOVE_LEFT, 8);
	bigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.f));
	bigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.625f, 0.f));
	bigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.f));

	bigSprite->setAnimationSpeed(MOVE_RIGHT, 8);
	bigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125f, 0.f));
	bigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.f));
	bigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.f));

	bigSprite->setAnimationSpeed(SKID_RIGHT, 8);
	bigSprite->addKeyframe(SKID_RIGHT, glm::vec2(0.375f, 0.25f));

	bigSprite->setAnimationSpeed(SKID_LEFT, 8);
	bigSprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.25f));

	bigSprite->setAnimationSpeed(JUMP_RIGHT, 8);
	bigSprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.25f));

	bigSprite->setAnimationSpeed(JUMP_LEFT, 8);
	bigSprite->addKeyframe(JUMP_LEFT, glm::vec2(0.25f, 0.25f));


	bigSprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	bigSprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

}

void Player::update(int deltaTime)
{
	if (!bigMario) sprite->update(deltaTime);
	else bigSprite->update(deltaTime);

	// change to big mario
	if (allowChangeTimer <= 0 && (Game::instance().getKey('m') || Game::instance().getKey('M'))){
		bigMario = !bigMario;

		if (bigMario) posPlayer.y -= 16;

		// Wait for .5 second before pressing 'M'
		allowChangeTimer = 500; 
	}

	allowChangeTimer -= deltaTime;

	// Increment Velocities
	if (vel.x >= 0 && Game::instance().getSpecialKey(GLUT_KEY_RIGHT)){
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
	else if (vel.x <= 0 && Game::instance().getSpecialKey(GLUT_KEY_LEFT)){

		if (!Game::instance().getSpecialKey(SHIFT_KEY)){
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
			if (!bJumping && !bFalling) changeAnimation(STAND_RIGHT);
		}

		// Check for skidding
		if (Game::instance().getSpecialKey(GLUT_KEY_LEFT) && !bJumping && !bFalling){
			changeAnimation(SKID_RIGHT);
		}
	}
	else if (vel.x < 0){
		// Release deceleration

		vel.x += RELEASE_DECELERATION;

		if (vel.x > -MIN_WALK_SPEED){
			vel.x = 0;
			if (!bJumping && !bFalling) changeAnimation(STAND_LEFT);
		}

		if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT) && !bJumping && !bFalling){
			changeAnimation(SKID_LEFT);
		}
	}


	if (vel.x > 0 && !bJumping && !bFalling)
	{
		if (sprite->animation() != MOVE_RIGHT && sprite->animation() != SKID_RIGHT) changeAnimation(MOVE_RIGHT);

	} else if (vel.x < 0 && !bJumping && !bFalling)
	{
		if (sprite->animation() != MOVE_LEFT && sprite->animation() != SKID_LEFT) changeAnimation(MOVE_LEFT);

	}


	if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;

		if (jumpAngle == 180)
		{
			bJumping = false;
			bFalling = true;
			posPlayer.y = startY;

			//changeAnimation(vel.x >= 0 ? STAND_RIGHT : STAND_LEFT);
		}
		else
		{
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));

			if (map->collisionMoveUp(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT), &posPlayer.y, bigMario) || jumpAngle > 90){
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
				changeAnimation(vel.x >= 0 ? STAND_RIGHT : STAND_LEFT);
			}

			if (Game::instance().getKey(SPACE_KEY) || Game::instance().getSpecialKey(GLUT_KEY_UP))
			{
				if (allowChangeTimer <= 0) {
					// Jump
					jumpMus.play();
        			jumpMus.setPlayingOffset(sf::Time::Zero);

					bJumping = true;
					jumpAngle = 0;
					startY = posPlayer.y;
					changeAnimation(vel.x >= 0 ? JUMP_RIGHT : JUMP_LEFT);
					allowChangeTimer = 200;
				}
			}
		}
	}

	posPlayer.x += vel.x;


	// Check for collisions left-right
	if (map->collisionMoveRight(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT)) || 
		map->collisionMoveLeft(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT))){
		posPlayer.x -= vel.x;
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	bigSprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::changeAnimation(int animId){
	sprite->changeAnimation(animId);
	bigSprite->changeAnimation(animId);
}

void Player::render()
{
	if (!bigMario) sprite->render(false);
	else bigSprite->render(false);
}

void Player::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2& pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}




