#ifndef PROCESS_HPP_INCLUDED
#define PROCESS_HPP_INCLUDED

#include <string>

/**
 * A child process spawned by this program.
 */
class Process {
private:
    /**
     * The command this process is running.  At present, this must be the name
     * of an executable file (as opposed to something more elaborate, such as a
     * command with a pipe or with output redirection).
     */
    std::wstring command;
    /**
     * The maximum amount of time for which to run the process before
     * terminating it.  A value that is less than or equal to zero indicates
     * that we should run the process to completion.
     */
    double timeout;
    /**
     * The maximum number of bytes to read from the process's standard output
     * stream.  A negative number indicates that we should read every byte.
     */
    int maxStdOutCaptureBytes;
    /**
     * The process's standard output stream.  This is not set until run() is
     * called.
     */
    std::wstring stdOut;
    /**
     * Whether the process timed out.  This is not set until run() is called.
     * See the comments for "timeout" for more information.
     */
    bool timedOut;
public:
    Process(std::wstring command2);
    void setTimeout(double timeout2);
    void setMaxStdOutCaptureBytes(int maxStdOutCaptureBytes2);
    void run();
    bool getTimedOut();
    std::wstring getStdOut();
};

#endif
