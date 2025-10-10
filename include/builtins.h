#ifndef BUILTINS_H
#define BUILTINS_H

#include <string>
#include <vector>
#include <functional>
#include <map>

class Shell; // Forward declaration

class BuiltinCommands {
private:
    Shell* shell;
    std::map<std::string, std::function<int(const std::vector<std::string>&)>> commands;
    
public:
    BuiltinCommands(Shell* sh);
    ~BuiltinCommands();
    
    bool isBuiltin(const std::string& command) const;
    int execute(const std::string& command, const std::vector<std::string>& args);
    
    // Comandos built-in específicos
    int cd(const std::vector<std::string>& args);
    int pwd(const std::vector<std::string>& args);
    int help(const std::vector<std::string>& args);
    int history(const std::vector<std::string>& args);
    int exit(const std::vector<std::string>& args);
    int alias(const std::vector<std::string>& args);
    int echo(const std::vector<std::string>& args);
    int export_cmd(const std::vector<std::string>& args);
    
    // Para extensiones de concurrencia
    int parallel(const std::vector<std::string>& args);
    int meminfo(const std::vector<std::string>& args);
};

#endif // BUILTINS_H