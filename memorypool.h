#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <new>


template <typename T>
class MemoryPool {
public:
    MemoryPool(size_t initial_size = 0, size_t expansion_size = 128) : expansion_size(expansion_size) {
        if(initial_size > 0) expandFreeList(expansion_size);
    }
    ~MemoryPool() {
        for(MemoryPool<T>* nextPtr = next; nextPtr != nullptr; nextPtr = next) {
            next = next->next;
            delete[] reinterpret_cast<char*>(nextPtr);
            //delete(nextPtr);
        }
    }
    void* alloc() {
        if (next == nullptr) {
            expandFreeList(expansion_size);
        }
        MemoryPool<T>* head = next;
        next = head->next;
        return head;
    }
    void free(void* doomed) {
        MemoryPool<T>* head = static_cast<MemoryPool<T>*>(doomed);
        head->next = next;
        next = head;
    }
private:
    MemoryPool<T> *next;
    const int expansion_size;
    void expandFreeList(int howMany) {
        size_t size = (sizeof(T) > sizeof(MemoryPool<T>*)) ? sizeof(T) : sizeof(MemoryPool<T>*);
        MemoryPool<T>* runner = reinterpret_cast<MemoryPool<T>*>(new char[size]);
        //T* pool = new T[howMany];
        //MemoryPool<T>* runner = reinterpret_cast<MemoryPool<T>*>(new(pool) T);
        next = runner;
        for(int i=1; i<howMany; ++i) {
            runner->next = reinterpret_cast<MemoryPool<T>*>(new char[size]);
            //runner->next = reinterpret_cast<MemoryPool<T>*>(new(pool + i) T);
            runner = runner->next;
        }
        runner->next = nullptr;

    }
};

#endif // MEMORYPOOL_H
