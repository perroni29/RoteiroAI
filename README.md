# Trip++ | Assistente de Viagens Inteligente 🌍

O **Trip++** é um chatbot especializado na geração de itinerários de viagem personalizados. O projeto utiliza uma arquitetura híbrida para unir a flexibilidade do **Python** no processamento de Inteligência Artificial com a robustez da interface em **C++**.

## 🛠️ Arquitetura e Tecnologias

Este projeto demonstra a integração entre diferentes linguagens de programação para resolver problemas complexos:

* **Frontend (C++)**: Interface de console de alto desempenho desenvolvida para gerenciar o fluxo de conversação e a entrada de dados do usuário (`main.cpp`).
* **Backend (Python)**: Motor lógico que utiliza a API do **Google Gemini** para processar Linguagem Natural (NLP) e gerar sugestões inteligentes (`chatbot_agent.py`).
* **Integração**: Utilização de `_popen` e fluxos de dados para comunicação em tempo real entre o binário C++ e o interpretador Python.
* **Persistência**: Gerenciamento de histórico de conversas em formato **JSON** (`history.json`), permitindo que a IA mantenha o contexto durante a sessão.

## 🚀 Funcionalidades

* **Roteiros Customizados**: Geração de cronogramas detalhados com base no destino e tempo de permanência.
* **Memória de Sessão**: O assistente lembra o que foi dito anteriormente na conversa.
* **Tratamento de Dados**: Sanitização de texto e suporte a caracteres UTF-8 para garantir uma exibição limpa no terminal Windows.
* **Comandos Especiais**: 
    * `/limpar`: Reseta a memória do chatbot e inicia um novo planejamento.
    * `/sair`: Encerra a aplicação de forma segura.

## 📂 Estrutura do Projeto

* `main.cpp`: Ciclo principal da aplicação e interface.
* `PythonAgent.cpp/h`: Abstração da lógica de comunicação entre linguagens.
* `chatbot_agent.py`: Script Python que consome a API generativa.
* `run_chatbot.bat`: Automatizador de execução para ambiente Windows.

## ⚙️ Como Executar

1.  **Configure o Python**:
    * Crie um ambiente virtual: `python -m venv env`
    * Ative o ambiente: `env\Scripts\activate.bat`
    * Instale a dependência: `pip install -U google-generativeai`

2.  **Compile o C++**:
    ```bash
    g++ main.cpp PythonAgent.cpp -o chatbot.exe
    ```

3.  **Inicie o Chatbot**:
    Execute o arquivo `run_chatbot.bat`.

---
*Este projeto foi desenvolvido como parte do portfólio técnico para Engenharia de Software no Inatel.*
