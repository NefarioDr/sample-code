#include <memory>
#include <iostream>
// alias template, support by c++11 and GCC version 4.7+
// 给一个模板类起别名，这个特性在C++11中规定，在GCC4.7及以上版本中支持。
template<class T>
using SharedPtr = std::shared_ptr<T>;

//　给一个模板方法做一个封装，理论上只有类型才有别名，方法只能是重新封装。
template <typename T, typename... Args>
inline SharedPtr<T> MakeShared(Args&&... args)
{
    return std::make_shared<T>(args...);
}

class Point {
public:
    Point(int x, int y) : _x(x), _y(y) {
        std::cout<< "Point ++++  " << _x << std::endl;
    }

    ~Point() {
        std::cout << "Point --- " << _y << std::endl;
    }

private:
    int _x;
    int _y;
};

int main() {
    SharedPtr<Point> a = MakeShared<Point>(1, 2);
    SharedPtr<Point> b = std::make_shared<Point>(3, 4);
}
