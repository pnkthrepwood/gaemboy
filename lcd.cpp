#include "gb.h"

void gb::lcd_setmode(byte mode)
{
	mem[0xFF41] |= mode;
}

void gb::lcd_update()
{
	lcd_mode_clk += last_t;
	
	switch(mem[0xFF41]&3) //STAT
	{
		case 2: //OAM read
			if (lcd_mode_clk >= 80)
			{
				lcd_mode_clk = 0;
				lcd_setmode(3);
			}
		break;
	
		case 3: //VRAM scanline
			if (lcd_mode_clk >= 172)
			{
				lcd_mode_clk = 0;
				lcd_setmode(0);
			}
		break;
		
		case 1: //HBlank
			if (lcd_mode_clk >= 204)
			{
				lcd_mode_clk = 0;
				lcd_scanline++;

				if (lcd_scanline == 143)
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
	
		case 0: //VBlank
			if (lcd_mode_clk >= 456)
			{
				lcd_mode_clk = 0;
				lcd_scanline++;
			
				if (lcd_scanline > 153)
				{
					lcd_setmode(2);
					lcd_scanline = 0;
				}

				lcd_setmode(3);
			}
		break;	
	}
	
}
