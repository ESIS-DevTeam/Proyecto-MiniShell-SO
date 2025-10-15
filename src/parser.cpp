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
    char quoteChar = '\0';

    for (size_t i = 0; i < input.length(); ) {
        char c = input[i];

        // Manejar escapes: \x -> x
        if (c == '\\') {
            if (i + 1 < input.length()) {
                current += input[i+1];
                i += 2;
                continue;
            } else {
                // Trailing backslash, ignóralo
                i++;
                continue;
            }
        }

        // Entrar/salir de comillas
        if (!inQuotes && (c == '"' || c == '\'')) {
            inQuotes = true;
            quoteChar = c;
            i++;
            continue;
        } else if (inQuotes && c == quoteChar) {
            inQuotes = false;
            quoteChar = '\0';
            i++;
            continue;
        }

        if (inQuotes) {
            // Dentro de comillas, todo es literal (ya manejamos escapes)
            current += c;
            i++;
            continue;
        }

        // No en comillas: manejar operadores pegados y espacios
        // Detectar >> primero
        if (i + 1 < input.length() && input[i] == '>' && input[i+1] == '>') {
            if (!current.empty()) { tokens.push_back(current); current.clear(); }
            tokens.push_back(">>");
            i += 2;
            continue;
        }

        // Operadores de un solo caracter
        if (c == '|' || c == '>' || c == '<' || c == '&') {
            if (!current.empty()) { tokens.push_back(current); current.clear(); }
            std::string op(1, c);
            tokens.push_back(op);
            i++;
            continue;
        }

        // Separador
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!current.empty()) { tokens.push_back(current); current.clear(); }
            i++;
            continue;
        }

        // Caracter normal
        current += c;
        i++;
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