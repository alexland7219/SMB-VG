#include <GL/glew.h>
#include <GL/glut.h>
#include "Game.h"
#include <iostream>

enum Screens {
	MAINMENU, INSTRUCTIONS, CREDITS, LEVEL1, LEVEL2, LIVES_LEFT
};

void Game::init()
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	screen.init(MAINMENU);
	nLives = 3;
	//scene.init(1);
	screenState = MAINMENU;
}

bool Game::update(int deltaTime)
{
	//scene.update(deltaTime);
	
	switch (screenState){
		case MAINMENU:
		case INSTRUCTIONS:
		case CREDITS:
		case LIVES_LEFT:
		{
			screen.update(deltaTime);

			int trans = screen.getTransition();
			if (trans != -1){
				screenState = trans;
				screen.setType(trans);

				if (screenState == LEVEL1 || screenState == LEVEL2){
					scene.init(1);
				}

			}
		
		}
		break;
		case LEVEL1:
		case LEVEL2:

		scene.update(deltaTime);

		if (scene.isOver()){
			nLives -= 1;

			scene.removePlayer();
			screen.init(LIVES_LEFT);
			screenState = LIVES_LEFT;
		}
		else if (scene.hasWon() && screenState == LEVEL1){


			screenState = LEVEL2;
			scene.init(2);
		}
		else if (scene.hasWon() && screenState == LEVEL2){
			screenState = MAINMENU;
			screen.setType(screenState);
		}
		
		break;
	}

	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	switch (screenState){
		case MAINMENU:
		case INSTRUCTIONS:
		case CREDITS:
		case LIVES_LEFT:
		screen.render();
		break;
		case LEVEL1:
		case LEVEL2:
		scene.render();
		scene.render();
		break;
	}

}

int Game::getLivesLeft() const{
	return nLives;
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





