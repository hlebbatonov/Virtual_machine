#include "common.h"

InputErrors CompileFile(char* assm_file_name, char* program_file_name);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Error: missing required files. Disassembler expects <assembler file> and <program file>.\n");
        return 1;
    }

    char* assm_file_name = argv[1];
    char* program_file_name = argv[2];
    InputErrors err = CompileFile(assm_file_name, program_file_name);
    printf("%d", err);
}

InputErrors CompileFile(char* assm_file_name, char* program_file_name)
{
    FILE* assm = fopen(assm_file_name, "r");
    if (assm == NULL) {printf("Error: no such file <%s>\n", assm_file_name); return ERR_FILE_NOT_OPENED;}

    FILE* program = fopen(program_file_name, "w");
    if (program == NULL) {printf("Error: couldn't create <%s>\n", program_file_name); fclose(assm); return ERR_FILE_NOT_OPENED;}

    char buffer[BUFFER_SIZE] = {};
    unsigned int cp = 0;

    while (fgets(buffer, sizeof(buffer), assm) != NULL)
    {
        cp++;

        if (buffer[0] == '\n' || buffer[0] == '\r' || buffer[0] == '\0') continue;

        int curr_instruction = -1;
        int curr_num = -1;

        int input_res = sscanf(buffer, "%d %d", &curr_instruction, &curr_num);

        if (input_res <= 0) \
            {printf("Error: incorrect line at %s:%u <%s>\n", assm_file_name, cp, program_file_name); \
            fclose(assm); fclose(program);return ERR_INSTRUCT_DID_NOT_READ;}
        else if ((curr_instruction < PUSH || curr_instruction > HLT)) \
            {printf("Error: unknown instruction at %s:%u <%s>\n", assm_file_name, cp, program_file_name); \
            fclose(assm); fclose(program); return ERR_INSTRUCT_DID_NOT_READ;}
        else if (input_res != 2 && curr_instruction == PUSH) \
            {printf("Error: incorrect PUSH arg at %s:%u <%s>\n", assm_file_name, cp, program_file_name); \
            fclose(assm); fclose(program); return ERR_NUM_DID_NOT_READ;}
        else if (input_res == 2 && curr_instruction != PUSH) \
            {printf("Error: too much args at %s:%u <%s>\n", assm_file_name, cp, program_file_name); \
            fclose(assm); fclose(program); return ERR_INSTRUCT_DID_NOT_READ;}

        char curr_instruction_name[INSTR_NAME_SIZE] = {};
        strcpy(curr_instruction_name, instructions_list[curr_instruction]);

        if (curr_instruction == PUSH) fprintf(program, "%s %d\n", curr_instruction_name, curr_num);
        else fprintf(program, "%s\n", curr_instruction_name);

    }
    fclose(assm);
    fclose(program);
    return INPUT_OK;

}
