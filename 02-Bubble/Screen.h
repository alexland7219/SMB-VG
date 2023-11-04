#ifndef _SCREEN_INCLUDE
#define _SCREEN_INCLUDE

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Texture.h"

// Screen displays one screeen with text and bg.


class Screen
{

public:
	Screen();
	~Screen();

	void init(int numScreen);
	void update(int deltaTime);
	void render();
    int getTransition();
	void setType(int newtype);

private:
	void initShaders();
	void initGlyphTextures();
	void renderText(string& text, glm::vec2 pos);

private:
	TileMap* map;
	TileMap* bgmap;
	
	ShaderProgram texProgram;
    int selection;
    int allowChangeTimer;
	int screenType; /* 0 = Main Menu; 1 = Instructions; 2 = Credits */
	float rollcredits;

    int nextScreen; /*-1 for no transition*/

	Texture textSpriteSheet, titleSpriteSheet;
	Sprite* textSprite;
	Sprite* titleSprite;

	glm::mat4 projection;
    glm::vec4 camera;
};


#endif

