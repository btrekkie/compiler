#include <assert.h>
#include <sstream>
#include "StringUtil.hpp"

using namespace std;

string StringUtil::asciiWstringToString(wstring wstr) {
    ostringstream str;
    for (int i = 0; i < (int)wstr.length(); i++) {
        wchar_t c = wstr[i];
        assert((c >= 0 && c < 256) || !L"Not an ASCII string");
        str << (char)c;
    }
    return str.str();
}

wstring StringUtil::stringToWstring(string str) {
    wstring wstr(str.begin(), str.end());
    return wstr;
}
