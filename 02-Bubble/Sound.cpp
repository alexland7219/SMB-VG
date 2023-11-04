#include <iostream>
#include <vector>
#include <SFML/Audio.hpp>
#include "Sound.h"

enum Songs {
	JUMP, DEATH, COIN, BREAK, MUSHROOM, STAR, WIN, MAINTHEME, ZELDA, GOOMBA, KOOPA
};

void Sound::init()
{
	musicVect.resize(11);

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

	musicVect[WIN].openFromFile("audio/star_theme.ogg");
	musicVect[WIN].setVolume(50);

	musicVect[STAR].openFromFile("audio/track1.ogg");
	musicVect[STAR].setVolume(25);
	musicVect[STAR].setLoop(true);

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

void Sound::stopAll() {
	for (int i = 0; i < 11; ++i) musicVect[i].stop();
}