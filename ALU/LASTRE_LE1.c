//CPE3202 LE1: The ALU
//Made By: Jhury Kevin Lastre
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<limits.h>

void ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals);
void display(unsigned char operand1, unsigned char operand2, unsigned char control_signals);
void decToBinary(unsigned int n, int bits);
unsigned char twosComp(unsigned char data);
bool detectCarry(unsigned char x, unsigned char y);
bool isNegative(unsigned char x);
void setFlags(unsigned int ACC, unsigned char OP1, unsigned char OP2);
void booths(unsigned char q, unsigned char m);
void displayCycle(unsigned char acc, unsigned char q, unsigned char q_prev, unsigned char m);

int main() {
	
	ALU(0x0A,0x1B,0x01); // 10 + 27 (add)
	ALU(0x0A,0x05,0x02); // 5 - 3 (subtract)
	ALU(0x07,0x05,0x03); // 7 * 5 (multiply 4 bits to 8 bit result)
	ALU(0x08,0x0D, 0x04); //AND
	ALU(0x08,0x0D, 0x05); //OR
	ALU(0x08,0x0D, 0x06); //NOT
	ALU(0x08,0x0D, 0x07); //XOR
	ALU(0x2D,0x02, 0x08); //LOGICAL RIGHT SHIFT
	ALU(0x2D,0x02, 0x09); //LOGICAL LEFT SHIFT
	return 0;
}


void ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals){
	display(operand1, operand2, control_signals);
	unsigned int ACC;
	switch(control_signals){
		case 0x01:
			if(isNegative(operand1)){
				printf("2's complement OP1\n");
				operand1 = twosComp(operand1);
			}
			if(isNegative(operand2)){
				printf("2's complement OP2\n");
				operand2 = twosComp(operand2);
			}
			ACC = operand1 + operand2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC, operand1, operand2);
			break;
		case 0x02:
			if(isNegative(operand1)){
				printf("2's complement OP1\n");
				operand1 = twosComp(operand1);
			}
			operand2 = twosComp(operand2);
			ACC = operand1 + operand2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC, operand1, operand2);
			break;
		case 0x03:
			booths(operand1, operand2);
			ACC = operand1 * operand2;
			printf("ACC = ");
			decToBinary(ACC, 8);
			printf("\n");
			setFlags(ACC, operand1, operand2);
			break;
		case 0x04:
			ACC = operand1 & operand2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC, operand1, operand2);
			break;
		case 0x05:
			ACC = operand1 | operand2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC, operand1, operand2);
			break;
		case 0x06:
			operand2 = 0x00;
			ACC = ~operand1;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC, operand1, operand2);
			break;
		case 0x07:
			ACC = operand1 ^ operand2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC, operand1, operand2);
			break;
		case 0x08:
			ACC = operand1 >> operand2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC, operand1, operand2);
			break;
		case 0x09:
			ACC = operand1 << operand2;
			printf("ACC = ");
			decToBinary(ACC, 16);
			printf("\n");
			setFlags(ACC, operand1, operand2);
			break;
		default:
			printf("Invalid Control Signal");
			break;		
	}
	printf("\n*********************************************************\n");
}

bool isNegative(unsigned char x) {
	//if msb is one then it is negative
    return x & (1 << 7);
}

void setFlags(unsigned int ACC, unsigned char OP1, unsigned char OP2){
	int CF = 0, ZF = 0, SF = 0, OF = 0;
	if(ACC == 0x00){
		ZF = 1;
	}
	if(ACC > CHAR_BIT){
		CF = 1;
	}
	if((OP1 < 0 && OP2 < 0 && ACC > 0) || (OP1 > 0 && OP2 > 0 && ACC < 0)){
		OF = 1;
	}
	if(ACC & (1 << 7)){
		SF = 1;
	}
	printf("ZF = %d   CF = %d   SF = %d   OF = %d", ZF, CF, SF, OF);
}

unsigned char twosComp(unsigned char data) {
  return ~data + 1;
}

bool detectCarry(unsigned char x, unsigned char y) {
  unsigned int result = x + y;
  return result > UINT_MAX;
}

void display(unsigned char operand1, unsigned char operand2, unsigned char control_signals){
	printf("*********************************************************\n");
	printf("Fetching operands...\n");
	printf("OP1 = ");
	decToBinary(operand1, 8);
	printf("\n");
	printf("OP2 = ");
	decToBinary(operand2, 8);
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

