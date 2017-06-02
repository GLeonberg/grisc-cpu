# Grisc CPU
A Custom Risc CPU

Features:    

- Memory Mapped I/O  
    - vidMem 0xF69F // base of video character memory (grows upward)  
    - keyboard 0xF69E // character input memory-mapped register (1 char)  
    - gpInit 0xD69E // global data pointer initializer (grows upward)  
    - pcInit 0xB69E // program counter initializer (grows upward)  
    - spInit 0xB69D // stack pointer initializer (grows downward)      
- 16-bit Architecture  
    - 16 bit words  
    - 16 bit instructions    
- RISC Architecture  
    - Load/Store  
    - 16 instructions       

Also Includes: (Software Toolchain)  
- gemu: An emulator written in C
- gasm: An assembler written in C

