#ifndef _SCREEN_INCLUDE
#define _SCREEN_INCLUDE

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Texture.h"
#include <SFML/Audio.hpp>

// Screen displays one screeen with text and bg.


class Screen
{

public:
	Screen();
	~Screen();

	void init();
	void update(int deltaTime);
	void render();
    bool getTransition();

private:
	void initShaders();
	void initGlyphTextures();
	void renderText(string& text, glm::vec2 pos);

private:
	TileMap* map;
	ShaderProgram texProgram;
    int selection;
    int allowChangeTimer;

    bool transition;

	Texture textSpriteSheet;
	Sprite* textSprite;

	glm::mat4 projection;
    glm::vec4 camera;
};


#endif

