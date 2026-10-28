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
#define MAX_EXE_FILE_SIZE 1000
#define START_RET_POINTS_ARR 20

const unsigned int screen_width = 10;
const unsigned int screen_height = 10;
const unsigned int video_ram_size = screen_width * screen_height;


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

enum InstructionsCodes
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
    CALL,
    RET,
    VPUSH,
    VPOP,
    VOUT,
    ADD,
    SUB,
    MUL,
    DIV,
    OUT,
    HLT
};

static const char* instr_names_list[] =
{
    [0]     = "UNKNOWN INSTRUCTION",
    [PUSH]  = "PUSH",
    [RPUSH] = "RPUSH",
    [RPOP]  = "RPOP",
    [JMP]   = "JMP",
    [JA]    = "JA",
    [JAE]   = "JAE",
    [JB]    = "JB",
    [JBE]   = "JBE",
    [JE]    = "JE",
    [JNE]   = "JNE",
    [CALL]  = "CALL",
    [RET]   = "RET",
    [VPUSH] = "VPUSH",
    [VPOP]  = "VPOP",
    [VOUT]  = "VOUT",
    [ADD]   = "ADD",
    [SUB]   = "SUB",
    [MUL]   = "MUL",
    [DIV]   = "DIV",
    [OUT]   = "OUT",
    [HLT]   = "HLT"
};
const int num_of_instr = sizeof(instr_names_list) / sizeof(instr_names_list[0]);

enum ArgTypes
{
    NO_ARG = 0,
    NUM,
    REG,
    INDEX,
    POINT
};

struct InstrInfo
{
    InstructionsCodes instr_code;
    const char* instr_name;
    unsigned int instr_num_args;
    ArgTypes instr_arg_type;
};

static const InstrInfo instr_info_list[] =
{
    [PUSH]  = {PUSH,  "PUSH",  1, NUM},
    [RPUSH] = {RPUSH, "RPUSH", 1, REG},
    [RPOP]  = {RPOP,  "RPOP",  1, REG},
    [JMP]   = {JMP,   "JMP",   1, POINT},
    [JA]    = {JA,    "JA",    1, POINT},
    [JAE]   = {JAE,   "JAE",   1, POINT},
    [JB]    = {JB,    "JB",    1, POINT},
    [JBE]   = {JBE,   "JBE",   1, POINT},
    [JE]    = {JE,    "JE",    1, POINT},
    [JNE]   = {JNE,   "JNE",   1, POINT},
    [CALL]  = {CALL,  "CALL",  1, POINT},
    [RET]   = {RET,   "RET",   0, NO_ARG},
    [VPUSH] = {VPUSH, "VPUSH", 1, INDEX},
    [VPOP]  = {VPOP,  "VPOP",  1, INDEX},
    [VOUT]  = {VOUT,  "VOUT",  0, NO_ARG},
    [ADD]   = {ADD,   "ADD",   0, NO_ARG},
    [SUB]   = {SUB,   "SUB",   0, NO_ARG},
    [MUL]   = {MUL,   "MUL",   0, NO_ARG},
    [DIV]   = {DIV,   "DIV",   0, NO_ARG},
    [OUT]   = {OUT,   "OUT",   0, NO_ARG},
    [HLT]   = {HLT,   "HLT",   0, NO_ARG}
};

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
    REG_DX,
    NUM_OF_REGS
};

static const char* reg_names[NUM_OF_REGS] =
{
    [REG_AX] = "AX",
    [REG_BX] = "BX",
    [REG_CX] = "CX",
    [REG_DX] = "DX",
};

