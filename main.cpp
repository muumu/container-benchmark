#include <inttypes.h>
#include <cstdlib>
#include <array>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <boost/ptr_container/ptr_vector.hpp>
#include "ptr_vector.h"
#include "mytimer.h"

const int LOOPSIZE = 1000000;

class Board {
    static const int X = 6;
    std::array<uint64_t, X> arr{};
public:
    Board() {}

    Board(const Board& bd) : arr(bd.arr) {}

    void fill_rand() {
        for (int i=0; i<6; ++i) {
            arr[i] = (arr[i]+rand())%8;
        }
    }

    bool operator<(const Board& bd) const {
        for (int i=0; i<X-1; ++i) {
            if (arr[i] != bd.arr[i]) {
                return arr[i] < bd.arr[i];
            }
        }
        return arr[X-1] < bd.arr[X-1];
    }

    bool operator==(const Board& bd) const {
        for(int i=0; i<X; ++i) {
            if(arr[i] != bd.arr[i]) return false;
        }
        return true;
    }

    Board& operator=(const Board& bd) {
        arr = bd.arr;
        return *this;
    }
};

class Code {
    std::vector<uint64_t> code;
public:
    Code() : code(1000) {}
};

class BoardPlus {
    Board bd;
    //Code code;
    unsigned value;
public:
    void fill_rand() {
        //bd.fill_rand();
        value = rand() % 256500;
    }
    bool operator<(const BoardPlus& bp) const {
        //return bd < bp.bd;
        return value < bp.value;
    }
    bool operator==(const BoardPlus& bp) const {
        //return bd == bp.bd;
        return value == bp.value;
    }
};


template <typename Container>
class ContainerWrapperBase {
protected:
    Container container_;
public:
    virtual ~ContainerWrapperBase() {};
    void reserve(int n) {
        container_.reserve(n);
    }
    size_t size() const {
        return container_.size();
    }
};

template <typename Container>
struct ContainerWrapper;

template <typename T>
class ContainerWrapper<ptr_vector<T>> : public ContainerWrapperBase<ptr_vector<T>> {
    using ContainerWrapperBase<ptr_vector<T>>::container_;
public:
    typedef T Element;
    void push_back(const T& elem) {
        container_.push_back(elem);
    }
    void sort() {
        container_.sort();
    }
    void unique() {
        container_.unique();
    }
};

template <typename T>
class ContainerWrapper<boost::ptr_vector<T>> : public ContainerWrapperBase<boost::ptr_vector<T>> {
    using ContainerWrapperBase<boost::ptr_vector<T>>::container_;
public:
    typedef T Element;
    void push_back(const T& elem) {
        container_.push_back(new T(elem));
    }
    void sort() {
        container_.sort();
    }
    void unique() {
        container_.unique();
    }
};

template <typename T>
class ContainerWrapper<std::vector<T>> : public ContainerWrapperBase<std::vector<T>> {
    using ContainerWrapperBase<std::vector<T>>::container_;
public:
    typedef T Element;
    void push_back(const T& elem) {
        container_.push_back(elem);
    }
    void sort() {
        std::sort(container_.begin(), container_.end());
    }
    void unique() {
        auto end_it = std::unique(container_.begin(), container_.end());
        container_.erase(end_it, container_.end());
    }
};

template <typename T>
class ContainerWrapper<std::vector<std::shared_ptr<T>>> :
        public ContainerWrapperBase<std::vector<std::shared_ptr<T>>> {
    using ContainerWrapperBase<std::vector<std::shared_ptr<T>>>::container_;
public:
    typedef T Element;
    void push_back(const T& elem) {
        container_.push_back(std::make_shared<T>(elem));
    }
    void sort() {
        std::sort(container_.begin(), container_.end(), ptr_less<std::shared_ptr<T>>());
    }
    void unique() {
        auto end_it = std::unique(container_.begin(), container_.end(), ptr_equal<std::shared_ptr<T>>());
        container_.erase(end_it, container_.end());
    }
};

template <typename Container>
class ContainerBenchmark {
    Container container_;
public:
    void operator()(MyTimer& timer) {
        typename Container::Element elem;
        container_.reserve(LOOPSIZE);
        timer.stop(); // end of construct part
        timer.start("push_back part");
        timer.pause();
        for (int i=0; i<LOOPSIZE; ++i) {
            elem.fill_rand(); // timer ignores the time consumption of this function call
            timer.resume();
            container_.push_back(elem);
            timer.pause();
        }
        timer.stop(); // end of push_back part

        timer.start("sort part");
        container_.sort();
        timer.stop(); // end od sort part

        std::cout << "container size before unique: " << container_.size() << std::endl;
        timer.start("unique part");
        container_.unique();
        timer.stop(); // end of uniue part

        std::cout << "container size after unique: " << container_.size() << std::endl;
        timer.start("destruct part");
    }
};

int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 0;
    typedef Board T;
    //typedef BoardPlus T;
    MyTimer timer;
    switch (n) {
    case 1:
        timer.start("construct part");
        ContainerBenchmark<ContainerWrapper<ptr_vector<T>>>()(timer);
        timer.stop();
        break;
    case 2:
        timer.start("construct part");
        ContainerBenchmark<ContainerWrapper<boost::ptr_vector<T>>>()(timer);
        timer.stop();
        break;
    case 3:
        timer.start("construct part");
        ContainerBenchmark<ContainerWrapper<std::vector<std::shared_ptr<T>>>>()(timer);
        timer.stop();
        break;
    case 4:
        timer.start("construct part");
        ContainerBenchmark<ContainerWrapper<std::vector<T>>>()(timer);
        timer.stop();
        break;
    }
    timer.showResults();
}
