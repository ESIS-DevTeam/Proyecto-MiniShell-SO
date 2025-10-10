#ifndef CONFIG_H
#define CONFIG_H

// Configuración del shell
namespace Config {
    constexpr int MAX_COMMAND_LENGTH = 4096;
    constexpr int MAX_ARGS = 256;
    constexpr int MAX_PATH_LENGTH = 1024;
    constexpr int MAX_PIPES = 10;
    constexpr int MAX_BACKGROUND_JOBS = 100;
    
    constexpr const char* DEFAULT_PATH = "/bin:/usr/bin:/usr/local/bin";
    constexpr const char* HISTORY_FILE = ".mini_shell_history";
    constexpr const char* CONFIG_FILE = ".mini_shell_rc";
    
    constexpr const char* SHELL_NAME = "mini-shell";
    constexpr const char* VERSION = "1.0.0";
}

#endif // CONFIG_H
// Opcional de usar