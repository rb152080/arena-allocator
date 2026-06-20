#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: capitalize these
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// boolean types, why are there different sizes?
typedef i8 b8;
typedef i32 b32;

// need to put parentheses around whole expression and every variable, parameter
// TODO: put better variable names
#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
// this is by power of 2
#define ALIGN_UP(n, p) (((u64)(n) + (u64)(p) - 1) & (~((u64)(p) - 1)))

#define ARENA_BASE_POSITION (sizeof(memory_arena))
// maybe make this max_align_t
// #define ARENA_ALIGNMENT (alignof(max_align_t))
#define ARENA_ALIGNMENT (sizeof(void *))

// this will zero out the memory
#define PUSH_STRUCT(arena, T) ((T *)arena_push((arena), sizeof(T), false))
// will not zero out the memory
#define PUSH_STRUCT_NZ(arena, T) ((T *)arena_push((arena), sizeof(T), true))
#define PUSH_ARRAY(arena, T, n)                                                \
    ((T *)arena_push((arena), sizeof(T) * (n), false))
#define PUSH_ARRAY_NZ(arena, T, n)                                             \
    ((T *)arena_push((arena), sizeof(T) * (n), true))

// virtual address space functions
void* platform_get_pagesize(u64 size);
void *platform_memory_reserve(u64 size);
b32 platform_memory_commit(void *ptr, u64 size);
b32 platform_memory_decommit(void *ptr, u64 size);
b32 platform_memory_release(void *ptr, u64 size);

typedef struct
{
    u64 capacity;
    u64 position;
} memory_arena;

memory_arena *arena_create(u64 capacity)
{
    memory_arena *arena = (memory_arena *)malloc(capacity);
    arena->capacity = capacity;
    arena->position = ARENA_BASE_POSITION;
    return arena;
}

void arena_destroy(memory_arena *arena) { free(arena); }

void *arena_push(memory_arena *arena, u64 size, b32 non_zero)
{
    u64 aligned_position = ALIGN_UP(arena->position, ARENA_ALIGNMENT);
    u64 new_position = aligned_position + size;
    if (new_position > arena->capacity)
    {
        return NULL;
    }
    arena->position = new_position;
    u8 *out = (u8 *)arena + aligned_position;
    if (!non_zero)
        memset(out, 0, size);
    return out;
}

void arena_pop(memory_arena *arena, u64 size)
{
    size = MIN(size, arena->position - ARENA_BASE_POSITION);
    arena->position -= size;
}

void arena_pop_to(memory_arena *arena, u64 position) {}

void arena_clear(memory_arena *arena);

int main(void) // explicity tells the compiler that it takes no arguments
{
    // printf("%zu\n", ARENA_ALIGNMENT);
    memory_arena *permanent_arena = arena_create(MiB(1));
    arena_destroy(permanent_arena);
    return 0;
}
