# Arena Allocator

made a [C version](./c-arena-allocator) and a [C++ version](./cpp-arena-allocator)

check out my [blog]() to learn more about arena allocators

- If you want to learn more about arena allocators, you can check out Ryan Fleury's [video](https://www.youtube.com/watch?v=TZ5a3gCCZYo) and [article](https://www.dgtlgrove.com/p/untangling-lifetimes-the-arena-allocator)

## Advantages of using Arena Allocators

- Typical `malloc`(C) and `new`(C++) go to the heap each time the respective keyword is called
- Arena allocators attempt to allocate all the heap memory all at once, typically in the beginning of the program
- So throughout the program, whenever you need to allocate memory, you just do it from that arena
- This is good because it reduces overhead, less system calls

## When to use an Arena Allocator

- When you know exactly how much memory you will be allocating
- Typically in games when there are graphical frames, you want to allocate memory for that frame
- Each second, the frame has to load
- But each second, you don't want to reallocate memory for that frame
- So, you can use an arena for frames

provide some example arena for frames

```c++
Arena arena();
while () {

    // at the end of the second, you reset the arena
    arena.reset()
}
```
