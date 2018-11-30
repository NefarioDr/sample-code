/*
Applies lvalue-to-rvalue, array-to-pointer, and function-to-pointer implicit conversions to the type T, removes cv-qualifiers, and defines the resulting type as the member typedef type. Formally:

If T names the type "array of U" or "reference to array of U", the member typedef type is U*.
Otherwise, if T is a function type F or a reference thereto, the member typedef type is std::add_pointer<F>::type.
Otherwise, the member typedef type is std::remove_cv<std::remove_reference<T>::type>::type.
These conversions model the type conversion applied to all function arguments when passed by value.
*/
#include <iostream>
#include <type_traits>
 
template <typename T, typename U>
struct decay_equiv : 
    std::is_same<typename std::decay<T>::type, U>::type 
{};
 
int main()
{
    std::cout << std::boolalpha
              << decay_equiv<int, int>::value << '\n'
              << decay_equiv<int&, int>::value << '\n'
              << decay_equiv<int&&, int>::value << '\n'
              << decay_equiv<const int&, int>::value << '\n'
              << decay_equiv<int[2], int*>::value << '\n'
              << decay_equiv<int(int), int(*)(int)>::value << '\n';
}
