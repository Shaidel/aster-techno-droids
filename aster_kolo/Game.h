#pragma once

#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>
#include <Windows.h>
#include <string>
#include "Engine.h"

using namespace std;
using namespace sf;

class Game
{
public:
	Game(void);
	~Game(void);

	void runGame();

protected:
	enum GameState {MENU,GAME,HIGHSCORE,GAME_OVER,END};
	GameState state;

private:
	Font font;

	Texture texture;

	Texture logo;


	Audio audio;

	bool unmute;

	void menu();
	void highscore();
	void single();
	void game_over();

};

