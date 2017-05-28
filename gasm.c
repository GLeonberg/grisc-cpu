// Greg's "Grisc" Assembler (gasm)

// maximum 80 characters per line
// comments and whitespace ignored
// instructions MUST be on one line each

// source code forms:
//	"_tag: operation reg, reg, reg // comment here"
//	"_operation reg, reg, reg, // comment here"
//  " // comment here"

//-------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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
// text parsing and recognition
char* getLine(FILE*);
int isInstruction(char* line)
int isPseudo(char* line);

// psuedo instruction translation
word* pseudoConvert(char* line)

// true instruction translation
byte regNum(char* regName);
byte opNum(char* operation);
int getType(byte opcode);

// instruction packing
word makeR(byte opNum, byte regA, byte regB, byte regC);
word makeI(byte opNum, byte regA, byte imm);

// command code creation
char* getOp(char* line);
char* getA(char* line);
char* getB(char* line);
char* getC(char* line);
char* getImm(char* line);

// main functions
void strip(FILE* input, FILE* output);
void expand(FILE* input, FILE* output);
void populateTags(FILE* input, tag* dict);
void translate(FILE* input, FILE* output);

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
	translate(tempB, output);

	// free and close everything and exit program
	fclose(input);
	fclose(temp);
	fclose(tempB);
	fclose(output);
	free(tags);
	return 0;
}

//-------------------------------------------------------

void strip(FILE* input, FILE* output)
{
	rewind(input);
	char* newLine;
	char line[lineLen+1];

	// for each line in the file
	while(!feof(input))
	{
		// read line into buffer
		bzero(&line, sizeof(line));
		fgets(line, sizeof(line), input);
		line[sizeof(line)] = 0;

		// check if line contains an instruction
		strsep(&newLine, "_")
		if(line != NULL)
		{
			// remove comments if they exists
			newLine = strsep(&newLine, "//");

			// write stripped line to file
			strcat(newLine, "\n");
			fwrite(newLine, strlen(newLine), 1, output);
		}
	}
}

//-------------------------------------------------------

void expand(FILE* input, FILE* output)
{


}

//-------------------------------------------------------

void populateTags(FILE* input, tag* dict)
{
	rewind(input);

	char* newLine;
	char line[lineLen+1];
	word pc = pcInit;

	// for each line in the file
	while(!feof(input))
	{
		// read line into buffer
		bzero(&line, sizeof(line));
		fgets(line, sizeof(line), input);
		line[sizeof(line)] = 0;

		// increment pc
		pc += 0x0001;
	}

}

//-------------------------------------------------------

//-------------------------------------------------------

//-------------------------------------------------------

//-------------------------------------------------------
