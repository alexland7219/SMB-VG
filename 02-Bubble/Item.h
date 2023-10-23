#ifndef _ITEM_INCLUDE
#define _ITEM_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Item is a class that represents an Enemy or an Object (Mushroom, star, shell).

class Item
{

public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int type);
	void update(int deltaTime);
	void render();

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);
	void changeAnimation(int animId);
	bool collisionStomped(const glm::ivec2& posPlayer, glm::ivec2 sizes);
	bool collisionKill(const glm::ivec2& posPlayer, glm::ivec2 sizes);

	void stomp(const glm::ivec2& posPlayer);
	bool isDead();
	glm::vec2 getPosition();
	glm::vec2 getSize();

	void invertXVelocity();
	void die();
	bool killsEnemies();

private:
	bool bJumping, bFalling;
	glm::ivec2 tileMapDispl;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite* sprite;

	bool koopaShell;
	bool deadAnimStart, itemKO;
	int deadAnimCounter;

	int typeItem;

	TileMap* map;

	glm::vec2 vel, posItem;

};


#endif // _ITEM_INCLUDE


