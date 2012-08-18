#ifndef STRING_UTIL_HPP_INCLUDED
#define STRING_UTIL_HPP_INCLUDED

#include <string>

/**
 * Provides static utility methods pertaining to strings.
 */
class StringUtil {
public:
    /**
     * Returns the string equivalent of the specified wstring, which must
     * consist exclusively of ASCII characters.
     */
    static std::string asciiWstringToString(std::wstring wstr);
    /**
     * Returns the wstring equivalent of the specified string.
     */
    static std::wstring stringToWstring(std::string str);
};

#endif
