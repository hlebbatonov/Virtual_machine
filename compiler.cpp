#include "common.h"
#include <cstdio>
#include <cstdlib>

struct PointsArr
{
    char** points_arr;
    size_t points_arr_capacity;
};

#define VALIDATE_POINT(validation, message_err) do                                           \
{                                                                                            \
    if (validation)                                                                          \
    {                                                                                        \
        printf("Error: %s at %s:%u\n", message_err, program_file_name, pc);                  \
        flag_are_errors_in_program = ERR_ARE_ERRORS;                                                        \
    }                                                                                        \
} while(0)

#define VALIDATE_LINE(validation, message_err) do                                            \
{                                                                                            \
    if (validation)                                                                          \
    {                                                                                        \
        printf("Error: %s at %s:%u\n", message_err, program_file_name, pc);                  \
        flag_are_errors_in_program = ERR_ARE_ERRORS;                                                        \
        flag_err_at_curr_line = ERR_ARE_ERRORS;                                                      \
    }                                                                                        \
} while(0)

#define SAFE_REALLOC_POINTS_ARR() do \
{                                                                                                           \
    size_t old_capacity = points_arr->points_arr_capacity;                                                  \
    points_arr->points_arr_capacity = (old_capacity == 0) ? 20 : old_capacity * 2;                          \
    char** new_points_arr = (char**)calloc(points_arr->points_arr_capacity, sizeof(char*));                 \
    if (new_points_arr == NULL) return ERR_CALLOC_FAILED;                                                   \
    if (points_arr->points_arr != NULL) {                                                                   \
        memcpy(new_points_arr, points_arr->points_arr, old_capacity * sizeof(char*));                       \
        free(points_arr->points_arr);                                                                       \
    }                                                                                                       \
    points_arr->points_arr = new_points_arr;                                                                \
} while(0)

InputErrors PassFirst(char* program_file_name, PointsArr* points_arr);
InputErrors PassSecond(char* program_file_name, char* executable_file_name, PointsArr* points_arr);
InstructionsCodes GetInstructionCode(char* point);
int GetPointPlace(char* buffer, PointsArr* points_arr);
int GetRegIndex(char* reg_name);
Errors InitPointArr(PointsArr* points_arr);
Errors InitCodeframe(CodeFrame* codeframe);


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: compiler <program_file file> <executable file>\n");
        return 1;
    }
    char* program_file_name = argv[1];
    char* executable_file_name = argv[2];

    PointsArr points_arr = {};
    Errors init_err = InitPointArr(&points_arr);
    if (init_err != OK) {printf("Fatal error: no memory\n"); return 1;}

    InputErrors err = PassFirst(program_file_name, &points_arr);
    if (err != INPUT_OK) printf("Linking points error(s) occured\n");
    err = PassSecond(program_file_name, executable_file_name, &points_arr);
    if (err != INPUT_OK) printf("Compiling error(s) occured\n");
    printf("%d", err);
}

InputErrors PassFirst(char* program_file_name, PointsArr* points_arr)
{
    if (program_file_name == NULL) {printf("Error: program filename crashed\n"); return ERR_FILE_NOT_OPENED;}
    FILE* program_file = fopen(program_file_name, "r");
    if (program_file == NULL) {printf("Error: no such file <%s>\n", program_file_name); return ERR_FILE_NOT_OPENED;}

    char point_name[POINT_NAME_SIZE] = {};
    unsigned int pc = 0;
    InputErrors flag_are_errors_in_program = INPUT_OK;

    while (fgets(point_name, POINT_NAME_SIZE, program_file) != NULL)
    {
        while (pc >= points_arr->points_arr_capacity) SAFE_REALLOC_POINTS_ARR();

        char curr_point[POINT_NAME_SIZE] = {};

        if (point_name[0] == '\n' || point_name[0] == '\r' || point_name[0] == '\0') continue;
        else if (point_name[0] == ':')
        {
            int input_res = sscanf(point_name, ":%s", curr_point);
            VALIDATE_POINT(input_res != 1, "invalid point");
            (points_arr->points_arr)[pc] = strdup(curr_point);
            continue;
        }

        char temp_instr[MAX_INSTR_SIZE] = {};
        if (sscanf(point_name, "%s", temp_instr) == 1)
        {
            InstructionsCodes temp_instr_code = GetInstructionCode(temp_instr);
            if (0 != temp_instr_code && instr_info_list[temp_instr_code].instr_arg_type != NO_ARG) pc += 2;
            else pc++;
        }
    }
    fclose(program_file);
    return flag_are_errors_in_program;
}

InputErrors PassSecond(char* program_file_name, char* executable_file_name, PointsArr* points_arr)
{
    if (program_file_name == NULL) {printf("Error: program filename crashed\n"); return ERR_FILE_NOT_OPENED;}
    FILE* program_file = fopen(program_file_name, "r");
    if (program_file == NULL) {printf("Error: no such file <%s>\n", program_file_name); return ERR_FILE_NOT_OPENED;}

    if (executable_file_name == NULL) {printf("Error: executable file filename crashed\n"); return ERR_FILE_NOT_OPENED;}
    FILE* executable_file = fopen(executable_file_name, "wb");
    if (executable_file == NULL) {printf("Error: no such file <%s>\n", executable_file_name); return ERR_FILE_NOT_OPENED;}

    CodeFrame codeframe = {};
    Errors init_codeframe_err = InitCodeframe(&codeframe);
    if (init_codeframe_err != OK) {fclose(program_file); fclose(executable_file); return ERR_CALLOC_FAILED;}

    char buffer[BUFFER_SIZE] = {};
    unsigned int pc = 0;
    InputErrors flag_are_errors_in_program = INPUT_OK;

    while (fgets(buffer, sizeof(buffer), program_file) != NULL)
    {
        InputErrors flag_err_at_curr_line = INPUT_OK;

        cell curr_instr = {};
        cell curr_arg = {};
        char temp_instr[MAX_INSTR_SIZE] = {};
        char temp_arg[MAX_ARG_SIZE] = {};

        if (buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == '\0' || buffer[0] == ':') continue;

        int input_instr_res = sscanf(buffer, "%s", temp_instr);
        VALIDATE_LINE(input_instr_res != 1, "invalid line");
        if (flag_err_at_curr_line != INPUT_OK) continue;

        curr_instr.instr = GetInstructionCode(temp_instr);
        VALIDATE_LINE(curr_instr.instr == 0, "unknown instruction");
        if (flag_err_at_curr_line != INPUT_OK) continue;

        if (instr_info_list[curr_instr.instr].instr_arg_type == NUM)
        {
            int input_res = sscanf(buffer, "%s %f", temp_instr, &curr_arg.num);
            VALIDATE_LINE(input_res != 2, "expected 1 float arg");
        }
        else if (instr_info_list[curr_instr.instr].instr_arg_type == POINT)
        {
            int input_res = sscanf(buffer, "%s %s", temp_instr, temp_arg);
            VALIDATE_LINE(input_res != 2, "expected 1 point name arg");
            curr_arg.point = GetPointPlace(temp_arg, points_arr);
            VALIDATE_LINE(curr_arg.point < 0, "missed point");
        }
        else if (instr_info_list[curr_instr.instr].instr_arg_type == REG)
        {
            int input_res = sscanf(buffer, "%s %s", temp_instr, temp_arg);
            VALIDATE_LINE(input_res != 2, "expected 1 register name arg");
            curr_arg.reg = GetRegIndex(temp_arg);
            VALIDATE_LINE(curr_arg.reg < 0, "invalid register name");
        }
        else if (instr_info_list[curr_instr.instr].instr_arg_type == INDEX)
        {
            int input_res = sscanf(buffer, "%s [%d]", temp_instr, &curr_arg.index);
            VALIDATE_LINE(input_res != 2, "expected 1 index arg");
            VALIDATE_LINE(curr_arg.index < 0 || curr_arg.index >= ram_size, "invalid index");
        }


        if (flag_err_at_curr_line == INPUT_OK)
        {
            codeframe.code_frame[pc] = curr_instr;
            if (instr_info_list[curr_instr.instr].instr_arg_type != NO_ARG)
            {
                codeframe.code_frame[pc + 1] = curr_arg;
                pc += 2;
            }
            else
                pc += 1;
        }

    }

    if (flag_are_errors_in_program == INPUT_OK)
    {
        size_t num_of_w_cells = fwrite(codeframe.code_frame, sizeof(cell), pc, executable_file);
        if (num_of_w_cells != (size_t)pc) flag_are_errors_in_program = ERR_ARE_ERRORS;
    }

    free(codeframe.code_frame);
    codeframe.code_frame = NULL;

    fclose(program_file);
    fclose(executable_file);
    return flag_are_errors_in_program;
}

InstructionsCodes GetInstructionCode(char* buffer)
{
    for (unsigned int i = 0; i < num_of_instr; i++)
    {
        if (strcmp(instr_names_list[i], buffer) == 0)
            return (InstructionsCodes)i;
    }
    return (InstructionsCodes)0;
}

int GetPointPlace(char* point, PointsArr* points_arr)
{
    for (unsigned int i = 0; i < points_arr->points_arr_capacity; i++)
    {
        if (points_arr->points_arr[i] != NULL && strcmp(points_arr->points_arr[i], point) == 0)
            return i;
    }
    return -1;
}

Errors InitPointArr(PointsArr* points_arr)
{
    char** new_points_arr = (char**)(calloc(START_POINTS_ARR_SIZE, sizeof(char*)));
    if (new_points_arr == NULL) return ERR_CALLOC_ERROR;

    points_arr->points_arr = new_points_arr;
    points_arr->points_arr_capacity = START_POINTS_ARR_SIZE;
    return OK;
}

Errors InitCodeframe(CodeFrame* codeframe)
{
    codeframe->code_frame = (cell*)calloc(START_CODEFRAME_SIZE, sizeof(cell));
    if (codeframe->code_frame == NULL) return ERR_CALLOC_ERROR;

    codeframe->code_frame_size = START_CODEFRAME_SIZE;
    return OK;
}

int GetRegIndex(char* reg_name)
{
    for (int i = 0; i < NUM_OF_REGS; i++)
    {
        if (strcmp(reg_names[i], reg_name) == 0)
            return i;
    }
    return -1;
}
