#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

#include "gb.h"
#include "datatype.h"

void wrong_opcode(byte op)
{
	printf("!!! Puto opcode esta puto mal: 0x%X\n", op);
	exit(0);
}


bool gb::flag_z(){ return (F>>7)&1; }
bool gb::flag_n(){ return (F>>6)&1; }
bool gb::flag_h(){ return (F>>5)&1; }
bool gb::flag_c(){ return (F>>4)&1; }

void gb::init()
{
	memset(mem, 0, MEM_SIZE);

	pc = 0;

	dbg_mode = false;
}

void gb::load(char* rom_name)
{
	FILE* rom = fopen(rom_name, "rb");
	fseek(rom, 0L, SEEK_END);
	long size = ftell(rom);
	fseek(rom, 0L, SEEK_SET);

	fread(mem, size, 1, rom);

	fclose(rom);
}

void gb::cycle()
{
	//Read opcode
	opcode = mem[pc];

	//Debug stuff
	printf("\n> Executing instruction: 0x%X", opcode);
	printf("\tat location 0x%X\n", pc);

	printf("AF: %X\t", AF);
	printf("Z N H C: %X %X %X %X\n", 
			(F & (1<<7)) >> 7,
			(F & (1<<6)) >> 6,
			(F & (1<<5)) >> 5,
			(F & (1<<4)) >> 4
			);
	printf("HL: %X\n", HL);
	printf("BC: %X\n", BC);
	printf("DE: %X\n", DE);

	for (int i = -10, j = 0; i <= 10; i +=1, j++)
	{
		if (pc+i < 0) continue;

		byte m = (mem[pc+i]);
		if (i == 0) printf("->");
		printf("\t0x%X: %x\n", pc+i, m);
	}
	printf("...\n");

	char c;
	if (dbg_mode) std::cin >> c;

	//Opcode stuff
	byte n1 = mem[pc+1];
	byte n2 = mem[pc+2];
	dbyte nnnn = (n2 << 8) | n1; 

	printf("n1:%X\tn2:%X\tnnnn:%X\n",n1,n2,nnnn);

	pc++;

	switch (opcode)
	{
		case 0x00: //NOP
		break;	

		case 0x01: //LD BC, nnnn
			BC = nnnn;	
		break;

		case 0x02: //LD (BC), A
			mem[BC] = (AF) >> 8;
		break;

		case 0x03: //INC BC
			(BC)++;
		break;

		case 0x04:
			//BC = (((BC)>>8)+1) << 8 | BC & 0x0F;
		break;

		case 0x05: //DEC B
			B--;
			F |= (B == 0)&1 	<<7;
			F |= 1 				<<6;
			F |= (B ==0xF)&1 	<<5; //??
		break;

		case 0x06: //LD B, d8
			B = n1;
			pc++;
		break;

		case 0x0D: //DEC C
			C--;
			F |= (C)&1==0 	<<7;
			F |= 1 			<<6;
			F |= (C)==0xF 	<<5; //??	
		break;

		case 0x0E: //LD C, d8
			C = n1;
			pc++;
		break;

		case 0x15: //DEC D
			E = n1;
			pc++;
		break;

		case 0x1E: //LD E, d8
			E = n1;
			pc++;
		break;

		case 0x1F: //RRA
			A = ((A>>1)&0x7) | 
					 ((A&0x1) <<7);
		break;

		case 0xC3: //JMP b16
			pc = nnnn;
		break;
	
		case 0x20:
			if (flag_z()) break;
			pc--;
			pc += n1; 
		break;	
	
		case 0x21: //LD HL, d16
			HL = nnnn;
			pc++;
			pc++;
		break;

		case 0x32: //LD (HL-), A-;
			mem[HL--] = (A); 
			printf("(HL): %X\n", mem[HL]);
			A--;
		break;

		case 0x7A: //LD A, D
			A = D;
		break;

		case 0x89: //ADC A, C
			A += C + flag_c();
			
			F |= ((Z)==0)&1		<<7;
			F |= F&0x40			<<6;
			F |= ((Z)==0xF)&1 	<<5; 
			F |= ((((A&0xF + C&0xF)0xF0)>>4)>0)<<4; 		
		break;

		case 0xAF: //XOR A
			A = 0; 
		break;
		
		case 0xF9: //ld sp, hl
			sp = HL;
		break;

		default:
				wrong_opcode(opcode);
	}


}
