#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

union cell
{
    int instr;
    int point;
    int reg;
    unsigned int index;
    float num;
};

typedef cell stack_elem;

#define ACCURACY 1e-5

#define CANARY_LEFT  (float)0xDEAD
#define CANARY_RIGHT (float)0xBAAD

#define POISON (float)0xDABB
#define POISON_FOR_FREE_MEM (float)0xBEDA

#define ELEMS_FOR_CANARY 1
#define NUM_OF_CANARIES 2

#define STACK_CANARY_LEFT 0xC00F
#define STACK_CANARY_RIGHT 0xBA0B

#define MIN_CAPACITY_TO_RESIZE 10
#define MAX_SIZE 1000000


struct stack_t
{
    uint32_t stack_canary_left;
    stack_elem* data;
    size_t size;
    size_t capacity;
    uint64_t stack_hash;
    uint64_t data_hash;
    uint32_t stack_canary_right;
};

enum Errors
{
    OK = 0, //info
    ERR_NULL_POINTER, //error
    ERR_STACK_NULL_POINTER, //error
    ERR_ZERO_LENGTH, //error
    ERR_LENGTH_TOO_BIG, //error
    ERR_CALLOC_ERROR, //fatal
    ERR_STACK_EXISTS, //error
    ERR_DEAD_LEFT_CANARY, //fatal
    ERR_DEAD_RIGHT_CANARY, //fatal
    ERR_STACK_OVERFLOWED, //fatal
    ERR_FREE_PART_CRASHED, //fatal
    ERR_DATA_HASH_CHANGED, //fatal
    ERR_STACK_HASH_CHANGED, //fatal
    ERR_DIV_ZERO,
    ERR_POP_FROM_EMPTY_STACK,
    ERR_INVALID_INDEX,
    ERR_UNKNOWN_INSTRUCTION
};

typedef struct
{
    const char* var_name;
    const char* file;
    const unsigned int line;
} var_info;

Errors StackPush(stack_t* stack, stack_elem elem);
Errors StackPop(stack_t* stack, stack_elem* elem);
Errors StackInit(stack_t* stack, size_t capacity);
Errors StackFree(stack_t* stack);
void StackDumpFunc(stack_t* stack, var_info info);
const char* GetErrorMessage(int error);
Errors ResizeData(stack_t* stack);
Errors StackVerify(stack_t* stack);
Errors StackTop(stack_t* stack, stack_elem* elem);
Errors StackCopy(stack_t* stack_dst, stack_t* stack_src);
uint64_t CalcHashDJB2(void* data, size_t size);
bool IsEqual(float a, float b);

#define NAME(x) #x
#define StackDump(stack) StackDumpFunc(stack, (var_info){NAME(stack), __FILE__, __LINE__})

#define ASSERT_RETURN_VALUE(err) do {if (err != OK) \
{                                                   \
    printf("ERROR at %s:%d ", __FILE__, __LINE__);  \
    printf("%s", GetErrorMessage(err));             \
    printf("\n");                                   \
    abort();                                        \
}} while(0)



#ifdef NDEBUG
#define VERIFY_AND_RETURN(stack_to_ver)
#else
#define VERIFY_AND_RETURN(stack_to_ver) do {Errors error_ver = StackVerify(stack_to_ver);\
    if (error_ver != OK) return error_ver;                                               \
    } while(0)
#endif

