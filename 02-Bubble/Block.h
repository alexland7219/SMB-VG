#ifndef _BLOCK_INCLUDE
#define _BLOCK_INCLUDE

#include "Sprite.h"
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>

class Block 
{
    public:

    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, int type);
    void update(int deltaTime);
    void render();
    void breakBlock();
    bool isBroken();
    void collectCoin();

    // Returns true if we need to animate a floating sprite
    bool bumpBlock();
	void setPosition(const glm::vec2& pos);

    private:

    int blockType;
    glm::ivec2 tileMapDispl;
    Texture spritesheet;
    Sprite* sprite;
    int animTimer;
    bool animBump;
	sf::Music breakblock, coinMus;

    bool blockKO;

    glm::vec2 vel, posItem;
};

#endif