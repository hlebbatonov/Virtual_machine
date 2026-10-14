#include "common.h"

const unsigned int num_of_instr = sizeof(instructions_list) / sizeof(instructions_list[0]);

InputErrors CompileFile(char* program_file_name, char* asm_file_name);
Instructions GetInstructionCode(char* buffer);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Error: missing required files. Compiler expects <program file> and <assembler file>.\n");
        return 1;
    }
    char* program_file_name = argv[1];
    char* asm_file_name = argv[2];

    InputErrors err = CompileFile(program_file_name, asm_file_name);
    printf("%d", err);
}

InputErrors CompileFile(char* program_file_name, char* asm_file_name)
{
    FILE* program = fopen(program_file_name, "r");
    if (program == NULL) {printf("Error: no such file <%s>\n", program_file_name); return ERR_FILE_NOT_OPENED;}

    FILE* asm_program = fopen(asm_file_name, "w");
    if (asm_program == NULL) {printf("Error: couldn't create <%s>\n", asm_file_name); fclose(program); return ERR_FILE_NOT_OPENED;}

    char buffer[BUFFER_SIZE] = {};
    unsigned int cp = 0;

    InputErrors flag_errors = INPUT_OK;

    while (fgets(buffer, sizeof(buffer), program) != NULL)
    {
        cp++;

        if (buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == '\0') continue;

        InputErrors flag_curr_error = INPUT_OK;
        char curr_instruction[MAX_INSTR_SIZE] = {};
        int curr_num = -1;

        int input_res = sscanf(buffer, "%s %d", curr_instruction, &curr_num);

        Instructions curr_instruction_code = GetInstructionCode(curr_instruction);

        if (input_res <= 0) \
            {printf("Error: incorrect line at %s:%u <%s>", program_file_name, cp, asm_file_name); \
            flag_errors = ERR_ARE_ERRORS; flag_curr_error = ERR_ARE_ERRORS;}
        else if ((curr_instruction_code < PUSH || curr_instruction_code > HLT)) \
            {printf("Error: unknown instruction at %s:%u <%s>\n", program_file_name, cp, asm_file_name); \
            flag_errors = ERR_ARE_ERRORS; flag_curr_error = ERR_ARE_ERRORS;}
        else if (input_res != 2 && curr_instruction_code == PUSH) \
            {printf("Error: incorrect PUSH arg at %s:%u <%s>\n", program_file_name, cp, asm_file_name); \
            flag_errors = ERR_ARE_ERRORS; flag_curr_error = ERR_ARE_ERRORS;}
        else if (input_res == 2 && curr_instruction_code != PUSH) \
            {printf("Error: too much args at %s:%u <%s>\n", program_file_name, cp, asm_file_name); \
             flag_errors = ERR_ARE_ERRORS; flag_curr_error = ERR_ARE_ERRORS;}

        if (flag_curr_error == INPUT_OK)
        {
            if (curr_instruction_code == PUSH) fprintf(asm_program, "%d %d\n", curr_instruction_code, curr_num);
            else fprintf(asm_program, "%d\n", curr_instruction_code);
        }
    }

    fclose(program);
    fclose(asm_program);
    return flag_errors;

}

Instructions GetInstructionCode(char* buffer)
{
    for (unsigned int i = 0; i < num_of_instr; i++)
    {
        if (strcmp(instructions_list[i], buffer) == 0)
            return (Instructions)i;
    }
    return (Instructions)0;
}
