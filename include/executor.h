#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"
#include <string>
#include <vector>
#include <unistd.h>

class Executor {
public:
    static int execute(const ParsedCommand& parsedCmd);
    static int executeSimple(const Command& cmd);
    static int executePipe(const std::vector<Command>& commands);
    static int executeWithRedirection(const Command& cmd);
    
private:
    static std::string resolvePath(const std::string& command);
    static bool isAbsolutePath(const std::string& path);
    static bool fileExists(const std::string& path);
    static char** vectorToCharArray(const std::vector<std::string>& vec);
    static void freeCharArray(char** arr, size_t size);
    static void setupRedirection(const Command& cmd);
    static void handleChildProcess(const Command& cmd);
    static int handleParentProcess(pid_t childPid, bool background);
    static void handleError(const std::string& msg);
};

#endif // EXECUTOR_H