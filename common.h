#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

#define MAX_INPUT_SIZE 100000000
#define START_INPUT_SIZE 16
#define BUFFER_SIZE 50
#define START_STACK_SIZE 10
#define ACCURACY 10e-7
#define MAX_INSTR_SIZE 50
#define INSTR_NAME_SIZE 50

enum InputErrors
{
    INPUT_OK = 0,
    ERR_FILE_NOT_OPENED,
    ERR_BUFFER_NOT_CREATED,
    ERR_INSTRUCT_DID_NOT_READ,
    ERR_NUM_DID_NOT_READ,
    ERR_CALLOC_FAILED,
    ERR_ARE_ERRORS
};

enum Instructions
{
    PUSH = 1,
    ADD,
    SUB,
    MUL,
    DIV,
    OUT,
    HLT
};

static const char* instructions_list[] =
{
    "UNKNOWN INSTRUCTION",
    "PUSH",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "OUT",
    "HLT"
};
