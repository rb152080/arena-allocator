#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint64_t u64;

typedef struct
{
    u64 capacity;
    u64 position;
} memory_arena;

#define ARENA_BASE_POSITION (sizeof(memory_arena))

memory_arena* arena_create(u64 capacity)
{
    memory_arena* arena = (memory_arena*)malloc(capacity);
    if (arena == NULL)
        return NULL;
    arena->capacity = capacity;
    arena->position = ARENA_BASE_POSITION;
    return arena;
}

void arena_destroy(memory_arena* arena)
{
    free(arena);
}

#define ALIGN_UP(n, p) (((u64)(n) + (u64)(p) - 1) & (~((u64)(p) - 1)))
#define ARENA_ALIGNMENT (alignof(max_align_t))

void* arena_push(memory_arena* arena, u64 size)
{
    u64 aligned_position = ALIGN_UP(arena->position, ARENA_ALIGNMENT);
    if (aligned_position + size < arena->position ||
        aligned_position + size > arena->capacity)
        return NULL;
    void* memory = (char*)arena + aligned_position;
    arena->position = aligned_position + size;
    return memory;
}

void arena_clear(memory_arena* arena)
{
    arena->position = ARENA_BASE_POSITION;
}

void update_game_physics(int* param) {}

void draw_frame(char* param) {}

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

int main(void)
{

    // 1. Initialize the arena at the start of the game
    // We call it frame_arena because it is memory specifically reserved for the
    // frames in the game
    memory_arena* frame_arena = arena_create(MiB(64));

    bool game_is_running = true;

    // This loop executes many times every second (60+ FPS)
    while (game_is_running)
    {
        // 2. Temporary Gameplay Graphics Allocations
        int* particle_positions =
            (int*)arena_push(frame_arena, sizeof(int) * 1000);
        char* frame_log_buffer = (char*)arena_push(frame_arena, 256);

        // 3. Simulating and rendering
        // These are arbitrary function names I came up with (for demonstration
        // purposes)
        update_game_physics(particle_positions);
        draw_frame(frame_log_buffer);

        // 4. Wiping the canvas
        // Once the frame is drawn to the screen, we snap the pointer back to
        // the beginning for the next frame
        arena_clear(frame_arena);
    }

    // 5. Clean up the heap memory once the user closes the game
    arena_destroy(frame_arena);

    return 0;
}
