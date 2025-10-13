#include "utils.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <sys/wait.h>
#include <cstring>
#include <cerrno>

namespace Utils {
    // Variables globales para tracking de memoria
    static size_t g_totalAllocations = 0;
    static size_t g_totalDeallocations = 0;
    static size_t g_currentUsage = 0;
    static size_t g_peakUsage = 0;
    
    // Handler para SIGINT (Ctrl+C)
    void sigintHandler(int sig) {
        std::cout << "\n" << COLOR_YELLOW 
                  << "Presiona Ctrl+D o escribe 'salir' para terminar" 
                  << COLOR_RESET << std::endl;
        // No salir del shell, solo informar
    }
    
    // Handler para SIGCHLD (procesos hijos terminados)
    void sigchldHandler(int sig) {
        // Recolectar procesos zombies
        while (waitpid(-1, nullptr, WNOHANG) > 0);
    }
    
    void setupSignalHandlers() {
        struct sigaction sa_int;
        struct sigaction sa_chld;
        
        // Configurar handler para SIGINT
        sa_int.sa_handler = sigintHandler;
        sigemptyset(&sa_int.sa_mask);
        sa_int.sa_flags = SA_RESTART;
        sigaction(SIGINT, &sa_int, nullptr);
        
        // Configurar handler para SIGCHLD
        sa_chld.sa_handler = sigchldHandler;
        sigemptyset(&sa_chld.sa_mask);
        sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
        sigaction(SIGCHLD, &sa_chld, nullptr);
    }
    
    void reapBackgroundProcesses() {
        int status;
        pid_t pid;
        
        // Recolectar todos los procesos terminados sin bloquear
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            if (WIFEXITED(status)) {
                std::cout << COLOR_GREEN << "[Proceso " << pid 
                          << " terminado con código " << WEXITSTATUS(status) 
                          << "]" << COLOR_RESET << std::endl;
            }
        }
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) {
            return "";
        }
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        
        while (std::getline(ss, token, delimiter)) {
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        
        return tokens;
    }
    
    std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
        std::string result;
        for (size_t i = 0; i < vec.size(); i++) {
            result += vec[i];
            if (i < vec.size() - 1) {
                result += delimiter;
            }
        }
        return result;
    }
    
    std::string getCurrentDirectory() {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            std::string path(cwd);
            
            // Abreviar HOME con ~
            const char* home = getenv("HOME");
            if (home != nullptr) {
                std::string homeStr(home);
                if (path.find(homeStr) == 0) {
                    path = "~" + path.substr(homeStr.length());
                }
            }
            
            return path;
        }
        return "?";
    }
    
    std::string getUsername() {
        const char* user = getenv("USER");
        if (user != nullptr) {
            return std::string(user);
        }
        
        uid_t uid = getuid();
        struct passwd* pw = getpwuid(uid);
        if (pw != nullptr) {
            return std::string(pw->pw_name);
        }
        
        return "user";
    }
    
    std::string getHostname() {
        char hostname[HOST_NAME_MAX];
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            return std::string(hostname);
        }
        return "localhost";
    }
    
    void printError(const std::string& msg) {
        std::cerr << COLOR_RED << "Error: " << msg << COLOR_RESET << std::endl;
    }
    
    void printSystemError(const std::string& prefix) {
        std::cerr << COLOR_RED << prefix << ": " << strerror(errno) 
                  << COLOR_RESET << std::endl;
    }
    
    void trackAllocation(size_t size) {
        g_totalAllocations++;
        g_currentUsage += size;
        if (g_currentUsage > g_peakUsage) {
            g_peakUsage = g_currentUsage;
        }
    }
    
    void trackDeallocation(size_t size) {
        g_totalDeallocations++;
        if (g_currentUsage >= size) {
            g_currentUsage -= size;
        }
    }
    
    MemoryInfo getMemoryInfo() {
        MemoryInfo info;
        info.totalAllocations = g_totalAllocations;
        info.totalDeallocations = g_totalDeallocations;
        info.currentUsage = g_currentUsage;
        info.peakUsage = g_peakUsage;
        return info;
    }
}