#include <fstream>
#include <sys/stat.h>
#include "FileManager.hpp"

using namespace std;

DirHandle::~DirHandle() {
    closedir(dir);
}

DirHandle* DirHandle::fromDir(string dirName) {
    DIR* dir2 = opendir(dirName.c_str());
    if (dir2 == NULL)
        return NULL;
    else {
        DirHandle* handle = new DirHandle();
        handle->dir = dir2;
        return handle;
    }
}

string DirHandle::getNextFilename() {
    struct dirent* dir2 = readdir(dir);
    while (dir2 != NULL &&
           (string(dir2->d_name) == "." || string(dir2->d_name) == ".."))
        dir2 = readdir(dir);
    if (dir2 != NULL)
        return dir2->d_name;
    else
        return "";
}

bool FileManager::doesFileExist(string filename) {
    ifstream input(filename.c_str());
    return input;
}

string FileManager::getTempFilename() {
    return tmpnam(NULL);
}

string FileManager::getParentDir(string filename) {
    int pos = (int)filename.rfind('/', filename.length() - 1);
    if (pos != (int)string::npos)
        return filename.substr(0, pos);
    else
        return "";
}

int FileManager::getSize(string filename) {
    struct stat results;
    if (stat(filename.c_str(), &results) == 0)
        return results.st_size;
    else
        return -1;
}
