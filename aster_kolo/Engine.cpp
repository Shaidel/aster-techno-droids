//Engine.cpp
#include "Engine.h"
#include <Windows.h>
#include <iostream>
#include <sstream>

using namespace sf;

//------------------FUNKCJE POMOCNICZE---------------
string toString(float liczba)
{
	stringstream ss;
	ss<<liczba;

	return ss.str();
}


void getInterval(const Vector2f *axVertices,int numVert, const Vector2f &xAxis,
	float &min, float &max)
{
	min = max = (axVertices[0].x * xAxis.x + axVertices[0].y * xAxis.y);

	for(int i=1;i<numVert;i++)
	{
		float d=(axVertices[i].x * xAxis.x + axVertices[i].y * xAxis.y);

		if(d<min) min = d;
		else if(d>max) max = d;
	}
}


bool IntervalIntersect(const Vector2f *A,int Anum,const Vector2f *B,int Bnum,
	const Vector2f &xAxis, const Vector2f &offset)
{
	float min0,max0;
	float min1,max1;

	getInterval(A,Anum,xAxis,min0,max0);
	getInterval(B,Bnum,xAxis,min1,max1);

	float h = offset.x * xAxis.x + offset.y*xAxis.y;
	min0 += h;
	max0 += h;

	float d0 = min0 - max1;
	float d1 = min1 - max0;

	if(d0 > 0.0f || d1 > 0.0f)
		return false;
	else
		return true;
}


bool pointInside(Vector2f point, Vector2f *vertices, int size)
{
	int i, j = 0;
	bool oddNODES = false;
	for (i = 0; i < size; i++)
	{
		j++;
		if (j == size)
			j = 0;
		if (vertices[i].y < point.y && vertices[j].y >= point.y || vertices[j].y < point.y && vertices[i].y >= point.y)
		{
			if (vertices[i].x + (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y) *
				(vertices[j].x - vertices[i].x) < point.x)
			{
				oddNODES = !oddNODES;
			}
		}
	}
	return oddNODES;
}

//---------------------------------------------------

Engine::Engine(sf::RenderWindow &win,bool b)
{

	window = &win;
	window->setMouseCursorVisible(false);

	bullets_player = Bullet(false,500,false);

	audio.loadMusic(false);
	audio.loadSounds();

	audio.playMusic();
	audio.setVolume(!b);

	liczba_asteroid = 0;
	nr_fali = 1;
	score = 0;

	//napisy
	if((!font.loadFromFile("data/Mecha.ttf")) || (!texture.loadFromFile("images/game1.jpg")))
	{
		cout<<"Error";
		return;
	};


	str_napis[0]="Lifes: ";
	str_napis[1]="";
	str_napis[2]="Ammo: ";
	str_napis[3]="Effect active for: ";
	str_napis[4]="Score: ";

	for(int i=0;i<5;i++)
	{
		napis[i].setFont(font);
		napis[i].setCharacterSize(30);
		napis[i].setColor(Color(255,255,255,90));
	}

	napis[0].setString(str_napis[0]+toString(player.getLifes()));
	napis[0].setPosition(10,10);

	napis[2].setString(str_napis[2]+toString(bullets_player.getAmmo()));
	napis[2].setPosition(1024-napis[2].getGlobalBounds().width-10,10);

	napis[4].setString(str_napis[4]+toString(score));
	napis[4].setPosition(1024/2-napis[4].getGlobalBounds().width/2,10);

	czas_fali.restart();
}


Engine::~Engine(void)
{
	window->setMouseCursorVisible(true);
	audio.stopMusic();
}


//void Engine::game_stop()
//{
//    bool game_over = false;
//    while(!game_over)
//    {
//        Event event;
//        if(player.getStatus() == Player::STATUS_DEAD)
//			game_over = true;
//    }
//}

void Engine::runEngine()
{
	bool menu = false;
	//bool game_over = false;

	sf::Clock zegar;
	asteroids.generate(2);
	liczba_asteroid = 4;

	while(!menu)
	{
		Event event;
		Vector2f mouse(Mouse::getPosition());
		if((Keyboard::isKeyPressed(Keyboard::Left)) || (sf::Joystick::isButtonPressed(0, 0)))
		{
				player.rotate(-4);
		}

		else if((Keyboard::isKeyPressed(Keyboard::Right)) || (sf::Joystick::isButtonPressed(0, 2)))
		{
			player.rotate(4);
		}

		else if((Keyboard::isKeyPressed(Keyboard::Up)) || (sf::Joystick::isButtonPressed(0, 3)))
		{
			player.accelerate();
		}

		else if((Keyboard::isKeyPressed(Keyboard::Down)) || (sf::Joystick::isButtonPressed(0, 1)))
		{
			player.deaccelerate();
		}

		if((Keyboard::isKeyPressed(Keyboard::Space)) || (sf::Joystick::isButtonPressed(0, 6)))
		{
			bullets_player.add(player.getAngle(),player.getPosition(2));
			audio.playSound(Audio::T_STRZAL);
		}

		if(player.getStatus() == Player::STATUS_DEAD)
			//game_over = true;
            //audio.stopMusic();
            //Sprite background(texture);

			menu = true;

		while(window->pollEvent(event))
		{
			if(Keyboard::isKeyPressed(Keyboard::Escape))
			{
				menu = true;
				audio.stopMusic();
				audio.playSound(Audio::T_CLICK);
			}
		}

		update();
		draw();
	}
}


void Engine::update()
{
	player.update();
	bullets_player.update();

	asteroids.update();
	collision();

	//generowanie asteroid/nowa fala
	if(asteroids.getSize() <= 0)
	{
		nr_fali++;

		liczba_asteroid = nr_fali*2;
		asteroids.generate(liczba_asteroid);

		czas_fali.restart();

		//rozdajemy bonusy!
		if(rand()%30>20)player.addLifes();
		if(rand()%30>15)bullets_player.addAmmunition();

		napis[0].setString(str_napis[0]+toString(player.getLifes()));
		napis[0].setPosition(10,10);

		napis[2].setString(str_napis[2]+toString(bullets_player.getAmmo()));
		napis[2].setPosition(1024-napis[2].getGlobalBounds().width-10,10);

	}

	//aktualizowanie calego czasu gry
	float czas = czas_gry.getElapsedTime().asSeconds();
	napis[1].setString(str_napis[1]+toString(czas)+"s");
	napis[1].setPosition(1024/2-napis[1].getGlobalBounds().width/2,40);


	//aktualizowanie czasu bonusu
	if(player.getSpecialEffectTime()==0)
	{
		Color color = napis[3].getColor();
		if(color.a > 0)
		{
			color.a-=5;
			napis[3].setColor(Color(255,255,255,color.a));
		}
	}
	else
	{
		Color color = napis[3].getColor();
		if(color.a < 255)
		{
			color.a+=5;
			napis[3].setColor(Color(255,255,255,color.a));
		}
	}
	napis[3].setString(str_napis[3]+toString(player.getSpecialEffectTime())+"s");
	napis[3].setPosition(1024-20-napis[3].getGlobalBounds().width,728);
}


void Engine::draw()
{
	Sprite background(texture);
	window->clear();

	window->draw(background);
	for(int i=0;i<5;i++)
		window->draw(napis[i]);


	window->draw(player);
	window->draw(bullets_player);
	window->draw(asteroids);

	window->display();
}


void Engine::collision()
{
	//sprawdzenie asteroid
	if(!asteroids.isEmpty())
	{
		Vector2f A[3] = {player.getPosition(1),player.getPosition(2),player.getPosition(3)};

		int numAsteroid = 0;
		while(numAsteroid < asteroids.getSize())
		{
			Vector2f *B = new Vector2f[asteroids.getSize(numAsteroid)];

			for(int i=0;i<asteroids.getSize(numAsteroid);i++)
				B[i] = asteroids.getPoint(numAsteroid,i);

			bool collision;

			for(int i=0;i<3;i++)
			{
				for(int j=0;j<asteroids.getSize(numAsteroid);j++)
				{
					collision=(check_collision(A,3,B,asteroids.getSize(numAsteroid),A[i]-B[j]));
					if(collision)
					{
						break;
					}
				}


				if(collision)
				{
					//score = score + 10;
					asteroids.del(numAsteroid);
					liczba_asteroid--;
					player.enemy_coll();
					napis[0].setString(str_napis[0]+toString(player.getLifes()));
					//napis[4].setString(str_napis[4]+toString(score));
					break;
				}
			}

			delete []B;
			numAsteroid++;
		}
	}
	//sprawdzenie pocisków
	if(!bullets_player.isEmpty())
	{
		napis[2].setString(str_napis[2]+toString(bullets_player.getAmmo()));
		napis[2].setPosition(1024-napis[2].getGlobalBounds().width-10,10);

		for(int i=0;i<bullets_player.getSize();i++)
		{
			for(int j=0;j<asteroids.getSize();j++)
			{
				Vector2f *vert = new Vector2f[asteroids.getSize(j)];
				for(int k=0;k<asteroids.getSize(j);k++)
				{
					vert[k] = asteroids.getPoint(j,k);
				}

				bool czy_w_polygonie = pointInside(bullets_player.getPoint(i),vert,asteroids.getSize(j));

				if(czy_w_polygonie)
				{
					asteroids.del(j);
					bullets_player.remove(i);
					liczba_asteroid--;
					score = score + 10;
					napis[4].setString(str_napis[4]+toString(score));

				}
				delete []vert;
				if(czy_w_polygonie)
					break;
			}//for
		}//for
	}//if
}


bool Engine::check_collision(Vector2f *A,int sizeA,
	Vector2f *B, int sizeB, const Vector2f &offset)   //  const sf::Vector2F& offset
{
	if(!A || !B) return false;

	Vector2f xAxis[64];
	int iNumAxes = 0;

	//test dla A
	for(int j=sizeA-1,i=0;i<sizeA;j=i,i++)
	{
		Vector2f E0 = A[j];
		Vector2f E1 = A[i];
		Vector2f E = E1-E0;

		xAxis[iNumAxes] = Vector2f(-E.y,E.x);

		if(!IntervalIntersect(A,sizeA,B,sizeB,xAxis[iNumAxes],offset))
			return false;

		iNumAxes++;
	}

	//test dla B
	for(int j=sizeB-1,i=0;i<sizeB;j=i,i++)
	{
		Vector2f E0 = B[j];
		Vector2f E1 = B[i];
		Vector2f E = E1-E0;
		xAxis[iNumAxes] = Vector2f(-E.y,E.x);

		if(!IntervalIntersect(A,sizeA,B,sizeB,xAxis[iNumAxes],offset))
			return false;

		iNumAxes++;
	}

	return true;
}
