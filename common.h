#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "stack.h"

#define MAX_INPUT_SIZE 100000000
#define START_CODEFRAME_SIZE 50
#define BUFFER_SIZE 50
#define START_STACK_SIZE 10
#define MAX_INSTR_SIZE 50
#define INSTR_NAME_SIZE 50
#define POINT_NAME_SIZE 16
#define START_POINTS_ARR_SIZE 20
#define MAX_ARG_SIZE 10
#define NUM_OF_REGS 4
#define MAX_EXE_FILE_SIZE 1000

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
    RPUSH,
    RPOP,
    JMP,
    JA,
    JAE,
    JB,
    JBE,
    JE,
    JNE,

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
    "RPUSH",
    "RPOP",
    "JMP",
    "JA",
    "JAE",
    "JB",
    "JBE",
    "JE",
    "JNE",

    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "OUT",
    "HLT"
};

const int num_of_instr = sizeof(instructions_list) / sizeof(instructions_list[0]);

struct CodeFrame
{
    cell* code_frame;
    size_t code_frame_size;
};

enum RegsIndexes
{
    REG_AX = 0,
    REG_BX,
    REG_CX,
    REG_DX
};

static const char* reg_names[NUM_OF_REGS] =
{
    [REG_AX] = "AX",
    [REG_BX] = "BX",
    [REG_CX] = "CX",
    [REG_DX] = "DX",
};
