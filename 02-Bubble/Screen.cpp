#include "Screen.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <string>
#include "Game.h"
#include <GL/glut.h>

#define ENTER_KEY 13

enum SelectOptions {
    PLAY_SELECT, INSTRUCTIONS_SELECT, CREDITS_SELECT, EXIT_SELECT
};

enum Screens {
	MAINMENU, INSTRUCTIONS, CREDITS, LEVEL1, LEVEL2, LIVES_LEFT
};


Screen::Screen()
{
    map = NULL;
	bgmap = NULL;
}

Screen::~Screen()
{
    if (map != NULL) delete map;
	if (bgmap!=NULL) delete bgmap;
}


void Screen::init(int numScreen){
    initShaders();
	initGlyphTextures();
	bgmap = TileMap::createTileMap("levels/level01-bg.txt", glm::vec2(0, 0), texProgram, true);

    nextScreen = -1;
    selection = PLAY_SELECT;
    allowChangeTimer = 0;
	rollcredits = -60;
	screenType = numScreen;

    camera = glm::vec4(0.f, float(SCREEN_WIDTH - 1), float(SCREEN_HEIGHT - 1), 0.f);
	projection = glm::ortho(camera.x, camera.y, camera.z, camera.w);

}

void Screen::update(int deltaTime){
    if (allowChangeTimer > 0) allowChangeTimer -= deltaTime;
	if (screenType == CREDITS) rollcredits += 0.4;
	if (rollcredits > 210) rollcredits = -100;

    if (Game::instance().getSpecialKey(GLUT_KEY_UP) && allowChangeTimer <= 0){
        selection = (selection + 3)%4;
        allowChangeTimer = 100;
    } else if (Game::instance().getSpecialKey(GLUT_KEY_DOWN) && allowChangeTimer <= 0){
        selection = (selection + 1)%4;
        allowChangeTimer = 100;

    } else if (Game::instance().getKey(ENTER_KEY) && allowChangeTimer <= 0 && screenType == MAINMENU){
		allowChangeTimer = 100;

        switch (selection){
			case 0:
			nextScreen = LEVEL1;
			break;
			case 1:
			nextScreen = INSTRUCTIONS;
			break;
			case 2:
			nextScreen = CREDITS;
			break;
			case 3:
			exit(0);
		}
    } else if ((screenType == INSTRUCTIONS || screenType == CREDITS) && Game::instance().getKey(ENTER_KEY) && allowChangeTimer <= 0){
		// Go back to main menu
		allowChangeTimer = 100;
		nextScreen = MAINMENU;
	}
	else if (Game::instance().getKey(ENTER_KEY) && allowChangeTimer <= 0 && screenType == LIVES_LEFT){
		allowChangeTimer = 100;
		nextScreen = LEVEL1;
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

	if (screenType != LIVES_LEFT || Game::instance().getLivesLeft() != 1) glClearColor(.57f, 0.57f, 1.0f, 1.0f);
	else glClearColor(0.8f, 0.0f, 0.0f, 1.0f);
	bgmap->render();

	switch (screenType){
		case MAINMENU:
		{
			if (selection == PLAY_SELECT){
				string enter = "= PLAY";
				renderText(enter, glm::vec2(65, 20));
			} else {
				string enter = "PLAY";
				renderText(enter, glm::vec2(75, 20));
			}

			if (selection == INSTRUCTIONS_SELECT){
				string enter = "= INSTRUCTIONS";
				renderText(enter, glm::vec2(65, 40));
			} else {
				string enter = "INSTRUCTIONS";
				renderText(enter, glm::vec2(75, 40));
			}

			if (selection == CREDITS_SELECT){
				string enter = "= CREDITS";
				renderText(enter, glm::vec2(65, 60));
			} else {
				string enter = "CREDITS";
				renderText(enter, glm::vec2(75, 60));
			}

			if (selection == EXIT_SELECT){
				string enter = "= EXIT";
				renderText(enter, glm::vec2(65, 80));
			} else {
				string enter = "EXIT";
				renderText(enter, glm::vec2(75, 80));
			}

		}
		break;

		case INSTRUCTIONS: // Instructions
		{
			string left1 = "USE ARROW KEYS TO";
			string left2 = "MOVE LEFT OR RIGHT";

			string jump = "PRESS SPACE TO JUMP";
			string run = "PRESS SHIFT TO RUN";
			string quit = "PRESS ESC TO QUIT";
			string back = "= BACK";

			renderText(left1, glm::vec2(20, 30));
			renderText(left2, glm::vec2(20, 40));

			renderText(jump, glm::vec2(20, 60));
			renderText(run, glm::vec2(20, 90));
			renderText(quit, glm::vec2(20, 120));
			renderText(back, glm::vec2(20, 180));

		}
		break;

		case CREDITS:
		{
			string ros = "ALEXANDRE ROS I ROGER";
			string wickenden = "ALEX WICKENDEN DOMINGO";

			renderText(ros, glm::vec2(35, rollcredits + 60));
			renderText(wickenden, glm::vec2(35, rollcredits + 90));
		}
		break;

		case LIVES_LEFT:
		{
			int lives = Game::instance().getLivesLeft();

			string text;
			string enter = "PRESS ENTER TO RETRY";
			string ty    = "THANK YOU FOR PLAYING";

			switch (lives){
				case 0:
				text = "GAME OVER";
				renderText(text, glm::vec2(35, 60));
				renderText(ty, glm::vec2(35, 160));
				break;
				case 1:
				text = "ONE LIFE LEFT ...";
				renderText(text, glm::vec2(35, 60));
				renderText(enter, glm::vec2(35, 160));
				break;
				case 2:
				text = "TWO LIVES LEFT";
				renderText(text, glm::vec2(35, 60));
				renderText(enter, glm::vec2(35, 160));

			}
		}

		break;
	}

}

int Screen::getTransition(){
    return nextScreen;
}

void Screen::setType(int newtype){ 
	screenType = newtype;
	nextScreen = -1;
	rollcredits = -90;
}

enum Glyphs {
	ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, XZERO, XONE, XTWO, XTHREE, XFOUR, XFIVE, XSIX, XSEVEN, XEIGHT, XNINE,
	COIN, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 
	PT, COMMA, DASH, EXCLAMATION, EQUALS, TWODOTS, APOSTROPHE, DOUBLEAPOSTROPHE
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

	textSprite->setAnimationSpeed(PT, 0);
	textSprite->addKeyframe(PT, glm::vec2(0.f, 0.25f));
	textSprite->setAnimationSpeed(COMMA, 0);
	textSprite->addKeyframe(COMMA, glm::vec2(0.0625f, 0.25f));
	textSprite->setAnimationSpeed(DASH, 0);
	textSprite->addKeyframe(DASH, glm::vec2(0.125f, 0.25f));
	textSprite->setAnimationSpeed(EXCLAMATION, 0);
	textSprite->addKeyframe(EXCLAMATION, glm::vec2(0.1875f, 0.25f));
	textSprite->setAnimationSpeed(EQUALS, 0);
	textSprite->addKeyframe(EQUALS, glm::vec2(0.0f, 0.375f));
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
				textSprite->changeAnimation(PT);
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




