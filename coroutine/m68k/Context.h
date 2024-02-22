#ifndef COROUTINE_M68K_CONTEXT_H
#define COROUTINE_M68K_CONTEXT_H 1

#pragma once

#include <assert.h>
#include <string.h>

#define COROUTINE __attribute__((noreturn)) void
#define COROUTINE_LIMITED_ADDRESS_SPACE

enum {COROUTINE_REGISTERS = 16};

struct coroutine_context
{
    void **stack_pointer;
    void *argument;
};

typedef COROUTINE(* coroutine_start)(struct coroutine_context *from, struct coroutine_context *self);

static inline void coroutine_initialize_main(struct coroutine_context * context) {
    context->stack_pointer = NULL;
}

static inline void coroutine_initialize(
    struct coroutine_context *context,
    coroutine_start start,
    void *stack,
    size_t size
) {
    assert(start && stack && size >= 1024);

    // Stack grows down. Force 16-byte alignment.
    char * top = (char*)stack + size;
    context->stack_pointer = (void**)((uintptr_t)top & ~0xF);

    *--context->stack_pointer = (void*)start;

    context->stack_pointer -= COROUTINE_REGISTERS;
    memset(context->stack_pointer, 0, sizeof(void*) * COROUTINE_REGISTERS);
}

struct coroutine_context * coroutine_transfer(struct coroutine_context * current, struct coroutine_context * target)
{
    rb_bug("not implemented");
}

static inline void coroutine_destroy(struct coroutine_context * context)
{
}

#endif /* COROUTINE_M68K_CONTEXT_H */