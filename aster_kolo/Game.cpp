#include "Game.h"
#include <fstream>

RenderWindow window;


Game::Game(void)
{
	ContextSettings settings;
	settings.antialiasingLevel = 8;

	window.create(VideoMode(1024,768),"Aster Techno Droids",Style::Fullscreen,
		settings);

    state = END;
	window.setFramerateLimit(100);

	if((!font.loadFromFile("data/Aerovias.ttf")) || (!texture.loadFromFile("images/space.jpg")))
	{
		MessageBox(NULL,"Font not found!","ERROR",NULL);
		return;
	}
    state = MENU;
}


Game::~Game(void)
{
	audio.stopMusic();

	fstream file("config.cfg",ios::out | ios::trunc);
	file << unmute;
	file.close();
}





void Game::runGame()
{
	while(state != END)
	{
		switch (state)
		{
		case GameState::MENU:
			menu();
			break;
		case GameState::GAME:
			single();
			break;
        case GameState::GAME_OVER:
            game_over();
            break;
		}
	}
}


void Game::menu()
{
	Text title("Aster Techno Droids",font,100);
	title.setStyle(Text::Bold);
	title.setColor(Color::Cyan);

	title.setPosition(1024/2-title.getGlobalBounds().width/2,100);

	const int ile = 2;

	Text tekst[ile];

	string str[] = {"Play [Enter]","Exit [Esc]"};

	for(int i=0;i<ile;i++)
	{
		tekst[i].setFont(font);
		tekst[i].setCharacterSize(65);

		tekst[i].setString(str[i]);
		tekst[i].setPosition(1024/2-tekst[i].getGlobalBounds().width/2,250+i*120);
	}

    Sprite background(texture);

	bool b;
	//opcje
	fstream file("config.cfg");
	if(file.is_open())
		file >> b;
	else
		b=true;
	file.close();

	Text options;
	string s="Music & Sounds: ";
	if(b==true)
			options.setString(s+"ON");
	else
		options.setString(s+"OFF");

	options.setFont(font);
	options.setCharacterSize(20);
	options.setPosition(10,10);

	audio.loadMusic(true);
	audio.loadSounds();

	audio.playMusic();

	audio.setVolume(!b);

	//Asteroidy
	Asteroids asteroids;
	asteroids.generate(20);



	while(state == MENU)
	{
		Vector2f mouse(Mouse::getPosition(window));
		Event event;

		while(window.pollEvent(event))
		{
			//Wciœniêcie ESC lub przycisk X
			if(event.type == Event::Closed || event.type == Event::KeyPressed &&
				event.key.code == Keyboard::Escape)
				state = END;

			//klikniêcie PLAY
			else if(tekst[0].getGlobalBounds().contains(mouse) &&
				event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left|| event.type == Event::KeyPressed &&
				event.key.code == Keyboard::Enter)
			{
				audio.stopMusic();
				audio.playSound(Audio::T_CLICK);
				state = GAME;
			}

			//klikniecie HIGHSCORE

			//klikniêcie EXIT
			else if(tekst[1].getGlobalBounds().contains(mouse) &&
				event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left)
			{
				state = END;
				audio.playSound(Audio::T_CLICK);
			}

			//wyciszenie/wlaczenie muzyki
			else if(options.getGlobalBounds().contains(mouse) &&
				event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left)
			{
				if(b)
				{
					b=false;
					options.setString(s+"OFF");
				}
				else
				{
					b=true;
					options.setString(s+"ON");
				}
				audio.setVolume(!b);
				audio.playSound(Audio::T_CLICK);
			}
		}
		for(int i=0;i<ile;i++)
			if(tekst[i].getGlobalBounds().contains(mouse))
			{
				if(tekst[i].getColor()==Color::White)
				{
					audio.playSound(Audio::T_MENU);
				}
				tekst[i].setColor(Color::Cyan);
			}
			else tekst[i].setColor(Color::White);

		if(options.getGlobalBounds().contains(mouse))
		{
			options.setColor(Color::Cyan);
		}
		else
			options.setColor(Color::White);

		window.clear();


        window.draw(background);
		asteroids.update();
		//window.draw(asteroids);

		window.draw(title);

		for(int i=0;i<ile;i++)
			window.draw(tekst[i]);

		window.draw(options);



		window.display();
	}
	unmute = b;
}

void Game::single()
{
	Engine engine(window,unmute);

	engine.runEngine();

	//state = MENU;
	state = GAME_OVER;
}

void Game::highscore()
{
    //
}

void Game::game_over()//na razie nie dziala
{
    Text title("GAME OVER",font,100);
	title.setStyle(Text::Bold);
	title.setColor(Color::Cyan);

	title.setPosition(1024/2-title.getGlobalBounds().width/2,100);

	const int ile = 2;

	Text tekst[ile];

	string str[] = {"Play [Enter]","Exit [Esc]"};

	for(int i=0;i<ile;i++)
	{
		tekst[i].setFont(font);
		tekst[i].setCharacterSize(65);

		tekst[i].setString(str[i]);
		tekst[i].setPosition(1024/2-tekst[i].getGlobalBounds().width/2,250+i*120);
	}

    Sprite background(texture);

	bool b;
	//opcje
	fstream file("config.cfg");
	if(file.is_open())
		file >> b;
	else
		b=true;
	file.close();

	Text options;
	string s="Music & Sounds: ";
	if(b==true)
			options.setString(s+"ON");
	else
		options.setString(s+"OFF");

	options.setFont(font);
	options.setCharacterSize(20);
	options.setPosition(10,10);

	audio.loadMusic(true);
	audio.loadSounds();

	audio.playMusic();

	audio.setVolume(!b);

	//Asteroidy
	Asteroids asteroids;
	asteroids.generate(20);



	while(state == GAME_OVER)
	{
		Vector2f mouse(Mouse::getPosition(window));
		Event event;

		while(window.pollEvent(event))
		{
			//Wciœniêcie ESC lub przycisk X
			if(event.type == Event::Closed || event.type == Event::KeyPressed &&
				event.key.code == Keyboard::Escape)
				state = END;

			//klikniêcie PLAY
			else if(tekst[0].getGlobalBounds().contains(mouse) &&
				event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left|| event.type == Event::KeyPressed &&
				event.key.code == Keyboard::Enter)
			{
				audio.stopMusic();
				audio.playSound(Audio::T_CLICK);
				state = GAME;
			}

			//klikniecie HIGHSCORE

			//klikniêcie EXIT
			else if(tekst[1].getGlobalBounds().contains(mouse) &&
				event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left)
			{
				state = END;
				audio.playSound(Audio::T_CLICK);
			}

			//wyciszenie/wlaczenie muzyki
			else if(options.getGlobalBounds().contains(mouse) &&
				event.type == Event::MouseButtonReleased && event.key.code == Mouse::Left)
			{
				if(b)
				{
					b=false;
					options.setString(s+"OFF");
				}
				else
				{
					b=true;
					options.setString(s+"ON");
				}
				audio.setVolume(!b);
				audio.playSound(Audio::T_CLICK);
			}
		}
		for(int i=0;i<ile;i++)
			if(tekst[i].getGlobalBounds().contains(mouse))
			{
				if(tekst[i].getColor()==Color::White)
				{
					audio.playSound(Audio::T_MENU);
				}
				tekst[i].setColor(Color::Cyan);
			}
			else tekst[i].setColor(Color::White);

		if(options.getGlobalBounds().contains(mouse))
		{
			options.setColor(Color::Cyan);
		}
		else
			options.setColor(Color::White);

		//window.clear();


        //window.draw(background);
		//asteroids.update();
		//window.draw(asteroids);

		window.draw(title);

		for(int i=0;i<ile;i++)
			window.draw(tekst[i]);

		//window.draw(options);



		window.display();
	}
	unmute = b;
}
