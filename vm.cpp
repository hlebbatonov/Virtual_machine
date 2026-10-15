#include "common.h"

struct CPU
{
    stack_t* stack;
    unsigned int cp;
    double AX, BX, CX, DX;
};

struct CodeFrame
{
    int* code_frame;
    size_t code_frame_size;
};

InputErrors ReadFromFileToCodeFrame(CodeFrame* codeframe, char* file_name);
Errors RunInstructions(CPU* cpu, CodeFrame* codeframe);
bool IsEqual(double a, double b);

int main(int argc, char *argv[])
{
    CPU cpu1 = {};
    CodeFrame codeframe1 = {};
    cpu1.stack = (stack_t*)calloc(1, sizeof(stack_t));
    Errors err_init = StackInit(cpu1.stack, START_STACK_SIZE);
    if (err_init != OK) return err_init;

    if (argc < 2)
    {
        printf("Error: missing required file. VM expects <assembler file>.\n");
        return 1;
    }
    char* file_name = argv[1];

    InputErrors err = ReadFromFileToCodeFrame(&codeframe1, file_name);

    if (err == INPUT_OK) RunInstructions(&cpu1, &codeframe1);
    free(codeframe1.code_frame);

}

InputErrors ReadFromFileToCodeFrame(CodeFrame* codeframe, char* file_name)
{
    codeframe->code_frame_size = START_INPUT_SIZE;

    FILE* file = fopen(file_name, "r");
    if (file == NULL)
        return ERR_FILE_NOT_OPENED;

    codeframe->code_frame = (int*)calloc(codeframe->code_frame_size, sizeof(int));
    if (codeframe->code_frame == NULL)
        return ERR_BUFFER_NOT_CREATED;

    char buffer[BUFFER_SIZE] = {};
    unsigned int cp = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        int curr_instruction = -1;
        int curr_num = -1;

        int input_res = sscanf(buffer, "%d %d", &curr_instruction, &curr_num);

        if (input_res <= 0)
            {free(codeframe->code_frame); codeframe->code_frame = NULL; fclose(file); return ERR_INSTRUCT_DID_NOT_READ;}
        else if ((curr_instruction < PUSH || curr_instruction > HLT)) \
            {free(codeframe->code_frame); codeframe->code_frame = NULL; fclose(file); return ERR_INSTRUCT_DID_NOT_READ;}
        else if (input_res != 2 && curr_instruction == PUSH)
            {free(codeframe->code_frame); codeframe->code_frame = NULL; fclose(file); return ERR_NUM_DID_NOT_READ;}
        else if (input_res == 2 && curr_instruction != PUSH)
            {free(codeframe->code_frame); codeframe->code_frame = NULL; fclose(file); return ERR_INSTRUCT_DID_NOT_READ;}

        if (cp >= codeframe->code_frame_size - 2)
        {
            codeframe->code_frame_size *= 2;
            int* new_code_frame = (int*)realloc(codeframe->code_frame, codeframe->code_frame_size * sizeof(int));
            if (new_code_frame == NULL)
            {
                codeframe->code_frame_size = 0;
                free(codeframe->code_frame);
                codeframe->code_frame = NULL;
                fclose(file);
                return ERR_CALLOC_FAILED;
            }
            codeframe->code_frame = new_code_frame;
        }

        (codeframe->code_frame)[cp++] = curr_instruction;
        if (curr_instruction == PUSH) (codeframe->code_frame)[cp++] = curr_num;
    }
    codeframe->code_frame_size = cp;
    int* new_code_frame = (int*)realloc(codeframe->code_frame, codeframe->code_frame_size * sizeof(int));
    if (new_code_frame == NULL)
    {
        codeframe->code_frame_size = 0;
        free(codeframe->code_frame);
        codeframe->code_frame = NULL;
        fclose(file);
        return ERR_CALLOC_FAILED;
    }
    codeframe->code_frame = new_code_frame;
    fclose(file);
    return INPUT_OK;
}

Errors RunInstructions(CPU* cpu, CodeFrame* codeframe)
{
    cpu->cp = 0;

    while (cpu->cp < codeframe->code_frame_size)
    {
        if (codeframe->code_frame[cpu->cp] == HLT) return OK;

        else if (codeframe->code_frame[cpu->cp] == PUSH)
        {
            Errors err = StackPush(cpu->stack, (double)(codeframe->code_frame[cpu->cp + 1]));
            if (err != OK) return err;
            cpu->cp += 2;
            continue;
        }
        else if (codeframe->code_frame[cpu->cp] == OUT)
        {
            double curr_elem = 0;
            Errors err = StackTop(cpu->stack, &curr_elem);
            if (err != OK) return err;
            printf("%g", curr_elem);
            cpu->cp++;
            continue;
        }
        else
        {
            double curr_elem_l = 0;
            double curr_elem_r = 0;

            Errors err = StackPop(cpu->stack, &curr_elem_l);
            if (err != OK) return err;
            err = StackPop(cpu->stack, &curr_elem_r);
            if (err != OK) return err;

            switch(codeframe->code_frame[cpu->cp])
            {
                case ADD: err = StackPush(cpu->stack, curr_elem_r + curr_elem_l); break;
                case SUB: err = StackPush(cpu->stack, curr_elem_r - curr_elem_l); break;
                case MUL: err = StackPush(cpu->stack, curr_elem_r * curr_elem_l); break;
                case DIV:
                {
                    if (IsEqual(curr_elem_l, 0) == false) err = StackPush(cpu->stack, curr_elem_r / curr_elem_l);
                    else return ERR_DIV_ZERO;
                    break;
                }
                default: return ERR_UNKNOWN_INSTRUCTION;
            }
            if (err != OK) return err;
        }
        cpu->cp++;
    }
    return OK;
}

bool IsEqual(double a, double b)
{
    return (-ACCURACY < a - b && a - b < ACCURACY);
}


