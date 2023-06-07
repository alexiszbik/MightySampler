
#include <cstring>
#include <string>
#include <vector>

void tokenize(std::string const &str,
            std::vector<std::string> &out)
{
    size_t start;
    size_t end = 0;
    const char delim = ' ';
 
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);

        auto sub = str.substr(start, end - start);

        out.push_back(sub);
    }
}

bool strEquals(const char* str1, const char* str2) {
    return strcmp(str1, str2) == 0;
}