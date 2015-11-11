#include <unistd.h>
#include <string.h>
#include "gb.h"
gb cpu;

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <iostream>
#include <dirent.h>
#include <stdio.h>

using namespace std;


void updateInput()
{
}

#define PIXEL_SIZE 2
int main(int argc, char** argv) 
{
	
 	sf::RenderWindow window(sf::VideoMode(SCR_W*PIXEL_SIZE, 
										  SCR_H*PIXEL_SIZE), 
							"gaemboy. HYPER ALPHA VERSION v0.001a");
	sf::RectangleShape shape;
	shape.setSize(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));
	shape.setFillColor(sf::Color::White);

	cpu.init();
	if (argc > 1) 
	{
		if (strcmp(argv[1], "-d") == 0) cpu.dbg_mode = true;
	}

	//char* rom = argv[2];
	//char gamepath[6+strlen(argv[2])+1];
	//strcpy(gamepath, "gaems/");
	//strcat(gamepath, argv[2]);
	cpu.load("mario.gb");

	sf::Clock clock;
	sf::Time dt;
	unsigned char ff = 0;
	
	clock.restart();
	while(1)
	{
	//	updateInput();
		cpu.cycle();

		for (int y = 0; y < SCR_H; ++y)
		{
			for (int x = 0; x < SCR_W; ++x)
			{	 
				shape.setFillColor(sf::Color(ff-x,ff-y,ff++));
				shape.setPosition(x*PIXEL_SIZE,y*PIXEL_SIZE);
				window.draw(shape);
			}
		}
		window.display();

		cpu.req_int(0);
	}		
}
