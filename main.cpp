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


int main(int argc, char** argv) 
{
	

 	sf::RenderWindow window(sf::VideoMode(SCR_W*10, 
										  SCR_H*10), 
							"gaemboy. HYPER ALPHA VERSION v0.001a");

	sf::RectangleShape shape;
	shape.setSize(sf::Vector2f(10, 10));
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

	while(1)
	{
	//	updateInput();
		cpu.cycle();
	}		
}
