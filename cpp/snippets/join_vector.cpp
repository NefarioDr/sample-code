#include <algorithm>
#include <experimental/iterator>
#include <iostream>
#include <vector>
#include <sstream>

/**
 * the std::experimental is defined from c++14, so you must specific your compiler with '-std=c++14',
 * or it will throw an error: no member named 'experimental' in namespace 'std'.
*/

int main() {
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};

    std::stringstream ss;
    std::copy(v.begin(), v.end(), std::experimental::make_ostream_joiner(ss, ", "));    
    
    std::cout << ss.str() << std::endl;
    return 0;
}
