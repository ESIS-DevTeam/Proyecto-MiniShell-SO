#ifndef SHELL_H
#define SHELL_H

#include <string>
#include <vector>
#include <map>

class Shell {
private:
    bool running;
    std::vector<std::string> history;
    std::map<std::string, std::string> aliases;
    static constexpr int MAX_HISTORY = 100;
    
public:
    Shell();
    ~Shell();
    
    void run();
    void displayPrompt();
    std::string readInput();
    void addToHistory(const std::string& command);
    void saveHistory();
    void loadHistory();
    const std::vector<std::string>& getHistory() const { return history; }
    bool isRunning() const { return running; }
    void stop() { running = false; }
};

#endif // SHELL_H