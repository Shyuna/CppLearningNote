#include <iostream>
#include <vector>
#include <optional>
#include <memory>
#include <type_traits>


struct ReducerState {
    virtual int result() = 0;

    virtual void add(int a) = 0;
};

//生产一个B类型的ReducerState
template<typename B>
struct Reducer {
    virtual std::unique_ptr<ReducerState> init() {
        static_assert(std::is_base_of<ReducerState, B>::value, "class is not derived from ReducerState!");
        return std::make_unique<B>();
    }
};

struct AddReducerState : ReducerState {
    int ret = 0;

    int result() override {
        return ret;
    }

    void add(int a) override {
        ret += a;
    }
};


struct MulReducerState : ReducerState {
    int ret = 1;

    int result() override {
        return ret;
    }

    void add(int a) override {
        ret *= a;
    }
};

struct AvgReducerState : ReducerState {
    int sum = 0;
    int cnt = 0;

    int result() override {
        return sum / cnt;
    }

    void add(int a) override {
        sum += a;
        cnt++;
    }
};


//Inputer用于提供fetch()接口，返回数字
struct Inputer {
    virtual std::optional<int> fetch() = 0;
};

struct StopInputerAdapter : Inputer {
    Inputer *input;
    int stopMark;

    StopInputerAdapter(Inputer *input, int stopMark) : input(input), stopMark(stopMark) {
    }

    std::optional<int> fetch() override {
        std::optional<int> tmp;
        tmp = input->fetch();
        if (tmp == stopMark) {
            return std::nullopt;
        }
        return tmp;
    }
};

struct CinInputer : Inputer {
    std::optional<int> fetch() override {
        int tmp;
        std::cin >> tmp;
        return tmp;
    }
};

struct VecInputer : Inputer {
    std::vector<int> vec;
    int loc = 0;

    VecInputer(std::vector<int> vec) {
        this->vec = std::move(vec);
    }

    std::optional<int> fetch() override {
        if (loc < vec.size()) {
            return vec[loc++];
        } else {
            return std::nullopt;
        }

    }
};

struct FilterStrategy {
    virtual bool shouldPass(int i) = 0;
};

struct FilterStrategyAbove : FilterStrategy {
    int threshold = 0;

    bool shouldPass(int i) override {
        return i >= threshold;
    }

    FilterStrategyAbove(int threshold) : threshold(threshold) {}
};

struct FilterStrategyBelow : FilterStrategy {
    int threshold = 0;

    bool shouldPass(int i) override {
        return i <= threshold;
    }

    FilterStrategyBelow(int threshold) : threshold(threshold) {}
};

struct FilterStrategyAnd : FilterStrategy {
    FilterStrategy *filterStrategy1;
    FilterStrategy *filterStrategy2;

    bool shouldPass(int i) override {
        return filterStrategy1->shouldPass(i) && filterStrategy2->shouldPass(i);
    }

    FilterStrategyAnd(FilterStrategy *filterStrategy1, FilterStrategy *filterStrategy2) : filterStrategy1(
            filterStrategy1), filterStrategy2(filterStrategy2) {}
};

//继承Inputer使用fetch()获取满足FilterStrategy的数字
struct FilterStrategyAdapter : Inputer {
    FilterStrategy *filterStrategy;
    Inputer *inputer;

    FilterStrategyAdapter(Inputer *inputer, FilterStrategy *filterStrategy) : inputer(inputer),
                                                                              filterStrategy(filterStrategy) {}

    std::optional<int> fetch() {
        std::optional<int> tmp = inputer->fetch();
        while (tmp.has_value()) {
            if (!filterStrategy->shouldPass(tmp.value())) {
                tmp = inputer->fetch();
                continue;
            }
            return tmp;
        }
        return std::nullopt;
    }
};

template<typename B>
int reduce(Inputer *inputer, Reducer<B> *red) {
    std::unique_ptr<ReducerState> reducerState = red->init();

    while (std::optional<int> tmp = inputer->fetch()) {
        if (!tmp.has_value()) {
            break;
        }
        reducerState->add(tmp.value());
    }
    return reducerState->result();
}


int main() {
    std::vector<int> vec = {1, 2, 1, 8, 5, 3, -1, 5};

    std::cout << reduce(new VecInputer(vec), new Reducer<AddReducerState>()) << std::endl;
    std::cout << reduce(new FilterStrategyAdapter(new StopInputerAdapter(new VecInputer(vec), -1),
                                                  new FilterStrategyAnd(new FilterStrategyAbove(2),
                                                                        new FilterStrategyBelow(5))),
                        new Reducer<AddReducerState>()) << std::endl;
    std::cout << reduce(new StopInputerAdapter(new CinInputer(), -1), new Reducer<AddReducerState>()) << std::endl;
    std::cout << reduce(new VecInputer(vec), new Reducer<AvgReducerState>());

    return 0;
}