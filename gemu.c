// Greg's "GRisc" Emulator (gemu)

//-------------------------------------------------------

#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <conio.h>

//-------------------------------------------------------

// typedefs
typedef int16_t word;
typedef uint16_t uword;
typedef unsigned char byte;

//-------------------------------------------------------

// memory constants
#define memLen (2^16) // total virtual memory addresses
#define vidLen (80*30) // number of video memory addresses

#define vidMem 0xF69F // base of video character memory (grows upward)
#define keyboard 0xF69E // character input memory-mapped register (1 char)
#define gpInit 0xD69E // global data pointer initializer (grows upward)
#define pcInit 0xB69E // program counter initializer (grows upward)
#define spInit 0xB69D // stack pointer initializer (grows downward)

//-------------------------------------------------------

// interrupt variables
int newCharIn = 0;
int newCharOut = 0;

//-------------------------------------------------------

// register declarations
word registers[16];

int zero = 0;
int at = 1;
int pc = 2;
int ra = 3;
int sp = 4;
int gp = 5;
int v0 = 6;
int v1 = 7;
int a0 = 8;
int a1 = 9;
int s0 = 10;
int s1 = 11;
int t0 = 12;
int t1 = 13;
int t2 = 14;
int t3 = 15;

//-------------------------------------------------------

// system ram declaration
word mem[memLen];

//-------------------------------------------------------

void emulate(int progLen);
void interpret(word instruction);
void dispUpdate();

//-------------------------------------------------------

void lw(byte, byte);
void sw(byte, byte);
void beq(byte, byte, byte);
void bne(byte, byte, byte);
void lui(byte, byte);
void ori(byte, byte);
void sgt(byte, byte, byte);
void seq(byte, byte, byte);
void nand(byte, byte, byte);
void sll(byte, byte, byte);
void srl(byte, byte, byte);
void add(byte, byte, byte);
void sub(byte, byte, byte);
void mul(byte, byte, byte);
void divide(byte, byte, byte);

//-------------------------------------------------------

int fileLen(FILE* input);

void err()
{
	perror("./grisc");
	exit(1);
}

//-------------------------------------------------------

int main(int argc, char* argv[])
{
	int i = 0;
	FILE* program, *data;
	word instruction = 0x0000;
	word dat = 0x0000;
	int progLen = 0;

	// check arguments
	if(argc != 3)
	{
		printf("Usage: grisc [program.hex] [data.hex]\n");
		exit(1);
	}

	// initialize registers
	for(i = 0; i < 16; i++)
		registers[i] = 0x0000;

	registers[pc] = pcInit;
	registers[sp] = spInit;
	registers[gp] = gpInit;

	// open initializer files
	if( (program = fopen(argv[1], "r")) == NULL) err();
	if( (data = fopen(argv[2], "r")) == NULL) err();

	// clear out memory
	for(i = 0; i < 0xFFFF; i++)
		mem[i] = 0x0000;

	// load in program data
	for(i = pcInit; (i < 0x2000) && (i < fileLen(program)); i++)
	{
		fread(&instruction, 2, 1, program);
		mem[i] = instruction;
		progLen++;
	}

	// load in memory data
	for(i = gpInit; (i < 0x2000) && (i < fileLen(program)); i++)
	{
		fread(&dat, 2, 1, data);
		mem[i] = dat;
	}

	// begin emulation
	emulate(progLen);

	return 0;
}

//-------------------------------------------------------

int fileLen(FILE* input)
{
	fseek(input, 0, SEEK_END);
	int end = ftell(input);
	fseek(input, 0, SEEK_SET);
	int start = ftell(input);
	return end-start;
}

//-------------------------------------------------------

void emulate(int progLen)
{
	int i = 0;
	for(i = 0; i < progLen; i++)
	{
		interpret(mem[i]);
		dispUpdate();
		newCharIn = _kbhit();
	}
}

//-------------------------------------------------------

void dispUpdate()
{
	if(newCharOut == 1)
	{
		int i = 0;
		system("cls");

		for(i = vidMem; (i <= 0xFFFF); i++)
			printf("%c", (char)mem[i]);

		newCharOut = 0;
	}
}

//-------------------------------------------------------

void interpret(word instruction)
{
	byte op = (byte)((instruction >> 12) & 0x000F);
	byte a = (byte)((instruction >> 8) & 0x000F);
	byte b = (byte)((instruction >> 4) & 0x000F);
	byte c = (byte)(instruction & 0x000F);
	byte imm = (byte)((instruction >> 8) & 0x00FF);

	switch(op)
	{
		// lw a, b
		case 0x01:
			lw(a, b);
			break;

		// sw a, b
		case 0x02:
			sw(a, b);
			break;

		// beq a, b
		case 0x03:
			beq(a, b, c);
			break;

		// bne a, b
		case 0x04:
			bne(a, b, c);
			break;

		// lui a, imm
		case 0x05:
			lui(a, imm);
			break;

		// ori a, imm
		case 0x06:
			ori(a, imm);
			break;

		// sgt a, b, c
		case 0x07:
			sgt(a, b, c);
			break;

		// seq a, b, c
		case 0x08:
			seq(a, b, c);
			break;

		// nand a, b, c
		case 0x09:
			nand(a, b, c);
			break;

		// sll a, b, c
		case 0x0A:
			sll(a, b, c);
			break;

		// srl a, b, c
		case 0x0B:
			srl(a, b, c);
			break;

		// add a, b, c
		case 0x0C:
			add(a, b, c);
			break;

		// sub a, b, c
		case 0x0D:
			sub(a, b, c);
			break;

		// mul a, b, c
		case 0x0E:
			mul(a, b, c);
			break;

		// div a, b, c
		case 0x0F:
			divide(a, b, c);
			break;

		// nop
		default:
			break;
	} 

}

//-------------------------------------------------------

void lw(byte reg, byte loc)
{
	registers[reg] = mem[(uword)registers[loc]];
}

void sw(byte reg, byte loc)
{
	mem[(uword)registers[loc]] = registers[reg];

	if(registers[loc] > vidMem)
		newCharOut = 1;
}

void beq(byte a, byte b, byte dest)
{
	if(registers[a] == registers[b])
		registers[pc] += registers[dest];
}

void bne(byte a, byte b, byte dest)
{
	if(registers[a] != registers[b])
		registers[pc] += registers[dest];
}

void lui(byte reg, byte imm)
{
	registers[reg] = ((word)imm << 8) & 0xFF00;
}

void ori(byte reg, byte imm)
{
	registers[reg] = registers[reg] | (word)imm;
}

void sgt(byte a, byte b, byte c)
{
	if(registers[b] > registers[c])
		registers[a] = 1;
	else
		registers[a] = 0;
}

void seq(byte a, byte b, byte c)
{
	if(registers[b] == registers[c])
		registers[a] = 1;
	else
		registers[a] = 0;
}

void nand(byte a, byte b, byte c)
{
	registers[a] = ~(registers[b] & registers[c]);
}

void sll(byte a, byte b, byte c)
{
	registers[a] = (word)((uword)registers[b] << (uword)registers[c]);
}

void srl(byte a, byte b, byte c)
{
	registers[a] = (word)((uword)registers[b] >> (uword)registers[c]);
}

void add(byte a, byte b, byte c)
{
	registers[a] = registers[b] + registers[c];
}

void sub(byte a, byte b, byte c)
{
	registers[a] = registers[b] - registers[c];
}

void mul(byte a, byte b, byte c)
{
	registers[a] = registers[b] * registers[c];
}

void divide(byte a, byte b, byte c)
{
	registers[a] = registers[b] / registers[c];
}
