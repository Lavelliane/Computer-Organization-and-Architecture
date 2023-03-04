#include <stdio.h>
#include <math.h>
#include <stdbool.h>

int CU(void);
void initMemory(void);
void display(unsigned int PC, unsigned int IR, unsigned char inst_code, unsigned int operand, 
			unsigned char BUS, unsigned int ADDR, unsigned int MAR, unsigned int IOAR, unsigned int IOBR, unsigned char CONTROL);
void MainMemory(void);
void IOMemory(void);

unsigned char dataMemory[2048]; // 2048 = 2^11 (where 11 is number of address bits)
unsigned char ioBuffer[32];
unsigned int ADDR;
unsigned char BUS;
unsigned char CONTROL;
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
    unsigned int PC, IR, MAR, MBR, IOAR, IOBR, operand;
    unsigned char inst_code;
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
			MBR = operand;
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
		else 
		{
			display(PC, IR, inst_code, operand, BUS, ADDR, MAR, IOAR, IOBR, CONTROL);
			printf("Instruction\t\t : EOP\nEnd of program.\n");
        	printf("**************************\n");
        	return 1;
		}
    }
    return 0;
   
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
    dataMemory[0x000] = 0x30; // WB - write data 0xFF to MBR
    dataMemory[0x001] = 0xFF;
    dataMemory[0x002] = 0x0C;
    dataMemory[0x003] = 0x00;
    dataMemory[0x004] = 0x14;
    dataMemory[0x005] = 0x00;
    dataMemory[0x006] = 0x19;
    dataMemory[0x007] = 0x2A;
    dataMemory[0x12A] = 0x38;
    dataMemory[0x12B] = 0x05;
    dataMemory[0x12C] = 0x28;
    dataMemory[0x12D] = 0x0A;
    dataMemory[0x12E] = 0xF8;
    dataMemory[0x12F] = 0x00;
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

