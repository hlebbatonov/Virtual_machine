#include "common.h"

InputErrors ReadFromFileToCodeFrame(int** code_frame, size_t* code_frame_size, char* file_name);
Errors RunInstructions(int* code_frame, size_t code_frame_size);
bool IsEqual(double a, double b);

int main(int argc, char *argv[])
{
    int* code_frame = NULL;
    size_t code_frame_size;

    if (argc < 2)
    {
        printf("Error: missing required file. VM expects <assembler file>.\n");
        return 1;
    }
    char* file_name = argv[1];
    InputErrors err = ReadFromFileToCodeFrame(&code_frame, &code_frame_size, file_name);

    if (err == INPUT_OK) RunInstructions(code_frame, code_frame_size);
    free(code_frame);

}

InputErrors ReadFromFileToCodeFrame(int** code_frame, size_t* code_frame_size, char* file_name)
{
    size_t input_size = START_INPUT_SIZE;

    FILE* file = fopen(file_name, "r");
    if (file == NULL)
        return ERR_FILE_NOT_OPENED;

    *code_frame = (int*)calloc(input_size, sizeof(int));
    if (*code_frame == NULL)
        return ERR_BUFFER_NOT_CREATED;

    char buffer[BUFFER_SIZE] = {};
    unsigned int cp = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        int curr_instruction = -1;
        int curr_num = -1;

        int input_res = sscanf(buffer, "%d %d", &curr_instruction, &curr_num);

        if (input_res <= 0) {free(*code_frame); *code_frame = NULL; *code_frame_size = cp; fclose(file); return ERR_INSTRUCT_DID_NOT_READ;}
        else if ((curr_instruction < PUSH || curr_instruction > HLT)) \
            {free(*code_frame); *code_frame = NULL; *code_frame_size = cp; fclose(file);  return ERR_INSTRUCT_DID_NOT_READ;}
        else if (input_res != 2 && curr_instruction == PUSH)
             {free(*code_frame); *code_frame = NULL; *code_frame_size = cp; fclose(file);  return ERR_NUM_DID_NOT_READ;}
        else if (input_res == 2 && curr_instruction != PUSH)
            {free(*code_frame); *code_frame = NULL; *code_frame_size = cp; fclose(file); return ERR_INSTRUCT_DID_NOT_READ;}

        if (cp >= input_size - 2)
        {
            input_size *= 2;
            int* new_code_frame = (int*)realloc(*code_frame, input_size * sizeof(int));
            if (new_code_frame == NULL)
            {
                input_size /= 2;
                free(*code_frame);
                *code_frame = NULL;
                fclose(file);
                return ERR_CALLOC_FAILED;
            }
            *code_frame = new_code_frame;
        }

        (*code_frame)[cp++] = curr_instruction;
        if (curr_instruction == PUSH) (*code_frame)[cp++] = curr_num;
    }
    *code_frame_size = cp;
    fclose(file);
    return INPUT_OK;
}

Errors RunInstructions(int* code_frame, size_t code_frame_size)
{
    stack_t stack = {};
    Errors err = StackInit(&stack, START_STACK_SIZE);
    if (err != OK) return err;

    size_t i = 0;

    while (i < code_frame_size)
    {
        if (code_frame[i] == HLT) return OK;

        else if (code_frame[i] == PUSH)
        {
            err = StackPush(&stack, (double)(code_frame[i + 1]));
            if (err != OK) return err;
            i += 2;
            continue;
        }
        else if (code_frame[i] == OUT)
        {
            double curr_elem = 0;
            err = StackTop(&stack, &curr_elem);
            if (err != OK) return err;
            printf("%g", curr_elem);
            i++;
            continue;
        }
        else
        {
            double curr_elem_l = 0;
            double curr_elem_r = 0;

            err = StackPop(&stack, &curr_elem_l);
            if (err != OK) return err;
            err = StackPop(&stack, &curr_elem_r);
            if (err != OK) return err;

            switch(code_frame[i])
            {
                case ADD: err = StackPush(&stack, curr_elem_r + curr_elem_l); break;
                case SUB: err = StackPush(&stack, curr_elem_r - curr_elem_l); break;
                case MUL: err = StackPush(&stack, curr_elem_r * curr_elem_l); break;
                case DIV:
                {
                    if (IsEqual(curr_elem_l, 0) == false) err = StackPush(&stack, curr_elem_r / curr_elem_l);
                    else return ERR_DIV_ZERO;
                    break;
                }
                default: return ERR_UNKNOWN_INSTRUCTION;
            }
            if (err != OK) return err;
        }
        i++;
    }
    return OK;
}

bool IsEqual(double a, double b)
{
    return (-ACCURACY < a - b && a - b < ACCURACY);
}


