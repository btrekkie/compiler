#include <sys/types.h>
#include <algorithm>
#include <fstream>
#include <signal.h>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include "FileManager.hpp"
#include "Process.hpp"

using namespace std;

Process::Process(std::string command2) {
    command = command2;
    timeout = -1;
    maxStdOutCaptureBytes = -1;
}

void Process::setTimeout(double timeout2) {
    timeout = timeout2;
}

void Process::setMaxStdOutCaptureBytes(int maxStdOutCaptureBytes2) {
    maxStdOutCaptureBytes = maxStdOutCaptureBytes2;
}

/* This is not the greatest implementation, due to the use of intermediate files
 * and the alteration of the original command.  I didn't want to have to deal
 * with pipes and sockets.
 */
void Process::run() {
    string outputFilename = FileManager::getTempFilename();
    ofstream output(outputFilename.c_str());
    output.close();
    string finishedFilename = FileManager::getTempFilename();
    
    pid_t processID = fork();
    if (processID == 0) {
        ostringstream fullCommand;
        fullCommand << command;
        if (maxStdOutCaptureBytes >= 0)
            fullCommand << " | head -c " << maxStdOutCaptureBytes;
        fullCommand << " > '" << outputFilename << '\'';
        system(fullCommand.str().c_str());
        ofstream output2(finishedFilename.c_str());
        output2.close();
        exit(0);
    }
    if (timeout > 0) {
        clock_t startTime = clock();
        clock_t endTime = (clock_t)(startTime + timeout * CLOCKS_PER_SEC);
        double sleepTimeSecs = 0.002;
        double timeLeft = timeout;
        while (timeLeft > 0 &&
               !FileManager::doesFileExist(finishedFilename)) {
            usleep((int)(1000000 * min(timeLeft, sleepTimeSecs)));
            if (timeLeft <= sleepTimeSecs)
                timeLeft = 0;
            else
                timeLeft = (double)(endTime - clock()) / CLOCKS_PER_SEC;
            sleepTimeSecs = min(0.3, 1.5 * sleepTimeSecs);
        }
        if (timeLeft <= 0) {
            kill(processID, SIGTERM);
            // TODO we should use SIGKILL if the process is stubborn
            timedOut = true;
        }
    }
    wait(NULL);
    ifstream input(outputFilename.c_str());
    ostringstream outputStr;
    outputStr << input.rdbuf();
    stdOut = outputStr.str();
    remove(finishedFilename.c_str());
    remove(outputFilename.c_str());
}

bool Process::getTimedOut() {
    return timedOut;
}

string Process::getStdOut() {
    return stdOut;
}
