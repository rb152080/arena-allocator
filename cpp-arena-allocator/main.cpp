#include <cstddef>
#include <cstdint>
#include <iostream>
#include <new>
#include <utility>

class Arena
{
  public:
    // for initializing private variables in a class, use () not {}
    // (): direct initialization
    // {}: direct list initialization
    // {} for initializing an object
    // https://github.com/rb152080/cpp-funcs/blob/main/constructor-initialization/main.cpp
    Arena(std::size_t size) : m_size(size), m_offset(0)
    {
        // ::operator new basically does malloc, returns a void*
        // you use it to avoid calling object constructors like Block*
        m_buffer = reinterpret_cast<std::byte*>(::operator new(size));
    }

    ~Arena()
    {
        ::operator delete(m_buffer);
    }

    void* allocate(std::size_t size,
                   std::size_t alignment = alignof(std::max_align_t))
    {
        // uintptr_t: unsigned integer type
        // you make it a uintptr_t so you can perform integer operations on it
        uintptr_t current { reinterpret_cast<uintptr_t>(m_buffer + m_offset) };
        // https://github.com/rb152080/heap-allocator/blob/main/main.cpp
        uintptr_t aligned { (current + alignment - 1) & ~(alignment - 1) };
        std::size_t padding { aligned - current };
        if (m_offset + size + padding > m_size)
            return nullptr;
        m_offset += padding;
        void* result { m_buffer + m_offset };
        m_offset += size;
        return result;
    }

    // this is called a templated factory method
    // it automatically handles the size, alignment, constructor calls
    // ... is part of C++ Variadic Templates and Parameter Packs
    // ... is a variable number of arguments
    template <typename T, typename... Args>
    // &&: universal reference/ forwarding reference
    // && avoids copying
    T* allocate(Args&&... args)
    {
        // implicit conversion from void* to T*
        void* object { allocate(sizeof(T), alignof(T)) };
        // better than doing !object
        if (object == nullptr)
            return nullptr;
        // placement new: instead of finding memory, use object's memory address
        // forwards the properties of the arguments to the constructor
        // std::forward avoids copying
        return new (object) T { std::forward<Args>(args)... };
    }

    void reset()
    {
        m_offset = 0;
    }

  private:
    std::byte* m_buffer {};
    std::size_t m_size {};
    // instead of having a pointer to where the empty space starts, we use an
    // unsigned integer offset variable to add to the buffer
    std::size_t m_offset {}; // where the free space starts
};

int main(void)
{
    Arena arena(1024);
    auto* x { arena.allocate<int>(4) };
    auto* y { arena.allocate<int>(8) };
    std::cout << *x << "\n";
    std::cout << *y << "\n";
    arena.reset();
    std::cout << *x << "\n"; // still there
    auto* z { arena.allocate<double>(4.8) };
    std::cout << *x << "\n"; // overrode x's data with z's data
    std::cout << *z << "\n";
    std::cout << *y << "\n"; // also overrode y's data
    return 0;
}
