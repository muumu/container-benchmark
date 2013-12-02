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
        for (int i=0; i<6; ++i) { // loop size is constant for fixed duplication rate
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


template <typename Container>
class ContainerWrapperBase {
protected:
    Container container_;
public:
    virtual ~ContainerWrapperBase() {};
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

        timer.start("push_back part");
        timer.pause();
        for (int i=0; i<LOOPSIZE; ++i) {
            elem.fill_rand(); // timer ignores the time consumption of this function call
            timer.resume();
            container_.push_back(elem);
            timer.pause();
        }
        timer.stop(); // push_back part end

        timer.start("sort part");
        container_.sort();
        timer.stop(); // sort part end

        std::cout << "container size before unique: " << container_.size() << std::endl;
        timer.start("unique part");
        container_.unique();
        timer.stop(); // uniue part end

        std::cout << "container size after unique: " << container_.size() << std::endl;
    }
};

int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 0;
    MyTimer timer;
    switch (n) {
    case 1:
        ContainerBenchmark<ContainerWrapper<ptr_vector<Board>>>()(timer);
        break;
    case 2:
        ContainerBenchmark<ContainerWrapper<boost::ptr_vector<Board>>>()(timer);
        break;
    case 3:
        ContainerBenchmark<ContainerWrapper<std::vector<std::shared_ptr<Board>>>>()(timer);
        break;
    case 4:
        ContainerBenchmark<ContainerWrapper<std::vector<Board>>>()(timer);
        break;
    }
    timer.showResults();
}
