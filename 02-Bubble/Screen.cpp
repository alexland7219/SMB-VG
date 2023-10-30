#include "Screen.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <string>
#include "Game.h"
#include <GL/glut.h>

#define ENTER_KEY 13

enum SelectOptions {
    PLAY, INSTRUCTIONS, CREDITS, EXIT
};

Screen::Screen()
{
    map = NULL;
}

Screen::~Screen()
{
    if (map != NULL) delete map;
}


void Screen::init(){
    initShaders();
	initGlyphTextures();

    transition = false;
    selection = PLAY;
    allowChangeTimer = 0;

    camera = glm::vec4(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	projection = glm::ortho(camera.x, camera.y, camera.z, camera.w);

}

void Screen::update(int deltaTime){
    if (allowChangeTimer > 0) allowChangeTimer -= deltaTime;

    if (Game::instance().getSpecialKey(GLUT_KEY_UP) && allowChangeTimer <= 0){
        selection = (selection - 1)%4;
        allowChangeTimer = 100;
    } else if (Game::instance().getSpecialKey(GLUT_KEY_DOWN) && allowChangeTimer <= 0){
        selection = (selection + 1)%4;
        allowChangeTimer = 100;

    } else if (Game::instance().getKey(ENTER_KEY) && allowChangeTimer <= 0){
        transition = true;
    }
}

void Screen::render(){
    glm::mat4 modelview;

	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	texProgram.setUniformInt("invertX", 0); // False

	glClearColor(.57f, 0.57f, 1.0f, 1.0f);

    string hello = "WELCOME !";
    renderText(hello, glm::vec2(50, 50));

    if (selection == PLAY){
        string enter = "= PLAY";
        renderText(enter, glm::vec2(65, 100));
    } else {
        string enter = "PLAY";
        renderText(enter, glm::vec2(75, 100));
    }

    if (selection == INSTRUCTIONS){
        string enter = "= INSTRUCTIONS";
        renderText(enter, glm::vec2(65, 120));
    } else {
        string enter = "INSTRUCTIONS";
        renderText(enter, glm::vec2(75, 120));
    }

    if (selection == CREDITS){
        string enter = "= CREDITS";
        renderText(enter, glm::vec2(65, 140));
    } else {
        string enter = "CREDITS";
        renderText(enter, glm::vec2(75, 140));
    }

    if (selection == EXIT){
        string enter = "= EXIT";
        renderText(enter, glm::vec2(65, 160));
    } else {
        string enter = "EXIT";
        renderText(enter, glm::vec2(75, 160));
    }

}

bool Screen::getTransition(){
    return transition;
}

enum Glyphs {
	ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, XZERO, XONE, XTWO, XTHREE, XFOUR, XFIVE, XSIX, XSEVEN, XEIGHT, XNINE,
	COIN, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 
	POINT, COMMA, DASH, EXCLAMATION, EQUALS, TWODOTS, APOSTROPHE, DOUBLEAPOSTROPHE
};

void Screen::initGlyphTextures(){
	textSpriteSheet.loadFromFile("images/letters.png", TEXTURE_PIXEL_FORMAT_RGBA);
	textSprite = Sprite::createSprite(glm::ivec2(8, 8), glm::vec2(0.0625, 0.0625), &textSpriteSheet, &texProgram);

	textSprite->setNumberAnimations(64);

	// Numbers
	for (int x = ZERO; x <= XNINE; ++x){
		textSprite->setAnimationSpeed(x, 0);
		textSprite->addKeyframe(x, glm::vec2(0.0625*(x % 10), (x > NINE ? 0.0625 : 0)));
	}

	textSprite->setAnimationSpeed(COIN, 0);
	textSprite->addKeyframe(COIN, glm::vec2(0.625f, 0.f));

	// Letters
	for (int x = A; x <= Z; ++x){
		int offset = x - A;
		textSprite->setAnimationSpeed(x, 0);
		textSprite->addKeyframe(x, glm::vec2(0.0625*(offset % (Q-A)), (x > P ? 0.1875f : 0.125f)));
	}

	textSprite->setAnimationSpeed(POINT, 0);
	textSprite->addKeyframe(POINT, glm::vec2(0.f, 0.25f));
	textSprite->setAnimationSpeed(COMMA, 0);
	textSprite->addKeyframe(COMMA, glm::vec2(0.0625f, 0.25f));
	textSprite->setAnimationSpeed(DASH, 0);
	textSprite->addKeyframe(DASH, glm::vec2(0.125f, 0.25f));
	textSprite->setAnimationSpeed(EXCLAMATION, 0);
	textSprite->addKeyframe(EXCLAMATION, glm::vec2(0.1875f, 0.25f));
	textSprite->setAnimationSpeed(EQUALS, 0);
	textSprite->addKeyframe(EQUALS, glm::vec2(0.25f, 0.25f));
	textSprite->setAnimationSpeed(TWODOTS, 0);
	textSprite->addKeyframe(TWODOTS, glm::vec2(0.3125f, 0.25f));
	textSprite->setAnimationSpeed(APOSTROPHE, 0);
	textSprite->addKeyframe(APOSTROPHE, glm::vec2(0.375f, 0.25f));
	textSprite->setAnimationSpeed(DOUBLEAPOSTROPHE, 0);
	textSprite->addKeyframe(DOUBLEAPOSTROPHE, glm::vec2(0.4375f, 0.25f));

	return;
}

void Screen::renderText(string& text, glm::vec2 pos){
	// Position top-left
	int i = 0;

	for (char c : text){
		// Rendering char c
		switch (c){
			case ' ':
				++i;
				continue;
			case '#':
				textSprite->changeAnimation(COIN);
				break;
			case '.':
				textSprite->changeAnimation(POINT);
				break;
			case ',':
				textSprite->changeAnimation(COMMA);
				break;
			case '-':
				textSprite->changeAnimation(DASH);
				break;
			case '!':
				textSprite->changeAnimation(EXCLAMATION);
				break;
			case ':':
				textSprite->changeAnimation(TWODOTS);
				break;
			case '=':
				textSprite->changeAnimation(EQUALS);
				break;
			case '\'':
				textSprite->changeAnimation(APOSTROPHE);
				break;
			case '\"':
				textSprite->changeAnimation(DOUBLEAPOSTROPHE);
				break;
			default:
				if (c >= int('0') && c <= int('9')) textSprite->changeAnimation(c - int('0'));
				else if (c >= int('A') && c <= int('Z')) textSprite->changeAnimation(c - int('A') + A);
		}
		
		textSprite->setPosition( glm::vec2(pos.x + 8*i, pos.y));
		textSprite->render(false);

		++i;
	}
}

void Screen::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}




