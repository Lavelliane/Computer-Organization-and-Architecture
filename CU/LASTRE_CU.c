#include <stdio.h>
#include <math.h>

int CU(void);
void initMemory(void);
void display(unsigned int PC, unsigned int IR, unsigned char inst_code, unsigned int operand);

unsigned char dataMemory[2048]; // 2048 = 2^11 (where 11 is number of address bits)

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

    //initialize Program Counter
    PC = dataMemoryStart;
    unsigned char ioBuffer[32];

    while(inst_code != 0x1F){
        //fetch upper byte
        IR = dataMemory[PC];
        IR = IR << 8;
        PC++;
        //fetch lower byte
        IR = IR | dataMemory[PC];
        PC++;
        //decode instruction
        inst_code = IR >> 11;
        operand = IR & 0x07FF;
        if(inst_code == 0x01)
        {
        	MAR = operand;
        	dataMemory[MAR] = MBR;
        	display(PC, IR, inst_code, operand);
        	printf("Instruction \t : WM\nWriting data to memory...\n");
        	printf("**************************\n");
		}
		else if(inst_code == 0x02)
		{
			MAR = operand;
			MBR = dataMemory[MAR];
			display(PC, IR, inst_code, operand);
			printf("Instruction \t : RM\nReading data from memory...\n");
			printf("MBR\t\t : 0x%x\n", MBR);
        	printf("**************************\n");
		}
		else if(inst_code == 0x03)
		{
			display(PC, IR, inst_code, operand);
			printf("Instruction \t : BR\nBranch to 0x12A on next cycle...\n");
			PC = 0x12A;
        	printf("**************************\n");
		}
		else if(inst_code == 0x04)
		{
			MAR = operand;
			MBR = dataMemory[MAR];
			display(PC, IR, inst_code, operand);
			printf("Instruction \t : RM\nReading data from IO buffer...\n");
			printf("MBR\t\t : 0x%x\n", MBR);
        	printf("**************************\n");
		}
		else if(inst_code == 0x05)
		{
			IOAR = operand;
			ioBuffer[IOAR] = IOBR;
			display(PC, IR, inst_code, operand);
			printf("Instruction \t : WIO\nWriting to IO Buffer...\n");
        	printf("**************************\n");
		}
		else if(inst_code == 0x06)
		{
			MBR = operand;
			display(PC, IR, inst_code, operand);
			printf("Instruction \t : WB\nLoading data to MBR...\n");
			printf("MBR\t\t : 0x%x\n", MBR);
        	printf("**************************\n");
		}
		else if(inst_code == 0x07)
		{
			IOBR = operand;
			display(PC, IR, inst_code, operand);
			printf("Instruction \t : WIB\nLoading data to IOBR...\n");
			printf("IOBR\t\t : 0x%x\n", IOBR);
        	printf("**************************\n");
		}
		else 
		{
			display(PC, IR, inst_code, operand);
			printf("Instruction \t : EOP\nEnd of program.\n");
        	printf("**************************\n");
        	return 1;
		}
    }
    return 0;
   
}
void display(unsigned int PC, unsigned int IR, unsigned char inst_code, unsigned int operand){
	unsigned int offset = 0x2;
	printf("**************************\n");
	printf("PC \t\t : 0x%x\n", PC-offset);
	printf("Fetching instruction...\n");
	printf("IR\t\t : 0x%x\n", IR);
	printf("Instruction Code: 0x%x\n", inst_code);
	printf("Operand\t\t : 0x%x\n", operand);
	
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

