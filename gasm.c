// Greg's "Grisc" Assembler (gasm)

// maximum 80 characters per line
// comments and whitespace ignored EXCEPT for inside an instruction
// instructions MUST be on one line each

// source code forms:
//	"_tag: operation reg, reg, reg // comment here"
//	"_operation reg, reg, reg, // comment here"
//  " // comment here"

//-------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __linux__ 
	#include <sys/unistd.h>
#else
	#include <process.h>
#endif

//-------------------------------------------------------

#define lineLen 80
#define maxTags 2000

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

// translation tables
char* registers[16];

registers[0] = "$zero";
registers[1] = "$at";
registers[2] = "$pc";
registers[3] = "$ra";
registers[4] = "$sp";
registers[5] = "$gp";
registers[6] = "$v0";
registers[7] = "$v1";
registers[8] = "$a0";
registers[9] = "$a1";
registers[10] = "$s0";
registers[11] = "$s1";
registers[12] = "$t0";
registers[13] = "$t1";
registers[14] = "$t2";
registers[15] = "$t3";

char* commands[16];

commands[0] = "nop";
commands[1] = "lw";
commands[2] = "sw";
commands[3] = "beq";
commands[4] = "bne";
commands[5] = "lui";
commands[6] = "ori";
commands[7] = "sgt";
commands[8] = "seq";
commands[9] = "nand";
commands[10] = "sll";
commands[11] = "srl";
commands[12] = "add";
commands[13] = "sub";
commands[14] = "mul";
commands[15] = "div";

//-------------------------------------------------------

// tag stuff
typedef struct tag tag;
struct tag
{
	char title[lineLen];
	word address;
};

//-------------------------------------------------------

// true instruction translation
byte regNum(char* regName);
byte opNum(char* operation);

// main functions
void strip(FILE* input, FILE* output);
void expand(FILE* input, FILE* output);
void populateTags(FILE* input, tag* dict);
void translate(FILE* input, FILE* output, tag* dict);

//-------------------------------------------------------

void err()
{
	perror("./gasm");
	exit(1);
}

int main(int argc, char* argv[])
{
	int i = 0;
	FILE *input = NULL, *output = NULL, *temp = NULL, *tempB = NULL, *tempC = NULL;
	word instruction = 0x0000;
	tag* tags = malloc(sizeof(tag)*maxTags);

	// clear out tags memory
	for(i = 0; i < maxTags; i++)
	{
		tags[i].address = 0x0000;
		bzero(&tags[i].title, sizeof(&tags[i].title));
	}

	// check arguments
	if(argc < 3)
	{
		printf("Usage: gasm [input] [output]\n\n");
		exit(1);
	}

	// open input, temp, and output files
	if((input = fopen(argv[1], "r")) == NULL) err();
	if((temp = fopen("temp.txt", "rw")) == NULL) err();
	if((tempB = fopen("tempB.txt", "rw")) == NULL) err();
	if((output = fopen(argv[2], "w")) == NULL) err();

	// go through input file and strip whitespace and comments, store in temp file
	strip(input, temp);

	// go through temp file and expand pseudo-instructions, store in tempB file
	expand(temp, tempB);
	
	// go through tempB file and create dict of tags & instruction addresses
	populateTags(tempB, tags);
	
	// go through tempB file and convert instructions into binary form, store in output file
	translate(tempB, output, tags);

	// free and close everything and exit program
	fclose(input);
	fclose(temp);
	fclose(tempB);
	fclose(output);
	
	#ifdef __linux__ 
		system("rm temp.txt");
		system("rm tempB.txt");
	#else
		system("del temp.txt");
		system("del tempB.txt");
	#endif
	
	free(tags);
	return 0;
}

//-------------------------------------------------------

void strip(FILE* input, FILE* output)
{
	rewind(input);
	char* newLine = NULL;
	char line[lineLen+1];

	// for each line in the file
	while(!feof(input))
	{
		// read line into buffer
		bzero(&line, sizeof(line));
		fgets(line, sizeof(line)-1, input);
		line[sizeof(line)] = 0;

		// check if line contains an instruction and remove leading spaces
		strsep(&newLine, "_")
		if(line != NULL)
		{
			// remove comments and trailing spaces if they exist
			newLine = strsep(&newLine, "//");
			newLine = strsep(&newLine, " ");

			// write stripped line to file
			strcat(newLine, "\n");
			fwrite(newLine, strlen(newLine), 1, output);
		}
	}
}

//-------------------------------------------------------

void expand(FILE* input, FILE* output)
{
	return;
}

//-------------------------------------------------------

void populateTags(FILE* input, tag* dict)
{
	rewind(input);

	char* newLine;
	char line[lineLen+1];
	word pc = pcInit;
	int index = 0;

	// for each line in the file
	while(!feof(input))
	{
		char* title = NULL;

		// read line into buffer
		bzero(&line, sizeof(line));
		fgets(line, sizeof(line)-1, input);
		line[sizeof(line)] = 0;

		// get tag if it exists, remove trailing spaces
		title = strsep(&line, ":");

		if(line != NULL)
		{
			// strip leading spaces
			while(title[0] = ' ')
				title++;

			// add tag to dict with pc number
			strcpy(dict[index].title, title);
			dict[index++].address = pc + 0x0001;
		}

		// increment pc
		pc += 0x0001;
	}

}

//-------------------------------------------------------

// TODO: Implement "la" instruction, "lw" & "sw" data tag matching
// TODO: add "nop" support?

void translate(FILE* input, FILE* output, tag* dict)
{
	rewind(input);
	word pc = pcInit;

	// for each line in the file
	while(!feof(input))
	{
		int i = 0;
		int immYes = 0, tagYes = 0; // flags
		char line[lineLen];
		char* tag = NULL, *ir = NULL;
		char *operation = NULL, *regA = NULL, *regB_Imm = NULL, *regC_Tag = NULL;
		word instruction = 0x0000, tagLoc = 0x0000;
		byte opcode = 0x00, A = 0x00, B = 0x00, C = 0x00, Imm = 0x00;

		// read line into buffer
		bzero(&line, sizeof(line));
		fgets(line, sizeof(line)-1, input);
		line[sizeof(line)] = 0;

		// strip away potential tag
		tag = strsep(&line, ":");
		ir = (line == NULL) ? tag : line;

		// separate components of instruction line
		operation = strsep(&ir, " ");
		regA = strsep(&ir, ", ");
		regB_Imm = strsep(&ir, ", ");
		regC_Tag = ir;

		// translate opcode to binary
		opcode = opNum(operation);

		// translate regA to binary
		A = regNum(regA);

		// check if immediate exists and fill out data for B and Imm
		if((B = regNum(regB_Imm)) = 0xFF) 
		{
			immYes = 1;
			Imm = (byte)(atoi(regB_Imm));
		}

		// check if tag exists and fill out data for C
		if(regC_Tag != NULL)
			if((C = regNum(regC_Tag)) = 0xFF) 
				tagYes = 1;

		// if tag was found, handle it
		if(tagYes)
		{
			byte lui_op = opNum("lui");
			byte ori_op = opNum("ori");
			byte upper = 0x00, lower = 0x00;

			// get tag address
			for(i = 0; i < maxTags; i++)
			{
				if(strcmp(regC_Tag, dict[i].title) == 0)
				{
					tagLoc = dict[i].address;
					break;
				}
			}

			// alter tag locations that occur afterwards to compensate
			for(i; i < maxTags; i++)
				dict[i].address += 0x0002;

			// split upper and lower components
			upper = (byte)((tagLoc >> 8)  & 0x00FF);
			lower = (byte)(tagLoc & 0x00FF);

			// write lui instruction
			instruction = (((word)opNum("lui")) << 12) & 0xF000;
			instruction = instruction | ((((word)regNum("$at")) << 8) & 0x0F00);
			instruction = instruction | (((word)upper) & 0x00FF);
			fwrite((void*)&instruction, sizeof(word), 1, output);

			// write ori instruction
			instruction = (((word)opNum("ori")) << 12) & 0xF000;
			instruction = instruction | ((((word)regNum("$at")) << 8) & 0x0F00);
			instruction = instruction | (((word)lower) & 0x00FF);
			fwrite((void*)&instruction, sizeof(word), 1, output);

			// write altered branch instruction
			instruction = (((word)opCode) << 12) & 0xF000;
			instruction = instruction | ((((word)A)) & 0x0F00);
			instruction = instruction | ((((word)B)) & 0x00F0);
			instruction = instruction | ((((word)regNum("$at"))) & 0x000F);
			fwrite((void*)&instruction, sizeof(word), 1, output);
		}

		// normal instructions
		else
		{

			// lw, sw
			if( (opcode == 0x01) || (opcode == 0x02) )
			{
				instruction = (((word)opCode) << 12) & 0xF000;
				instruction = instruction | ((((word)A)) & 0x0F00);
				instruction = instruction | ((((word)B)) & 0x00F0);
				fwrite((void*)&instruction, sizeof(word), 1, output);
			}

			// lui, ori
			else if((opcode == 0x05) || (opcode == 0x06))
			{
				instruction = (((word)opCode) << 12) & 0xF000;
				instruction = instruction | ((((word)A)) & 0x0F00);
				instruction = instruction | ((((word)Imm)) & 0x00FF)
				fwrite((void*)&instruction, sizeof(word), 1, output);
			}

			// R type
			else
			{
				instruction = (((word)opCode) << 12) & 0xF000;
				instruction = instruction | ((((word)A)) & 0x0F00);
				instruction = instruction | ((((word)B)) & 0x00F0);
				instruction = instruction | ((((word)C)) & 0x000F);
				fwrite((void*)&instruction, sizeof(word), 1, output);
			}

		}

		// increment pc
		pc += 0x0001;
	}

}

//-------------------------------------------------------

byte opNum(char* operation)
{
	int i = 0;
	byte num = 0x00;

	for(i = 0; i < 16; i++)
		if( strcmp(operation, commands[i]) == 0)
			num = (byte)i;

	return num;
}

//-------------------------------------------------------

byte regNum(char* regName)
{
	int i = 0;
	byte num = 0xFF;

	for(i = 0; i < 16; i++)
		if( strcmp(regName, registers[i]) == 0)
			num = (byte)i;

	return num;
}

//-------------------------------------------------------


