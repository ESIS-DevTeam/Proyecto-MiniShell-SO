#include "builtins.h"
#include "shell.h"
#include "utils.h"
#include <iostream>
#include <unistd.h>
#include <limits.h>
#include <cstring>
#include <pthread.h>
#include <sys/resource.h>

BuiltinCommands::BuiltinCommands(Shell* sh) : shell(sh) {
    // Registrar comandos built-in
    commands["cd"] = [this](const std::vector<std::string>& args) { return cd(args); };
    commands["pwd"] = [this](const std::vector<std::string>& args) { return pwd(args); };
    commands["help"] = [this](const std::vector<std::string>& args) { return help(args); };
    commands["history"] = [this](const std::vector<std::string>& args) { return history(args); };
    commands["salir"] = [this](const std::vector<std::string>& args) { return exit(args); };
    commands["exit"] = [this](const std::vector<std::string>& args) { return exit(args); };
    commands["echo"] = [this](const std::vector<std::string>& args) { return echo(args); };
    commands["meminfo"] = [this](const std::vector<std::string>& args) { return meminfo(args); };
}

BuiltinCommands::~BuiltinCommands() {
    // Limpieza si es necesaria
}

bool BuiltinCommands::isBuiltin(const std::string& command) const {
    return commands.find(command) != commands.end();
}

int BuiltinCommands::execute(const std::string& command, const std::vector<std::string>& args) {
    auto it = commands.find(command);
    if (it != commands.end()) {
        return it->second(args);
    }
    return -1;
}

int BuiltinCommands::cd(const std::vector<std::string>& args) {
    std::string path;
    
    if (args.size() < 2) {
        // Sin argumentos, ir a HOME
        const char* home = getenv("HOME");
        if (home == nullptr) {
            Utils::printError("Variable HOME no definida");
            return 1;
        }
        path = home;
    } else {
        path = args[1];
    }
    
    if (chdir(path.c_str()) != 0) {
        Utils::printSystemError("cd: " + path);
        return 1;
    }
    
    return 0;
}

int BuiltinCommands::pwd(const std::vector<std::string>& args) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << cwd << std::endl;
        return 0;
    } else {
        Utils::printSystemError("pwd");
        return 1;
    }
}

int BuiltinCommands::help(const std::vector<std::string>& args) {
    std::cout << COLOR_CYAN << COLOR_BOLD;
    std::cout << "\n═══════════════════════════════════════════════\n";
    std::cout << "          MINI-SHELL - AYUDA\n";
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << COLOR_RESET;
    
    std::cout << "\n" << COLOR_YELLOW << "Comandos Internos:" << COLOR_RESET << "\n";
    std::cout << "  cd [dir]       - Cambiar directorio\n";
    std::cout << "  pwd            - Mostrar directorio actual\n";
    std::cout << "  history        - Mostrar historial de comandos\n";
    std::cout << "  help           - Mostrar esta ayuda\n";
    std::cout << "  echo [texto]   - Imprimir texto\n";
    std::cout << "  meminfo        - Información de memoria\n";
    std::cout << "  salir/exit     - Salir del shell\n";
    
    std::cout << "\n" << COLOR_YELLOW << "Operadores:" << COLOR_RESET << "\n";
    std::cout << "  >              - Redirección de salida (sobrescribir)\n";
    std::cout << "  >>             - Redirección de salida (append)\n";
    std::cout << "  <              - Redirección de entrada\n";
    std::cout << "  |              - Pipe (conectar comandos)\n";
    std::cout << "  &              - Ejecutar en background\n";
    
    std::cout << "\n" << COLOR_YELLOW << "Ejemplos:" << COLOR_RESET << "\n";
    std::cout << "  ls -la\n";
    std::cout << "  ls > archivo.txt\n";
    std::cout << "  cat < entrada.txt\n";
    std::cout << "  ls | grep txt\n";
    std::cout << "  sleep 10 &\n";
    std::cout << "  /usr/bin/gcc --version\n";
    
    std::cout << "\n" << COLOR_GREEN << "Rutas:" << COLOR_RESET << "\n";
    std::cout << "  - Rutas absolutas: /ruta/completa/programa\n";
    std::cout << "  - Rutas relativas: se busca en /bin y /usr/bin\n";
    
    std::cout << "\n";
    return 0;
}

int BuiltinCommands::history(const std::vector<std::string>& args) {
    std::cout << COLOR_CYAN << "Historial de comandos:" << COLOR_RESET << "\n";
    if (shell == nullptr) {
        std::cout << "(No hay acceso al shell)\n";
        return 1;
    }

    const std::vector<std::string>& hist = shell->getHistory();
    if (hist.empty()) {
        std::cout << "(Historial vacío)\n";
        return 0;
    }

    for (size_t i = 0; i < hist.size(); ++i) {
        std::cout << i + 1 << ": " << hist[i] << std::endl;
    }

    return 0;
}

int BuiltinCommands::exit(const std::vector<std::string>& args) {
    std::cout << COLOR_YELLOW << "Saliendo del shell..." << COLOR_RESET << std::endl;
    shell->stop();
    return 0;
}

int BuiltinCommands::echo(const std::vector<std::string>& args) {
    for (size_t i = 1; i < args.size(); i++) {
        std::cout << args[i];
        if (i < args.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    return 0;
}

int BuiltinCommands::meminfo(const std::vector<std::string>& args) {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        std::cout << COLOR_CYAN << "Información de Memoria:" << COLOR_RESET << "\n";
        std::cout << "  Uso máximo de memoria: " << usage.ru_maxrss << " KB\n";
        std::cout << "  Fallos de página menores: " << usage.ru_minflt << "\n";
        std::cout << "  Fallos de página mayores: " << usage.ru_majflt << "\n";
        std::cout << "  Cambios de contexto voluntarios: " << usage.ru_nvcsw << "\n";
        std::cout << "  Cambios de contexto involuntarios: " << usage.ru_nivcsw << "\n";
    } else {
        Utils::printSystemError("getrusage");
        return 1;
    }
    
    return 0;
}

// Extensión: Comando parallel (ejemplo básico)
int BuiltinCommands::parallel(const std::vector<std::string>& args) {
    std::cout << COLOR_YELLOW << "Comando 'parallel' no implementado aún" << COLOR_RESET << std::endl;
    std::cout << "Extensión futura: ejecutar comandos en paralelo con pthread" << std::endl;
    return 0;
}