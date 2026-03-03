#include <iostream>     
#include <string>      
#include <cstdlib>     
#include <windows.h>    // API do Windows 
#include "PythonAgent.h" // Nossa integração

using namespace std;

// Constante global para a chave da API
const string GEMINI_API_KEY = "AIzaSyBHUhg6XRGJcnvX4izCdPcYT-qa9bwMbkM";

// Obrigando o chatbot a usar UTF-8 para garantir que acentos sejam renderizados corretamente
void setupConsole() {
    // Define a saída (cout) para UTF-8
    SetConsoleOutputCP(65001);
    
    // Define a entrada (cin) para UTF-8
    SetConsoleCP(65001);
    
    // Ajusta o locale da biblioteca C para compatibilidade
    setlocale(LC_ALL, ".UTF8"); 
}

// "Frontend" do projeto, para o usuário identificar o chatbot
void printHeader() {
    cout << "=============================================" << endl;
    cout << "       TRIP++ | Assistente de Viagens        " << endl;
    cout << "=============================================" << endl;
}

// Ponto de entrada principal do programa
int main() 
{
    // 1. Inicialização do ambiente
    setupConsole();
    printHeader();

    // 2. Instanciação do Agente de Comunicação
    // Inicialização do objeto que servirá de ponte entre C++ e Python
    PythonAgent agent(
        GEMINI_API_KEY,
        ".\\env\\Scripts\\python.exe", // Caminho do interpretador no VENV
        "chatbot_agent.py"             // Script de lógica (Backend)
    );

    // 3. Interface Inicial
    cout << endl << "Ola! Para onde vamos viajar?" << endl;
    cout << "Posso montar roteiros, calcular gastos e dar dicas." << endl << endl;
    
    cout << " [COMANDOS]" << endl;
    cout << " /limpar  - Nova Viagem (Reseta o contexto/memoria JSON)" << endl;
    cout << " /sair    - Encerra o processo" << endl << endl;

    string pergunta;

    // 4. Main Loop (Loop de Eventos)
    // Mantém a aplicação viva aguardando interações do usuário.
    while (true) 
    {
        // Prompt de entrada
        cout << "Voce >> ";
        getline(cin, pergunta); // Captura a linha inteira, incluindo espaços

        // Verifica condição de parada
        if (pergunta == "/sair") {
            cout << "\nTrip++: Boa viagem! Ate a proxima." << endl;
            break; // Quebra o loop e encerra o programa
        }

        cout << "\n(Processando...)" << endl;

        // 5. Comunicação com o Backend
        // O método sendMessage dispara um sub-processo Python, passa a pergunta,
        // aguarda o processamento da IA e recebe um JSON bruto como retorno.
        string jsonResponse = agent.sendMessage(pergunta);

        // 6. Parsing e Tratamento de Dados
        // O método extractResponse limpa o JSON e extrai apenas o texto útil.
        string resposta = agent.extractResponse(jsonResponse);

        // 7. Renderização da Resposta
        cout << endl << "Trip++ diz:" << endl;
        cout << "---------------------------------------------" << endl;
        cout << resposta << endl;
        cout << "---------------------------------------------" << endl << endl;
    }

    return 0; // Retorna 0 indicando sucesso ao sistema operacional
}