#include "common.h"

struct CPU
{
    stack_t* stack;
    unsigned int pc;
    cell regs[NUM_OF_REGS];
    stack_t* ret_points;
    cell* video_ram;
    size_t video_ram_size;
};

Errors FuncPUSH(CPU* cpu, CodeFrame* codeframe);
Errors FuncRPUSH(CPU* cpu, CodeFrame* codeframe);
Errors FuncRPOP(CPU* cpu, CodeFrame* codeframe);
Errors FuncJMP(CPU* cpu, CodeFrame* codeframe);
Errors FuncJA(CPU* cpu, CodeFrame* codeframe);
Errors FuncJAE(CPU* cpu, CodeFrame* codeframe);
Errors FuncJB(CPU* cpu, CodeFrame* codeframe);
Errors FuncJBE(CPU* cpu, CodeFrame* codeframe);
Errors FuncJE(CPU* cpu, CodeFrame* codeframe);
Errors FuncJNE(CPU* cpu, CodeFrame* codeframe);
Errors FuncCALL(CPU* cpu, CodeFrame* codeframe);
Errors FuncRET(CPU* cpu, CodeFrame* codeframe);
Errors FuncVPUSH(CPU* cpu, CodeFrame* codeframe);
Errors FuncVPOP(CPU* cpu, CodeFrame* codeframe);
Errors FuncVOUT(CPU* cpu, CodeFrame* codeframe);
Errors FuncOperand(CPU* cpu, CodeFrame* codeframe);
Errors FuncOUT(CPU* cpu, CodeFrame* codeframe);

bool IsValAbove(cell val_a, cell val_b);
bool IsValAboveEqual(cell val_a, cell val_b);
bool IsValBelow(cell val_a, cell val_b);
bool IsValBelowEqual(cell val_a, cell val_b);
bool IsValEqual(cell val_a, cell val_b);
bool IsValNotEqual(cell val_a, cell val_b);

Errors OperandADD(cell val_a, cell val_b, cell* val_res);
Errors OperandSUB(cell val_a, cell val_b, cell* val_res);
Errors OperandMUL(cell val_a, cell val_b, cell* val_res);
Errors OperandDIV(cell val_a, cell val_b, cell* val_res);

Errors GetValuesFromStack(cell* val_a, cell* val_b, CPU* cpu, CodeFrame* codeframe);
Errors FuncJMPCond(CPU* cpu, CodeFrame* codeframe);

typedef Errors (*RunOneInstr)(CPU* cpu, CodeFrame* codeframe);
const RunOneInstr instr_list[] =
{
    [PUSH] = FuncPUSH,
    [RPUSH] = FuncRPUSH,
    [RPOP] = FuncRPOP,
    [JMP] = FuncJMP,
    [JA] = FuncJMPCond,
    [JAE] = FuncJMPCond,
    [JB] = FuncJMPCond,
    [JBE] = FuncJMPCond,
    [JE] = FuncJMPCond,
    [JNE] = FuncJMPCond,
    [CALL] = FuncCALL,
    [RET] = FuncRET,
    [VPUSH] = FuncVPUSH,
    [VPOP] = FuncVPOP,
    [VOUT] = FuncVOUT,
    [ADD] = FuncOperand,
    [SUB] = FuncOperand,
    [MUL] = FuncOperand,
    [DIV] = FuncOperand,
    [OUT] = FuncOUT
};

typedef bool (*Comparator)(cell val_a, cell val_b);
const Comparator comparators_list[] =
{
    [JA] = IsValAbove,
    [JAE] = IsValAboveEqual,
    [JB] = IsValBelow,
    [JBE] = IsValBelowEqual,
    [JE] = IsValEqual,
    [JNE] = IsValNotEqual
};

typedef Errors (*Operand)(cell val_a, cell val_b, cell* val_res);
const Operand operands_list[] =
{
    [ADD] = OperandADD,
    [SUB] = OperandSUB,
    [MUL] = OperandMUL,
    [DIV] = OperandDIV
};


