#include <boost/signals2.hpp>
#include <iostream>
#include <experimental/iterator>

struct WithOutParam {
    void operator()() const {
        std::cout << "Signal emits without parameters." << std::endl;
    }
};

void PassArgs(int a, int b) {
    std::cout << "Signal emits with 2 integers: " << a << ", " << b << std::endl;
}

std::function<int (int, int)> CallbackWithReturn = [](int a, int b) -> int {
    std::cout << "Signal emits with 2 integers: " << a << ", " << b << ", Return : " << (a + b) << std::endl;
    return a + b;
};


// --- to aggregate all returns ---
std::function<int (int, int)> algo_add = [](int a, int b) -> int { return a + b; };
std::function<int (int, int)> algo_sub = [](int a, int b) -> int { return a - b; };
int algo_mul(int a, int b) { return a * b; };
std::function<int (int, int)> algo_div = [](int a, int b) -> int { return a / b; };

template <typename Container>
struct Aggregate {
    typedef Container result_type;

    template <typename InputIterator>
    Container operator()(InputIterator first, InputIterator last) const {
        Container values;

        while(first != last) {
            values.push_back(*first);
            ++first;
        }
        return values;
    }
};


// ------------------------------------------------
// 封装signal和slot到一个class中
class Button
{
    typedef boost::signals2::signal<void (int x, int y)> OnClick;
public:
    typedef OnClick::slot_type OnClickSlotType;
    // forward slots through Button interface to its private signal
    boost::signals2::connection doOnClick(const OnClickSlotType & slot);

    // simulate user clicking on GUI button at coordinates 52, 38
    void simulateClick();
private:
        OnClick onClick;
};

boost::signals2::connection Button::doOnClick(const OnClickSlotType & slot)
{
  return onClick.connect(slot);
}

void Button::simulateClick()
{
  onClick(52, 38);
}

void printCoordinates(long x, long y)
{
  std::cout << "(" << x << ", " << y << ")\n";
}
// ------------------------------------------------

void append_cr() { std::cout << std::endl; }

int main() {
    boost::signals2::signal<void ()> sig_void;
    WithOutParam wop;
    sig_void.connect(wop);
    sig_void();
    append_cr();

// 传递参数给slot
    boost::signals2::signal<void (int, int)> sig_args;
    sig_args.connect(&PassArgs);
    sig_args(66, 88);
    append_cr();

// 获得slot的返回值
    boost::signals2::signal<int (int, int)> sig_return;
    sig_return.connect(CallbackWithReturn);
    boost::optional<int> ret = sig_return(5, 3);
    std::cout << "call slot with return : " << *ret << std::endl;
    append_cr();

// 返回所有slot的值
    boost::signals2::signal<int (int, int), Aggregate<std::vector<int>>> sig_returns;
    sig_returns.connect(algo_add);
    sig_returns.connect(algo_sub);
    sig_returns.connect(&algo_mul);
    boost::signals2::connection cdiv = sig_returns.connect(algo_div);

    std::vector<int> results = sig_returns(5, 3);
    std::stringstream ss;
    ss << "MultiReturns : ";
    std::copy(results.begin(), results.end(), std::experimental::make_ostream_joiner(ss, ", "));
    std::cout << ss.str() << std::endl;
    append_cr();

// connection管理，断开algo_mul和algo_div链接
// 也可以使用boost::signals2::scoped_connection作自动管理，　
    sig_returns.disconnect(&algo_mul);
    cdiv.disconnect();

    results = sig_returns(5, 3);
    ss.str(""); // 清空stringstream中内容，clear()方法用来清除错误标志
    ss << "MultiReturns with disconnect mul and div: ";
    std::copy(results.begin(), results.end(), std::experimental::make_ostream_joiner(ss, ", "));
    std::cout << ss.str() << std::endl;
    append_cr();

// 封装到一个class中使用signal/slot
    Button button;
    button.doOnClick(&printCoordinates);
    button.simulateClick(); // 模拟触发signal

    return 0;
}
