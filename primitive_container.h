#include <algorithm>
#include <iostream>
#include <cstdlib>

template <typename T, size_t S>
class primitive_static_vector {
    T arr_[S];
    int pos_;
public:
    typedef T* iterator;
    typedef const T* const_iterator;

    primitive_static_vector() : pos_(0) {}

    iterator begin() {
        return arr_;
    }

    iterator end() {
        return arr_ + pos_;
    }

    T& operator[](int i) {
        return arr_[i];
    }

    const T& operator[](int i) const {
        return arr_[i];
    }

    void push_back(T elem) {
        arr_[pos_++] = elem;
    }

    size_t size() const {
        return static_cast<size_t>(pos_);
    }

    void erase(iterator first, iterator last) {
        iterator ed = end();
        while(last != ed) {
            *(first++) = *(last++);
        }
        pos_ = first - arr_;
    }

    void clear() {
        pos_ = 0;
    }
};

