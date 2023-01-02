/**
 *\file
 */

#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <assert.h>
//#include <windows.h>

#define CANARY_PROT 1 /// state value for turning on canary protection of stack and stack data
#define HASH_PROT   2 /// state value for turning on hash protection of stack and stack data

#ifndef PROT_LEVEL
#define PROT_LEVEL CANARY_PROT
#endif

#if (PROT_LEVEL & HASH_PROT)
#include "..\hash\hash.h"
#endif

typedef unsigned long long canary_t; /// sets canary type
typedef void* elem_t;                  /// sets type of data elements


static int ERRNO = 0;                              /// sets a "non-error" value
static const int CANARIES_NUMBER = 2;              /// sets a number of "canaries"
static const void* POISON = (const void *)2;
//static const size_t POISON = 0xDEADBEEF;           /// sets "poison" value (a value to indicate errors in stack data values)
static const canary_t CANARY = 0xAB8EACAAAB8EACAA; /// sets value of "canary" (a value to indicate safety of stack and stack data)
static const int START_CAPACITY = 10;

enum errors
{
    STACK_FOPEN_FAILED   = 0x1 << 0,
    STACK_ALLOC_FAIL     = 0x1 << 1,
    STACK_BAD_READ_STK   = 0x1 << 2,
    STACK_BAD_READ_DATA  = 0x1 << 3,
    STACK_STACK_OVERFLOW = 0x1 << 4,
    STACK_INCORRECT_SIZE = 0x1 << 5,
    STACK_VIOLATED_DATA  = 0x1 << 6,
    STACK_VIOLATED_STACK = 0x1 << 7,
    STACK_DATA_MESSED_UP = 0x1 << 8
};

struct Debug_info
{
    const char *func      = nullptr; /// name of called function
    const char *file      = nullptr; /// name of file where the called function is
    const char *stk_name  = nullptr; /// name of stack
    const char *call_func = nullptr; /// name of calling function
    const char *call_file = nullptr; /// name of file where the calling function is
    int creat_line = 0;              /// line where stack is created (initialised)
    int call_line = 0;               /// line where calling function is situated
    int line = 0;                    /// line where called function starts
};

/// struct with info about stack
struct Stack
{
    #if (PROT_LEVEL & CANARY_PROT)
    canary_t left_canary = CANARY; /// "canary" to avoid foreign data contamination of stack
    #endif

    struct Debug_info info = {};

    elem_t *data = nullptr;

    int size = 0;                  /// number of initialised elements in data
    int capacity = 0;

    #if (PROT_LEVEL & HASH_PROT)
    hash_t hash_sum = 0;
    #endif

    #if (PROT_LEVEL & CANARY_PROT)
    canary_t right_canary = CANARY; /// "canary" to avoid foreign data contamination of stack
    #endif
};


/**
 *creates stack data
 * \param [out
 ] stk      pointer to struct Stack
 * \param [in] capacity start capacity for data
 * \param [in] err      show if situation error or not error
 * \return              null if success, else error code
 */
int   stack_init (Stack *stk, int capacity, int *err = &ERRNO);

/**
 *push value in stack data
 * \param [out] stk     pointer to struct Stack
 * \param [in]  value   value to push
 * \param [in]  err     show if situation error or not error
 * \return              null if success, else error code
 */
int    stack_push (Stack *stk, elem_t value, int *err = &ERRNO);

/**
 *pop latest element of data
 * \param [out] stk     pointer to struct Stack
 * \param [in]  err     show if situation error or not error
 * \return              latest element of data
 */
elem_t stack_pop  (Stack *stk,               int *err = &ERRNO);

int   __debug_stack_init (Stack *stk, int capacity, const char *stk_name, const char *call_func, const int call_line,
                                                     const char *call_file, const int creat_line, int *err = &ERRNO);
int    __debug_stack_push (Stack *stk, elem_t value, const int call_line, int *err = &ERRNO);
elem_t __debug_stack_pop  (Stack *stk,               const int call_line, int *err = &ERRNO);

static int   stack_realloc (Stack *stk, int previous_capacity, int *err = &ERRNO);
static void  fill_stack    (Stack *stk, int start, int *err);
static int   stack_error   (Stack *stk, int *err, int need_in_dump = 1);
static void  stack_dump    (Stack *stk, int *err, FILE *file);// = log_file);
static void  stack_resize  (Stack *stk, int *err);
void         stack_dtor    (Stack *stk);

static void   log_status       (Stack *stk, int *err, FILE *file );//= log_file);
static void   log_info         (Stack *stk, int *err, FILE *file );//= log_file);
static void   log_data         (Stack *stk, FILE *file);// = log_file);
static void   log_data_members (Stack *stk, FILE *file);// = log_file);
/*int is_bad_read_ptr (void *p);


int is_bad_read_ptr (void *p)
{
    MEMORY_BASIC_INFORMATION mbi = {};

    const int PROTECT_MASK = PAGE_EXECUTE | PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    if (!(VirtualQuery (p, &mbi, sizeof (mbi))))
    {
        return 1;
    }
    else if (!(mbi.Protect & PROTECT_MASK))
    {
        return 1;
    }

    return 0;
}*/

#ifdef STACK_DEBUG

#define stack_init(stk, capacity)     __debug_stack_init (stk, capacity, #stk, __PRETTY_FUNCTION__, __LINE__, __FILE__, __LINE__)
#define stack_push(stk, value)        __debug_stack_push (stk, value, __LINE__)
#define stack_pop(stk)               __debug_stack_pop  (stk, __LINE__)

#endif

#endif /* STACK_H */
