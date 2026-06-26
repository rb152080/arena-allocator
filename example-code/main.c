#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define ALIGN_UP(n, p) (((size_t)(n) + (size_t)(p) - 1) & (~((size_t)(p) - 1)))
#define ARENA_ALIGNMENT (alignof(max_align_t))

typedef struct
{
    size_t capacity;
    size_t position;
} memory_arena;

#define ARENA_BASE_POSITION (sizeof(memory_arena))

memory_arena* arena_create(size_t capacity)
{
    memory_arena* arena = (memory_arena*)malloc(capacity);
    arena->capacity = capacity;
    arena->position = ARENA_BASE_POSITION;
    return arena;
}

void arena_destroy(memory_arena* arena)
{
    free(arena);
}

void* arena_push(memory_arena* arena, size_t size)
{
    size_t aligned_position = ALIGN_UP(arena->position + size, ARENA_ALIGNMENT);
    if (aligned_position < arena->position ||
        aligned_position > arena->capacity)
        return NULL;
    void* memory = (char*)arena + aligned_position;
    arena->position = aligned_position;
    return memory;
}

void arena_clear(memory_arena* arena)
{
    arena->position = ARENA_BASE_POSITION;
}

int main()
{

    // initialize the arena; we call it frame_arena because it is memory
    // specifically reserved for the frames in the game
    memory_arena* frame_arena = arena_create(MiB(64));

    bool game_is_running = true;
    // the loop runs as long as the game is active
    while (game_is_running)
    {

        // at the end of the second, you reset the arena
        arena_clear(frame_arena);
    }

    // calls `free` and cleans up memory
    arena_destroy(frame_arena);
}
