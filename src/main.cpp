#include "shell.h"
#include "utils.h"
#include <iostream>
#include <csignal>

int main(int argc, char* argv[]) {
    // Configurar manejo de señales
    Utils::setupSignalHandlers();
    
    // Banner de bienvenida
    std::cout << COLOR_CYAN << COLOR_BOLD;
    std::cout << "╔════════════════════════════════════════╗\n";
        std::cout << "║      Mini-Shell-ESIS v1.0.0            ║\n";
    std::cout << "║   Sistema Operativos - UNJBG           ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";
    std::cout << COLOR_RESET << std::endl;
    
    // Crear e iniciar el shell
    Shell shell;
    shell.run();
    
    // Mensaje de despedida
    std::cout << COLOR_GREEN << "¡Hasta pronto!" << COLOR_RESET << std::endl;
    
    return 0;
}