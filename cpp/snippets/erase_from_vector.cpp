#include <iostream>
#include <vector>
 
int main()
{
    // Create a vector containing integers
    std::vector<int> v = {7, 5, 5, 16, 5, 5, 8};
 
    for (auto it = v.begin(); it != v.end(); ++it) {
        if (*it == 5) v.erase(it);
    }
    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
 
    v = {5, 7, 5, 5, 16, 5, 5, 8, 5};
 
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
        if (*it == 5) v.erase(std::next(it).base());
    }

    std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << std::endl;
}
