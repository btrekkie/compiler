#include "DirHandle.hpp"

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
