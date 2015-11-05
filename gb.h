#pragma once 

#include "datatype.h"

#define MEM_SIZE 65536

#define SCR_H 32
#define SCR_W 64
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

	dbyte I;
	dbyte R;

	dbyte sp;	
	dbyte pc;	

	byte opcode;

	void init();
	void load(char* rom_name);
	void cycle();

	bool dbg_mode;

	bool flag_z();
	bool flag_n();
	bool flag_h();
	bool flag_c();
};
