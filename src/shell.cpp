#include "shell.h"
#include "parser.h"
#include "config.h"
#include "executor.h"
#include "builtins.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>

Shell::Shell() : running(true) {
    loadHistory();
}

Shell::~Shell() {
    saveHistory();
}

void Shell::run() {
    BuiltinCommands builtins(this);
    
    while (running) {
        // Recolectar procesos zombies en background
        Utils::reapBackgroundProcesses();
        
        // Mostrar prompt y leer entrada
        displayPrompt();
        std::string input = readInput();
        
        // Saltar líneas vacías
        if (input.empty()) {
            continue;
        }
        
        // Agregar al historial
        addToHistory(input);
        
        // Parsear comando
        ParsedCommand parsedCmd = Parser::parse(input);
        
        if (!parsedCmd.isValid) {
            Utils::printError(parsedCmd.errorMsg);
            continue;
        }
        
        // Si hay comandos parseados
        if (!parsedCmd.commands.empty()) {
            const std::string& cmd = parsedCmd.commands[0].args[0];
            
            // Verificar si es built-in
            if (builtins.isBuiltin(cmd)) {
                builtins.execute(cmd, parsedCmd.commands[0].args);
            } else {
                // Ejecutar comando externo
                Executor::execute(parsedCmd);
            }
        }
    }
}

void Shell::displayPrompt() {
    std::string cwd = Utils::getCurrentDirectory();

    std::cout << COLOR_CYAN << "Mini-Shell-ESIS" << COLOR_RESET << ":"
              << COLOR_BLUE << cwd << COLOR_RESET
              << COLOR_YELLOW << " $ " << COLOR_RESET;
    std::cout.flush();
}

std::string Shell::readInput() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        // EOF detectado (Ctrl+D)
        running = false;
        std::cout << std::endl;
        return "";
    }
    return Utils::trim(line);
}

void Shell::addToHistory(const std::string& command) {
    if (command.empty()) {
        return;
    }

    if (!history.empty() && command == history.back()) {
        return;
    }
    
    history.push_back(command);
    
    // Mantener límite de historial
    if (history.size() > MAX_HISTORY) {
        history.erase(history.begin());
    }
}

void Shell::saveHistory() {
    std::ofstream file(Config::HISTORY_FILE);
    if (file.is_open()) {
        for (const auto& cmd : history) {
            file << cmd << "\n";
        }
        file.close();
    }
}

void Shell::loadHistory() {
    std::ifstream file(Config::HISTORY_FILE);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line) && history.size() < MAX_HISTORY) {
            if (!line.empty()) {
                history.push_back(line);
            }
        }
        file.close();
    }
}