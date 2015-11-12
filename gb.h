#pragma once 

#include "datatype.h"

#define MEM_SIZE 0xFFFF +1

#define SCR_H 144
#define SCR_W 160
#define SCR_SIZE SCR_H*SCR_W

struct gb
{
//Memory
	byte mem[MEM_SIZE];

//Registers
	union
	{
		struct
		{
			byte F;
			byte A;
		};
		dbyte AF;
	};	
	union
	{
		struct
		{
			byte C;
			byte B;
		};
		dbyte BC;
	};	
	union
	{
		struct
		{
			byte E;
			byte D;
		};
		dbyte DE;
	};	
	union
	{
		struct
		{
			byte L;
			byte H;
		};
		dbyte HL;
	};

	dbyte sp;	
	dbyte pc;	

	byte opcode;

	bool ime_flag;

	int clk_t;
	int clk_m;

	int last_t;
	int last_m;

//CPU
	void init();
	void load(char* rom_name);
	void cycle();
	void exec_instr();
	void check_interrupts();	

	void req_int(byte I);

	bool dbg_mode;
	bool dbg_stepby;
	void dbg_fetch();

//LCD
	void lcd_setmode(byte mode);
	void lcd_update();	
	int lcd_mode_clk;
	//byte lcd_scanline; 0xFF44
	bool lcd_draw_flag;

//Helpers
	bool flag_z();
	bool flag_n();
	bool flag_h();
	bool flag_c();

	void reset_z();
	void reset_n();
	void reset_h();
	void reset_c();

	void set_z(byte b);
	void set_c(byte b);
	void set_h(byte b);
	void set_n(byte b);

	
};
