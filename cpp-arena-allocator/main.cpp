#include <cstddef>
#include <cstdint>
#include <new>

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
        // you use it to avoid calling object constructors
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
        uintptr_t aligned { (current + alignment - 1) & ~(alignment - 1) };
        std::size_t padding { aligned - current };
        if (m_offset + size + padding > m_size)
            return nullptr;
        m_offset += padding;
        void* result { m_buffer + m_offset };
        m_offset += size;
        return result;
    }

    template <typename T, typename... Args>
    T* allocate(Args&&... args)
    {
        // implicit conversion from void* to T*
        T* object { allocate(sizeof(T), alignof(T)) };
        // better than doing !object
        if (object == nullptr)
            return nullptr;
        return new (object) { std::forward<Args>(args)... };
    }

  private:
    std::byte* m_buffer {};
    std::size_t m_size {};
    std::size_t m_offset {}; // pointer to where the free space starts
};
