#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"

enum Screens {
	MAINMENU, INSTRUCTIONS, CREDITS, LEVEL1, LEVEL2
};

void Game::init()
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	welcome.init();
	//scene.init(1);
	screen = MAINMENU;
}

bool Game::update(int deltaTime)
{
	//scene.update(deltaTime);
	
	switch (screen){
		case MAINMENU:

		welcome.update(deltaTime);
		if (welcome.getTransition()){
			screen = LEVEL1;
			scene.init(1);
		}

		break;

		case LEVEL1:

		scene.update(deltaTime);
		
		break;
	}

	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	switch (screen){
		case MAINMENU:
		welcome.render();
		break;
		case LEVEL1:
		scene.render();
		break;
	}

}

void Game::keyPressed(int key)
{
	if(key == 27) // Escape code
		bPlay = false;
	keys[key] = true;
}

void Game::keyReleased(int key)
{
	keys[key] = false;
}

void Game::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Game::specialKeyReleased(int key)
{
	specialKeys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const
{
	return keys[key];
}

bool Game::getSpecialKey(int key) const
{
	return specialKeys[key];
}





