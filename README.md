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

## Real World Example: Frame Arena

For demonstration and readability purposes, we use `malloc` and `free` here, but people often bypass them and instead directly use the underlying system kernel functions like `mmap` and `ummap`. 

```c

```

## Learn More about Arena Allocators

If you want to learn more about arena allocators, you can check out Ryan Fleury's [video](https://www.youtube.com/watch?v=TZ5a3gCCZYo) and [article](https://www.dgtlgrove.com/p/untangling-lifetimes-the-arena-allocator)