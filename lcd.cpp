#include "gb.h"

#include <stdio.h>
#include <iostream>

void gb::lcd_setmode(byte mode)
{

	mem[0xFF41] = (mem[0xFF41]&0xFC)|mode;
}

void gb::lcd_update()
{
	lcd_mode_clk += last_t;
	
	byte* lcd_scanline = &mem[0xFF44];

	//STAT, Bit 6: LYC ?= LY 
	mem[0xFF41] &= 0xFB;
	if (mem[0xFF44] == mem[0xFF45]) 
	{
		mem[0xFF41] |= 0x04;
		if (mem[0xFF41]&0x40) mem[0xFF0F] |= 0x02;
	}

	if (dbg_mode) 
	{
		printf("\nscanzor %i\n", (*lcd_scanline));
		printf("mode %X\tclock %i\n", mem[0xFF41]&3, lcd_mode_clk);
	}

	switch(mem[0xFF41]&3) //STAT
	{
		case 2: //OAM read
			if (lcd_mode_clk >= 80)
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
			if (lcd_mode_clk >= 172)
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
			if (lcd_mode_clk >= 204)
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
			if (lcd_mode_clk >= 456)
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
