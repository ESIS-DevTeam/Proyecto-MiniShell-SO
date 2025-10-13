#include "executor.h"
#include "utils.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

int Executor::execute(const ParsedCommand& parsedCmd) {
    if (parsedCmd.commands.empty()) {
        return -1;
    }
    
    // Detectar si hay pipes
    bool hasPipes = false;
    for (const auto& cmd : parsedCmd.commands) {
        if (cmd.type == PIPE_COMMAND || parsedCmd.commands.size() > 1) {
            hasPipes = true;
            break;
        }
    }
    
    if (hasPipes) {
        return executePipe(parsedCmd.commands);
    } else {
        return executeSimple(parsedCmd.commands[0]);
    }
}

int Executor::executeSimple(const Command& cmd) {
    if (cmd.args.empty()) {
        return -1;
    }
    
    // Resolver ruta del comando
    std::string path = resolvePath(cmd.args[0]);
    
    if (path.empty()) {
        Utils::printError("Comando no encontrado: " + cmd.args[0]);
        return -1;
    }
    
    // Crear proceso hijo
    pid_t pid = fork();
    
    if (pid < 0) {
        Utils::printSystemError("Error en fork");
        return -1;
    }
    
    if (pid == 0) {
        // Proceso hijo
        
        // Configurar redirecciones si existen
        setupRedirection(cmd);
        
        // Preparar argumentos para execv
        char** argv = vectorToCharArray(cmd.args);
        
        // Ejecutar comando
        execv(path.c_str(), argv);
        
        // Si llegamos aquí, exec falló
        Utils::printSystemError("Error al ejecutar " + cmd.args[0]);
        freeCharArray(argv, cmd.args.size());
        exit(1);
    } else {
        // Proceso padre
        return handleParentProcess(pid, cmd.background);
    }
}

int Executor::executePipe(const std::vector<Command>& commands) {
    int numCmds = commands.size();
    int pipes[numCmds - 1][2];
    pid_t pids[numCmds];
    
    // Crear todos los pipes
    for (int i = 0; i < numCmds - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            Utils::printSystemError("Error creando pipe");
            return -1;
        }
    }
    
    // Crear procesos para cada comando
    for (int i = 0; i < numCmds; i++) {
        pids[i] = fork();
        
        if (pids[i] < 0) {
            Utils::printSystemError("Error en fork");
            return -1;
        }
        
        if (pids[i] == 0) {
            // Proceso hijo
            
            // Redirigir stdin del pipe anterior (excepto primer comando)
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            
            // Redirigir stdout al siguiente pipe (excepto último comando)
            if (i < numCmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            // Cerrar todos los descriptores de pipes
            for (int j = 0; j < numCmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            // Configurar redirecciones adicionales
            setupRedirection(commands[i]);
            
            // Resolver y ejecutar comando
            std::string path = resolvePath(commands[i].args[0]);
            if (path.empty()) {
                std::cerr << "Comando no encontrado: " << commands[i].args[0] << std::endl;
                exit(1);
            }
            
            char** argv = vectorToCharArray(commands[i].args);
            execv(path.c_str(), argv);
            
            // Si llegamos aquí, exec falló
            perror("execv");
            exit(1);
        }
    }
    
    // Proceso padre: cerrar todos los pipes
    for (int i = 0; i < numCmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Esperar a todos los hijos
    int status;
    int lastStatus = 0;
    for (int i = 0; i < numCmds; i++) {
        waitpid(pids[i], &status, 0);
        if (i == numCmds - 1) {
            lastStatus = WEXITSTATUS(status);
        }
    }
    
    return lastStatus;
}

void Executor::setupRedirection(const Command& cmd) {
    // Redirección de entrada
    if (!cmd.inputFile.empty()) {
        int fd = open(cmd.inputFile.c_str(), O_RDONLY);
        if (fd < 0) {
            perror(("Error abriendo " + cmd.inputFile).c_str());
            exit(1);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    
    // Redirección de salida
    if (!cmd.outputFile.empty()) {
        int flags = O_WRONLY | O_CREAT;
        
        if (cmd.type == REDIRECT_OUTPUT_APPEND) {
            flags |= O_APPEND;
        } else {
            flags |= O_TRUNC;
        }
        
        int fd = open(cmd.outputFile.c_str(), flags, 0644);
        if (fd < 0) {
            perror(("Error abriendo " + cmd.outputFile).c_str());
            exit(1);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
}

std::string Executor::resolvePath(const std::string& command) {
    // Si es ruta absoluta, verificar existencia
    if (isAbsolutePath(command)) {
        if (fileExists(command)) {
            return command;
        }
        return "";
    }
    
    // Buscar en /bin
    std::string binPath = "/bin/" + command;
    if (fileExists(binPath)) {
        return binPath;
    }
    
    // Buscar en /usr/bin
    std::string usrBinPath = "/usr/bin/" + command;
    if (fileExists(usrBinPath)) {
        return usrBinPath;
    }
    
    return "";
}

bool Executor::isAbsolutePath(const std::string& path) {
    return !path.empty() && path[0] == '/';
}

bool Executor::fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0) && (buffer.st_mode & S_IXUSR);
}

char** Executor::vectorToCharArray(const std::vector<std::string>& vec) {
    char** arr = new char*[vec.size() + 1];
    
    for (size_t i = 0; i < vec.size(); i++) {
        arr[i] = new char[vec[i].length() + 1];
        strcpy(arr[i], vec[i].c_str());
    }
    
    arr[vec.size()] = nullptr;
    return arr;
}

void Executor::freeCharArray(char** arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        delete[] arr[i];
    }
    delete[] arr;
}

int Executor::handleParentProcess(pid_t childPid, bool background) {
    if (background) {
        std::cout << "[Proceso en background] PID: " << childPid << std::endl;
        return 0;
    } else {
        int status;
        waitpid(childPid, &status, 0);
        return WEXITSTATUS(status);
    }
}