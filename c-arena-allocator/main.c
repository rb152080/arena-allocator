#include <stdalign.h>
#include <stdbool.h>
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

// need to put parentheses around whole expression and every variable/ parameter
// TODO: put better variable names
#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
// this is by power of 2
#define ALIGN_UP_POW2(n, p) (((u64)(n) + (u64)(p) - 1) & (~((u64)(p) - 1)))

#define ARENA_BASE_POSITION (sizeof(memory_arena))
// maybe make this max_align_t
// #define ARENA_ALIGNMENT (alignof(max_align_t))
#define ARENA_ALIGNMENT (sizeof(void*))

// this will zero out the memory
#define PUSH_STRUCT(arena, T) ((T*)arena_push((arena), sizeof(T), false))
// will not zero out the memory
#define PUSH_STRUCT_NZ(arena, T) ((T*)arena_push((arena), sizeof(T), true))
#define PUSH_ARRAY(arena, T, n)                                                \
    ((T*)arena_push((arena), sizeof(T) * (n), false))
#define PUSH_ARRAY_NZ(arena, T, n)                                             \
    ((T*)arena_push((arena), sizeof(T) * (n), true))

// virtual address space functions
u32 platform_get_pagesize(void);
void* platform_memory_reserve(u64 size);
b32 platform_memory_commit(void* ptr, u64 size);
b32 platform_memory_decommit(void* ptr, u64 size);
b32 platform_memory_release(void* ptr, u64 size);

typedef struct
{
    u64 reserve_size; // u64 capacity;
    u64 commit_size;  // chunk size for commits

    u64 position;
    u64 commit_position;
} memory_arena;

memory_arena* arena_create(u64 reserve_size, u64 commit_size)
{
    // memory_arena* arena = (memory_arena*)malloc(capacity);
    // arena->capacity = capacity;
    // arena->position = ARENA_BASE_POSITION;
    // return arena;

    u32 pagesize = platform_get_pagesize();
    printf("pagesize: %d\n", pagesize);
    reserve_size = ALIGN_UP_POW2(reserve_size, pagesize);
    commit_size = ALIGN_UP_POW2(commit_size, pagesize);

    memory_arena* arena = platform_memory_reserve(reserve_size);
    if (!platform_memory_commit(arena, commit_size))
        return NULL;
    arena->reserve_size = reserve_size;
    arena->commit_size = commit_size;
    arena->position = ARENA_BASE_POSITION;
    arena->commit_position = commit_size;
    return arena;
}

void arena_destroy(memory_arena* arena)
{
    // free(arena);
    platform_memory_release(arena, arena->reserve_size);
}

void* arena_push(memory_arena* arena, u64 size, b32 non_zero)
{
    u64 aligned_position = ALIGN_UP_POW2(arena->position, ARENA_ALIGNMENT);
    u64 new_position = aligned_position + size;
    // if (new_position > arena->capacity)
    //     return NULL;
    if (new_position > arena->reserve_size)
        return NULL;
    if (new_position > arena->commit_position)
    {
        u64 new_commit_position = new_position;
        new_commit_position += arena->commit_size - 1;
        new_commit_position -= new_commit_position % arena->commit_size;
        new_commit_position = MIN(new_commit_position, arena->reserve_size);

        u8* memory = (u8*)arena + arena->commit_position;
        u64 commit_size = new_commit_position - arena->commit_position;

        if (!platform_memory_commit(memory, commit_size))
            return NULL;
        arena->commit_position = new_commit_position;
    }
    arena->position = new_position;
    u8* out = (u8*)arena + aligned_position;
    if (!non_zero)
        memset(out, 0, size);
    return out;
}

void arena_pop(memory_arena* arena, u64 size)
{
    size = MIN(size, arena->position - ARENA_BASE_POSITION);
    arena->position -= size;
}

void arena_pop_to(memory_arena* arena, u64 position)
{
    u64 size = position < arena->position ? arena->position - position : 0;
    arena_pop(arena, size);
}

void arena_clear(memory_arena* arena)
{
    arena_pop_to(arena, ARENA_BASE_POSITION);
}

// #ifdef __linux__ also works
// #if defined() can be combined
// ex: #if defined(__linux__) && defined(__x86_64__)
#if defined(__linux__)

#define _DEFAULT_SOURCE

#include <sys/mman.h>
#include <unistd.h>

u32 platform_get_pagesize(void)
{
    return (u32)sysconf(_SC_PAGESIZE);
}

void* platform_memory_reserve(u64 size)
{
    void* out = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (out == MAP_FAILED) ? NULL : out;
}

b32 platform_memory_commit(void* ptr, u64 size)
{
    i32 ret = mprotect(ptr, size, PROT_READ | PROT_WRITE);
    return ret == 0;
}

b32 platform_memory_decommit(void* ptr, u64 size)
{
    i32 ret = mprotect(ptr, size, PROT_NONE);
    if (ret != 0)
        return false;
    ret = madvise(ptr, size, MADV_DONTNEED);
    return ret == 0;
}

b32 platform_memory_release(void* ptr, u64 size)
{
    i32 ret = munmap(ptr, size);
    return ret == 0;
}

#endif

int main(void) // explicity tells the compiler that it takes no arguments
{
    // printf("%zu\n", ARENA_ALIGNMENT);
    memory_arena* permanent_arena = arena_create(GiB(1), MiB(1));
    arena_destroy(permanent_arena);
    return 0;
}
