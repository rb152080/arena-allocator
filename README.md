# Arena Allocator

- Made a [C version](./c-arena-allocator) and a [C++ version](./cpp-arena-allocator)
- If you want to learn more about arena allocators, you can check out Ryan Fleury's [video](https://www.youtube.com/watch?v=TZ5a3gCCZYo) and [article](https://www.dgtlgrove.com/p/untangling-lifetimes-the-arena-allocator)
## Advantages of using Arena Allocators

- Typical `malloc`(C) and `new`(C++) go to the heap each time the respective keyword is called
- Arena allocators attempt to allocate all the heap memory all at once, typically in the beginning of the program
- So throughout the program, whenever you need to allocate memory, you just do it from that arena

## When to use an Arena Allocator

- When you know exactly how much memory you will be allocating
