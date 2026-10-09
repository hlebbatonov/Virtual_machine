#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

#define MAX_INPUT_SIZE 10000000000
#define START_INPUT_SIZE 16
#define BUFFER_SIZE 50
#define START_STACK_SIZE 10
#define ACCURACY 10e-7

enum InputErrors
{
    INPUT_OK = 0,
    ERR_FILE_NOT_OPENED,
    ERR_BUFFER_NOT_CREATED,
    ERR_INSTRUCT_DID_NOT_READ,
    ERR_NUM_DID_NOT_READ,
    ERR_CALLOC_FAILED
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

InputErrors ReadFromFileToCodeFrame(int** code_frame, size_t* code_frame_size, FILE* file);
Errors RunInstructions(int* code_frame, size_t code_frame_size);
bool IsEqual(double a, double b);

int main()
{
    int* code_frame = {};
    size_t code_frame_size;

    FILE* file = stdin;
    InputErrors err = ReadFromFileToCodeFrame(&code_frame, &code_frame_size, file);

    if (err == INPUT_OK) RunInstructions(code_frame, code_frame_size);
    free(code_frame);

}

InputErrors ReadFromFileToCodeFrame(int** code_frame, size_t* code_frame_size, FILE* file)
{
    size_t input_size = START_INPUT_SIZE;

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

        if (input_res <= 0) {free(*code_frame); *code_frame_size = cp; return ERR_INSTRUCT_DID_NOT_READ;}
        else if ((curr_instruction < PUSH || curr_instruction > HLT)) \
            {free(*code_frame); *code_frame_size = cp; return ERR_INSTRUCT_DID_NOT_READ;}
        else if (input_res != 2 && curr_instruction == PUSH) {free(*code_frame); *code_frame_size = cp; return ERR_NUM_DID_NOT_READ;}
        else if (input_res == 2 && curr_instruction != PUSH) {free(*code_frame); *code_frame_size = cp; return ERR_INSTRUCT_DID_NOT_READ;}

        if (cp >= input_size - 2)
        {
            input_size *= 2;
            int* new_code_frame = (int*)realloc(*code_frame, input_size * sizeof(int));
            if (new_code_frame == NULL)
            {
                input_size /= 2;
                free(*code_frame);
                return ERR_CALLOC_FAILED;
            }
            *code_frame = new_code_frame;
        }

        (*code_frame)[cp++] = curr_instruction;
        if (curr_instruction == PUSH) (*code_frame)[cp++] = curr_num;
        if (curr_instruction == HLT) {*code_frame_size = cp; return INPUT_OK;}
    }
    *code_frame_size = cp;
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
            i++;
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


