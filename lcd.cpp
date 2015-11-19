#include "gb.h"

#include <stdio.h>
#include <iostream>

inline char* lcd_modename(int mode)
{
	switch(mode)
	{
		case 0:return "VBlank"; 
		case 3:return "VRAM scanline"; 
		case 2:return "OAM read"; 
		case 1:return "HBlank"; 
	}
}

inline int lcd_modecyc(int mode)
{
	switch(mode)
	{
		case 0:return 204; 
		case 3:return 172; 
		case 2:return 80; 
		case 1:return 456;
	}
}

void gb::lcd_setmode(byte mode)
{

	mem[0xFF41] = (mem[0xFF41]&0xFC)|mode;
}

void gb::lcd_update()
{
	lcd_mode_clk += last_t;
	
	byte* lcd_scanline = &mem[0xFF44];
	int lcd_mode = mem[0xFF41]&3;

	//STAT, Bit 6: LYC ?= LY 
	mem[0xFF41] &= 0xFB;
	if (mem[0xFF44] == mem[0xFF45]) 
	{
		mem[0xFF41] |= 0x04;
		if (mem[0xFF41]&0x40) mem[0xFF0F] |= 0x02;
	}

	if (dbg_mode) 
	{
		printf(">LCD Status\n");
		printf("scanline[0xFF44]: %i\n", (*lcd_scanline));
		printf("mode: %X -%s-\n", lcd_mode, lcd_modename(lcd_mode));
		printf("clock: %i/%i\n", lcd_mode_clk, lcd_modecyc(lcd_mode));
	}

	switch(lcd_mode) //STAT
	{
		case 2: //OAM read
			if (lcd_mode_clk >= lcd_modecyc(2))
			{
				if (mem[0xFF41]&0x20)
				{
					mem[0xFF0F] |= 0x02;
				}
				
				lcd_mode_clk = 0;
				lcd_setmode(3);
			}
		break;
	
		case 3: //VRAM scanline
			if (lcd_mode_clk >= lcd_modecyc(3))
			{
				if (mem[0xFF41]&0x10)
				{
					mem[0xFF0F] |= 0x02;
				}

				lcd_mode_clk = 0;
				lcd_setmode(0);
			}
		break;
		
		case 0: //HBlank
			if (lcd_mode_clk >= lcd_modecyc(0))
			{
				if (mem[0xFF41]&0x08)
				{
					mem[0xFF0F] |= 0x02;
				}
				lcd_mode_clk = 0;
				(*lcd_scanline)++;


				if ((*lcd_scanline) == 143)
				{
					lcd_setmode(1);
					lcd_draw_flag = true;
				}
				else
				{
					lcd_setmode(2);
				}
			}
		break;
	
		case 1: //VBlank
			if (lcd_mode_clk >= lcd_modecyc(1))
			{

				lcd_mode_clk = 0;
				if (*lcd_scanline == 143)
				{
					mem[0xFF0F] |= 0x01; //Req VBlank
				}

				(*lcd_scanline)++;

				std::cout << ">>vblank now!" << std::endl;
				printf("STAT: 0x%X\t", mem[0xFF40]);
				for (int i = 0; i < 8; i++)
					printf("%d",(mem[0xFF40]>>(7-i))&1);
				char cc;
				std::cin >> cc;
				if (cc == ',') {
					dbg_mode = dbg_stepby = true;
				}
	
				if ((*lcd_scanline) > 153)
				{
					lcd_setmode(2);
					(*lcd_scanline) = 0;
				}

				lcd_setmode(3);
			}
		break;	
	}
	
}
