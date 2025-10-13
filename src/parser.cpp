#include "parser.h"
#include "utils.h"
#include <sstream>
#include <algorithm>

ParsedCommand Parser::parse(const std::string& input) {
    ParsedCommand result;
    
    if (input.empty()) {
        result.isValid = false;
        result.errorMsg = "Comando vacío";
        return result;
    }
    
    // Tokenizar entrada
    std::vector<std::string> tokens = tokenize(input);
    
    if (tokens.empty()) {
        result.isValid = false;
        result.errorMsg = "No se pudo parsear el comando";
        return result;
    }
    
    // Procesar tokens
    processTokens(tokens, result);
    
    return result;
}

std::vector<std::string> Parser::tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;
    
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        
        if (c == '"' || c == '\'') {
            inQuotes = !inQuotes;
            continue;
        }
        
        if (!inQuotes && (c == ' ' || c == '\t')) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

void Parser::processTokens(const std::vector<std::string>& tokens, ParsedCommand& result) {
    Command currentCmd;
    size_t i = 0;
    
    while (i < tokens.size()) {
        const std::string& token = tokens[i];
        
        // Detectar pipe
        if (isPipeOperator(token)) {
            if (currentCmd.args.empty()) {
                result.isValid = false;
                result.errorMsg = "Sintaxis inválida: pipe sin comando previo";
                return;
            }
            result.commands.push_back(currentCmd);
            currentCmd = Command();
            currentCmd.type = PIPE_COMMAND;
            i++;
            continue;
        }
        
        // Detectar redirección de salida
        if (token == ">") {
            if (i + 1 >= tokens.size()) {
                result.isValid = false;
                result.errorMsg = "Sintaxis inválida: falta archivo después de '>'";
                return;
            }
            currentCmd.type = REDIRECT_OUTPUT;
            currentCmd.outputFile = tokens[++i];
            i++;
            continue;
        }
        
        // Detectar redirección de salida con append
        if (token == ">>") {
            if (i + 1 >= tokens.size()) {
                result.isValid = false;
                result.errorMsg = "Sintaxis inválida: falta archivo después de '>>'";
                return;
            }
            currentCmd.type = REDIRECT_OUTPUT_APPEND;
            currentCmd.outputFile = tokens[++i];
            i++;
            continue;
        }
        
        // Detectar redirección de entrada
        if (token == "<") {
            if (i + 1 >= tokens.size()) {
                result.isValid = false;
                result.errorMsg = "Sintaxis inválida: falta archivo después de '<'";
                return;
            }
            currentCmd.type = REDIRECT_INPUT;
            currentCmd.inputFile = tokens[++i];
            i++;
            continue;
        }
        
        // Detectar background
        if (isBackgroundOperator(token)) {
            currentCmd.background = true;
            i++;
            continue;
        }
        
        // Argumento normal
        currentCmd.args.push_back(token);
        i++;
    }
    
    // Agregar último comando
    if (!currentCmd.args.empty()) {
        result.commands.push_back(currentCmd);
    }
    
    // Validar que haya al menos un comando
    if (result.commands.empty()) {
        result.isValid = false;
        result.errorMsg = "No se encontró ningún comando válido";
    }
}

bool Parser::isRedirectOperator(const std::string& token) {
    return token == ">" || token == ">>" || token == "<";
}

bool Parser::isPipeOperator(const std::string& token) {
    return token == "|";
}

bool Parser::isBackgroundOperator(const std::string& token) {
    return token == "&";
}