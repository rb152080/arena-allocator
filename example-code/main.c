

void arena_destroy(memory_arena* arena)
{
    free(arena);
}

int main()
{

    // initialize the arena; we call it frame_arena because it is memory
    // specifically reserved for the frames in the game
    memory_arena* frame_arena = arena_create(MiB(64), MiB(4));

    bool game_is_running = true;
    // the loop runs as long as the game is active
    while (game_is_running)
    {

        // at the end of the second, you reset the arena
        arena.reset()
    }

    // calls `free` and cleans up memory
    arena_destroy(frame_arena);
}
