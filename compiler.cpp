#include <cstdio>
#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT_SIZE 10000000000
#define START_INPUT_SIZE 16
#define BUFFER_SIZE 50
#define START_STACK_SIZE 10

enum InputErrors
{
    INPUT_OK = 0,
    ERR_FILE_NOT_OPENED,
    ERR_BUFFER_NOT_CREATED,
    ERR_INSTRUCT_DID_NOT_READ,
    ERR_NUM_DID_NOT_READ,
    ERR_CALLOC_FAILED
};

int main()
{
    char program_name[] = "instr.txt";
    FILE* program = fopen(program_name, "r");
    if (program == NULL) {printf("Error: no such file <%s>", program_name); return 1;}


}
