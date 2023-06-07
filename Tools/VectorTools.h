#include <vector>
#include <algorithm>

template<typename T> inline int getIndex(std::vector<T> v, T K)
{
    auto it = std::find(v.begin(), v.end(), K);
 
    // If element was found
    if (it != v.end())
    {
        return it - v.begin();
    }
    else {
        return -1;
    }
}