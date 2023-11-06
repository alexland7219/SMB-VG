#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Sound.h"
#include "Game.h"

#define JUMP_ANGLE_STEP 4
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
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, SKID_RIGHT, SKID_LEFT, JUMP_RIGHT, JUMP_LEFT, DEATH, FLAGPOLE,
	MID_LEFT, MID_RIGHT
};


void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{	
	bJumping = false;
	bigMario = false;
	starMario = false;
	powerDownAnim = false;
	bFalling = false;
	gameOver = false;
	allowChangeTimer = 0;
	invincibleCounter = 0;
	vel = glm::vec2(0.f, 0.f);
	jumpHeight = 69;
	starTime = 0;
	midTimer = -1;

	// Small Mario

	spritesheet.loadFromFile("images/mario128-s.png", TEXTURE_PIXEL_FORMAT_RGBA);
	spritesheetBig.loadFromFile("images/big-mario-s.png", TEXTURE_PIXEL_FORMAT_RGBA);

	sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.125, 0.125), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(10);

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

	sprite->setAnimationSpeed(DEATH, 1);
	sprite->addKeyframe(DEATH, glm::vec2(0.25f, 0.125f));

	sprite->setAnimationSpeed(FLAGPOLE, 1);
	sprite->addKeyframe(FLAGPOLE, glm::vec2(0.75f, 0.125f));

	sprite->changeAnimation(STAND_RIGHT, false);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));


	// Big Mario
	bigSprite = Sprite::createSprite(glm::ivec2(16, 32), glm::vec2(0.125, 0.125), &spritesheetBig, &shaderProgram);
	bigSprite->setNumberAnimations(12);

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
	bigSprite->addKeyframe(SKID_RIGHT, glm::vec2(0.375f, 0.125f));

	bigSprite->setAnimationSpeed(SKID_LEFT, 8);
	bigSprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.125f));

	bigSprite->setAnimationSpeed(JUMP_RIGHT, 8);
	bigSprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.125f));

	bigSprite->setAnimationSpeed(JUMP_LEFT, 8);
	bigSprite->addKeyframe(JUMP_LEFT, glm::vec2(0.25f, 0.125f));

	bigSprite->setAnimationSpeed(MID_RIGHT, 1);
	bigSprite->addKeyframe(MID_RIGHT, glm::vec2(0.5f, 0.125f));

	bigSprite->setAnimationSpeed(MID_LEFT, 1);
	bigSprite->addKeyframe(MID_LEFT, glm::vec2(0.625f, 0.125f));

	bigSprite->changeAnimation(STAND_RIGHT, false);
	tileMapDispl = tileMapPos;
	bigSprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

	// Star Mario

	//spritesheet.loadFromFile("images/mario128-s.png", TEXTURE_PIXEL_FORMAT_RGBA);
	starSprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.125, 0.125), &spritesheet, &shaderProgram);
	starSprite->setNumberAnimations(9);

	starSprite->setAnimationSpeed(STAND_LEFT, 8);
	starSprite->addKeyframe(STAND_LEFT, glm::vec2(1.0 - 0.125, 0.f));
	starSprite->addKeyframe(STAND_LEFT, glm::vec2(1.0 - 0.125, 0.25f));
	starSprite->addKeyframe(STAND_LEFT, glm::vec2(1.0 - 0.125, 0.5f));
	starSprite->addKeyframe(STAND_LEFT, glm::vec2(1.0 - 0.125, 0.75f));

	starSprite->setAnimationSpeed(STAND_RIGHT, 8);
	starSprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.f));
	starSprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.25f));
	starSprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.5f));
	starSprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.75f));

	starSprite->setAnimationSpeed(MOVE_LEFT, 8);
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.625f, 0.25f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.5f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.75f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.625f, 0.f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.25f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.5f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.625f, 0.75f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.25f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.625f, 0.5f));
	starSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.75f));

	starSprite->setAnimationSpeed(MOVE_RIGHT, 8);
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125f, 0.f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.25f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.5f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125f, 0.75f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.25f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125f, 0.5f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.75f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125f, 0.25f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.5f));
	starSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.75f));

	starSprite->setAnimationSpeed(SKID_RIGHT, 8);
	starSprite->addKeyframe(SKID_RIGHT, glm::vec2(0.625f, 0.125f));
	starSprite->addKeyframe(SKID_RIGHT, glm::vec2(0.625f, 0.375f));
	starSprite->addKeyframe(SKID_RIGHT, glm::vec2(0.625f, 0.625f));
	starSprite->addKeyframe(SKID_RIGHT, glm::vec2(0.625f, 0.875f));

	starSprite->setAnimationSpeed(SKID_LEFT, 8);
	starSprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.125f));
	starSprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.375f));
	starSprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.625f));
	starSprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.875f));

	starSprite->setAnimationSpeed(JUMP_RIGHT, 8);
	starSprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.125f));
	starSprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.375f));
	starSprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.625f));
	starSprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.875f));

	starSprite->setAnimationSpeed(JUMP_LEFT, 8);
	starSprite->addKeyframe(JUMP_LEFT, glm::vec2(0.5f, 0.125f));
	starSprite->addKeyframe(JUMP_LEFT, glm::vec2(0.5f, 0.375f));
	starSprite->addKeyframe(JUMP_LEFT, glm::vec2(0.5f, 0.625f));
	starSprite->addKeyframe(JUMP_LEFT, glm::vec2(0.5f, 0.875f));

	starSprite->setAnimationSpeed(FLAGPOLE, 1);
	starSprite->addKeyframe(FLAGPOLE, glm::vec2(0.75f, 0.125f));
	starSprite->addKeyframe(FLAGPOLE, glm::vec2(0.75f, 0.375f));
	starSprite->addKeyframe(FLAGPOLE, glm::vec2(0.75f, 0.625f));
	starSprite->addKeyframe(FLAGPOLE, glm::vec2(0.75f, 0.875f));

	starSprite->changeAnimation(STAND_RIGHT, false);
	tileMapDispl = tileMapPos;
	starSprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

	// Star Big Mario

	spritesheetBig.loadFromFile("images/big-mario-s.png", TEXTURE_PIXEL_FORMAT_RGBA);
	starbigSprite = Sprite::createSprite(glm::ivec2(16, 32), glm::vec2(0.125, 0.125), &spritesheetBig, &shaderProgram);
	starbigSprite->setNumberAnimations(12);

	starbigSprite->setAnimationSpeed(STAND_LEFT, 8);
	starbigSprite->addKeyframe(STAND_LEFT, glm::vec2(1.0 - 0.125, 0.f));
	starbigSprite->addKeyframe(STAND_LEFT, glm::vec2(1.0 - 0.125, 0.25f));
	starbigSprite->addKeyframe(STAND_LEFT, glm::vec2(1.0 - 0.125, 0.5f));
	starbigSprite->addKeyframe(STAND_LEFT, glm::vec2(1.0 - 0.125, 0.75f));

	starbigSprite->setAnimationSpeed(STAND_RIGHT, 8);
	starbigSprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.f));
	starbigSprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.25f));
	starbigSprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.5f));
	starbigSprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.75f));

	starbigSprite->setAnimationSpeed(MOVE_LEFT, 8);
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.625f, 0.25f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.5f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.75f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.625f, 0.f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.25f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.5f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.625f, 0.75f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.75f, 0.25f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.625f, 0.5f));
	starbigSprite->addKeyframe(MOVE_LEFT, glm::vec2(0.5f, 0.75f));

	starbigSprite->setAnimationSpeed(MOVE_RIGHT, 8);
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125f, 0.f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.25f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.5f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125f, 0.75f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.25f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125f, 0.5f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.75f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125f, 0.25f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.25f, 0.5f));
	starbigSprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.75f));

	starbigSprite->setAnimationSpeed(SKID_RIGHT, 8);
	starbigSprite->addKeyframe(SKID_RIGHT, glm::vec2(0.375f, 0.125f));
	starbigSprite->addKeyframe(SKID_RIGHT, glm::vec2(0.375f, 0.375f));
	starbigSprite->addKeyframe(SKID_RIGHT, glm::vec2(0.375f, 0.625f));
	starbigSprite->addKeyframe(SKID_RIGHT, glm::vec2(0.375f, 0.875f));

	starbigSprite->setAnimationSpeed(SKID_LEFT, 8);
	starbigSprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.125f));
	starbigSprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.375f));
	starbigSprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.625f));
	starbigSprite->addKeyframe(SKID_LEFT, glm::vec2(0.f, 0.875f));

	starbigSprite->setAnimationSpeed(JUMP_RIGHT, 8);
	starbigSprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.125f));
	starbigSprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.375f));
	starbigSprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.625f));
	starbigSprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.125f, 0.875f));

	starbigSprite->setAnimationSpeed(JUMP_LEFT, 8);
	starbigSprite->addKeyframe(JUMP_LEFT, glm::vec2(0.25f, 0.125f));
	starbigSprite->addKeyframe(JUMP_LEFT, glm::vec2(0.25f, 0.375f));
	starbigSprite->addKeyframe(JUMP_LEFT, glm::vec2(0.25f, 0.625f));
	starbigSprite->addKeyframe(JUMP_LEFT, glm::vec2(0.25f, 0.875f));

	starbigSprite->setAnimationSpeed(FLAGPOLE, 1);
	starbigSprite->addKeyframe(FLAGPOLE, glm::vec2(0.75f, 0.125f));

	starbigSprite->setAnimationSpeed(MID_RIGHT, 4);
	starbigSprite->addKeyframe(MID_RIGHT, glm::vec2(0.5f, 0.125f));
	starbigSprite->addKeyframe(MID_RIGHT, glm::vec2(0.5f, 0.375f));
	starbigSprite->addKeyframe(MID_RIGHT, glm::vec2(0.5f, 0.625f));
	starbigSprite->addKeyframe(MID_RIGHT, glm::vec2(0.5f, 0.875f));

	starbigSprite->setAnimationSpeed(MID_LEFT, 4);
	starbigSprite->addKeyframe(MID_LEFT, glm::vec2(0.625f, 0.125f));
	starbigSprite->addKeyframe(MID_LEFT, glm::vec2(0.625f, 0.375f));
	starbigSprite->addKeyframe(MID_LEFT, glm::vec2(0.625f, 0.625f));
	starbigSprite->addKeyframe(MID_LEFT, glm::vec2(0.625f, 0.875f));


	starbigSprite->changeAnimation(STAND_RIGHT, false);
	tileMapDispl = tileMapPos;
	starbigSprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

}

void Player::update(int deltaTime)
{
	if (starMario) {
		starTime -= deltaTime;
		if (starTime <= 0) {
			Sound::instance().stop(5); // Stop Star Music
			starMario = !starMario;
		}
	}
	if (deadAnimStart){
		deadAnimCounter -= deltaTime;

		if (deadAnimCounter < 2300) posPlayer.y += 1;
		else if (deadAnimCounter < 2700) posPlayer.y -= 1;
		
		sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

		if (posPlayer.y > 256) posPlayer.y -= 1;
		if (posPlayer.y >= 256 && deadAnimCounter < 0) gameOver = true;

		return;
	}

	if (midTimer >= 0){
		midTimer -= deltaTime;

		if (midTimer < 0)
			changeAnimation(vel.x >= 0 ? STAND_RIGHT : STAND_LEFT);

		if (midTimer < 0 && powerDownAnim){
			powerDownAnim = false;
			bigMario = false;
			posPlayer.y += 16;
		}
	}

	else if (flagpoleAnimStart){

		flagpoleAnimCounter += deltaTime;
		if (flagpoleAnimCounter > 500 && !flagpoleTouchdown) posPlayer.y += 2;
		sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
		sprite->update(deltaTime);

		if (flagpoleTouchdown) posPlayer.x += 1.3;

		if (map->collisionMoveDown(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT), &posPlayer.y, NULL, NULL) && !flagpoleTouchdown){
			sprite->changeAnimation(MOVE_RIGHT, false);
			// End of animation
			flagpoleTouchdown = true;
		}

		//if (posPlayer.x > ) exit(0);
		// if (flagpoleAnimCounter > 5000) exit(0);

		return;
	}
	if (starMario) {
		if (!bigMario) starSprite->update(deltaTime);
		else starbigSprite->update(deltaTime);
	}
	else {
		if (!bigMario) sprite->update(deltaTime);
		else bigSprite->update(deltaTime);
	}

	if (invincibleCounter > 0) invincibleCounter -= deltaTime;

	// change to big mario
	if (allowChangeTimer <= 0 && (Game::instance().getKey('m') || Game::instance().getKey('M'))){

		if (!bigMario){ // Power up
			bigMario = true;
			posPlayer.y -= 16;
		} 
		else {          // Poweer down
			powerDownAnim = true;
		}

		changeAnimation(vel.x >= 0 ? MID_RIGHT : MID_LEFT);
		midTimer = 150;
		// Wait for .5 second before pressing 'M'
		allowChangeTimer = 200; 
	}

	// Change to star mario
	if (allowChangeTimer <= 0 && (Game::instance().getKey('s') || Game::instance().getKey('S'))) {
		starMario = !starMario;
		if (starMario) {
			starTime = 12800;
			Sound::instance().play(5); // Play star music
		}
		else {
			Sound::instance().stop(5); // Stop only star music
		}
		//if (starMario && !bigMario) posPlayer.y -= 16;

		// Wait for .5 second before pressing 'S'
		allowChangeTimer = 500;
	}

	allowChangeTimer -= deltaTime;

	// Increment Velocities
	if (vel.x >= 0 && Game::instance().getSpecialKey(GLUT_KEY_RIGHT)){
		if (Game::instance().getSpecialKey(SHIFT_KEY) || starMario){
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

		if (!Game::instance().getSpecialKey(SHIFT_KEY) && !starMario){
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
			if (!bJumping && !bFalling && midTimer < 0) changeAnimation(STAND_RIGHT);
		}

		// Check for skidding
		if (Game::instance().getSpecialKey(GLUT_KEY_LEFT) && !bJumping && !bFalling && midTimer < 0){
			if (sprite->animation() != SKID_RIGHT) changeAnimation(SKID_RIGHT);
		}
	}
	else if (vel.x < 0){
		// Release deceleration

		vel.x += RELEASE_DECELERATION;

		if (vel.x > -MIN_WALK_SPEED){
			vel.x = 0;
			if (!bJumping && !bFalling && midTimer < 0) changeAnimation(STAND_LEFT);
		}

		if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT) && !bJumping && !bFalling && midTimer < 0){
			if (sprite->animation() != SKID_LEFT) changeAnimation(SKID_LEFT);
		}
	}


	if (vel.x > 0 && !bJumping && !bFalling)
	{
		if (sprite->animation() != MOVE_RIGHT && sprite->animation() != SKID_RIGHT && midTimer < 0) changeAnimation(MOVE_RIGHT);

	} else if (vel.x < 0 && !bJumping && !bFalling)
	{
		if (sprite->animation() != MOVE_LEFT && sprite->animation() != SKID_LEFT && midTimer < 0) changeAnimation(MOVE_LEFT);
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
			posPlayer.y = int(startY - jumpHeight * sin(3.14159f * jumpAngle / 180.f));

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
		bool flagpole = false;
		int flagX = 0;

		if (map->collisionMoveDown(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT), &posPlayer.y, &flagpole, &flagX))
		{
			if (bFalling){
				bFalling = false;
				if (midTimer < 0) changeAnimation(vel.x >= 0 ? STAND_RIGHT : STAND_LEFT);
			}

			if (Game::instance().getKey(SPACE_KEY) || Game::instance().getSpecialKey(GLUT_KEY_UP))
			{
				if (allowChangeTimer <= 0) {
					// Jump
					Sound::instance().play(0); // Play jump music

					jump(60);
				}
			}
		} else if (flagpole){
			flagpoleAnimStart = true;
			if (starMario) {
				starMario = false;
				Sound::instance().stop(5); // stop star music
			}
			flagpoleTouchdown = false;
			flagpoleAnimCounter = 0;
			changeAnimation(FLAGPOLE);
			vel.x = 0;
			vel.y = 0;
			posPlayer.x = flagX;

			if (bigMario){
				bigMario = false;
				posPlayer.y += 16;
			}
		}
	}

	posPlayer.x += vel.x;

	if (posPlayer.y > 240 && !deadAnimStart){
		// Fall, immediate death
		bigMario = false;
		deadAnimStart = true;
		deadAnimCounter = 3000;
		changeAnimation(DEATH);

		Sound::instance().stop(5); // Stop Star Music
		Sound::instance().play(1); // Start death musics
	}

	// Check for collisions left-right
	bool flagpole = false;
	int flagX = 0;

	if (map->collisionMoveRight(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT - 8), false, &flagpole, &flagX) || 
		map->collisionMoveLeft(posPlayer, glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT - 8), false)){
		posPlayer.x -= vel.x;
	} else if (flagpole){

		flagpoleAnimStart = true;
		flagpoleTouchdown = false;
		flagpoleAnimCounter = 0;
		starMario = false;
		changeAnimation(FLAGPOLE);
		vel.x = 0;
		vel.y = 0;
		posPlayer.x = flagX;

		if (bigMario){
				bigMario = false;
				posPlayer.y += 16;
			}

	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	bigSprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	starbigSprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	starSprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::changeAnimation(int animId){
	if (animId != MID_LEFT && animId != MID_RIGHT) sprite->changeAnimation(animId, starMario);

	if (animId != DEATH && animId != FLAGPOLE) bigSprite->changeAnimation(animId, starMario);

	if (animId != DEATH) {
		starbigSprite->changeAnimation(animId, starMario);
		if (animId != MID_LEFT && animId != MID_RIGHT) starSprite->changeAnimation(animId, starMario);
	}
}

void Player::render()
{
	if (starMario) {
		if (!bigMario) starSprite->render(false);
		else starbigSprite->render(false);
	}
	else {
		if (!bigMario) sprite->render(false);
		else bigSprite->render(false);
	}
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

void Player::die(){
	if (deadAnimStart) return; // Cannot die twice
	if (invincibleCounter > 0) return;
	if (starMario) return;

	if (bigMario) { // Power DOWN
		Sound::instance().play(11); // Damage sound

		powerDownAnim = true;
		changeAnimation(vel.x >= 0 ? MID_RIGHT : MID_LEFT);
		midTimer = 150;
		//posPlayer.y += 16;
		invincibleCounter = 1000;
		return;
	}
	// Initiate animation to die
	bigMario = false;
	deadAnimStart = true;
	deadAnimCounter = 3000;
	changeAnimation(DEATH);

	Sound::instance().stop(5); // Stop Star Music
	Sound::instance().play(1); // Start death musics

}

void Player::jump(int height){
	jumpHeight = height;

	bJumping = true;
	jumpAngle = 0;
	startY = posPlayer.y;
	changeAnimation(vel.x >= 0 ? JUMP_RIGHT : JUMP_LEFT);
	allowChangeTimer = 200;

}

void Player::mushroom(){
	if (deadAnimStart) return;

	bigMario = true;
	posPlayer.y -= 16;
	allowChangeTimer = 150; 

	changeAnimation(vel.x >= 0 ? MID_RIGHT : MID_LEFT);
	midTimer = 150;
}

void Player::star(){
	if (deadAnimStart) return;

	starMario = true;
	starTime = 12800;
	Sound::instance().play(5); // Play star
}

void Player::flagpole(glm::vec2 posPole){
	Sound::instance().stop(5); // Stop star
	starMario = false;

	setPosition(posPole);

	if (bigMario){
		bigMario = false;
		posPlayer.y += 16;
	}

	changeAnimation(FLAGPOLE);
}

bool Player::isDead(){ return gameOver; }
bool Player::won(){ return flagpoleAnimStart && (flagpoleAnimCounter > 7000); }
bool Player::hasWinningAnimStarted(){ return flagpoleAnimStart; }
bool Player::hasDeathAnimStarted(){ return deadAnimStart; }
glm::vec2 Player::getPosition(){ return posPlayer; }
glm::vec2 Player::getSize(){ return glm::ivec2(MARIO_WIDTH, MARIO_HEIGHT); }
bool Player::getStar() { return starMario; }
