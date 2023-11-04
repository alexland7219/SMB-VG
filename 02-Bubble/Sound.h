#ifndef _SOUND_INCLUDE
#define _SOUND_INCLUDE

#include <SFML/Audio.hpp>

class Sound
{
private:
	Sound(){}

public:
	static Sound& instance() {
		static Sound S;

		return S;
	}

	void init();

	void play(int songIdx);
	void stop(int songIdx);
	int  getStatus(int songIdx); // 0 = Stopped, 1 = Paused, 2 = Playing

	void stopAll();

private:
	sf::Music musicVect[13];
};

#endif //SOUND_INCLUDE