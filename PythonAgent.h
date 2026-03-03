#ifndef PYTHONAGENT_H
#define PYTHONAGENT_H

#include <string>

class PythonAgent {
private:
    std::string apiKey;
    std::string pythonPath;
    std::string scriptPath;

    std::string executeCommand(const std::string& command);
    std::string escapeString(const std::string& str);

public:
    // Atualizado para receber 3 argumentos, igual ao .cpp e a main
    PythonAgent(const std::string& key, const std::string& pyPath, const std::string& script);

    std::string sendMessage(const std::string& message);
    std::string clearHistory();
    std::string extractResponse(const std::string& jsonOutput);
    bool isError(const std::string& jsonOutput);
};

#endif // PYTHONAGENT_H