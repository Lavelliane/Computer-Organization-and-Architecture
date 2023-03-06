//Author: Jhury Kevin Lastre
//BS CpE - 3
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<limits.h>
#include <math.h>


int CU(void);
void initMemory(void);
void display(unsigned int PC, unsigned int IR, unsigned char inst_code, unsigned int operand, 
			unsigned char BUS, unsigned int ADDR, unsigned int MAR, unsigned int IOAR, unsigned int IOBR, unsigned char CONTROL);
void MainMemory(void);
void IOMemory(void);
unsigned char twosComp(unsigned char data);

int ALU(void);
void displayALU(unsigned char temp_ACC, unsigned char temp_OP2, unsigned char control_signals);
void decToBinary(unsigned int n, int bits);
unsigned char twosComp(unsigned char data);
bool detectCarry(unsigned char x, unsigned char y);
bool isNegative(unsigned char x);
void setFlags(unsigned char ACC);
void booths(unsigned char q, unsigned char m);
void displayCycle(unsigned char acc, unsigned char q, unsigned char q_prev, unsigned char m);

unsigned char dataMemory[2048]; // 2048 = 2^11 (where 11 is number of address bits)
unsigned char ioBuffer[32];
unsigned int ADDR;
unsigned char BUS;
unsigned char CONTROL;
unsigned char FLAGS;
bool IOM = 0;
bool RW = 0;
bool OE = 0;

int main()
{
	
	
    initMemory(); // this function populates the memory

    if (CU() == 1) // check the return value
        printf("Program run successfully !");
    else
        printf("Error encountered, program terminated !");
        
    return 0;
}
	
int CU(void)
{
    unsigned int PC, IR, MAR, MBR, IOAR, IOBR, operand, temp_ACC;
    unsigned char inst_code, temp_OP1, temp_OP2;
    unsigned int dataMemoryStart = 0x000;
    unsigned int programMemoryStart = 0x400;
    
    unsigned int Increment, Fetch, IO, Memory;

    //initialize Program Counter
    PC = dataMemoryStart;
    
	MainMemory();
	
    while(inst_code != 0x1F){
    	/* setting external control signals */
    	CONTROL = inst_code;
    	IOM = 1;
    	RW = 0;
    	OE = 1;
    	/* Fetching Instruction (2 cycle) */
    	Fetch = 1;
    	IO = 0;
    	Memory = 0;
        //fetch upper byte
        ADDR = PC;
        MainMemory();
        if(Fetch == 1){
        	IR = dataMemory[PC];
	        IR = IR << 8;
	        PC++;
	        ADDR = PC;
		}
        //fetch lower byte
        MainMemory();
        if(Fetch == 1){
        	IR = IR | BUS;
        	PC++;
		}
        
        //decode instruction
        inst_code = IR >> 11;
        operand = IR & 0x07FF;
        
        // ------------------ CASES ---------------------
       if(inst_code == 0x01)
        {
        	MAR = operand;
        	/*setting local control signals */
        	Fetch = 0;
        	Memory = 1;
        	IO = 0;
        	/*setting external control signals */
        	CONTROL = inst_code;
        	IOM = 1;
        	RW = 1;
        	OE = 1;
        	ADDR = MAR;
        	if(Memory)
        		BUS = MBR;
        	MainMemory();
        	/* echo */
        	display(PC, IR, inst_code, operand, BUS, ADDR, MAR, IOAR, IOBR, CONTROL);
        	printf("Instruction\t\t : WM\nWriting data to memory...\n");
        	printf("**************************\n");
        	OE = 0;
		}
		else if(inst_code == 0x02)
		{
			MAR = operand;
			/*setting local control signals */
        	Fetch = 0;
        	Memory = 1;
        	IO = 0;
        	/*setting external control signals */
        	CONTROL = inst_code;
        	IOM = 1;
        	RW = 0;
        	OE = 1;
        	ADDR = MAR;
        	//--
        	MainMemory();
        	if(Memory)
        		MBR = BUS;
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR, IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : RM\nReading data from memory...\n");
			printf("MBR\t\t : 0x%x\n", MBR);
        	printf("**************************\n");
		}
		else if(inst_code == 0x03)
		{
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR, IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : BR\nBranch to 0x12A on next cycle...\n");
			PC = 0x12A;
        	printf("**************************\n");
		}
		else if(inst_code == 0x04)
		{
			IOAR = operand;
        	/*setting local control signals */
        	Fetch = 0;
        	Memory = 0;
        	IO = 1;
        	/*setting external control signals */
        	CONTROL = inst_code;
        	IOM = 0;
        	RW = 1;
        	OE = 1;
        	ADDR = IOAR;
        	if(IO)
        		BUS = IOBR;
        	IOMemory();
        	/* echo */
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR, IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : RM\nReading data from IO buffer...\n");
			printf("MBR\t\t : 0x%x\n", IOBR);
        	printf("**************************\n");
		}
		else if(inst_code == 0x05)
		{
			IOAR = operand;
			/*setting local control signals */
        	Fetch = 0;
        	Memory = 0;
        	IO = 1;
        	/*setting external control signals */
        	CONTROL = inst_code;
        	IOM = 0;
        	RW = 0;
        	OE = 1;
        	ADDR = IOAR;
        	//--
        	IOMemory();
        	if(IO)
        		IOBR = BUS;
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR, IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : WIO\nWriting to IO Buffer...\n");
        	printf("**************************\n");
		}
		else if(inst_code == 0x06)
		{
			MBR = operand & 0x00FF;
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR, IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : WB\nLoading data to MBR...\n");
			printf("MBR\t\t : 0x%x\n", MBR);
        	printf("**************************\n");
		}
		else if(inst_code == 0x07)
		{
			IOBR = operand;
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : WIB\nLoading data to IOBR...\n");
			printf("IOBR\t\t : 0x%x\n", IOBR);
        	printf("**************************\n");
		}
		else if(inst_code == 0x09){
			CONTROL = inst_code;
			Fetch = 0; 
			Memory = 1; 
			IO = 0;
			if(Memory){
				BUS = MBR;
			}
			ALU();
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : WACC\nWrite data on BUS to ACC...\n");
			printf("BUS\t\t : 0x%x\n", BUS);
        	printf("**************************\n");
			
		}
		else if(inst_code == 0x0B){
			CONTROL = inst_code;
			Fetch = 0; 
			Memory = 1; 
			IO = 0;
			if(Memory){
				MBR = BUS;
			}
			ALU();
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : RACC\nRead ACC to bus\n");
			printf("BUS\t\t : 0x%x\n", BUS);
        	printf("**************************\n");
			
		}
		else if(inst_code == 0x0E){
			CONTROL = inst_code;
			Fetch = 0; 
			Memory = 1; 
			IO = 0;
			unsigned char tempMBR = MBR;
			MBR = IOBR;
			IOBR = tempMBR;
			printf("Instruction\t\t : SWAP\nSwap MBR and IOBR\n");
			printf("MBR\t\t : 0x%x\nIOBR\t\t : 0x%x\n", MBR, IOBR);
        	printf("**************************\n");
			
		}
		else if(inst_code == 0x11){
				Fetch = 0; 
				Memory = 1; 
				IO = 0; 
				CONTROL = 0x1D; // subtract
				IOM = 0; RW = 0; OE = 0; 
				if(Memory)
					BUS = MBR;
				ALU();
				if((FLAGS & 0x04) == 0x04){	// check if SF is 0
					printf("Branching to address in operand...\n");
					PC = operand;
					printf("Branched to 0x%03X for next cycle.\n", PC);
				}
				
			printf("Instruction\t\t : BRLT\nBranch when SF = 1\n");
			printf("PC\t\t : 0x%x\n", PC);
        	printf("**************************\n");
		
		}
		else if(inst_code == 0x12){
				Fetch = 0; 
				Memory = 1; 
				IO = 0; 
				CONTROL = 0x1D; // subtract
				IOM = 0; RW = 0; OE = 0; 
				if(Memory)
					BUS = MBR;
				ALU();
				if((FLAGS & 0x04) != 0x04){	
					printf("Branching to address in operand...\n");
					PC = operand;
					printf("Branched to 0x%03X for next cycle.\n", PC);
				}
				
			printf("Instruction\t\t : BRGT\nBranch when SF = 0\n");
			printf("PC\t\t : 0x%x\n", PC);
        	printf("**************************\n");
			
		}
		else if(inst_code == 0x13){
				Fetch = 0; 
				Memory = 1; 
				IO = 0; 
				CONTROL = 0x1D; // subtract
				IOM = 0; RW = 0; OE = 0; 
				if(Memory)
					BUS = MBR;
				ALU();
				if((FLAGS & 0x01) != 0x01){	 //zf = 0
					printf("Branching to address in operand...\n");
					PC = operand;
					printf("Branched to 0x%03X for next cycle.\n", PC);
				}
				break;	
			printf("Instruction\t\t : BRNE\nBranch when ZF = 0\n");
			printf("PC\t\t : 0x%x\n", PC);
        	printf("**************************\n");
			
		}
		else if(inst_code == 0x14){
				Fetch = 0; Memory = 1; IO = 0; 
				CONTROL = 0x1D; // subtract
				IOM = 0; RW = 0; OE = 0; 
				if(Memory)
					BUS = MBR;
				ALU();
				if((FLAGS & 0x01) == 0x01){	// check if SF is 0
					printf("Branching to address in operand...\n");
					PC = operand;
					printf("Branched to 0x%03X for next cycle.\n", PC);
				}
				
			printf("Instruction\t\t : BRE\nBranch when ZF = 1\n");
			printf("PC\t\t : 0x%x\n", PC);
        	printf("**************************\n");
		
		}
		else if(inst_code == 0x15){
			/* Setting local control signals */ 
			Fetch = 0; Memory = 1; IO = 0; // operation is bus access through MBR 
			 
			/* Setting global control signals */ 
			CONTROL = inst_code; // setup the Control Signals 
			IOM = 0; RW = 0; OE = 0; // operation neither "write" or “read” 
			 
			if(Memory) 
				BUS = MBR; // load data on BUS to MBR (ACC high byte)
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : SHR\nSHIFT RIGHT...\n");
        	printf("**************************\n");
			ALU();
			
		}
		else if(inst_code == 0x16){
			/* Setting local control signals */ 
			Fetch = 0; Memory = 1; IO = 0; // operation is bus access through MBR 
			 
			/* Setting global control signals */ 
			CONTROL = inst_code; // setup the Control Signals 
			IOM = 0; RW = 0; OE = 0; // operation neither "write" or “read” 
			 
			if(Memory) 
				BUS = MBR; // load data on BUS to MBR (ACC high byte)
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : SHL\nSHIFT LEFT...\n");
        	printf("**************************\n");
			ALU();
			
		}
		else if(inst_code == 0x17){
			/* Setting local control signals */ 
			Fetch = 0; Memory = 1; IO = 0; // operation is bus access through MBR 
			 
			/* Setting global control signals */ 
			CONTROL = inst_code; // setup the Control Signals 
			IOM = 0; RW = 0; OE = 0; // operation neither "write" or “read” 
			 
			if(Memory) 
				BUS = MBR; // load data on BUS to MBR (ACC high byte)
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : XOR\nXOR Operation...\n");
        	printf("**************************\n");
			ALU();
			
		}
		else if(inst_code == 0x18){
			/* Setting local control signals */ 
			Fetch = 0; Memory = 1; IO = 0; // operation is bus access through MBR 
			 
			/* Setting global control signals */ 
			CONTROL = inst_code; // setup the Control Signals 
			IOM = 0; RW = 0; OE = 0; // operation neither "write" or “read” 
			 
			if(Memory) 
				BUS = MBR; // load data on BUS to MBR (ACC high byte)
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : NOT\nNegating...\n");
        	printf("**************************\n");
			ALU();
			
		}
		else if(inst_code == 0x19){
			/* Setting local control signals */ 
			Fetch = 0; Memory = 1; IO = 0; // operation is bus access through MBR 
			 
			/* Setting global control signals */ 
			CONTROL = inst_code; // setup the Control Signals 
			IOM = 0; RW = 0; OE = 0; // operation neither "write" or “read” 
			 
			if(Memory) 
				BUS = MBR; // load data on BUS to MBR (ACC high byte)
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : OR\nOR operation...\n");
        	printf("**************************\n");
			ALU();
			
		}
		else if(inst_code == 0x1A){
			/* Setting local control signals */ 
			Fetch = 0; Memory = 1; IO = 0; // operation is bus access through MBR 
			 
			/* Setting global control signals */ 
			CONTROL = inst_code; // setup the Control Signals 
			IOM = 0; RW = 0; OE = 0; // operation neither "write" or “read” 
			 
			if(Memory) 
				BUS = MBR; // load data on BUS to MBR (ACC high byte)
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : AND\nAND operation...\n");
        	printf("**************************\n");
			ALU();
			
		}
		else if(inst_code == 0x1B){
			/* Setting local control signals */ 
			Fetch = 0; Memory = 1; IO = 0; // operation is bus access through MBR 
			 
			/* Setting global control signals */ 
			CONTROL = inst_code; // setup the Control Signals 
			IOM = 0; RW = 0; OE = 0; // operation neither "write" or “read” 
			 
			if(Memory) 
				BUS = MBR; // load data on BUS to MBR (ACC high byte)
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : MUL\nMultiplying ACC and BUS...\n");
        	printf("**************************\n");
			ALU();
			
		}
		else if(inst_code == 0x1D){
			/* Setting local control signals */ 
			Fetch = 0; Memory = 1; IO = 0; // operation is bus access through MBR 
			 
			/* Setting global control signals */ 
			CONTROL = inst_code; // setup the Control Signals 
			IOM = 0; RW = 0; OE = 0; // operation neither "write" or “read” 
			 
			if(Memory) 
				BUS = MBR; // load data on BUS to MBR (ACC high byte)
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : SUB\nSubtracting ACC and BUS...\n");
        	printf("**************************\n");
			ALU();
			
		}
		else if(inst_code == 0x1E){
			/* Setting local control signals */ 
			Fetch = 0; Memory = 1; IO = 0; // operation is bus access through MBR 
			 
			/* Setting global control signals */ 
			CONTROL = inst_code; // setup the Control Signals 
			IOM = 0; RW = 0; OE = 0; // operation neither "write" or “read” 
			 
			if(Memory) 
				BUS = MBR; // load data on BUS to MBR (ACC high byte)
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : ADD\nAdding ACC and BUS...\n");
        	printf("**************************\n");
			ALU();	
		}
		
		else 
		{
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR, IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : EOP\nEnd of program.\n");
        	printf("**************************\n");
        	return 1;
		}
		getch();
    }
    return 0;
   
}
unsigned char twosComp(unsigned char data) {
  return ~data + 1;
}
void display(unsigned int PC, unsigned int IR, unsigned char inst_code, unsigned int operand, 
			unsigned char BUS, unsigned int ADDR, unsigned int MAR, unsigned int IOAR, unsigned int IOBR, unsigned char CONTROL){
	unsigned int offset = 0x2;
	printf("**************************\n");
	printf("Fetching instruction...\n");
	
	printf("BUS\t\t\t : 0x%x\n", BUS);
	printf("ADDR\t\t\t : 0x%x\n", ADDR);
	printf("PC\t\t\t : 0x%x\n", PC-offset);
	printf("MAR\t\t\t : 0x%x\n", MAR);
	printf("IOAR\t\t\t : 0x%x\n", IOAR);
	printf("IR\t\t\t : 0x%x\n", IR);
	printf("IOBR\t\t\t : 0x%x\n", IOBR);
	printf("CONTROL\t\t\t : 0x%x\n", CONTROL);
	
	printf("Instruction Code\t : 0x%x\n", inst_code);
	printf("Operand\t\t\t : 0x%x\n", operand);
	
}
void initMemory(void)
{
	 printf("Initializing Main Memory...\n\n");
	 IOM = 1, RW = 1, OE = 1;
	 ADDR = 0x000; BUS = 0x30; MainMemory(); // WB write data 0x15 to MBR
	 ADDR = 0x001; BUS = 0x15; MainMemory();
	 ADDR = 0x002; BUS = 0x0C; MainMemory(); // WM  MBR to memory 0x400
	 ADDR = 0x003; BUS = 0x00; MainMemory();
	 ADDR = 0x004; BUS = 0x30; MainMemory(); // WB 0x05 to MBR
	 ADDR = 0x005; BUS = 0x05; MainMemory();
	 ADDR = 0x006; BUS = 0x48; MainMemory(); // WACC write BUS to ACC
	 ADDR = 0x007; BUS = 0x00; MainMemory();
	 ADDR = 0x008; BUS = 0x30; MainMemory(); // WB write data 0x08 to MBR
	 ADDR = 0x009; BUS = 0x08; MainMemory();
	 ADDR = 0x00A; BUS = 0xF0; MainMemory(); // ADD 
	 ADDR = 0x00B; BUS = 0x00; MainMemory();
	 ADDR = 0x00C; BUS = 0x14; MainMemory(); // RM - read memory at 0x400
	 ADDR = 0x00D; BUS = 0x00; MainMemory();
	 ADDR = 0x00E; BUS = 0xD8; MainMemory(); // MUL ACC x MBR 
	 ADDR = 0x00F; BUS = 0x00; MainMemory();
	 
	 ADDR = 0x010; BUS = 0x58; MainMemory(); // RACC
	 ADDR = 0x011; BUS = 0x00; MainMemory();
	 ADDR = 0x012; BUS = 0x0C; MainMemory(); // WM
	 ADDR = 0x013; BUS = 0x01; MainMemory();
	 ADDR = 0x014; BUS = 0x38; MainMemory(); // WIB
	 ADDR = 0x015; BUS = 0x0B; MainMemory();
	 ADDR = 0x016; BUS = 0x28; MainMemory(); // WIO
	 ADDR = 0x017; BUS = 0x00; MainMemory();
	 ADDR = 0x018; BUS = 0x30; MainMemory(); // WB
	 ADDR = 0x019; BUS = 0x10; MainMemory();
	 ADDR = 0x01A; BUS = 0xE8; MainMemory(); // SUB
	 ADDR = 0x01B; BUS = 0x00; MainMemory();
	 ADDR = 0x01C; BUS = 0x58; MainMemory(); // RACC 
	 ADDR = 0x01D; BUS = 0x00; MainMemory();
	 ADDR = 0x01E; BUS = 0x28; MainMemory(); // WIO
	 ADDR = 0x01F; BUS = 0x01; MainMemory();
	 
	 ADDR = 0x020; BUS = 0xB0; MainMemory(); // SHL
	 ADDR = 0x021; BUS = 0x00; MainMemory();
	 ADDR = 0x022; BUS = 0xB0; MainMemory(); // SHL
	 ADDR = 0x023; BUS = 0x00; MainMemory();
	 ADDR = 0x024; BUS = 0xA8; MainMemory(); // SHR
	 ADDR = 0x025; BUS = 0x00; MainMemory();
	 ADDR = 0x026; BUS = 0x14; MainMemory(); // RM
	 ADDR = 0x027; BUS = 0x01; MainMemory();
	 ADDR = 0x028; BUS = 0xC8; MainMemory(); // OR 
	 ADDR = 0x029; BUS = 0x00; MainMemory();
	 ADDR = 0x02A; BUS = 0xC0; MainMemory(); // NOT
	 ADDR = 0x02B; BUS = 0x00; MainMemory();
	 ADDR = 0x02C; BUS = 0x20; MainMemory(); // RIO
	 ADDR = 0x02D; BUS = 0x01; MainMemory();
	 ADDR = 0x02E; BUS = 0x70; MainMemory(); // SWAP
	 ADDR = 0x02F; BUS = 0x00; MainMemory();
	
	 ADDR = 0x030; BUS = 0xB8; MainMemory(); // XOR
	 ADDR = 0x031; BUS = 0x00; MainMemory();
	 ADDR = 0x032; BUS = 0x30; MainMemory(); // WB
	 ADDR = 0x033; BUS = 0xFF; MainMemory();
	 ADDR = 0x034; BUS = 0xD0; MainMemory(); // AND
	 ADDR = 0x035; BUS = 0x00; MainMemory();
	 ADDR = 0x036; BUS = 0x14; MainMemory(); // RM
	 ADDR = 0x037; BUS = 0x01; MainMemory();
	 ADDR = 0x038; BUS = 0xA0; MainMemory(); // BRE 0x03C
	 ADDR = 0x039; BUS = 0x3C; MainMemory();
	 ADDR = 0x03A; BUS = 0x30; MainMemory(); // WB 
	 ADDR = 0x03B; BUS = 0xF0; MainMemory();
	 ADDR = 0x03C; BUS = 0x90; MainMemory(); // BRGT
	 ADDR = 0x03D; BUS = 0x40; MainMemory();
	 ADDR = 0x03E; BUS = 0x88; MainMemory(); // BRLT
	 ADDR = 0x03F; BUS = 0x44; MainMemory();
	 
	 ADDR = 0x040; BUS = 0x30; MainMemory(); // WB
	 ADDR = 0x041; BUS = 0x00; MainMemory();
	 ADDR = 0x042; BUS = 0x48; MainMemory(); // WACC 
	 ADDR = 0x043; BUS = 0x00; MainMemory();
	 ADDR = 0x044; BUS = 0x30; MainMemory(); // WB
	 ADDR = 0x045; BUS = 0x03; MainMemory();
	 ADDR = 0x046; BUS = 0x48; MainMemory(); // WACC
	 ADDR = 0x047; BUS = 0x00; MainMemory();
	 ADDR = 0x048; BUS = 0x30; MainMemory(); // MUL 
	 ADDR = 0x049; BUS = 0x00; MainMemory();
	 ADDR = 0x04A; BUS = 0xA0; MainMemory(); // BRE
	 ADDR = 0x04B; BUS = 0x52; MainMemory();
	 ADDR = 0x04C; BUS = 0x30; MainMemory(); // WB
	 ADDR = 0x04D; BUS = 0x01; MainMemory();
	 ADDR = 0x04E; BUS = 0xE8; MainMemory(); // SUB
	 ADDR = 0x04F; BUS = 0x00; MainMemory();
	 
	 ADDR = 0x050; BUS = 0x18; MainMemory(); // BR 
	 ADDR = 0x051; BUS = 0x48; MainMemory();
	 ADDR = 0x052; BUS = 0xF8; MainMemory(); // MUL
	 ADDR = 0x053; BUS = 0x00; MainMemory();
}
void MainMemory(void) 
{ 
 	if(IOM==1) 
 	{ 
	 	if(RW==0 && OE==1) // memory read 
	 		BUS = dataMemory[ADDR]; 
	 	else if(RW==1 && OE==1) // memory write 
	 		dataMemory[ADDR] = BUS; 
 	} 
}
void IOMemory(void) 
{ 
 	if(IOM==0) 
 	{ 
	 	if(RW==0 && OE==1) // memory read 
	 		BUS = ioBuffer[ADDR]; 
	 	else if(RW==1 && OE==1) // memory write 
	 		ioBuffer[ADDR] = BUS; 
 	} 
}
int ALU(void){

	static int ACC;
	unsigned char temp_OP2;
	unsigned int temp_ACC;
	
	switch(CONTROL){
		case 0x1E:
			temp_OP2 = BUS;
			temp_ACC = (int) ACC + temp_OP2;
			ACC = (unsigned char) temp_ACC;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags((unsigned char)ACC);
			break;
		case 0x1D:
			temp_OP2 = BUS;
			if(isNegative(temp_ACC)){
				printf("2's complement OP1\n");
				temp_ACC = twosComp(temp_ACC);
			}
			temp_OP2 = twosComp(temp_OP2);
			temp_ACC = (int) ACC + temp_OP2;
			ACC = (unsigned char) temp_ACC;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags((unsigned char)ACC);
			break;
		case 0x1B:
			temp_OP2 = BUS;
			booths(temp_ACC, temp_OP2);
			temp_ACC = (int) ACC * temp_OP2;
			ACC = (unsigned char) temp_ACC;
			printf("ACC = ");
			decToBinary(ACC, 8);
			printf("\n");
			setFlags((unsigned char)ACC);
			break;
		case 0x1A:
			temp_OP2 = BUS;
			ACC = ACC & temp_OP2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags((unsigned char)ACC);
			break;
		case 0x19:
			temp_OP2 = BUS;
			ACC = ACC | temp_OP2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags((unsigned char)ACC);
			break;
		case 0x18:
			temp_OP2 = 0x00;
			ACC = ~ACC;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags((unsigned char)ACC);
			break;
		case 0x17:
			temp_OP2 = BUS;
			ACC = ACC ^ temp_OP2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags((unsigned char)ACC);
			break;
		case 0x16:
			temp_OP2 = BUS;
			ACC = ACC >> temp_OP2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags((unsigned char)ACC);
			break;
		case 0x15:
			temp_OP2 = BUS;
			ACC = ACC << temp_OP2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags((unsigned char)ACC);
			break;
		case 0x09: //WACC
			ACC = BUS;
			printf("ACC               : 0x%02X\n", ACC & 0x00FF);
			setFlags((unsigned char)ACC);
			break;
		case 0x0B: //RACC
			BUS = ACC;
			printf("BUS               : 0x%02X\n", BUS & 0x00FF);
			setFlags((unsigned char)ACC);
			break;
		default:
			printf("Invalid Control Signal");
			break;		
	}
	
	printf("\n****************************\n");
	return 0;
}

bool isNegative(unsigned char x) {
	//if msb is one then it is negative
    return x & (1 << 7);
}

void setFlags(unsigned char ACC){
	
	//ZF
	if(ACC == 0x0000){
		FLAGS = FLAGS | 0x01;
	}
	//CF
	if(ACC > CHAR_BIT){
		FLAGS = FLAGS | 0x02;
	}
	//OF
	if(ACC > CHAR_BIT){
		FLAGS = FLAGS | 0x80;
	}
	//SF
	if((ACC & 0x0000FF00)==0xFF00){
		FLAGS = FLAGS | 0x04;
	}
	printf("ZF = %x CF = %x OF = %x SF = %x\n", (FLAGS & 0x01), ((FLAGS >> 1) & 0x01), ((FLAGS >> 7)& 0x01), ((FLAGS >> 3)& 0x01));
	FLAGS = 0x00;
}


bool detectCarry(unsigned char x, unsigned char y) {
  unsigned int result = x + y;
  return result > UINT_MAX;
}

void displayALU(unsigned char temp_ACC, unsigned char temp_OP2, unsigned char control_signals){
	printf("*********************************************************\n");
	printf("Fetching operands...\n");
	printf("OP1 = ");
	decToBinary(temp_ACC, 8);
	printf("\n");
	printf("OP2 = ");
	decToBinary(temp_OP2, 8);
	printf("\n");
	switch(control_signals){
		case 0x01:
			printf("Operation = ADD\n");
			printf("Adding OP1 & OP2\n");
			break;
		case 0x02:
			printf("Operation = SUB\n");
			printf("2's complement OP2\n");
			printf("Adding OP1 & OP2\n");
			break;
		case 0x03:
			printf("Operation = MULTIPLY\n");
			printf("MULTIPLYING OP1 (4-bit) & OP2 (4-bit)\n");
			break;
		case 0x04:
			printf("Operation = AND\n");
			printf("PERFORMING OP1 & OP2\n");
			break;
		case 0x05:
			printf("Operation = OR\n");
			printf("PERFORMING OP1 | OP2\n");
			break;
		case 0x06:
			printf("Operation = NOT\n");
			printf("PERFORMING ~OP1\n");
			break;
		case 0x07:
			printf("Operation = XOR\n");
			printf("PERFORMING OP1 ^ OP2\n");
			break;
		case 0x08:
			printf("Operation = LOGICAL SHIFT RIGHT\n");
			printf("PERFORMING LOGICAL SHIFT RIGHT\n");
			break;
		case 0x09:
			printf("Operation = LOGICAL SHIFT LEFT\n");
			printf("PERFORMING LOGICAL SHIFT LEFT\n");
			break;
		default:
			printf("Invalid Control Signal");
			break;
		
	}
}
void booths(unsigned char q, unsigned char m){
    int i;
    unsigned char acc = 0;
    unsigned char q_prev = 0;
    printf("ACC\tQ\tQ-1\tM\t\n");
    for(i = 0; i < 4; i++){
       displayCycle(acc, q, q_prev, m);
       unsigned char q_lsb = q & 1;
       unsigned char acc_msb = (acc & 8) >> 3;
       if((q_lsb == 0 && q_prev == 0) || (q_lsb == 1 && q_prev == 1)){
           //combine acc and q
           unsigned char combined = (acc << 4) | q;
           //shift right arithmetic
           unsigned char shifted = (combined >> 1) | (combined & 0x80);
           //split acc and q
           acc = (shifted >> 4) & 0x0F;
           q = shifted & 0x0F;
           q_prev = q_lsb;
       }else if((q_lsb == 1 && q_prev == 0)){
       	 acc = acc + (twosComp(m) & 0x0F);
       	 //combine acc and q
           unsigned char combined = (acc << 4) | q;
           //shift right arithmetic
           unsigned char shifted = (combined >> 1) | (combined & 0x80);
           //split acc and q
           acc = (shifted >> 4) & 0x0F;
           q = shifted & 0x0F;
           q_prev = q_lsb;
	   }else{
	   	 acc = acc + m;
	   	 //combine acc and q
           unsigned char combined = (acc << 4) | q;
           //shift right arithmetic
           unsigned char shifted = (combined >> 1) | (combined & 0x80);
           //split acc and q
           acc = (shifted >> 4) & 0x0F;
           q = shifted & 0x0F;
           q_prev = q_lsb;
	   }
    }
    displayCycle(acc, q, q_prev, m);
}
void displayCycle(unsigned char acc, unsigned char q, unsigned char q_prev, unsigned char m){
	
	decToBinary(acc, 4);
	printf("\t");
	decToBinary(q, 4);
	printf("\t");
	decToBinary(q_prev, 4);
	printf("\t");
	decToBinary(m, 4);
	printf("\n");
	
}
void decToBinary(unsigned int n, int bits)
{
    unsigned int num = n;
	unsigned int mask = 1 << bits - 1;
	int i;
	for (i = 0; i < bits; i++) {
	    if ((num & mask) == 0)
	        printf("0");
	    else
	        printf("1");
	
	    num = num << 1;
	}
}

