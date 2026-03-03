#include "PythonAgent.h"
#include <cstdio>
#include <string>

using namespace std;

PythonAgent::PythonAgent(const string& key, const string& pyPath, const string& script) {
    apiKey = key;
    pythonPath = pyPath;
    scriptPath = script;
}

string PythonAgent::executeCommand(const string& command) {
    char buffer[4096];
    string result = "";

    // Redireciona stderr para evitar poluição de logs
    string fullCommand = command + " 2>nul";

    FILE* pipe = _popen(fullCommand.c_str(), "r");
    if (!pipe) {
        return "{\"status\":\"error\",\"response\":\"Erro ao executar comando Python\"}";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }

    _pclose(pipe);

    // Extrai o JSON
    size_t jsonStart = result.find("{");
    size_t jsonEnd = result.rfind("}");

    if (jsonStart != string::npos && jsonEnd != string::npos && jsonEnd > jsonStart) {
        result = result.substr(jsonStart, jsonEnd - jsonStart + 1);
    } else {
        return "{\"status\":\"error\",\"response\":\"Python nao retornou JSON valido\"}";
    }

    return result;
}

string PythonAgent::escapeString(const string& str) {
    string escaped = "";
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

string PythonAgent::sendMessage(const string& message) {
    string escapedMsg = escapeString(message);

    string command = 
        pythonPath + " " + scriptPath + 
        " \"" + apiKey + "\" send \"" + escapedMsg + "\"";

    return executeCommand(command);
}

string PythonAgent::clearHistory() {
    string command =
        pythonPath + " " + scriptPath +
        " \"" + apiKey + "\" clear";

    return executeCommand(command);
}

string PythonAgent::extractResponse(const string& jsonOutput) {

    size_t pos = jsonOutput.find("\"response\"");
    if (pos == string::npos) {
        return "Erro: campo 'response' nao encontrado no JSON";
    }

    pos = jsonOutput.find(":", pos);
    if (pos == string::npos) {
        return "Erro: formato JSON invalido";
    }

    pos = jsonOutput.find("\"", pos);
    if (pos == string::npos) {
        return "Erro: formato JSON invalido (aspas ausentes)";
    }

    pos++; // depois da aspa inicial

    size_t end = pos;
    bool escape = false;

    while (end < jsonOutput.size()) {
        if (!escape && jsonOutput[end] == '\\') {
            escape = true;
        }
        else if (!escape && jsonOutput[end] == '"') {
            break;
        }
        else {
            escape = false;
        }
        end++;
    }

    if (end >= jsonOutput.size()) {
        return "Erro ao encontrar fim da resposta";
    }

    string raw = jsonOutput.substr(pos, end - pos);

    // Desserializar escapes
    string processed = "";
    for (int i = 0; i < raw.size(); i++) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            char n = raw[i + 1];
            switch (n) {
                case 'n': processed += '\n'; break;
                case 't': processed += '\t'; break;
                case 'r': processed += '\r'; break;
                case '"': processed += '\"'; break;
                case '\\': processed += '\\'; break;
                default: processed += n; break;
            }
            i++;
        } else {
            processed += raw[i];
        }
    }

    return processed;
}

bool PythonAgent::isError(const string& jsonOutput) {
    return jsonOutput.find("\"status\":\"error\"") != string::npos;
}
