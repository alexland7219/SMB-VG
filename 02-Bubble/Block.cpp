#include "Block.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>

enum BlockAnims
{
    QUESTION, COIN, BREAKABLE, UNBREAKABLE 
};

enum BlockTypes
{
    T_COIN = 2,
    T_BREAKABLE = 1,
    T_QUESTION = 5
};

void Block::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int type)
{
    blockKO = false;
    blockType = type;
    vel = glm::vec2(0.f, 0.f);

    spritesheet.loadFromFile("images/blocks.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25, 0.25), &spritesheet, &shaderProgram);


    sprite->setNumberAnimations(4);

    sprite->setAnimationSpeed(QUESTION, 4);
    sprite->addKeyframe(QUESTION, glm::vec2(0.f, 0.25f));
    sprite->addKeyframe(QUESTION, glm::vec2(0.25f, 0.25f));
    sprite->addKeyframe(QUESTION, glm::vec2(0.5f, 0.25f));
    sprite->addKeyframe(QUESTION, glm::vec2(0.75f, 0.25f));
    
    sprite->setAnimationSpeed(COIN, 4);
    sprite->addKeyframe(COIN, glm::vec2(0.f, 0.5f));
    sprite->addKeyframe(COIN, glm::vec2(0.25f, 0.5f));
    sprite->addKeyframe(COIN, glm::vec2(0.5f, 0.5f));
    sprite->addKeyframe(COIN, glm::vec2(0.75f, 0.5f));
    
    sprite->setAnimationSpeed(BREAKABLE, 1);
    sprite->addKeyframe(BREAKABLE, glm::vec2(0.f, 0.f));

    sprite->setAnimationSpeed(UNBREAKABLE, 1);
    sprite->addKeyframe(UNBREAKABLE, glm::vec2(0.75f, 0.f));
    
    if (blockType == T_COIN) sprite->changeAnimation(COIN);
    else if (blockType == T_QUESTION) sprite->changeAnimation(QUESTION);
    else sprite->changeAnimation(BREAKABLE);


	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));

}

void Block::update(int deltaTime){
    posItem.x += vel.x;

    sprite->update(deltaTime);

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));

}

void Block::render()
{
	sprite->render(false);
}

void Block::setPosition(const glm::vec2& pos)
{
	posItem = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
}
