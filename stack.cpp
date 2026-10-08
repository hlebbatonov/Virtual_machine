#include "stack.h"

Errors StackPush(stack_t* stack, stack_elem elem)
{
    Errors err = OK;

    VERIFY_AND_RETURN(stack);

    if (stack->size >= stack->capacity)
    {
        stack->capacity *= 2;
        err = ResizeData(stack);
        if (err != OK) {stack->capacity /= 2; return err;}
    }
    stack->data[stack->size++ + ELEMS_FOR_CANARY] = elem;

    stack->data_hash = 0;
    stack->data_hash = CalcHashDJB2(stack->data, (stack->capacity + NUM_OF_CANARIES) * sizeof(stack_elem));

    stack->stack_hash = 0;
    stack->stack_hash = CalcHashDJB2(stack, sizeof(*stack));

    VERIFY_AND_RETURN(stack);
    return OK;
}

Errors StackPop(stack_t* stack, stack_elem* elem)
{
    VERIFY_AND_RETURN(stack);
    if (stack->size == 0) return ERR_ZERO_LENGTH;

    *elem = stack->data[--stack->size + ELEMS_FOR_CANARY];
    stack->data[stack->size + ELEMS_FOR_CANARY] = POISON;

    if (stack->capacity > MIN_CAPACITY_TO_RESIZE && stack->size * 4 <= stack->capacity)
    {
        stack->capacity /= 2;

        Errors err = ResizeData(stack);
        if (err != OK) {stack->capacity *= 2; return err;}
    }

    stack->data_hash = 0;
    stack->data_hash = CalcHashDJB2(stack->data, (stack->capacity + NUM_OF_CANARIES) * sizeof(stack_elem));

    stack->stack_hash = 0;
    stack->stack_hash = CalcHashDJB2(stack, sizeof(*stack));

    VERIFY_AND_RETURN(stack);
    return OK;
}

Errors StackCopy(stack_t* stack_dst, stack_t* stack_src)
{
    VERIFY_AND_RETURN(stack_src);
    if(stack_dst == NULL) return ERR_STACK_NULL_POINTER;
    if(stack_dst->data != NULL) return ERR_STACK_EXISTS;

    stack_dst->capacity = stack_src->capacity;
    stack_dst->size = stack_src->size;

    stack_dst->stack_canary_left  = STACK_CANARY_LEFT;
    stack_dst->stack_canary_right = STACK_CANARY_RIGHT;

    stack_dst->data = (stack_elem*)calloc(stack_dst->capacity + NUM_OF_CANARIES, sizeof(stack_elem));
    if (stack_dst->data == NULL) return ERR_NULL_POINTER;

    memcpy(stack_dst->data, stack_src->data, (stack_src->capacity + NUM_OF_CANARIES) * sizeof(stack_elem));


    stack_dst->data_hash = 0;
    stack_dst->data_hash = CalcHashDJB2(stack_dst->data, (stack_dst->capacity + NUM_OF_CANARIES) * sizeof(stack_elem));

    stack_dst->stack_hash = 0;
    stack_dst->stack_hash = CalcHashDJB2(stack_dst, sizeof(*stack_dst));

    VERIFY_AND_RETURN(stack_dst);
    VERIFY_AND_RETURN(stack_src);
    return OK;
}

Errors StackInit(stack_t* stack, size_t capacity)
{
    if (stack == NULL) return ERR_STACK_NULL_POINTER;
    if (stack->data != NULL || stack->capacity != 0 || stack->size != 0) return ERR_STACK_EXISTS;
    if (capacity == 0) return ERR_ZERO_LENGTH;

    stack->capacity = capacity;
    stack->size = 0;

    stack->stack_canary_left = STACK_CANARY_LEFT;
    stack->stack_canary_right = STACK_CANARY_RIGHT;

    stack->data = (stack_elem*)calloc(capacity + NUM_OF_CANARIES, sizeof(stack_elem));
    if (stack->data == NULL) return ERR_CALLOC_ERROR;

    for (size_t i = 0; i < stack->capacity + NUM_OF_CANARIES; i++) stack->data[i] = POISON;

    stack->data[0] = CANARY_LEFT;
    stack->data[capacity + ELEMS_FOR_CANARY] = CANARY_RIGHT;

    stack->data_hash = CalcHashDJB2(stack->data, (stack->capacity + NUM_OF_CANARIES) * sizeof(stack_elem));

    stack->stack_hash = 0;
    stack->stack_hash = CalcHashDJB2(stack, sizeof(*stack));

    VERIFY_AND_RETURN(stack);
    return OK;
}

Errors StackFree(stack_t* stack)
{
    VERIFY_AND_RETURN(stack);

    for (size_t i = 0; i < stack->capacity + NUM_OF_CANARIES; i++) {stack->data[i] = POISON;}

    free(stack->data);
    stack->data = NULL;
    stack->capacity = POISON_FOR_FREE_MEM;
    stack->size = POISON_FOR_FREE_MEM;
    stack->stack_canary_left = POISON_FOR_FREE_MEM;
    stack->stack_canary_right = POISON_FOR_FREE_MEM;
    stack->stack_hash = POISON_FOR_FREE_MEM;
    stack->data_hash = POISON_FOR_FREE_MEM;

    return OK;
}

Errors StackTop(stack_t *stack, stack_elem* elem)
{
    VERIFY_AND_RETURN(stack);
    if (stack->size == 0) return ERR_ZERO_LENGTH;
    else *elem = stack->data[stack->size - 1 + ELEMS_FOR_CANARY];

    VERIFY_AND_RETURN(stack);
    return OK;
}

const char* GetErrorMessage(int error)
{
    switch(error)
    {
        case OK:                     return "OK";
        case ERR_NULL_POINTER:       return "ERR_NULL_POINTER";
        case ERR_ZERO_LENGTH:        return "ERR_ZERO_LENGTH";
        case ERR_LENGTH_TOO_BIG:     return "ERR_LENGTH_TOO_BIG";
        case ERR_CALLOC_ERROR:       return "ERR_CALLOC_ERROR";
        case ERR_STACK_EXISTS:       return "ERR_STACK_EXISTS";
        case ERR_STACK_NULL_POINTER: return "ERR_STACK_NULL_POINTER";
        case ERR_DEAD_LEFT_CANARY:   return "ERR_DEAD_LEFT_CANARY";
        case ERR_DEAD_RIGHT_CANARY:  return "ERR_DEAD_RIGHT_CANARY";
        case ERR_STACK_OVERFLOWED:   return "ERR_STACK_OVERFLOWED";
        case ERR_FREE_PART_CRASHED:  return "ERR_FREE_PART_CRASHED";
        case ERR_DATA_HASH_CHANGED:  return "ERR_DATA_HASH_CHANGED";
        case ERR_STACK_HASH_CHANGED: return "ERR_STACK_HASH_CHANGED";
        default:                     return "ERROR IS NOT IN ENUM";
    }
}

Errors ResizeData(stack_t* stack)
{
    if (stack == NULL) return ERR_STACK_NULL_POINTER;
    if (stack->data == NULL) return ERR_NULL_POINTER;
    if (stack->stack_canary_left  != STACK_CANARY_LEFT)  return ERR_DEAD_LEFT_CANARY;
    if (stack->stack_canary_right != STACK_CANARY_RIGHT) return ERR_DEAD_RIGHT_CANARY;
    if (stack->size > stack->capacity) return ERR_STACK_OVERFLOWED;

    stack_elem* new_data = (stack_elem*)realloc(stack->data, (stack->capacity + NUM_OF_CANARIES) * sizeof(stack_elem));
    if (new_data == NULL) return ERR_CALLOC_ERROR;

    stack->data = new_data;

    for (size_t i = stack->size + ELEMS_FOR_CANARY; i < stack->capacity + ELEMS_FOR_CANARY; i++) stack->data[i] = POISON;

    stack->data[stack->capacity + ELEMS_FOR_CANARY] = CANARY_RIGHT;

    return OK;
}

Errors StackVerify(stack_t *stack)
{
    //Verify pointers
    if (stack == NULL) return ERR_STACK_NULL_POINTER;
    if (stack->data == NULL) return ERR_NULL_POINTER;

    //Verify stack canaries
    if (stack->stack_canary_left != STACK_CANARY_LEFT) return ERR_DEAD_LEFT_CANARY;
    if (stack->stack_canary_right != STACK_CANARY_RIGHT) return ERR_DEAD_RIGHT_CANARY;

    //Verify hashes
    uint64_t curr_stack_hash = stack->stack_hash;
    stack->stack_hash = 0;
    uint64_t calc_stack_hash = CalcHashDJB2(stack, sizeof(*stack));
    stack->stack_hash = curr_stack_hash;
    if (calc_stack_hash != curr_stack_hash) return ERR_STACK_HASH_CHANGED;

    uint64_t calc_data_hash = CalcHashDJB2(stack->data, (stack->capacity + NUM_OF_CANARIES) * sizeof(stack_elem));
    if (calc_data_hash != stack->data_hash) return ERR_DATA_HASH_CHANGED;

    //Verify data canaries
    if (stack->data[0] != CANARY_LEFT) return ERR_DEAD_LEFT_CANARY;
    if (stack->data[stack->capacity + ELEMS_FOR_CANARY] != CANARY_RIGHT) return ERR_DEAD_RIGHT_CANARY;

    //Verify sizes
    if (stack->size > stack->capacity) return ERR_STACK_OVERFLOWED;
    if (stack->capacity <= 0) return ERR_ZERO_LENGTH;
    if (stack->capacity > MAX_SIZE) return ERR_LENGTH_TOO_BIG;

    //Verify free memory
    for (size_t i = stack->size + ELEMS_FOR_CANARY; i < stack->capacity + ELEMS_FOR_CANARY; i++)
        if (stack->data[i] != POISON) return ERR_FREE_PART_CRASHED;

    return OK;
}

void StackDumpFunc(stack_t* stack, var_info info)
{
    printf("---------PRINTING STACK---------\n");
    if (stack == NULL) printf("Stack_t \"%s\" CALLED at %s:%u\n HAS NULL POINTER!!!", info.var_name, info.file, info.line);
    else printf("Printing stack_t \"%s\"[%p] CALLED at %s:%u\n", info.var_name, stack, info.file, info.line);
    printf("%s = %zu, %s = %zu\n", NAME(stack->size), stack->size, NAME(stack->capacity), stack->capacity);
    printf("\"%s\"[%p]\n{\n", NAME(stack->data), stack->data);
    if (stack->data != NULL/*  */)
    {
        for (unsigned int i = 0; i < stack->capacity + NUM_OF_CANARIES; i++)
        {
            printf("\t[%u] = <", i);
            if (stack->data[i] == CANARY_LEFT) printf("CANARY_LEFT");
            else if (stack->data[i] == CANARY_RIGHT) printf("CANARY_RIGHT");
            else if (stack->data[i] == POISON) printf("POISON");
            else printf(STACK_ELEM_SPEC, stack->data[i]);
            printf(">\n");
        }
        printf("}\n");
    }
    else printf("%s IS NULL!!!\n", NAME(stack->data));
    printf("---------\n");
}

uint64_t CalcHashDJB2(void* data, size_t size)
{
    //https://metanit.com/c/hashtable/1.2.php

    uint64_t hash = 5381;
    uint8_t* ptr = (uint8_t*)data;

    for (size_t i = 0; i < size; i++)
        hash = (hash << 5) + hash + ptr[i];

    return hash;
}
