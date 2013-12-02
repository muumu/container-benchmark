#ifndef PTR_VECTOR_H
#define PTR_VECTOR_H

#include <vector>

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

template <typename T>
class ptr_vector {
    ptr_vector(const ptr_vector&);
    ptr_vector& operator=(const ptr_vector&);
    std::vector<T*> vec_;
public:
    typedef typename std::vector<T*>::iterator iterator;
    ptr_vector() {}

    explicit ptr_vector(int n) {
        vec_.reserve(n);
    }

    ~ptr_vector() {
        clear();
    }

    void clear() {
        for(T* elem : vec_) {
            delete elem;
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

    void push_back(T* t) {
        vec_.push_back(t);
    }

    template <typename Comparator>
    void sort(Comparator& comparator = ptr_less<T*>()) {
        std::sort(vec_.begin(), vec_.end(), comparator);
    }

    void sort() {
        std::sort(vec_.begin(), vec_.end(), ptr_less<T*>());
    }

    void unique() {
        vec_.erase(ptr_unique<T>(vec_.begin(), vec_.end()), vec_.end());
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
    
};


#endif // PTR_VECTOR_H
