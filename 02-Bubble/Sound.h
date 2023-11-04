#ifndef _SOUND_INCLUDE
#define _SOUND_INCLUDE

#include <SFML/Audio.hpp>
#include <vector>

class Sound
{
private:
	Sound(){};

public:
	static Sound& instance() {
		static Sound S;

		return S;
	}

	void init();

	void play(int songIdx);

	void stopAll();

private:
	vector<sf::Music> musicVect;
};

#endif //SOUND_INCLUDE