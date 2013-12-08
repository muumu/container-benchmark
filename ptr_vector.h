#ifndef PTR_VECTOR_H
#define PTR_VECTOR_H

#include <vector>
#include <new>
#include "memorypool.h"

template <typename T>
struct ptr_less {
    bool operator()(const T left, const T right) const {
        return *left < *right;
    }
};

template <typename T>
struct ptr_greater {
    bool operator()(const T left, const T right) const {
        return *left > *right;
    }
};

template <typename T>
struct ptr_equal {
    bool operator()(const T left, const T right) const {
        return *left == *right;
    }
};

template <typename T, typename ForwardIterator>
inline ForwardIterator ptr_unique(ForwardIterator first, ForwardIterator last) {
    if (first==last) return last;

    ForwardIterator result = first;
    while (++first != last) {
        if(**result == **first) delete static_cast<T*>(*first);
        else *(++result)=*first;
    }
    return ++result;
}

//#define USE_MEMPOOL
template <typename T>
class ptr_vector {
    ptr_vector(const ptr_vector&);
    ptr_vector& operator=(const ptr_vector&);
    std::vector<T*> vec_;
    MemoryPool<T> mempool;
public:
    typedef typename std::vector<T*>::iterator iterator;
    #ifdef USE_MEMPOOL
    ptr_vector(size_t reserve_size = 0, size_t expansion_size = 128) : mempool(reserve_size, expansion_size){}
    #else
    ptr_vector() {}
    explicit ptr_vector(int n) {
        vec_.reserve(n);
    }
    #endif

    ~ptr_vector() {
        #ifndef USE_MEMPOOL
        clear();
        #endif
    }

    void clear() {
        for(T* elem : vec_) {
            #ifdef USE_MEMPOOL
            mempool.free(elem);
            #else
            delete elem;
            #endif
        }
        vec_.clear();
    }

    void reserve(int n) {
        vec_.reserve(n);
    }

    const T& operator[](int i) const {
        return *vec_[i];
    }

    T& operator[](int i) {
        return *vec_[i];
    }

    void push_back(const T& t) {
        #ifdef USE_MEMPOOL
        alloc(t);
        #else
        vec_.push_back(new T(t));
        #endif
    }

    template <typename Comparator>
    void sort(Comparator& comparator = ptr_less<T*>()) {
        std::sort(vec_.begin(), vec_.end(), comparator);
    }

    void sort() {
        std::sort(vec_.begin(), vec_.end(), ptr_less<T*>());
    }

    void unique() {
        #ifdef USE_MEMPOOL
        if(vec_.size() == 0) return;
        auto first = vec_.begin();
        auto last = vec_.end();
        iterator result = first;
        while (++first != last) {
            if(**result == **first) mempool.free(static_cast<T*>(*first));
            else *(++result)=*first;
        }
        vec_.erase(++result, vec_.end());
        #else
        vec_.erase(ptr_unique<T>(vec_.begin(), vec_.end()), vec_.end());
        #endif
    }

    void shrink_to_fit() {
        vec_.shrink_to_fit();
    }

    int size() const {
        return vec_.size();
    }

    iterator begin() {
        return vec_.begin();
    }

    iterator end() {
        return vec_.end();
    }

private:
    #ifdef USE_MEMPOOL
    void alloc(const T& t) {
        //T* elem = static_cast<T*>(mempool.alloc());
        T* elem = new(mempool.alloc()) T(t);
        //*elem = t;
        vec_.push_back(elem);
    }
    #endif
};


#endif // PTR_VECTOR_H
