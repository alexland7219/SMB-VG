#ifndef _BLOCK_INCLUDE
#define _BLOCK_INCLUDE

#include "Sprite.h"

class Block 
{
    public:

    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int type);
    void update(int deltaTime);
    void render();
    void breakBlock();
    bool isBroken();
    bool isBreaking();
    void collectCoin(bool isCoinBlock);

    // Returns true if we need to animate a floating sprite
    bool bumpBlock();
	void setPosition(const glm::vec2& pos);

    private:

    int blockType;
    glm::ivec2 tileMapDispl;
    Texture spritesheet;
    Sprite* sprite;
    int animTimer, breakTimer;
    bool animBump, animBreak;
	// Plays "Break" & "Coin" music

    bool blockKO;

    glm::vec2 vel, posItem;
};

#endif