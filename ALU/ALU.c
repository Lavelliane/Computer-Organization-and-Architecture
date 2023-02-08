#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

void print_binary(unsigned char n)
{
    int i;
    for (i = 7; i >= 0; i--)
    {
        if (n & (1 << i))
            printf("1");
        else
            printf("0");
    }
    printf("\n");
}
unsigned char twos_compliment(signed char value)
{
    return (unsigned char)(~value + 1);
}

int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signals)
{
    printf("Fetching Operands...\n");
    printf("OP1:");
    print_binary(operand1);
    printf("OP2:");
    print_binary(operand2);

    if (control_signals == 0x02)
    {
        printf("Operation = SUB");
        printf("2's complement of OP2");
        printf("Adding OP1 & OP2...");
        printf("ACC = ");
        printf("%x", operand1 + twos_compliment(operand2));
        

    }
}

int main()
{
    ALU(0x03, 0x05, 0x02);
}