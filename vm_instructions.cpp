#include "vm_instructions.h"

Errors FuncPUSH(CPU* cpu, CodeFrame* codeframe)
{
    Errors err = StackPush(cpu->stack, codeframe->code_frame[cpu->pc + 1]);
    if (err != OK) return err;
    cpu->pc += 2;
    return OK;
}

Errors FuncRPUSH(CPU* cpu, CodeFrame* codeframe)
{
    Errors err = StackPush(cpu->stack, cpu->regs[codeframe->code_frame[cpu->pc + 1].reg]);
    if (err != OK) return err;
    cpu->pc += 2;
    return OK;
}

Errors FuncRPOP(CPU* cpu, CodeFrame* codeframe)
{
    Errors err = StackPop(cpu->stack, &cpu->regs[codeframe->code_frame[cpu->pc + 1].reg]);
    if (err != OK) return err;
    cpu->pc += 2;
    return OK;
}

Errors FuncJMP(CPU* cpu, CodeFrame* codeframe)
{
    cpu->pc = codeframe->code_frame[cpu->pc + 1].point;
    return OK;
}

Errors FuncJMPCond(CPU* cpu, CodeFrame* codeframe)
{
    cell val_a = {};
    cell val_b = {};
    Errors err = GetValuesFromStack(&val_a, &val_b, cpu, codeframe);
    if (err != OK) return err;

    if (comparators_list[codeframe->code_frame[cpu->pc].instr](val_a, val_b) == true)
        FuncJMP(cpu, codeframe);
    else cpu->pc += 2;

    return OK;
}

Errors FuncOUT(CPU* cpu, CodeFrame* codeframe)
{
    cell val = {};
    Errors err = StackPop(cpu->stack, &val);
    if (err != OK) return err;

    printf("%g\n", val.num);
    cpu->pc++;
    return OK;
}

Errors FuncOperand(CPU* cpu, CodeFrame* codeframe)
{
    cell val_a = {};
    cell val_b = {};
    cell val_res = {};

    Errors err = StackPop(cpu->stack, &val_b);
    if (err != OK) return err;
    err = StackPop(cpu->stack, &val_a);
    if (err != OK) return err;

    err = operands_list[codeframe->code_frame[cpu->pc].instr](val_a, val_b, &val_res);
    if (err != OK) return err;
    err = StackPush(cpu->stack, val_res);
    if (err != OK) return err;
    cpu->pc++;
    return OK;
}

Errors GetValuesFromStack(cell* val_a, cell* val_b, CPU* cpu, CodeFrame* codeframe)
{
    Errors err = StackPop(cpu->stack, val_b);
    if (err != OK) return err;
    err = StackPop(cpu->stack, val_a);
    if (err != OK) return err;

    err = StackPush(cpu->stack, *val_a);
    if (err != OK) return err;
    err = StackPush(cpu->stack, *val_b);
    if (err != OK) return err;

    return OK;
}

bool IsValAbove(cell val_a, cell val_b)
{
    return (val_a.num > val_b.num);
}

bool IsValAboveEqual(cell val_a, cell val_b)
{
    return (val_a.num >= val_b.num);
}

bool IsValBelow(cell val_a, cell val_b)
{
    return (val_a.num < val_b.num);
}

bool IsValBelowEqual(cell val_a, cell val_b)
{
    return (val_a.num <= val_b.num);
}

bool IsValEqual(cell val_a, cell val_b)
{
    return (val_a.num == val_b.num);
}

bool IsValNotEqual(cell val_a, cell val_b)
{
    return (val_a.num != val_b.num);
}

Errors OperandADD(cell val_a, cell val_b, cell* val_res)
{
    val_res->num = val_a.num + val_b.num;
    return OK;
}

Errors OperandSUB(cell val_a, cell val_b, cell* val_res)
{
    val_res->num = val_a.num - val_b.num;
    return OK;
}

Errors OperandMUL(cell val_a, cell val_b, cell* val_res)
{
    val_res->num = val_a.num * val_b.num;
    return OK;
}

Errors OperandDIV(cell val_a, cell val_b, cell* val_res)
{
    if (val_b.num == 0) return ERR_DIV_ZERO;
    val_res->num = val_a.num / val_b.num;
    return OK;
}
