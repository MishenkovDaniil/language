#include <assert.h>
#include <stdio.h>

#include "stack.h"

static FILE *log_file = fopen ("log.txt", "w"); // output file

static int stack_realloc (Stack *stk, int previous_capacity, int *err)
{
    assert (stk);
    assert (err);

    /*if (is_bad_read_ptr(stk) || err == nullptr || previous_capacity < 0)
    {
        printf ("ERROR: stack pointer or error pointer is a nullptr or previous capacity at func stack_realloc is under zero\n");
    }*/

    if (previous_capacity)
    {
        size_t mem_size = 0;

        #if (PROT_LEVEL & CANARY_PROT)
        stk->data = (elem_t *)((char *)stk->data - sizeof (canary_t));

        mem_size = stk->capacity * sizeof (elem_t) + CANARIES_NUMBER * sizeof (canary_t);
        #else
        mem_size = stk->capacity * sizeof (elem_t);
        #endif

        stk->data = (elem_t *)realloc (stk->data, mem_size);

        #if (PROT_LEVEL & CANARY_PROT)
        stk->data = (elem_t *)((char *)stk->data + sizeof (canary_t));

        *((canary_t *)(stk->data + previous_capacity)) = 0xDEADBEEF;//POISON;
        *((canary_t *)(stk->data + stk->capacity)) = CANARY;
        #endif
    }
    else
    {
        #if (PROT_LEVEL & CANARY_PROT)

        stk->data = (elem_t *)calloc (stk->capacity * sizeof (elem_t) + CANARIES_NUMBER * sizeof (canary_t), 1);

        *((canary_t *)(stk->data)) = CANARY;

        stk->data = (elem_t *)(((char *)(stk->data)) + sizeof (canary_t));

        *((canary_t *)(stk->data + stk->capacity)) = CANARY;

        #else

        stk->data = (elem_t *)calloc (stk->capacity, sizeof (elem_t));

        #endif
    }

    if (stk->data == nullptr)
    {
        *err |= STACK_ALLOC_FAIL;

        #ifdef STACK_DEBUG
        stack_dump(stk, err, stderr);
        #endif

        return *err;
    }

    return 0;
}

void fill_stack (Stack *stk, int start, int *err)
{
    assert (stk && stk->data);

    if (!(stk && stk->data && err))
    {
        printf ("ERROR: stack, stack data or error pointer is a null pointer\n");
    }

    for (int i = start; i < stk->capacity; i++)
    {
        (stk->data)[i] = (elem_t)POISON;
    }
}

int stack_push (Stack *stk, elem_t value, int *err)
{
    assert (stk && stk->data);
    assert (err);

    if (err == nullptr)
    {
        err = &ERRNO;
    }

    stack_error (stk, err);

    if (*err)
    {
        return *err;
    }

    stack_resize (stk, err);

    (stk->data)[stk->size++] = value;

    #if (PROT_LEVEL & HASH_PROT)

    stk->hash_sum = m_gnu_hash (stk->data, stk->capacity * sizeof(elem_t));

    #endif

    stack_error (stk, err);

    return 0;
}

int __debug_stack_push (Stack *stk, elem_t value, const int call_line, int *err)
{
    if (stack_error (stk, err, 0))
    {
        #ifdef STACK_DEBUG
        stack_dump (stk, err);
        #endif

        return 0;//POISON;
    }
    (stk->info).file = __FILE__;
    (stk->info).func = __PRETTY_FUNCTION__;
    (stk->info).line = __LINE__;
    (stk->info).call_line = call_line;

    return stack_push (stk, value, err);
}

elem_t stack_pop (Stack *stk, int *err)
{
    assert (stk && stk->data);
    assert (err);

    if (err == nullptr)
    {
        err = &ERRNO;
    }

    stack_error (stk, err);

    if (*err)
    {
        return nullptr;//(elem_t)*err;
    }

    stack_resize (stk, err);

    (stk->size)--;

    if (stk->size < 0)
    {
        stack_error (stk, err);

        return (elem_t)POISON;
    }

    elem_t latest_value = (stk->data)[stk->size];

    (stk->data)[stk->size] = (elem_t)POISON;

    #if (PROT_LEVEL & HASH_PROT)

    stk->hash_sum = m_gnu_hash (stk->data, stk->capacity * sizeof (elem_t));

    #endif

    stack_error(stk, err);

    return latest_value;
}

elem_t __debug_stack_pop (Stack *stk, const int call_line, int *err)
{
    if (stack_error (stk, err, 0))
    {
        #ifdef STACK_DEBUG
        stack_dump (stk, err);
        #endif

        return 0;//POISON;
    }
    (stk->info).file = __FILE__;
    (stk->info).func = __PRETTY_FUNCTION__;
    (stk->info).line = __LINE__;
    (stk->info).call_line = call_line;

    return stack_pop (stk, err);
}

int stack_init (Stack *stk, int capacity, int *err)
{
    assert (stk);
    assert (err);

    if (err == nullptr)
    {
        err = &ERRNO;
    }

    /*if (is_bad_read_ptr (stk) || capacity <= 0)
    {
        stack_error (stk, err);
        return *err;
    }*/

    stk->capacity = capacity;

    if (!(stack_realloc (stk, 0, err)))
    {
        fill_stack (stk, 0, err);

        #if (PROT_LEVEL & HASH_PROT)

        stk->hash_sum = m_gnu_hash (stk->data, stk->capacity * sizeof (elem_t));

        #endif
    }

    stack_error (stk, err);

    return *err;
}

int __debug_stack_init (Stack *stk, int capacity, const char *stk_name, const char *call_func, const int call_line,
                         const char *call_file, const int creat_line, int *err)
{
    /*if (is_bad_read_ptr (stk))
    {
        stack_error (stk, err);
        return *err;
    }*/
    (stk->info).call_func = call_func;
    (stk->info).call_file = call_file;
    (stk->info).creat_line = creat_line;
    stk->info.call_line = call_line;

    (stk->info).file = __FILE__;
    (stk->info).func = __PRETTY_FUNCTION__;
    (stk->info).line = __LINE__;

    (*(stk_name) != '&') ? (stk->info).stk_name = stk_name : (stk->info).stk_name = stk_name + 1;

    return stack_init (stk, capacity, err);
}

static void stack_resize (Stack *stk, int *err)
{
    assert (stk && stk->data);

    if (stack_error (stk, err, 0)) //defin-îì
    {
        #ifdef STACK_DEBUG
        stack_dump (stk, err);
        #endif
    }

    int current_size = stk->size;
    int previous_capacity = stk->capacity;

    if (current_size)
    {
        if (current_size > (stk->capacity - 1))
        {
            stk->capacity *= 2;

            stack_realloc (stk, previous_capacity, err);
            fill_stack    (stk, previous_capacity, err);
        }
        else if (stk->capacity > current_size * 4 && previous_capacity > 10)
        {
            stk->capacity /= 2;

            stack_realloc (stk, previous_capacity, err);
        }
    }

    if (stack_error (stk, err, 0) != STACK_DATA_MESSED_UP)
    {
        #ifdef STACK_DEBUG
        stack_dump (stk, err);
        #endif
    }
    else
    {
        *err = 0;
    }
}

static int stack_error (Stack *stk, int *err, int need_in_dump)
{
    assert (stk);
    assert (err);

    do
    {
    if (!log_file)
    {
        *err |= STACK_FOPEN_FAILED;

        break;
    }
    /*if (is_bad_read_ptr (stk))
    {
        fprintf (stderr, "stk is a bad ptr\n");
        *err |= STACK_BAD_READ_STK;

        break;
    }
    if (is_bad_read_ptr (stk->data))
    {
        *err |= STACK_BAD_READ_DATA;

        break;
    }*/
    if (stk->size > stk->capacity)
    {
        *err |= STACK_STACK_OVERFLOW;
    }
    if (stk->size < 0 || stk->capacity <= 0)
    {
        *err |= STACK_INCORRECT_SIZE;
    }

    #if (PROT_LEVEL & CANARY_PROT)

    if ((*((canary_t *)((char*)stk->data - sizeof (canary_t))) != CANARY) ||
        (*((canary_t *)(stk->data + stk->capacity)) != CANARY))
    {
        *err |= STACK_VIOLATED_DATA;
    }
    if (stk->left_canary != CANARY || stk->right_canary != CANARY)
    {
        *err |= STACK_VIOLATED_STACK;
    }

    #endif

    #if (PROT_LEVEL & HASH_PROT)

    if (stk->hash_sum != m_gnu_hash (stk->data, stk->capacity * sizeof (elem_t)))
    {
        *err |= STACK_DATA_MESSED_UP;
    }

    #endif
    } while (0);

    #ifdef STACK_DEBUG
    if (need_in_dump)
    {
        stack_dump (stk, err);
    }
    #endif

    return *err;
}

static void stack_dump (Stack *stk, int *err, FILE *file)
{
    if (*err & STACK_BAD_READ_DATA)
    {
        fprintf (stderr, "data is a bad ptr\n");
    }

    assert (stk && stk->data);
    assert (err);
    assert (file);

    if (stk && stk->data && err && file)
    {
        log_info (stk, err, file);
        log_data (stk, file);

        fprintf (file, "\n\n");
    }
    else
        ;
}

void stack_dtor (Stack *stk)
{
    //assert (stk && stk->data);

    if (stk && stk->data)
    {
        stk->data = (elem_t *)((char *)stk->data - sizeof (canary_t));

        free (stk->data);

        stk->data = nullptr;
        stk = nullptr;
    }
    else
    {
        printf ("nothing to dtor");
    }
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

static void log_info (Stack *stk, int *err, FILE *file)
{
    assert (stk && stk->data);
    assert (err);
    assert (file);

    fprintf (file,
            "%s at %s(%d)\n"
            "stack [%p]", stk->info.func, stk->info.file, stk->info.line, stk);

    log_status (stk, err, file);
}
static void log_status (Stack *stk, int *err, FILE *file)
{
    assert (stk && stk->data);
    assert (err);
    assert (file);

    int err_number = 10;

    const char **status = (const char **)calloc (err_number, sizeof (const char *));

    int index = 0;

    if (*err)
    {
        fprintf (file, "(ERROR:");

        if (*err & STACK_FOPEN_FAILED)
        {
            status[index++] = "opening log file failed";
        }
        if (*err & STACK_ALLOC_FAIL)
        {
            status[index++] = "memory allocation failed";
        }
        if ((*err) & STACK_STACK_OVERFLOW)
        {
            status[index++] = "stack overflow";
        }
        if ((*err) & STACK_INCORRECT_SIZE)
        {
            status[index++] = "capacity or size of stack is under zero";
        }
        if ((*err) & STACK_VIOLATED_DATA)
        {
            status[index++] = "access rights of stack data are invaded";
        }
        if ((*err) & STACK_VIOLATED_STACK)
        {
            status[index++] = "access rights of stack are invaded";
        }
        if ((*err) & STACK_DATA_MESSED_UP)
        {
            status[index++] = "one or more values in stack data are unexpectidly changed";
        }
    }
    else
    {
        status[index++] = "(ok";
    }

    status[index] = nullptr;

    for (int i = 0; i < index; i++)
    {
        if (i < index - 1)
        {
            fprintf (file, "%s, ", status[i]);
        }
        else
        {
            fprintf (file, "%s", status[i]);
        }
    }
    fprintf (file, ")\n");
}

static void log_data (Stack *stk, FILE *file)
{
    assert (stk && stk->data);
    assert (file);

    fprintf (file,
            "%s at %s in %s(%d)(called at %d):\n"
            "data [%p]:\n",
            (stk->info).stk_name, (stk->info).call_func, (stk->info).call_file,
            (stk->info).creat_line, stk->info.call_line, stk->data);
    log_data_members (stk, log_file);
}

void log_data_members (Stack *stk, FILE *file)
{
    assert (stk && stk->data);
    assert (file);

    fprintf (file, "\tsize = %ld\n", stk->size);
    fprintf (file, "\tcapacity = %ld\n", stk->capacity);

    for (int i = 0; i < stk->size; i++)
    {
        fprintf (file, "\t*[%ld""] = %d\n", i, (stk->data)[i]);
    }
    for (int i = stk->size; i < stk->capacity; i++)
    {
        fprintf (file, "\t [%ld] = %d\n", i, (stk->data)[i]);
    }
}
