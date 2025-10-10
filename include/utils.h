#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <signal.h>

// Colores para el prompt
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"

// Utilidades generales
namespace Utils {
    // Manejo de strings
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    std::string join(const std::vector<std::string>& vec, const std::string& delimiter);
    
    // Sistema
    std::string getCurrentDirectory();
    std::string getUsername();
    std::string getHostname();
    
    // Manejo de señales
    void setupSignalHandlers();
    void sigintHandler(int sig);
    void sigchldHandler(int sig);
    
    // Procesos en background
    void reapBackgroundProcesses();
    
    // Manejo de errores
    void printError(const std::string& msg);
    void printSystemError(const std::string& prefix);
    
    // Memoria (para extensión meminfo)
    struct MemoryInfo {
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t currentUsage;
        size_t peakUsage;
    };
    
    MemoryInfo getMemoryInfo();
    void trackAllocation(size_t size);
    void trackDeallocation(size_t size);
}

// Macros para debugging (se pueden desactivar en release)
#ifdef DEBUG
    #define DEBUG_PRINT(x) std::cerr << "[DEBUG] " << x << std::endl
#else
    #define DEBUG_PRINT(x)
#endif

#endif // UTILS_H