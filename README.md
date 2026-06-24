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

Arena allocators are perfect when you have a good idea of your maximum memory requirements, or when you need to allocate a large amount of temporary objects that share the same lifetime. They are particularly useful in games where there are graphical frames. Modern video games typically render 60, 120, or even 144+ frames per second. 


Each frame would use the same amount of memory, and it would typically have to be refreshed every second, for each frame. Typically, this would mean that for each second, the program has to allocate memory for that frame. This is a situation in which you do not want to reallocate memory for that frame since it is a costly operation that would reduce performance in games. So, this is a perfect example for when you can use an arena for frames. You can have an arena specifically for managing the memory of frames. You allocate the objects needed for the frame onto that arena, and after each second is over, you reset the pointer back to the start of the arena. That way the memory allocation and deallocation only happens once: at the start of the game and at the end of the game.

## Real World Example: Frame Arena

For demonstration and readability purposes, we use `malloc` and `free` here, but people often bypass them and instead use the underlying system kernel functions like `mmap` and `ummap`. 

```c

```

## Learn More about Arena Allocators

If you want to learn more about arena allocators, you can check out Ryan Fleury's [video](https://www.youtube.com/watch?v=TZ5a3gCCZYo) and [article](https://www.dgtlgrove.com/p/untangling-lifetimes-the-arena-allocator)