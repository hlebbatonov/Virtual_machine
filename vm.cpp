#include "vm_instructions.h"

InputErrors ReadFromFileToCodeFrame(CodeFrame* codeframe, char* executable_file_name);
Errors RunInstructions(CPU* cpu, CodeFrame* codeframe);
Errors InitCodeframe(CodeFrame* codeframe);
Errors InitRam(CPU* cpu);
Errors InitVideoRam(CPU* cpu);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: vm <executable file>\n");
        return 1;
    }
    char* executable_file_name = argv[1];

    CPU cpu1 = {};
    CodeFrame codeframe1 = {};
    InitWindow(screen_width * 30, screen_height * 30, "Virtual Machine");
    SetTargetFPS(60);

    Errors err_init = InitCodeframe(&codeframe1);
    if (err_init != OK) return err_init;

    cpu1.stack = (stack_t*)calloc(1, sizeof(stack_t));
    err_init = StackInit(cpu1.stack, START_STACK_SIZE);
    if (err_init != OK) return err_init;

    cpu1.ret_points = (stack_t*)calloc(1, sizeof(stack_t));
    err_init = StackInit(cpu1.ret_points, START_RET_POINTS_ARR);
    if (err_init != OK) {printf("Fatal error: no memory\n"); return 1;}

    err_init = InitRam(&cpu1);
    if (err_init != OK) {printf("Fatal error: no memory\n"); return 1;}

    cpu1.video_ram = (video_ram_t*)calloc(1, sizeof(video_ram_t));
    if (cpu1.video_ram == NULL) {printf("Fatal error: no memory\n"); return 1;}

    err_init = InitVideoRam(&cpu1);
    if (err_init != OK) {printf("Fatal error: no memory\n"); return 1;}


    InputErrors err = ReadFromFileToCodeFrame(&codeframe1, executable_file_name);
    Errors err_run = OK;
    if (err == INPUT_OK)
        err_run = RunInstructions(&cpu1, &codeframe1);
    printf("Execution code: %d\n", err_run);
    fflush(stdout);

    if (err_run != OK) return 1;
    else
    {
        SetTargetFPS(10);
        while (!WindowShouldClose())
        {
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTextureEx(cpu1.video_ram->texture, (Vector2){0, 0}, 0.0f, 30.0f, WHITE);
            EndDrawing();
        }
        UnloadTexture(cpu1.video_ram->texture);
        UnloadImage(cpu1.video_ram->canvas);
        free(cpu1.video_ram->video_ram);
        free(cpu1.video_ram);
        free(codeframe1.code_frame);
        return 0;
    }
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
        else
        {
            Errors err = instr_list[codeframe->code_frame[cpu->pc].instr](cpu, codeframe);
            if (err != OK) return err;
        }

    }
    return OK;
}

Errors InitCodeframe(CodeFrame* codeframe)
{
    if (codeframe == NULL) return ERR_NULL_POINTER;
    codeframe->code_frame = (cell*)calloc(MAX_EXE_FILE_SIZE, sizeof(cell));
    if (codeframe->code_frame == NULL) return ERR_CALLOC_ERROR;

    codeframe->code_frame_size = START_CODEFRAME_SIZE;
    return OK;
}

Errors InitRam(CPU* cpu)
{
    if (cpu == NULL) return ERR_NULL_POINTER;
    cpu->ram = (cell*)calloc(ram_size, sizeof(cell));
    if (cpu->ram == NULL) return ERR_CALLOC_ERROR;

    cpu->ram_size = ram_size;
    return OK;
}

Errors InitVideoRam(CPU* cpu)
{
    if (cpu == NULL) return ERR_NULL_POINTER;
    if (cpu->video_ram == NULL) return ERR_NULL_POINTER;
    cpu->video_ram->height = screen_height;
    cpu->video_ram->width = screen_width;
    cpu->video_ram->video_ram_size = cpu->video_ram->height * cpu->video_ram->width;

    cpu->video_ram->video_ram = (ColorCodes*)calloc(cpu->video_ram->video_ram_size, sizeof(ColorCodes));
    if (cpu->video_ram->video_ram == NULL) return ERR_NULL_POINTER;

    cpu->video_ram->canvas = GenImageColor(cpu->video_ram->width, cpu->video_ram->height, colors_list[COLOR_BLACK]);
    cpu->video_ram->pixels = (Color*)cpu->video_ram->canvas.data;

    if (cpu->video_ram->pixels == NULL)
    {
        free(cpu->video_ram->video_ram);
        return ERR_NULL_POINTER;
    }

    cpu->video_ram->texture = LoadTextureFromImage(cpu->video_ram->canvas);
    SetTextureFilter(cpu->video_ram->texture, TEXTURE_FILTER_POINT);

    return OK;
}





