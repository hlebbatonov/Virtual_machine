#include "vm_instructions.h"
#include "math.h"

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

Errors FuncSQRT(CPU* cpu, CodeFrame* codeframe)
{
    cell val_a = {};
    Errors err = StackPop(cpu->stack, &val_a);
    if (err != OK) return err;
    if (val_a.num < 0  || isnan(val_a.num)) return ERR_NEGATIVE_VAL_UNDER_SQRT;
    else
    {
        val_a.num = sqrtf(val_a.num);
        err = StackPush(cpu->stack, val_a);
        if (err != OK) return err;
    }
    cpu->pc++;
    return OK;
}

Errors FuncCALL(CPU* cpu, CodeFrame* codeframe)
{
    cell ret_pc = {};
    ret_pc.point = cpu->pc + 2;
    Errors err = StackPush(cpu->ret_points, ret_pc);
    if (err != OK) return err;

    cpu->pc = codeframe->code_frame[cpu->pc + 1].point;
    return OK;
}

Errors FuncRET(CPU* cpu, CodeFrame* codeframe)
{
    cell ret_pc = {};
    Errors err = StackPop(cpu->ret_points, &ret_pc);
    if (err != OK) return err;

    cpu->pc = ret_pc.point;
    return OK;
}

Errors FuncMPUSH(CPU* cpu, CodeFrame* codeframe)
{
    cell index = {};
    cell val = {};
    index = codeframe->code_frame[cpu->pc + 1];
    if (index.index >= cpu->ram_size) return ERR_INVALID_INDEX;

    Errors err = StackPop(cpu->stack, &val);
    if (err != OK) return err;

    cpu->ram[index.index] = val;

    cpu->pc += 2;
    return OK;
}

Errors FuncMPOP(CPU* cpu, CodeFrame* codeframe)
{
    cell index = {};
    cell val = {};
    index = codeframe->code_frame[cpu->pc + 1];
    if (index.index >= cpu->ram_size) return ERR_INVALID_INDEX;

    val = cpu->ram[index.index];

    Errors err = StackPush(cpu->stack, val);
    if (err != OK) return err;

    cpu->pc += 2;
    return OK;
}

Errors FuncMOUT(CPU* cpu, CodeFrame* codeframe)
{
    for (unsigned int i = 0; i < screen_height; i++)
    {
        for (unsigned int j = 0; j < screen_width; j++)
        {
            printf("%7g ", cpu->ram[i * screen_width + j].num);
        }
        printf("\n");
    }

    cpu->pc += 1;
    return OK;
}

Errors FuncVPUSH(CPU* cpu, CodeFrame* codeframe)
{
    cell index = {};
    cell val = {};
    index = codeframe->code_frame[cpu->pc + 1];
    if (index.index >= cpu->video_ram->video_ram_size) return ERR_INVALID_INDEX;

    Errors err = StackPop(cpu->stack, &val);
    if (err != OK) return err;
    val.color = (ColorCodes)((int)val.num);
    cpu->video_ram->video_ram[index.index] = val.color;
    cpu->pc += 2;
    return OK;
}

Errors FuncVPOP(CPU* cpu, CodeFrame* codeframe)
{
    cell index = {};
    cell val = {};
    index = codeframe->code_frame[cpu->pc + 1];
    if (index.index >= cpu->video_ram->video_ram_size) return ERR_INVALID_INDEX;

    val.color = cpu->video_ram->video_ram[index.index];

    Errors err = StackPush(cpu->stack, val);
    if (err != OK) return err;

    cpu->pc += 2;
    return OK;
}

Errors FuncVOUT(CPU* cpu, CodeFrame* codeframe)
{
    for (size_t i = 0; i < (size_t)cpu->video_ram->video_ram_size; i++)
    {
        ColorCodes color_code = cpu->video_ram->video_ram[i];
        cpu->video_ram->pixels[i] = colors_list[color_code];
    }

    UpdateTexture(cpu->video_ram->texture, cpu->video_ram->pixels);

    cpu->pc += 1;
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
