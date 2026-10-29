#include "common.h"
#include "raylib.h"

const struct Color colors_list[] = {
    [COLOR_BLACK]          = {   0,   0,   0, 255 },
    [COLOR_BLUE]           = {   0,   0, 170, 255 },
    [COLOR_GREEN]          = {   0, 170,   0, 255 },
    [COLOR_CYAN]           = {   0, 170, 170, 255 },
    [COLOR_RED]            = { 170,   0,   0, 255 },
    [COLOR_MAGENTA]        = { 170,   0, 170, 255 },
    [COLOR_YELLOW]         = { 170,  85,   0, 255 },
    [COLOR_WHITE]          = { 170, 170, 170, 255 },
};

struct video_ram_t
{
    ColorCodes* video_ram;
    unsigned int video_ram_size;
    unsigned int width;
    unsigned int height;
    Image canvas;
    Color* pixels;
    Texture2D texture;
};

struct CPU
{
    stack_t* stack;

    unsigned int pc;

    cell regs[NUM_OF_REGS];

    stack_t* ret_points;

    cell* ram;
    size_t ram_size;

    video_ram_t* video_ram;
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
Errors FuncMPUSH(CPU* cpu, CodeFrame* codeframe);
Errors FuncMPOP(CPU* cpu, CodeFrame* codeframe);
Errors FuncMOUT(CPU* cpu, CodeFrame* codeframe);
Errors FuncVPUSH(CPU* cpu, CodeFrame* codeframe);
Errors FuncVPOP(CPU* cpu, CodeFrame* codeframe);
Errors FuncVOUT(CPU* cpu, CodeFrame* codeframe);
Errors FuncOperand(CPU* cpu, CodeFrame* codeframe);
Errors FuncSQRT(CPU* cpu, CodeFrame* codeframe);
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
    [MPUSH] = FuncMPUSH,
    [MPOP] = FuncMPOP,
    [MOUT] = FuncMOUT,
    [VPUSH] = FuncVPUSH,
    [VPOP] = FuncVPOP,
    [VOUT] = FuncVOUT,
    [ADD] = FuncOperand,
    [SUB] = FuncOperand,
    [MUL] = FuncOperand,
    [DIV] = FuncOperand,
    [SQRT] = FuncSQRT,
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

static const char* colors[] =
{
    "\033[40m",   // 0: Черный
    "\033[41m",   // 1: Красный
    "\033[42m",   // 2: Зеленый
    "\033[43m",   // 3: Желтый
    "\033[44m",   // 4: Синий
    "\033[45m",   // 5: Фиолетовый
    "\033[46m",   // 6: Голубой
    "\033[47m",   // 7: Светло-серый
    "\033[100m",  // 8: Темно-серый
    "\033[101m",  // 9: Ярко-красный
    "\033[102m",  // 10: Ярко-зеленый
    "\033[103m",  // 11: Ярко-желтый
    "\033[104m",  // 12: Ярко-синий
    "\033[105m",  // 13: Ярко-фиолетовый
    "\033[106m",  // 14: Ярко-голубой
    "\033[107m"   // 15: Белый
};
const unsigned int num_of_colors = sizeof(colors) / sizeof(colors[0]);

