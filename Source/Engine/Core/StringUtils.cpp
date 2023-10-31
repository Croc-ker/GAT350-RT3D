#include "StringUtils.h"

std::string nc::StringUtils::ToUpper(const std::string& str)
{
    std::string result = str;
    for (char& c : result) {
        if (c >= 'a' && c <= 'z') {
            c -= ('a' - 'A');
        }
    }
    return result;
}

std::string nc::StringUtils::ToLower(const std::string& str)
{
    std::string result = str;
    for (char& c : result) {
        if (c >= 'A' && c <= 'Z') {
            c += ('a' - 'A');
        }
    }
    return result;
}

bool nc::StringUtils::IsEqualIgnoreCase(const std::string string1, const std::string string2)
{
    return (ToLower(string1) == ToLower(string2));
}

std::string nc::StringUtils::CreateUnique(const std::string& input)
{
    std::string str;
    static unsigned int counter = 0;
    str = input + std::to_string(counter);
    counter++;
    return str;
}
