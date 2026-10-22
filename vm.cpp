#include "common.h"

struct CPU
{
    stack_t* stack;
    unsigned int pc;
    float AX, BX, CX, DX;
};


InputErrors ReadFromFileToCodeFrame(CodeFrame* codeframe, char* executable_file_name);
Errors RunInstructions(CPU* cpu, CodeFrame* codeframe);
Errors InitCodeframe(CodeFrame* codeframe);

int main(int argc, char *argv[])
{
    CPU cpu1 = {};
    CodeFrame codeframe1 = {};

    Errors err_init = InitCodeframe(&codeframe1);
    if (err_init != OK) return err_init;

    cpu1.stack = (stack_t*)calloc(1, sizeof(stack_t));
    err_init = StackInit(cpu1.stack, START_STACK_SIZE);
    if (err_init != OK) return err_init;

    if (argc < 2)
    {
        printf("Error: missing required file. VM expects <assembler file>.\n");
        return 1;
    }
    char* executable_file_name = argv[1];

    InputErrors err = ReadFromFileToCodeFrame(&codeframe1, executable_file_name);
    if (err == INPUT_OK) {Errors err1 = RunInstructions(&cpu1, &codeframe1);}

    free(codeframe1.code_frame);

}

InputErrors ReadFromFileToCodeFrame(CodeFrame* codeframe, char* executable_file_name)
{
    FILE* executable_file = fopen(executable_file_name, "rb");
    if (executable_file == NULL) {printf("Error: no such file <%s>\n", executable_file_name); return ERR_FILE_NOT_OPENED;}

    size_t num_of_r_cells = fread(codeframe->code_frame, sizeof(cell), MAX_EXE_FILE_SIZE, executable_file);
    if (ferror(executable_file))
        {fclose(executable_file); printf("Error: invalid .bin file"); return ERR_ARE_ERRORS;}
    codeframe->code_frame_size = num_of_r_cells;

    fclose(executable_file);

    return INPUT_OK;
}

Errors RunInstructions(CPU* cpu, CodeFrame* codeframe)
{
    cpu->pc = 0;

    while (cpu->pc < codeframe->code_frame_size)
    {
        if (codeframe->code_frame[cpu->pc].instr == 0) return OK;
        if (codeframe->code_frame[cpu->pc].instr == HLT) return OK;
        else if (codeframe->code_frame[cpu->pc].instr == JMP)
        {
            cpu->pc = codeframe->code_frame[cpu->pc + 1].point;
            continue;
        }
        /*else if (codeframe->code_frame[cpu->pc].instr > JMP && codeframe->code_frame[cpu->pc].instr <= JNE)
        {
            bool need_to_jump = false;
            cell curr_elem_l = {};
            cell curr_elem_r = {};

            Errors err = StackPop(cpu->stack, &curr_elem_l);
            if (err != OK) return err;
            err = StackPop(cpu->stack, &curr_elem_r);
            if (err != OK) return err;

            switch(codeframe->code_frame[cpu->pc].instr)
            {
                case JA: need_to_jump = (curr_elem_l.num > curr_elem_r.num); break


            }
            cpu->pc = codeframe->code_frame[cpu->pc + 1].point;
            continue;
        }*/
        else if (codeframe->code_frame[cpu->pc].instr == PUSH)
        {
            Errors err = StackPush(cpu->stack, codeframe->code_frame[cpu->pc + 1]);
            if (err != OK) return err;
            cpu->pc += 2;
            continue;
        }
        else if (codeframe->code_frame[cpu->pc].instr == OUT)
        {
            cell curr_elem = {};
            Errors err = StackTop(cpu->stack, &curr_elem);
            if (err != OK) return err;
            printf("%g\n", curr_elem.num);
            cpu->pc++;
            continue;
        }
        else
        {
            cell curr_elem_l = {};
            cell curr_elem_r = {};
            cell curr_res = {};

            Errors err = StackPop(cpu->stack, &curr_elem_l);
            if (err != OK) return err;
            err = StackPop(cpu->stack, &curr_elem_r);
            if (err != OK) return err;

            switch(codeframe->code_frame[cpu->pc].instr)
            {
                case ADD: curr_res.num = curr_elem_r.num + curr_elem_l.num; break;
                case SUB: curr_res.num = curr_elem_r.num - curr_elem_l.num; break;
                case MUL: curr_res.num = curr_elem_r.num * curr_elem_l.num; break;
                case DIV:
                {
                    if (IsEqual(curr_elem_l.num, 0) == false)
                        curr_res.num = curr_elem_r.num / curr_elem_l.num;
                    else return ERR_DIV_ZERO;
                    break;
                }
                default: return ERR_UNKNOWN_INSTRUCTION;
            }
            err = StackPush(cpu->stack, curr_res);
            if (err != OK) return err;
            cpu->pc++;
            continue;
        }
        cpu->pc++;
    }
    return OK;
}

Errors InitCodeframe(CodeFrame* codeframe)
{
    codeframe->code_frame = (cell*)calloc(MAX_EXE_FILE_SIZE, sizeof(cell));
    if (codeframe->code_frame == NULL) return ERR_CALLOC_ERROR;

    codeframe->code_frame_size = START_CODEFRAME_SIZE;
    return OK;
}


