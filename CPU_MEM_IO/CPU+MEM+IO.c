//Author: Jhury Kevin Lastre
//BS CpE - 3
//Version: CPU+MEM+IO V2.0
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
void setFlags(unsigned int ACC);
void booths(int a, int b);
void displayCycle(unsigned char acc, unsigned char q, unsigned char q_prev, unsigned char m);
int* charToBinary(unsigned char num);
void setToMemory(int* binary, int col, int row, short int cs);
void setBit(long* num, int pos, int value);
unsigned char reconstruct(int col, int row, int cs);
int count_bits(unsigned int x);
void InputSim(void);
void SevenSegment();


/*booths vars*/
int booth[16]={0},M[8]={0},M_[8]={0},q=0;
void multiplicand(int);
void multiplier(int);
void add(int m[8]);
void arithmeticShiftRight();
void neg();
int dec();
/* booth vars end */

//unsigned char dataMemory[2048]; // 2048 = 2^11 (where 11 is number of address bits)
unsigned char ioBuffer[32];
unsigned char iOData[32];
unsigned int ADDR;
unsigned char BUS;
unsigned char CONTROL;
unsigned char FLAGS;
bool IOM = 0;
bool RW = 0;
bool OE = 0;

long A1[32], A2[32], A3[32], A4[32], A5[32], A6[32], A7[32], A8[32]; // chip group A 
long B1[32], B2[32], B3[32], B4[32], B5[32], B6[32], B7[32], B8[32];


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
	int row, col, cs;
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
        	/* decoding address data */ 
		 	 col = PC & 0x001F; 
		 	 row = (PC >> 5) & 0x001F; 
		 	 cs = PC >> 10; 
        	IR = reconstruct(col, row, cs);
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
        	printf("MBR\t\t : \t0x%x\n", MBR);
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
			printf("Instruction\t\t : BR\nBranch to 0x%x on next cycle...\n", ADDR);
			PC = ADDR;
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
        	RW = 0;
        	OE = 1;
        	ADDR = IOAR;
        	IOMemory();
        	if(IO)
        		IOBR = BUS;
        	/* echo */
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR, IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : RIO\nReading data from IO buffer...\n");
			printf("IOBR\t\t : 0x%x\n", IOBR);
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
        	RW = 1;
        	OE = 1;
        	ADDR = IOAR;
        	//--
        	if(IO)
        		BUS = IOBR;
        	IOMemory();
        	InputSim();
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
			ALU();
			if(Memory){
				MBR = BUS;
			}
			
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR,  IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : RACC\nRead ACC to bus\n");
			printf("BUS\t\t : 0x%x\n", BUS);
			printf("MBR\t\t : 0x%x\n", MBR);
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
				printf("===============BRLT===============\n");

				ALU();
				if((FLAGS & 0x04) == 0x04){	// check if SF is 0
					printf("Branching to address in operand...\n");
					PC = operand;
					printf("Branched to 0x%X for next cycle.\n", PC);
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
				printf("===============BRGT===============\n");
				ALU();
				if((FLAGS & 0x04) != 0x04){	
					printf("Branching to address in operand...\n");
					PC = operand;
					printf("Branched to 0x%X for next cycle.\n", PC);
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
				printf("===============BRNE===============\n");
				ALU();
				if((FLAGS & 0x01) != 0x01){	 //zf = 0
					printf("Branching to address in operand...\n");
					PC = operand;
					printf("Branched to 0x%X for next cycle.\n", PC);
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
				printf("===============BRE===============\n");
				ALU();
				if((FLAGS & 0x01) == 0x01){	// check if SF is 0
					printf("Branching to address in operand...\n");
					PC = operand;
					printf("Branched to 0x%X for next cycle.\n", PC);
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
void InputSim(void) 
{ 
 unsigned char data; 
 int i;
 for(i=7; i>=0; i--) 
 { 
 /* load source data */ 
 data = iOData[0x001]; 
 /* shift bit to LSB */ 
 data = data >> i; 
 /* mask upper bits */ 
 data = data & 0x01; 
 /* position bit */ 
 data = data << (7 - i); 
 /* write bit to dest buffer */ 
 iOData[0x01F] = iOData[0x01F] | data; 
 } 
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
	 ADDR=0x0; 	BUS=0x30; 	MainMemory();
ADDR=0x1; 	BUS=0x15; 	MainMemory();
ADDR=0x2; 	BUS=0x0C; 	MainMemory();
ADDR=0x3; 	BUS=0x0; 	MainMemory();
ADDR=0x4; 	BUS=0x30; 	MainMemory();
ADDR=0x5; 	BUS=0x5; 	MainMemory();
ADDR=0x6; 	BUS=0x48; 	MainMemory();
ADDR=0x7; 	BUS=0x0; 	MainMemory();
ADDR=0x8; 	BUS=0x30; 	MainMemory();
ADDR=0x9; 	BUS=0x8; 	MainMemory();
ADDR=0xa; 	BUS=0xF0; 	MainMemory();
ADDR=0xb; 	BUS=0x0; 	MainMemory();
ADDR=0xc; 	BUS=0x14; 	MainMemory();
ADDR=0xd; 	BUS=0x0; 	MainMemory();
ADDR=0xe; 	BUS=0xD8; 	MainMemory();
ADDR=0xf; 	BUS=0x0; 	MainMemory();
ADDR=0x10; 	BUS=0x58; 	MainMemory();
ADDR=0x11; 	BUS=0x0; 	MainMemory();
ADDR=0x12; 	BUS=0x0C; 	MainMemory();
ADDR=0x13; 	BUS=0x1; 	MainMemory();
ADDR=0x14; 	BUS=0x38; 	MainMemory();
ADDR=0x15; 	BUS=0xb; 	MainMemory();
ADDR=0x16; 	BUS=0x28; 	MainMemory();
ADDR=0x17; 	BUS=0x0; 	MainMemory();
ADDR=0x18; 	BUS=0x30; 	MainMemory();
ADDR=0x19; 	BUS=0x10; 	MainMemory();
ADDR=0x1a; 	BUS=0xE8; 	MainMemory();
ADDR=0x1b; 	BUS=0x0; 	MainMemory();
ADDR=0x1c; 	BUS=0x58; 	MainMemory();
ADDR=0x1d; 	BUS=0x0; 	MainMemory();
ADDR=0x1e; 	BUS=0x28; 	MainMemory();
ADDR=0x1f; 	BUS=0x1; 	MainMemory();
ADDR=0x20; 	BUS=0xB0; 	MainMemory();
ADDR=0x21; 	BUS=0x0; 	MainMemory();
ADDR=0x22; 	BUS=0xB0; 	MainMemory();
ADDR=0x23; 	BUS=0x0; 	MainMemory();
ADDR=0x24; 	BUS=0xA8; 	MainMemory();
ADDR=0x25; 	BUS=0x0; 	MainMemory();
ADDR=0x26; 	BUS=0x14; 	MainMemory();
ADDR=0x27; 	BUS=0x1; 	MainMemory();
ADDR=0x28; 	BUS=0xC8; 	MainMemory();
ADDR=0x29; 	BUS=0x0; 	MainMemory();
ADDR=0x2a; 	BUS=0xC0; 	MainMemory();
ADDR=0x2b; 	BUS=0x0; 	MainMemory();
ADDR=0x2c; 	BUS=0x20; 	MainMemory();
ADDR=0x2d; 	BUS=0x1f; 	MainMemory();
ADDR=0x2e; 	BUS=0x70; 	MainMemory();
ADDR=0x2f; 	BUS=0x0; 	MainMemory();
ADDR=0x30; 	BUS=0xB8; 	MainMemory();
ADDR=0x31; 	BUS=0x0; 	MainMemory();
ADDR=0x32; 	BUS=0x30; 	MainMemory();
ADDR=0x33; 	BUS=0xff; 	MainMemory();
ADDR=0x34; 	BUS=0xD0; 	MainMemory();
ADDR=0x35; 	BUS=0x0; 	MainMemory();
ADDR=0x36; 	BUS=0x14; 	MainMemory();
ADDR=0x37; 	BUS=0x1; 	MainMemory();
ADDR=0x38; 	BUS=0xA0; 	MainMemory();
ADDR=0x39; 	BUS=0x3c; 	MainMemory();
ADDR=0x3a; 	BUS=0x30; 	MainMemory();
ADDR=0x3b; 	BUS=0x2e; 	MainMemory();
ADDR=0x3c; 	BUS=0x90; 	MainMemory();
ADDR=0x3d; 	BUS=0x42; 	MainMemory();
ADDR=0x3e; 	BUS=0xD0; 	MainMemory();
ADDR=0x3f; 	BUS=0x0; 	MainMemory();
ADDR=0x40; 	BUS=0x88; 	MainMemory();
ADDR=0x41; 	BUS=0x46; 	MainMemory();
ADDR=0x42; 	BUS=0x30; 	MainMemory();
ADDR=0x43; 	BUS=0x0; 	MainMemory();
ADDR=0x44; 	BUS=0x48; 	MainMemory();
ADDR=0x45; 	BUS=0x0; 	MainMemory();
ADDR=0x46; 	BUS=0x30; 	MainMemory();
ADDR=0x47; 	BUS=0x3; 	MainMemory();
ADDR=0x48; 	BUS=0x48; 	MainMemory();
ADDR=0x49; 	BUS=0x0; 	MainMemory();
ADDR=0x4a; 	BUS=0x30; 	MainMemory();
ADDR=0x4b; 	BUS=0x0; 	MainMemory();
ADDR=0x4c; 	BUS=0xA0; 	MainMemory();
ADDR=0x4d; 	BUS=0x54; 	MainMemory();
ADDR=0x4e; 	BUS=0x30; 	MainMemory();
ADDR=0x4f; 	BUS=0x1; 	MainMemory();
ADDR=0x50; 	BUS=0xE8; 	MainMemory();
ADDR=0x51; 	BUS=0x0; 	MainMemory();
ADDR=0x52; 	BUS=0x18; 	MainMemory();
ADDR=0x53; 	BUS=0x4a; 	MainMemory();
ADDR=0x54; 	BUS=0xF8; 	MainMemory();
ADDR=0x55; 	BUS=0x0; 	MainMemory();
	
}
void MainMemory(void) 
{ 
	int row, col, i;
	short int cs;
	if(OE && IOM == 1){
		/* decoding address data */ 
	 	 col = ADDR & 0x001F; 
	 	 row = (ADDR >> 5) & 0x001F; 
	 	 cs = ADDR >> 10; 
	 	 if(RW==0) { // memory read 
	 		BUS = reconstruct(col, row, cs);
	 	}
	 	 else if(RW==1) {
	 	 	int* binary = charToBinary(BUS);
	 	 	//Pass in the binary, col, row, cs
	 	 	setToMemory(binary, col, row, cs);
		 }
	}	
}
void IOMemory(void) 
{ 
	 if(OE) 
	 { 
		 if(RW && !IOM) // check if memory write and IO Memory access
		 { 
			 if(ADDR >= 0x000 && ADDR <= 0x00F) // check the address if valid 
			 	iOData[ADDR] = BUS; // write data in BUS to IO Memory
		 } 
		 else 
		 { 
			 if(ADDR >= 0x010 && ADDR <= 0x01F) // check the address if valid
			 	BUS = iOData[ADDR]; // load data to BUS 
		 } 
	 } 
}
unsigned char reconstruct(int col, int row, int cs){
	int i;
	unsigned char result[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char final = 0;
	if(!cs){
		result[7] = getBit(A1[row], col);
		result[6] = getBit(A2[row], col);
		result[5] = getBit(A3[row], col);
		result[4] = getBit(A4[row], col);
		result[3] = getBit(A5[row], col);
		result[2] = getBit(A6[row], col);
		result[1] = getBit(A7[row], col);
		result[0] = getBit(A8[row], col);
		for (i = 0; i < 8; i++) {
        	final |= result[i] << (8 - i - 1);
	    }
	    
	    return final;
	}
	result[7] = getBit(B1[row], col);
		result[6] = getBit(B2[row], col);
		result[5] = getBit(B3[row], col);
		result[4] = getBit(B4[row], col);
		result[3] = getBit(B5[row], col);
		result[2] = getBit(B6[row], col);
		result[1] = getBit(B7[row], col);
		result[0] = getBit(B8[row], col);
		for (i = 0; i < 8; i++) {
        	final |= result[i] << (8 - i - 1);
	    }
	    return final;
	
}
int getBit(long num, int pos) {
    return (num >> pos) & 1;
}
void setToMemory(int* binary, int col, int row, short int cs){
	
	if(!cs){
		setBit(&A1[row], col, binary[0]);
		setBit(&A2[row], col, binary[1]);
		setBit(&A3[row], col, binary[2]);
		setBit(&A4[row], col, binary[3]);
		setBit(&A5[row], col, binary[4]);
		setBit(&A6[row], col, binary[5]);
		setBit(&A7[row], col, binary[6]);
		setBit(&A8[row], col, binary[7]);
		return;
	}
	setBit(&B1[row], col, binary[0]);
	setBit(&B2[row], col, binary[1]);
	setBit(&B3[row], col, binary[2]);
	setBit(&B4[row], col, binary[3]);
	setBit(&B5[row], col, binary[4]);
	setBit(&B6[row], col, binary[5]);
	setBit(&B7[row], col, binary[6]);
	setBit(&B8[row], col, binary[7]);
}
int* charToBinary(unsigned char num) {
	int i;
    int* bits = malloc(8 * sizeof(int)); // Allocate memory for 8 bits
    if (bits == NULL) {
        printf("Error: memory allocation failed\n");
        return NULL;
    }
    
    for (i = 7; i >= 0; i--) {
        bits[i] = (num >> i) & 1;
    }
    
    return bits;
}
void setBit(long* num, int pos, int value) {
    if (value == 0) {
        // Clear the bit at the given position
        *num &= ~(1u << pos);
    } else {
        // Set the bit at the given position
        *num |= (1u << pos);
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
			setFlags(temp_ACC);
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
			setFlags(temp_ACC);
			break;
		case 0x1B:
			temp_OP2 = BUS;
			booths((int)ACC, (int)temp_OP2);
			temp_ACC = (int) ACC * temp_OP2;
			ACC = temp_ACC & 0xFF;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(temp_ACC);
			break;
		case 0x1A:
			temp_OP2 = BUS;
			ACC = ACC & temp_OP2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC);
			break;
		case 0x19:
			temp_OP2 = BUS;
			ACC = ACC | temp_OP2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC);
			break;
		case 0x18:
			temp_OP2 = 0x00;
			temp_ACC = ~ACC;
			ACC = (~ACC) & 0xFF;
			printf("ACC = ");
			decToBinary(ACC, 8);
			printf("\n");
			setFlags(temp_ACC);
			break;
		case 0x17:
			temp_OP2 = BUS;
			temp_ACC = (ACC ^ temp_OP2);
			ACC = (ACC ^ temp_OP2) & 0xFF;
			printf("ACC = ");
			decToBinary(ACC, 8);
			printf("\n");
			setFlags(ACC);
			break;
		case 0x16:
			temp_OP2 = BUS;
			ACC = ACC << 1;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC);
			break;
		case 0x15:
			temp_OP2 = BUS;
			ACC = ACC >> 1;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC);
			break;
		case 0x09: //WACC
			ACC = BUS;
			printf("ACC               : 0x%02X\n", ACC & 0x00FF);
			setFlags(ACC);
			break;
		case 0x0B: //RACC
			BUS = ACC;
			printf("BUS               : 0x%02X\n", BUS & 0x00FF);
			setFlags(ACC);
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

void setFlags(unsigned int ACC){
	
	// Set Zero Flag (ZF)
    unsigned char zf = (ACC == 0) ? 1 : 0;

    // Set Sign Flag (SF)
    unsigned char sf = (ACC & 0x80) ? (1 << 2) : 0;

    // Set Overflow Flag (OF)
    unsigned char of = (ACC > 255) ? (1 << 7) : 0;

    // Set Carry Flag (CF)
    unsigned char cf = (ACC > 255) ? (1 << 1) : 0;
    
    // Set the flag bits in FLAGS variable
    FLAGS = (zf & 0x01) | (cf & 0x02) | (of & 0x80) | (sf & 0x04);

    printf("ZF = %x CF = %x OF = %x SF = %x\n", (FLAGS & 0x01), (FLAGS & 0x02) >> 1, (FLAGS & 0x80) >> 7, (FLAGS & 0x04) >> 2);

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
void booths(int a, int b){
   int n=8,i,sum=0;
   multiplicand(a);
   multiplier(b);
	neg();
	printf("ACC\t\tQ\t\tQ_prev\t\tM\n");
	while(n>0){
		if(booth[15]==0&&q==1){
			add(M); //AC=AC+M
		}else if(booth[15]==1&&q==0){
			add(M_); //AC=AC-M
		}
		arithmeticShiftRight();
		for(i = 0; i < 8; i++){
			printf("%d", booth[i]);
		}
		printf("\t");
		for(i = 8; i < 16; i++){
			printf("%d", booth[i]);
		}
		printf("\t");
		printf("%d", q);
		printf("\t\t");
		decToBinary((unsigned int)a, 8);
		printf("\n");
		n--;
	}
	return;
}
int count_bits(unsigned int x) {
    int count = 0;
    while (x) {
        count++;
        x >>= 1;
    }
    return count;
}
void displayCycle(unsigned char acc, unsigned char q, unsigned char q_prev, unsigned char m){
	
	decToBinary(acc, 8);
	printf("\t");
	decToBinary(q, 8);
	printf("\t");
	decToBinary(q_prev, 8);
	printf("\t");
	decToBinary(m, 8);
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
void SevenSegment()
{
    if(iOData[ADDR]==0x01)
    {
        printf("    X\n");
        printf("    X\n");
        printf("    X\n");
        printf("    X\n");
        printf("    X\n");
        printf("    X\n");
        printf("    X\n");
    }    
    else if(iOData[ADDR]==0x02)
    {
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf(" XXXXX\n");
        printf(" X    \n");
        printf(" X    \n");
        printf(" XXXXX\n");
    }    
    else if(iOData[ADDR]==0x03)
    {
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf(" XXXXX\n");
    }    
    else if(iOData[ADDR]==0x04)
    {
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf("     X\n");
    }
    else if(iOData[ADDR]==0x05)
    {
        printf(" XXXXX\n");
        printf(" X    \n");
        printf(" X    \n");
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf(" XXXXX\n");
    }    
    else if(iOData[ADDR]==0x06)
    {
        printf(" XXXXX\n");
        printf(" X    \n");
        printf(" X    \n");
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
    }
    else if(iOData[ADDR]==0x07)
    {
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf("     X\n");
        printf("     X\n");
        printf("     X\n");
        printf("     X\n");
    }  
    else if(iOData[ADDR]==0x08)
    {
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
    }
    else if(iOData[ADDR]==0x09)
    {
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
        printf("     X\n");
        printf("     X\n");
        printf(" XXXXX\n");
    }
    else if(iOData[ADDR]==0x00)
    {
        printf(" XXXXX\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" X   X\n");
        printf(" XXXXX\n");
    }
   // getc(stdin);
}
/*BOOTH UTIL FUNCTIONS BEGIN HERE  */

/* Converts multiplier to binary and adds in booth array as Q*/
void multiplier(int a){
	if(a>=0){
		int i=15;
		while(a>0){
			int rem=a%2;
			a=a/2;
			booth[i]=rem;
			i--;
		}
	}else{
		a=a+128;
		int i=15;
		while(a>0){
			int rem=a%2;
			a=a/2;
			booth[i]=rem;
			i--;
		}
		booth[8]=1;
	}
	
}

/*Converts multiplicand to binary*/
void multiplicand(int a){
	if(a>=0){
		int i=7;
		while(a>0){
			int rem=a%2;
			a=a/2;
			M[i]=rem;
			i--;
		}
	}else{
		a=a+128;
		int i=7;
		while(a>0){
			int rem=a%2;
			a=a/2;
			M[i]=rem;
			i--;
		}
		M[0]=1;
	}
	
}

/*ADDS M or -M to AC*/
void add(int m[8]){
	int sum=0,carry=0,i;
	for(i=7;i>=0;i--){
		sum=m[i]+booth[i]+carry;
		if(sum==2){
			sum=0;
			carry=1;
		}else if(sum==3){
			sum=1;
			carry=1;
		}else{
			carry=0;
		}
		booth[i]=sum;
	}	
}

/* neg() creates -M from M */
void neg(){
	int i,carry=0,sum=0;
	for(i=0;i<8;i++){
		if(M[i]==0){
			M_[i]=1;
		}else{
			M_[i]=0;
		}
	}
	sum=1+M_[7];
	if(sum==2){
		carry=1;
		sum=0;
	}
	M_[7]=sum;
	for(i=6;i>=0;i--){
		sum=M_[i]+carry;
		if(sum==2){
			carry=1;
			sum=0;
		}else{
			carry=0;
		}
		M_[i]=sum;
	}
	
}

void arithmeticShiftRight(){
	int i=0;
	q=booth[15];
	for(i=15;i>0;i--){
		booth[i]=booth[i-1];
	}	
}

int dec()
{
	int result=0,i=15,b=0;
	while(i>0&&b<15)
	{
		result+=(booth[i]*pow(2,b));
		b++,i--;
	}
	if(booth[0]==1)
		return (-32768+result);
	else
		return result;
}
/* BOOTH UTIL ENDS */
