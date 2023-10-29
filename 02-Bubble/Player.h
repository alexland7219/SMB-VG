#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include <SFML/Audio.hpp>

// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render();

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);
	void changeAnimation(int animId);
	bool isDead();
	bool won();
	bool hasDeathAnimStarted();
	void die();
	void jump(int height);
	void mushroom();
	void flagpole(glm::vec2 posPole);

	glm::vec2 getPosition();
	glm::vec2 getSize();

private:

	bool bJumping, bFalling;
	glm::ivec2 tileMapDispl;
	int jumpAngle, startY, jumpHeight;
	Texture spritesheet;
	Sprite* sprite;

	Texture spritesheetBig;
	Sprite* bigSprite;

	TileMap* map;

	bool bigMario, gameOver, deadAnimStart, flagpoleAnimStart, flagpoleTouchdown;
	glm::vec2 vel, posPlayer;

	int allowChangeTimer, deadAnimCounter, invincibleCounter, flagpoleAnimCounter;
	sf::Music jumpMus, deathMus;
};


#endif // _PLAYER_INCLUDE


