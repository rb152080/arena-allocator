# Arena Allocator

made a [C version](./c-arena-allocator) and a [C++ version](./cpp-arena-allocator)

check out my [blog]() to learn more about arena allocators

## Why you should care about Arena Allocators

Arena allocators are useful when writing high-performance C/C++ code, especially when you have to allocate memory several times during your program.

## The Problem with `malloc` and `new`

Typical `malloc`(in C) and `new`(in C++) calls go to the heap each time the respective keyword is called. This introduces CPU overhead since the memory allocator has to search for free space. It also requires frequent system calls, forcing transitions from user mode to kernel mode to get more memory from the operating system. In addition, repeatedly allocating and deallocating from the heap can cause memory fragmentation over time.

## Advantages of using Arena Allocators

Arena allocators request a massive chunk of memory all at once, typically in the beginning of the program. So throughout the program, whenever you need to allocate memory, you can just do it from that arena and avoid needing to do a syscall each time. Because of this, arenas reduce the risk of memory leaks since we only call `malloc` and `free` once for the entire arena and not for every individual object we allocate.

In addition, allocating from an arena happens in $O(1)$ time, since arenas use a pointer to the start of the free memory. Every time we allocate an object, we just move the pointer up by the size of that object. Once we are done using all of the objects in the arena, we can restore the pointer back to the start of the arena. Now, if we were to create new objects in the arena, it would just overwrite the ones we don't need anymore.

## When to use an Arena Allocator

Arena allocators are perfect when you have a good idea of your maximum memory requirements, or when you need to allocate a large amount of temporary objects that share the same lifetime. They are particularly useful in games where there are graphical frames. 

Modern video games render 60 to 144+ frames per second. This means that the program needs to allocate memory up to hundreds of times per second to display frames for the user to see. This is a situation in which you do not want to call `malloc` and `free` for every frame since it is a costly operation that would reduce performance and cause lag in games. 

So, this is a perfect example for when to use an arena allocator. You allocate a dedicated arena specifically for frame data when the game boots up. When a frame needs to be displayed, all of the data needed for the frame to render is allocated in the arena. Once the frame is displayed, the arena pointer gets reset back to the start of the arena and the cycle repeats itself for every frame. That way the physical memory allocation and deallocation only happens once at the start and end of the game, keeping your frame rate smooth.

## Arena Implementation

### Creating the Arena Object

We can create an arena object using a `struct`. We `typedef uint64_t` as `u64` for convenience. We use fixed-width integer types so that our memory calculations are universal across all systems and leave no room for platform ambiguity. We also use unsigned integers since we do not have to deal with negative numbers.

```c
typedef uint64_t u64;

typedef struct
{
    u64 capacity;
    u64 position;
} memory_arena;
```

### Initializing and Destroying the Arena

We now need a function that acts as a constructor to initialize the arena struct's fields. We create a macro for the arena's base position (`ARENA_BASE_POSITION`), which marks where the usable free memory begins. We reserve `sizeof(memory_arena)` at the very front of the allocation to store the two fields of the struct (`capacity`, `position`), which take up 16 bytes in total (8 bytes each).

Typically, developers using arenas know exactly how much memory they will need altogether, so they can allocate it all at once. This initialization is the only time the program triggers a syscall to the operating system to reserve memory on the heap.

```c
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
```

This next function acts as our destructor. When we are fully done with our arena, we can discard it using a single call to `free`.

```c
void arena_destroy(memory_arena* arena)
{
    free(arena);
}
```

***Note:** For demonstration and readability purposes, we use `malloc` and `free` here. In production code, developers often bypass these and instead directly use underlying system-level kernel functions like `mmap` and `munmap` (found in `<sys/mman.h>` if you are on Linux).*

### Pushing Something onto the Arena

To allocate memory, our push function takes in two parameters: the arena we want to use and the size of the data we want to request.

The `ALIGN_UP` and `ARENA_ALIGNMENT` macros handle memory alignment. This is important because CPUs require data to sit on specific memory addresses (typically powers of 2) for fast retrieval. We use an alignment of `alignof(max_align_t)`, which represents the strictest alignment requirement for standard scalar types in C (16 bytes for a `long double`).

```c
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
```

### Resetting the Arena

The great thing about arenas is how fast and cheap it is to clear it. All you need to do is reset the pointer back to the base position.

```c
void arena_clear(memory_arena* arena)
{
    arena->position = ARENA_BASE_POSITION;
}
```

## Real-World Example: The Frame Arena

With our arena functions completed, we can now see them in action.

```c
// same thing as multiplying; bit shifting is faster though
#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

int main(void)
{

    // 1. Initialize the arena
    // We call it frame_arena because it is memory specifically reserved for the frames in the game
    memory_arena* frame_arena = arena_create(MiB(64));

    bool game_is_running = true;

    // This loop executes many times every second (60+ FPS)
    while (game_is_running)
    {
        // 2. Temporary Gameplay Graphics Allocations
        int* particle_positions = (int*)arena_push(frame_arena, sizeof(int) * 1000);
        char* frame_log_buffer = (char*)arena_push(frame_arena, 256);

        // 3. Simulating and rendering
        // These are arbitrary function names I came up with (for demonstration purposes)
        update_game_physics(particle_positions);
        draw_frame(frame_log_buffer);

        // 4. Wiping the canvas
        // Once the frame is drawn to the screen, we snap the pointer back to the beginning for the next frame
        arena_clear(frame_arena);
    }

    // 5. Clean up the heap memory once the user closes the game
    arena_destroy(frame_arena);

    return 0;
}

```

## Learn More about Arena Allocators

If you want to learn more about arena allocators, you can check out Ryan Fleury's [video](https://www.youtube.com/watch?v=TZ5a3gCCZYo) and [article](https://www.dgtlgrove.com/p/untangling-lifetimes-the-arena-allocator).