- Registers:  
    - (00) $zero // holds hardware "0"  
    - (01) $at // assembler translation  
    - (02) $pc // program counter  
    - (03) $ra // return address
    - (04) $sp // stack pointer
    - (05) $gp // global data pointer
    - (06) $v0 // return value register  
    - (07) $v1 // return value register  
    - (08) $a0 // argument register  
    - (09) $a1 // argument register  
    - (10) $s0 // saved register  
    - (11) $s1 // saved register  
    - (12) $t0 // temporary register  
    - (13) $t1 // temporary register  
    - (14) $t2 // temporary register  
    - (15) $t3 // temporary register    

- Instruction Format:  
    - R  
{opcode}{regA}{regB}{regC}  
{4 bits}{4bit}{4bit}{4bit}  
    - I  
{opcode}{regA}{immediate}  
{4 bits}{4bit}{8 bits   }  
    - M  
{opcode}{regA}{regB}{junk}  
{4 bits}{4bit}{4bit}{4bit}    

- Instruction Set:  
    - lw regA, regB // load word (M)  
    - sw regA, regB // store word (M)  
    - beq regA, regB, regC // branch if equal (R)  
    - bne regA, regB, regC // branch if not equal (R)  
    - lui regA, imm // load upper immediate (I)  
    - ori regA, imm // or immediate (I)  
    - sgt regA, regB, regC // set greater than (R)  
    - seq regA, regB, regC // set equal to (R)  
    - nand regA, regB, regC // nand (R)  
    - sll regA, regB, regC // shift left logical (R)  
    - srl regA, regB, regC // shift right logical (R)  
    - add regA, regB, regC // add (R)  
    - sub regA, regB, regC // subtract (R)  
    - mul regA, regB, regC // multiply (R)  
    - div regA, regB, regC // divide (R)    

- Planned Pseudo-Instructions: (Assembler Subroutine to Translate)  
    - move  
    - load immediate  
    - load address  
    - set less than  
    - branch greater than  
    - branch less than  
    - jump  
    - jump register  
    - jump and link  
    - bitwise and  
    - bitwise or  
    - bitwise not  
    - bitwise xor  
    - bitwise and immediate  
    - bitwise not immediate  
    - bitwise xor immediate  
    - bitwise nand immediate  
    - shift left logical immediate  
    - shift right logical immediate  
    - add immediate  
    - sub immediate  
    - multiply immediate  
    - divide immediate  

