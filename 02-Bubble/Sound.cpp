#include <iostream>
#include <SFML/Audio.hpp>
#include "Sound.h"

enum Songs {
	JUMP, DEATH, COIN, BREAK, MUSHROOM, STAR, WIN, MAINTHEME, ZELDA, GOOMBA, KOOPA
}; //  0     1     2       3       4      5    6      7          8      9     10

void Sound::init()
{
	musicVect[JUMP].openFromFile("audio/jump.ogg");
	musicVect[JUMP].setVolume(20);

	musicVect[DEATH].openFromFile("audio/death.ogg");
	musicVect[DEATH].setVolume(100);

	musicVect[COIN].openFromFile("audio/coin.ogg");
	musicVect[COIN].setVolume(15);

	musicVect[BREAK].openFromFile("audio/blockbreak.ogg");
	musicVect[BREAK].setVolume(200);

	musicVect[MUSHROOM].openFromFile("audio/Mushroom.ogg");
	musicVect[MUSHROOM].setVolume(15);

	musicVect[STAR].openFromFile("audio/star_theme.ogg");
	musicVect[STAR].setVolume(100);

	musicVect[WIN].openFromFile("audio/win.ogg");
	musicVect[WIN].setVolume(50);

	musicVect[MAINTHEME].openFromFile("audio/track1.ogg");
	musicVect[MAINTHEME].setVolume(25);
	musicVect[MAINTHEME].setLoop(true);

	musicVect[ZELDA].openFromFile("audio/zelda.ogg");
	musicVect[ZELDA].setVolume(40);
	musicVect[ZELDA].setLoop(true);

	musicVect[GOOMBA].openFromFile("audio/goomba.ogg");
	
	musicVect[KOOPA].openFromFile("audio/koopa.ogg");
}

void Sound::play(int songIdx) {
	if (songIdx < 0 || songIdx > 10) return;

	musicVect[songIdx].setPlayingOffset(sf::Time::Zero);
	musicVect[songIdx].play();
}

void Sound::stop(int songIdx){
	if (songIdx < 0 || songIdx > 10) return;

	musicVect[songIdx].stop();
}

int Sound::getStatus(int songIdx){
	if (songIdx < 0 || songIdx > 10) return -1;

	return musicVect[songIdx].getStatus();
}

void Sound::stopAll() {
	for (int i = 0; i < 11; ++i) musicVect[i].stop();
}