#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

enum CommandType {
    SIMPLE_COMMAND,
    PIPE_COMMAND,
    REDIRECT_OUTPUT,
    REDIRECT_OUTPUT_APPEND,
    REDIRECT_INPUT,
    BACKGROUND_COMMAND
};

struct Command {
    std::vector<std::string> args;
    CommandType type;
    std::string inputFile;
    std::string outputFile;
    bool background;
    
    Command() : type(SIMPLE_COMMAND), background(false) {}
};

struct ParsedCommand {
    std::vector<Command> commands;  // Para manejar pipes
    bool isValid;
    std::string errorMsg;
    
    ParsedCommand() : isValid(true) {}
};

class Parser {
public:
    static ParsedCommand parse(const std::string& input);
    static std::vector<std::string> tokenize(const std::string& input);
    static bool isRedirectOperator(const std::string& token);
    static bool isPipeOperator(const std::string& token);
    static bool isBackgroundOperator(const std::string& token);
    
private:
    static void processTokens(const std::vector<std::string>& tokens, ParsedCommand& result);
};

#endif // PARSER_H