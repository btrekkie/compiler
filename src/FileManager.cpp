#include <fstream>
#include <sys/stat.h>
#include "FileManager.hpp"
#include "StringUtil.hpp"

using namespace std;

DirHandle::~DirHandle() {
    closedir(dir);
}

DirHandle* DirHandle::fromDir(wstring dirName) {
    DIR* dir2 = opendir(StringUtil::asciiWstringToString(dirName).c_str());
    if (dir2 == NULL)
        return NULL;
    else {
        DirHandle* handle = new DirHandle();
        handle->dir = dir2;
        return handle;
    }
}

wstring DirHandle::getNextFilename() {
    struct dirent* dir2 = readdir(dir);
    while (dir2 != NULL &&
           (string(dir2->d_name) == "." || string(dir2->d_name) == ".."))
        dir2 = readdir(dir);
    if (dir2 != NULL)
        return StringUtil::stringToWstring(dir2->d_name);
    else
        return L"";
}

bool FileManager::doesFileExist(wstring filename) {
    ifstream input(StringUtil::asciiWstringToString(filename).c_str());
    return input;
}

wstring FileManager::getTempFilename() {
    return StringUtil::stringToWstring(tmpnam(NULL));
}

wstring FileManager::getParentDir(wstring filename) {
    int pos = (int)filename.rfind(L'/', filename.length() - 1);
    if (pos != (int)wstring::npos)
        return filename.substr(0, pos);
    else
        return L"";
}

int FileManager::getSize(wstring filename) {
    struct stat results;
    if (stat(StringUtil::asciiWstringToString(filename).c_str(), &results) == 0)
        return results.st_size;
    else
        return -1;
}
