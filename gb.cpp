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

void gb::set_z(byte b)
{
	reset_z();
	F = F|(b<<7);
}
void gb::set_n(byte b)
{
	reset_n();
	F = F|(b<<6);
}
void gb::set_h(byte b)
{
	reset_h();
	F = F|(b<<5);
}
void gb::set_c(byte b)
{
	reset_c();
	F = F|(b<<4);
}

void gb::reset_z(){ F &= 0x70;}
void gb::reset_n(){ F &= 0xB0;}
void gb::reset_h(){ F &= 0xD0;}
void gb::reset_c(){ F &= 0xE0;}

bool gb::flag_z(){ return (F)&0x80;}
bool gb::flag_n(){ return (F)&0x40;}
bool gb::flag_h(){ return (F)&0x20;}
bool gb::flag_c(){ return (F)&0x10;}

void gb::req_int(byte I)
{
	switch(I)
	{
		case 0: //VBlank
			mem[0xFF0F] |= 1;
		break;
		
		case 1: //LCD
			mem[0xFF0F] |= 2;
		break;
		
		case 2: //Timer
			mem[0xFF0F] |= 4;
		break;

		case 3: //Serial
			mem[0xFF0F] |= 8;
		break;
		
		case 4: //Joypad
			mem[0xFF0F] |= 16;
		break;

		default:
		break;
	}
}

void gb::init()
{
	printf("> Cleaning %i bytes of memory...\n", MEM_SIZE);
	memset(mem, 0, MEM_SIZE);

	AF = 0x01B0;
	BC = 0x0013;
	DE = 0x00D8;
	HL = 0x014D;

	pc = 0x100;
	sp = 0xFFFE;	

	mem[0xFF05] = 0x00; //TIMA
	mem[0xFF06] = 0x00; //TMA
	mem[0xFF07] = 0x00; //TAC
	mem[0xFF10] = 0x80; //NR10
	mem[0xFF11] = 0xBF; //NR11
	mem[0xFF12] = 0xF3; //NR12
	mem[0xFF14] = 0xBF; //NR14
	mem[0xFF16] = 0x3F; //NR21
	mem[0xFF17] = 0x00; //NR22
	mem[0xFF19] = 0xBF; //NR24
	mem[0xFF1A] = 0x7F; //NR30
	mem[0xFF1B] = 0xFF; //NR31
	mem[0xFF1C] = 0x9F; //NR32
	mem[0xFF1E] = 0xBF; //NR33
	mem[0xFF20] = 0xFF; //NR41
	mem[0xFF21] = 0x00; //NR42
	mem[0xFF22] = 0x00; //NR43
	mem[0xFF23] = 0xBF; //NR30
	mem[0xFF24] = 0x77; //NR50
	mem[0xFF25] = 0xF3; //NR51
	mem[0xFF26] = 0xF1; //NR52
	mem[0xFF40] = 0x91; //LCDC
	mem[0xFF42] = 0x00; //SCY
	mem[0xFF43] = 0x00; //SCX
	mem[0xFF45] = 0x00; //LYC
	mem[0xFF47] = 0xFC; //BGP
	mem[0xFF48] = 0xFF; //OBP0
	mem[0xFF49] = 0xFF; //OBP1
	mem[0xFF4A] = 0x00; //WY
	mem[0xFF4B] = 0x00; //WX
	mem[0xFFFF] = 0x00; //IE


	dbg_mode = false;
}

void gb::load(char* rom_name)
{
	FILE* rom = fopen(rom_name, "rb");
	fseek(rom, 0L, SEEK_END);
	long size = ftell(rom);
	fseek(rom, 0L, SEEK_SET);

	if (dbg_mode)
		printf("> Loading rom with size %li\n", size);

	fread(mem, size, 1, rom);

	fclose(rom);
}

void gb::check_interrupts()
{
	//Interrupt?
	byte IF = mem[0xFF0F]; //Request
	byte IF_v = IF&1; //VBlank
	byte IF_l = IF&2; //LCD
	byte IF_t = IF&4; //Timer
	byte IF_s = IF&8; //Serial
	byte IF_j = IF&16; //Joypad

	byte IE = mem[0xFFFF]; //Enable
	byte IE_v = IE&1; //VBlank
	byte IE_l = IE&2; //LCD
	byte IE_t = IE&4; //Timer
	byte IE_s = IE&8; //Serial
	byte IE_j = IE&16; //Joypad

	if (IF_v && IE && ime_flag) 
	{
		mem[0xFF0F] &= 0xFE;
		ime_flag = false;
		mem[--sp] = (pc>>8)&0xFF;
		mem[--sp] = pc&0xFF;
		pc = 0x40;
	}		
}

void gb::cycle()
{
	//Fetch
	opcode = mem[pc];
	if (dbg_mode) dbg_fetch();

	//Execute
	exec_instr();
	check_interrupts();
}

// ---- ---- HERE BE DRAGONS! 

void gb::dbg_fetch()
{

	//Debug stuff
	printf("\n> Executing instruction: 0x%X", opcode);
	printf("\tat location 0x%X\n", pc);

	printf("\t%s\n", memo(opcode));

	printf("---- ---- ---- ----\n");

	printf("A: %X\t", A);
	printf("Z N H C\t\t"); 
	printf("sp: %X\n", sp);

	printf("BC: %X\t", BC);
	printf("%X %X %X %X\t", 
			flag_z(), flag_n(),
			flag_h(), flag_c());
	printf("\tpc: %X\n", pc);

	printf("DE: %X\t\t\t", DE);
	printf("ime: %d\tIF:%X IE:%X \n", 
			ime_flag, mem[0xFF0F]&0x1F, mem[0xFFFF]&0x1F);
	printf("HL: %X\n", HL);

	for (int i = -5, j = 0; i <= 5; i +=1, j++)
	{
		if (pc+i < 0) continue;

		byte m = (mem[pc+i]);
		if (i == 0) printf("->");
		printf("\t0x%X: %x\n", pc+i, m);
	}
	printf("...\n");

	if (dbg_mode) 
	{
		char dbg_key_cont;
		std::cin >> dbg_key_cont;
	}
}

void gb::exec_instr()
{
	//Opcode stuff
	byte n1 = mem[pc+1];
	byte n2 = mem[pc+2];
	dbyte nnnn = (n2 << 8) | n1; 

	pc++;

	byte aux_carry = 0;
	switch (opcode)
	{
//0x0
		case 0x00: //NOP
		break;	
		case 0x01: //LD BC, nnnn
			BC = nnnn;	
			pc+=2;
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
		case 0x08: //LD (a16), SP
			sp = mem[nnnn];	
			pc+=2;
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
		case 0x0F: //RRCA
			set_c(A&1);
			A = ((A&1)<<7) | (A>>1);	
		break;
//0x1
		case 0x10: //STOP 0
			printf("STOP 0");
			exit(EXIT_SUCCESS);
		break;
		case 0x11: //LD DE, d16
			DE = nnnn;
			pc+=2;
		break;
		case 0x12: //LD (DE), A
			mem[DE] = A;
		break;
		case 0x13: //INC DE	
			DE++;
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
			pc += (char)n1;
		break;
		case 0x1A: //LD A, (DE)
			A = mem[DE];
		break;
		case 0x1B: //DEC DE
			DE--;
		break;
		case 0x1C: //INC E
			set_h((E&0xF) == 0);
			E++;
			set_z(E == 0);
			reset_n();
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
//0x2
		case 0x20: //JR NZ, r8
			if (flag_z()) break;
			pc--;
			pc += (char)n1; 
		break;	
		case 0x21: //LD HL, d16
			HL = nnnn;
			pc+=2;
		break;
		case 0x22: //LD (HL+), A
			mem[HL++] = A;
		break;
		case 0x25: //DEC H
			set_h(half_carry_sum(H, -1));
			H--;
			set_z(H==0);
			set_n(1);
		break;
		case 0x28: //JR Z, r8
			if (flag_z()) pc += (char)n1  -1;
			else pc++;
		break;
		case 0x2A: //LD A, (HL+)
			A = mem[HL++];
		break;
		case 0x2C: //INC E
			set_h((E&0xF) == 0);
			E++;
			set_z(E == 0);
			reset_n();
		break;
//0x3X
		case 0x30: //JR NC, r8
			if (!flag_c()) pc = n1-1;
			else pc++;
		break;
		case 0x31: //LD SP, d16
			sp = nnnn;
			pc += 2;
		break;
		case 0x32: //LD (HL-), A-;
			mem[HL--] = A; 
			A--;
		break;
		case 0x33: //INC SP	
			sp++;	
		break;
		case 0x34: //INC (HL)
			set_h(half_carry_sum(mem[HL], 1));
			mem[HL]++;
			set_z(mem[HL]==0);
			set_n(0);
		break;
		case 0x35: //DEC (HL)
			mem[HL]--;
			set_z(mem[HL]==0);
			set_n(1);
			set_h((mem[HL]&0xF)==0xF);
		break;
		case 0x36: //LD (HL), d8
			mem[HL] = n1;
			pc++;
		break;
		case 0x3E: //LD A, d8
			A = n1;
			pc++;
		break;
//0x4X
		case 0x40: //LD B, B
			B = B;
		break;
		case 0x41: //LD B, C
			B = C;
		break;
		case 0x42: //LD B, D
			B = D;
		break;
		case 0x43: //LD B, E
			B = E;
		break;
		case 0x44: //LD B, H
			B = H;
		break;
		case 0x45: //LD B, L
			B = L;
		break;
		case 0x46: //LD B, (HL)
			B = mem[HL];
		break;
		case 0x47: //LD B, A
			B = A;
		break;
		case 0x48: //LD C, B
			C = B;
		break;
		case 0x49: //LD C, C
			C = C;
		break;
		case 0x4A: //LD C, D
			C = D;
		break;
		case 0x4B: //LD C, E
			C = E;
		break;
		case 0x4C: //LD C, H
			C = H;
		break;
		case 0x4D: //LD C, L
			C = L;
		break;
		case 0x4E: //LD C, (HL)
			C = mem[HL];
		break;
		case 0x4F: //LD C, A
			C = A;
		break;
//0x5x
		case 0x50: //LD D, B
			D = B;
		break;
		case 0x51: //LD D, C
			D = C;
		break;
		case 0x52: //LD D, D
			D = D;
		break;
		case 0x53: //LD D, E
			D = E;
		break;
		case 0x54: //LD D, H
			D = H;
		break;
		case 0x55: //LD D, L
			D = L;
		break;
		case 0x56: //LD D, (HL)
			D = mem[HL];
		break;
		case 0x57: //LD D, A
			D = A;
		break;
		case 0x58: //LD E, B
			E = B;
		break;
		case 0x59: //LD E, C
			E = C;
		break;
		case 0x5A: //LD E, D
			E = D;
		break;
		case 0x5B: //LD E, E
			E = E;
		break;
		case 0x5C: //LD E, H
			E = H;
		break;
		case 0x5D: //LD E, L
			E = L;
		break;
		case 0x5E: //LD E, (HL)
			E = mem[HL];
		break;
		case 0x5F: //LD E, A
			E = A;
		break;
//0x6X
		case 0x60: //LD H, B
			H = B;
		break;
		case 0x61: //LD H, C
			H = C;
		break;
		case 0x62: //LD H, D
			H = D;
		break;
		case 0x63: //LD H, E
			H = E;
		break;
		case 0x64: //LD H, H
			H = H;
		break;
		case 0x65: //LD H, L
			H = L;
		break;
		case 0x66: //LD H, (HL)
			H = mem[HL];
		break;
		case 0x67: //LD H, A
			H = A;
		break;
		case 0x68: //LD L, B
			L = B;
		break;
		case 0x69: //LD L, C
			L = C;
		break;
		case 0x6A: //LD L, D
			L = D;
		break;
		case 0x6B: //LD L, E
			L = E;
		break;
		case 0x6C: //LD L, H
			L = H;
		break;
		case 0x6D: //LD L, L
			L = L;
		break;
		case 0x6E: //LD L, (HL)
			L = mem[HL];
		break;
		case 0x6F: //LD L, A
			L = A;
		break;
//0x7X
		case 0x70: //LD (HL), B
			mem[HL] = B;
		break;
		case 0x71: //LD (HL), C
			mem[HL] = C;
		break;
		case 0x72: //LD (HL), D
			mem[HL] = D;
		break;
		case 0x73: //LD (HL), E
			mem[HL] = E;
		break;
		case 0x74: //LD (HL), H
			mem[HL] = H;
		break;
		case 0x75: //LD (HL), L
			mem[HL] = L;
		break;
		case 0x76: //HALT
			wrong_opcode(opcode); //to-do
		break;
		case 0x77: //LD (HL), A
			mem[HL] = A; 
		break;
		case 0x78: //LD A, B
			A = B;
		break;
		case 0x79: //LD A, C
			A = C;
		break;
		case 0x7A: //LD A, D
			A = D;
		break;
		case 0x7B: //LD A, E
			A = E;
		break;
		case 0x7C: //LD A, H
			A = H;
		break;
		case 0x7D: //LD A, L
			A = L;
		break;
		case 0x7E: //LD A, (HL)
			A = mem[HL];
		break;
		case 0x7F: //LD A, A
			A = A;
		break;
//0x8X
		case 0x80: //ADD A, B
			set_h(half_carry_sum(A,B));
			set_c(carry_sum(A,B));
			A = A+B;
			set_z(A==0);
			set_n(0);
		break;
		case 0x85: //ADC A, L
			reset_n();
			set_h(half_carry_sum(L, flag_c()));
			set_c(carry_sum(L, flag_c()));
			A += L + flag_c();
			set_z(A==0);
		break;
		case 0x89: //ADC A, C
			reset_n();
			set_h(half_carry_sum(C, flag_c()));
			set_c(carry_sum(C, flag_c()));
			A += C + flag_c();
			set_z(A==0);
		break;
//0x9X
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
		case 0x93: //SUB E
			set_h(half_carry_sum(A, -E));
			set_c(carry_sum(A, -E));
			A -= E;
			set_z(A==0);
			set_n(1);
		break;
		case 0x94: //SUB H
			set_h(half_carry_sum(A, -H));
			set_c(carry_sum(A, -H));
			A -= H;
			set_z(A==0);
			set_n(1);
		break;
		case 0x95: //SUB L
			set_h(half_carry_sum(A, -L));
			set_c(carry_sum(A, -L));
			A -= L;
			set_z(A==0);
			set_n(1);
		break;
		case 0x96: //SUB HL
			set_h(half_carry_sum(A, -mem[HL]));
			set_c(carry_sum(A, -mem[HL]));
			A -= mem[HL];
			set_z(A==0);
			set_n(1);
		break;
		case 0x97: //SUB A
			set_h(half_carry_sum(A, -A));
			set_c(carry_sum(A, -A));
			A -= A;
			set_z(A==0);
			set_n(1);
		break;
		

//0xAX
		case 0xA0: //AND B
			A = A&B;
			set_z(A==0);
			set_n(0);
			set_h(1);
			set_c(0);
		break;
		case 0xA1: //AND C
			A = A&C;
			set_z(A==0);
			set_n(0);
			set_h(1);
			set_c(0);
		break;
		case 0xA2: //AND D
			A = A&D;
			set_z(A==0);
			set_n(0);
			set_h(1);
			set_c(0);
		break;
		case 0xA3: //AND E
			A = A&E;
			set_z(A==0);
			set_n(0);
			set_h(1);
			set_c(0);
		break;
		case 0xA4: //AND H
			A = A&H;
			set_z(A==0);
			set_n(0);
			set_h(1);
			set_c(0);
		break;
		case 0xA5: //AND L
			A = A&L;
			set_z(A==0);
			set_n(0);
			set_h(1);
			set_c(0);
		break;
		case 0xA6: //AND (HL)
			A = A&mem[HL];
			set_z(A==0);
			set_n(0);
			set_h(1);
			set_c(0);
		break;
		case 0xA7: //AND A
			A = A&A;
			set_z(A==0);
			set_n(0);
			set_h(1);
			set_c(0);
		break;
		case 0xA8: //XOR B
			A = A^B; 
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xA9: //XOR C
			A = A^C; 
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xAA: //XOR D
			A = A^D; 
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xAB: //XOR E
			A = A^E; 
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xAC: //XOR H
			A = A^H; 
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xAD: //XOR L
			A = A^L; 
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xAE: //XOR HL
			A = A^mem[HL]; 
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xAF: //XOR A
			A = A^A; 
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
//0xBX
		case 0xB0: //OR B
			A = A | B;
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xB1: //OR C
			A = A | C;
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xB2: //OR D
			A = A | D;
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xB3: //OR E
			A = A | E;
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xB4: //OR H
			A = A | H;
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xB5: //OR L
			A = A | L;
			reset_n();
			reset_h();
			reset_c();
			set_z(A==0);
		break;
		case 0xB6: //OR (HL)
			A = A | mem[HL];
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
//0xCX
		case 0xC0: //RET NZ
			if (flag_z()) break;
			pc = (mem[sp+1]<<8) | mem[sp];
			sp+=2;
		break;
		case 0xC3: //JMP b16
			pc = nnnn;
		break;
		case 0xC5: //PUSH BC
			mem[--sp] = BC;
		break;
		case 0xCD: //CALL a16
			mem[--sp] = pc+2;
			pc = nnnn;
		break;
//0xDX
		case 0xDA: //JP C, a16
			if (flag_c()) pc = nnnn;
		break;
		case 0xD7: //RST 10H
			mem[--sp] = pc;
			pc = (A<<4)|0x10;
		break;
		case 0xDF: //RST 18H
			mem[--sp] = pc;
			pc = (A<<4)|0x18;
		break;
//0xEX
		case 0xE0: //LDH (a8), A
			mem[0xFF00+n1] = A;
			pc++;
		break;
		case 0xE3: //LD (C), A
			mem[0xFF00+C] = A;
		break;
		case 0xE6: //AND d8
			set_n(0);
			set_h(1);
			set_c(0);
			A = A^n1;
			set_z(A==0);
			pc++;
		break;
		case 0xE9: //JP (HL)
			pc = mem[HL];
		break;
		case 0xEA: //LD (a16), A
			mem[nnnn] = A;
			pc++;
			pc++;
		break;
		case 0xEE: //XOR d8
			A = A^n1;
			set_z(A==0);
			reset_n();
			reset_c();
			reset_h();
		break;
		case 0xEF: //RST 28h
			mem[--sp] = ((pc&0xFF00)>>8)&0xFF;
			mem[--sp] = pc&0xFF;
			pc = 0x0028;
		break;
//0xFX
		case 0xF0: //LDH A, (a8)
			A = mem[0xFF00+n1];
			pc++;
		break;
		case 0xF2: //LD A, (C)
			A = mem[0xFF00+C];
		break;
		case 0xF3: //DI
			//mem[0xFFFF] = 0x00;
			ime_flag = false;
		break;
		case 0xF4: //wrong
			wrong_opcode(opcode);
		break;
		case 0xF5: //PUSH AF
			mem[--sp] = A;
			mem[--sp] = F;
		break;
		case 0xF7: //RST 30h
			mem[--sp] = ((pc&0xFF00)>>8)&0xFF;
			mem[--sp] = pc&0xFF;
			pc = 0x0030;
		break;
		case 0xF9: //ld sp, hl
			sp = HL;
		break;
		case 0xFA: //LD A, (a16)
			A = nnnn;
			pc += 2;
		break;
		case 0xFB: //EI
			//mem[0xFFFF] = 0xFF;
			ime_flag = true;
		break;
		case 0xFE: //CP d8
			set_z(A==n1);
			set_n(1);
			set_h(0);
			set_c(A<n1);
			pc++;
		break;
		case 0xFD: //wrong
			wrong_opcode(opcode);
		break;
		case 0xFF: //RST 38h
			mem[--sp] = ((pc&0xFF00)>>8)&0xFF;
			mem[--sp] = pc&0xFF;
			pc = 0x0038;
		break;

//
		default:
				wrong_opcode(opcode);
	}


}
