#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

#include "gb.h"
#include "datatype.h"

#include "memotech.h"

void wrong_opcode(byte op)
{
	printf("!!! Puto opcode esta puto mal: 0x%X\n", op);
	exit(0);
}

byte half_carry_sum(byte A, byte B)
{
	((((A&0xF + B&0xF)&0xF0)>>4)>0); 	
}
byte carry_sum(byte A, byte B)
{
	((((A&0x00FF + B&0x00FF)&0xF00)>>8)>0); 	
}

void gb::set_z(byte b){
	printf("\nZ..F: %X\n", F);
	printf("z) b:%X\tb<<3: %X\n",b,b<<7);
	reset_z();
	F = F|(b<<7);
	printf("F: %X\n", F);
}
void gb::set_n(byte b){
	printf("\nN..F: %X\n", F);
	printf("z) b:%X\tb<<2: %X\n",b,b<<6);
	reset_n();
	F = F|(b<<6);
	printf("F: %X\n", F);
}
void gb::set_h(byte b){
	printf("H..F: %X\n", F);
	printf("h) b:%X\tb<<1: %X\n",b,b<<5);
	reset_h();
	F = F|(b<<5);
	printf("F: %X\n", F);
}
void gb::set_c(byte b){
	reset_c();
	F = F|(b<<4);
}

void gb::reset_z(){ F &= 0x70;}
void gb::reset_n(){ F &= 0xB0;}
void gb::reset_h(){ F &= 0xD0;}
void gb::reset_c(){ F &= 0xE0;}

bool gb::flag_z(){ return (F)&0x80; }
bool gb::flag_n(){ return (F)&0x40; }
bool gb::flag_h(){ return (F)&0x20; }
bool gb::flag_c(){ return (F)&0x10; }

void gb::init()
{
	memset(mem, 0, MEM_SIZE);

	pc = 0x100;

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

	printf("\t%s\n", memo(opcode));

	printf("---- ---- ---- ----\n", memo(opcode));

	printf("AF: %X\t", AF);
	printf("Z N H C: %X %X %X %X\t", 
			flag_z(), flag_n(),
			flag_h(), flag_c());
	printf("sp: %X\t", sp);
	printf("pc: %X\n", pc);

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

	char dbg_key_cont;
	if (dbg_mode) std::cin >> dbg_key_cont;

	//Opcode stuff
	byte n1 = mem[pc+1];
	byte n2 = mem[pc+2];
	dbyte nnnn = (n2 << 8) | n1; 

	printf("n1:%X\tn2:%X\tnnnn:%X\n",n1,n2,nnnn);

	pc++;


	byte aux_carry = 0;
	switch (opcode)
	{
		case 0x00: //NOP
		break;	

		case 0x01: //LD BC, nnnn
			BC = nnnn;	
			pc++;
		break;

		case 0x02: //LD (BC), A
			mem[BC] = (AF) >> 8;
		break;

		case 0x03: //INC BC
			(BC)++;
		break;

		case 0x04: //INC B
			set_h((B&0xF) == 0);
			B++;
			set_z(B == 0);
			reset_n();
		break;

		case 0x05: //DEC B
			set_h((B&0xF)==0);
			B--;
			set_n(1);
			set_z(B==0);
		break;

		case 0x06: //LD B, d8
			B = n1;
			pc++;
		break;

		case 0x07: //RLCA
			reset_h();
			reset_n();
			set_c(A>>7);
			A = (A<<1) | ((A>>7)&1);
		break;

		case 0x0F: //RRCA
			set_c(A&1);
			A = ((A&1)<<7) | (A>>1);	
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

		case 0x11: //LD DE, d16
			DE = nnnn;
			pc++;
			pc++;
		break;

		case 0x14: //INC D
			set_n(0);
			set_h((D&0xF)==0xF);
			D++;
			set_z(D==0);
		break;

		case 0x15: //DEC D
			set_n(1);
			set_h((D&0xF)==0);
			D--;
			set_z(D==0);
		break;

		case 0x16: //LD D, d8
			D = n1;
			pc++;
		break;		

		case 0x17: //RLA
			aux_carry = A&0x80;
			A = (A << 1) & (flag_c());
			set_c(aux_carry);

			reset_z();
			reset_n();
			reset_h();
		break;

		case 0x18: //JR, r8
			pc--;
			pc += n1;
		break;

		case 0x1B: //DEC DE
			DE--;
		break;

		case 0x1E: //LD E, d8
			E = n1;
			pc++;
		break;

		case 0x1F: //RRA
			aux_carry = A&1;
			A = (A>>1) & (flag_c()<<7);
			set_c(aux_carry);

			reset_z();
			reset_n();
			reset_h();
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
			mem[HL] = A; 
			printf("(HL): %X\n", mem[HL]);
			HL--;
			A--;
		break;

		case 0x3E: //LD A, d8
			A = n1;
			pc++;
		break;

		case 0x60: //LD H, B
			H = B;
		break;

		case 0x61: //LD H, C
			H = C;
		break;

		case 0x73: //LD (HL), E
			mem[HL] = E;
		break;

		case 0x77: //LD (HL), A
			mem[HL] = A; 
			printf("(HL): %X\n", mem[HL]);
			A--;
		break;

		case 0x7A: //LD A, D
			A = D;
		break;

		case 0x80: //ADD A, B
			set_h(half_carry_sum(A,B));
			set_c(carry_sum(A,B));
			A = A+B;
			set_z(A==0);
			set_n(0);
		break;

		case 0x89: //ADC A, C
			reset_n();

			set_h(half_carry_sum(C, flag_c()));
			set_c(carry_sum(C, flag_c()));
			A += C + flag_c();
			set_z(A==0);
			
		break;

		case 0x90: //SUB B
			set_h(half_carry_sum(A, -B));
			set_c(carry_sum(A, -B));
			A -= B;
			set_z(A==0);
			set_n(1);
		break;

		case 0x92: //SUB D
			set_h(half_carry_sum(A, -D));
			set_c(carry_sum(A, -D));
			A -= D;
			set_z(A==0);
			set_n(1);
		break;

		case 0xAF: //XOR A
			A = A^A; 
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;

		case 0xB7: //OR A
			A = A|A; 
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;

		case 0xC3: //JMP b16
			pc = nnnn;
		break;
	
		case 0xDA: //JP C, a16
			if (flag_c()) pc = nnnn;
		break;
	
		case 0xDF: //RST 18H
			wrong_opcode(opcode);
			pc = mem[0x18];	
		break;

		case 0xE0: //LDH (a8), A
			mem[0xFF00+n1] = A;
		break;

		case 0xE3: //LD (C), A
			mem[0xFF00+C] = A;
		break;

		case 0xE9: //JP (HL)
			pc = mem[HL];
		break;
	
		case 0xEA: //LD (a16), A
			mem[nnnn] = A;
			pc++;
			pc++;
		break;
		
		case 0xF0: //LDH A, (a8)
			A = mem[0xFF00+n1];
		break;
		
		case 0xF9: //ld sp, hl
			sp = HL;
		break;

		case 0xF3: //LD A, (C)
			A = mem[0xFF00+C];
		break;

		case 0xFE: //CP d8
			set_z(A==n1);
			set_n(1);
			set_h(0);
			set_c(A<n1);
		break;

		case 0xFF: //RST 38h
			mem[--sp] = pc;
			pc = 0x3800;
		break;

		default:
				wrong_opcode(opcode);
	}


}
