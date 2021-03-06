#ifndef FILE_MANAGER_HPP_INCLUDED
#define FILE_MANAGER_HPP_INCLUDED

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
    static DirHandle* fromDir(std::wstring dirName);
    /**
     * Returns the name of the next subfile, or L"" if there are no more
     * subfiles.  The name does not include the directory name; it is just the
     * bare, relative filename.  This method does not return L"." or L"..".
     */
    std::wstring getNextFilename();
};

/**
 * Provides functionality pertaining to files.
 */
class FileManager {
public:
    /**
     * Returns whether there is a file with the specified filename.
     */
    static bool doesFileExist(std::wstring filename);
    /**
     * Returns the name of a file that does not presently exist to which the
     * program may write.
     */
    static std::wstring getTempFilename();
    /**
     * Returns the full name of the directory in which the specified file
     * resides.  The behavior is unspecified if this information is cannot be
     * determined from the filename alone (e.g. "filename" is L"../") or if the
     * file has no parent directory.
     */
    static std::wstring getParentDir(std::wstring filename);
    /**
     * Returns the number of bytes in the specified file.  Returns -1 if there
     * was an error obtaining this information.
     */
    static int getSize(std::wstring filename);
};

#endif
