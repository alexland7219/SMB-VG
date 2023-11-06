#include "Block.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include "Sound.h"

enum BlockAnims
{
    QUESTION, COIN, BREAKABLE, UNBREAKABLE, BREAK
};

enum QuestionBlockContainer
{
    Q_COIN, Q_MUSHROOM, Q_STAR
};

enum BlockTypes
{
    T_COIN = 2,
    T_BREAKABLE = 1,
    T_QUESTION_COIN = 5,
    T_QUESTION_MUSH = 6,
    T_QUESTION_STAR = 7,
    T_UNBREAKABLE = 10,
};

void Block::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int type)
{
    blockKO = false;
    blockType = type;
    vel = glm::vec2(0.f, 0.f);

    spritesheet.loadFromFile("images/blocks.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.125, 0.125), &spritesheet, &shaderProgram);

    sprite->setNumberAnimations(5);

    sprite->setAnimationSpeed(QUESTION, 4);
    sprite->addKeyframe(QUESTION, glm::vec2(0.5f, 0.f));
    sprite->addKeyframe(QUESTION, glm::vec2(0.625f, 0.f));
    sprite->addKeyframe(QUESTION, glm::vec2(0.75f, 0.f));
    sprite->addKeyframe(QUESTION, glm::vec2(0.875f, 0.f));
    
    sprite->setAnimationSpeed(COIN, 4);
    sprite->addKeyframe(COIN, glm::vec2(0.f, 0.125f));
    sprite->addKeyframe(COIN, glm::vec2(0.125f, 0.125f));
    sprite->addKeyframe(COIN, glm::vec2(0.25f, 0.125f));
    sprite->addKeyframe(COIN, glm::vec2(0.375f, 0.125f));
    
    sprite->setAnimationSpeed(BREAKABLE, 1);
    sprite->addKeyframe(BREAKABLE, glm::vec2(0.f, 0.f));

    sprite->setAnimationSpeed(UNBREAKABLE, 1);
    sprite->addKeyframe(UNBREAKABLE, glm::vec2(0.375f, 0.f));

    sprite->setAnimationSpeed(BREAK, 10);
    sprite->addKeyframe(BREAK, glm::vec2(0.75f, 0.125f));
    sprite->addKeyframe(BREAK, glm::vec2(0.875f, 0.125f));
    sprite->addKeyframe(BREAK, glm::vec2(0.125f, 0.f));

    
    if (blockType == T_COIN) sprite->changeAnimation(COIN, false);
    else if (blockType == T_QUESTION_COIN || blockType == T_QUESTION_MUSH || blockType == T_QUESTION_STAR) sprite->changeAnimation(QUESTION, false);
    else sprite->changeAnimation(BREAKABLE, false);


	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));

}

void Block::update(int deltaTime){
    if (animBump && !blockKO){
        if (animTimer > 100) posItem.y -= 1;
        else if (animTimer > 0){
            posItem.y += 1;
            if (blockType == T_QUESTION_COIN || blockType == T_QUESTION_MUSH || blockType == T_QUESTION_STAR){
                sprite->changeAnimation(UNBREAKABLE, false);
                blockType = T_UNBREAKABLE;
            } 
        }
        else animBump = 0;

        animTimer -= deltaTime;
    }

    if (animBreak && !blockKO){
        breakTimer -= deltaTime;
        if (breakTimer <= 0) blockKO = true;
    }

    posItem.x += vel.x;

    sprite->update(deltaTime);

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));

}

void Block::render()
{
	if (!blockKO) sprite->render(false);
}

void Block::setPosition(const glm::vec2& pos)
{
	posItem = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posItem.x), float(tileMapDispl.y + posItem.y)));
}

void Block::breakBlock()
{
    if (blockKO) return;

    sprite->changeAnimation(BREAK, false);
    animBreak = true;
    breakTimer = 200;

    Sound::instance().play(3); // Play break sound
}

bool Block::isBroken(){ return blockKO; }

bool Block::bumpBlock(){
    if (animBump) return false;
    if (blockType == T_UNBREAKABLE) return false;
    // Move the sprite 3px up and 3px down in the span of .5 seconds
    animTimer = 200;
    animBump = true;
    return true;
}

void Block::collectCoin(bool isCoinBlock){
    blockKO = isCoinBlock;
    Sound::instance().play(2); // Play coin sound

}

bool Block::isBreaking(){ return animBreak; }