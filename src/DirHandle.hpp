#ifndef DIR_HANDLE_HPP_INCLUDED
#define DIR_HANDLE_HPP_INCLUDED

#include <dirent.h>
#include <string>

/**
 * A handle allowing iteration over the files in a directory.
 */
class DirHandle {
private:
    /**
     * The DIR object we are using for iteration.
     */
    DIR* dir;
public:
    ~DirHandle();
    /**
     * Returns a new DirHandle for the directory with the specified filename.
     * Returns NULL if there is no such directory.
     */
    static DirHandle* fromDir(std::string dirName);
    /**
     * Returns the name of the next subfile, or "" if there are no more
     * subfiles.  The name does not include the directory name; it is just the
     * bare, relative filename.  This method does will return "." or "..".
     */
    std::string getNextFilename();
};

#endif
