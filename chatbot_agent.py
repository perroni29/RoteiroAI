#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys
import json
import os
import warnings
import re

# --- CONFIGURAÇÕES ---
HISTORY_FILE = 'history.json'  # Arquivo onde a memória será salva

# SUPRIMIR LOGS DO TENSORFLOW/GRPC
os.environ['GRPC_VERBOSITY'] = 'ERROR'
os.environ['GLOG_minloglevel'] = '3'
os.environ['PYTHONWARNINGS'] = 'ignore'
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
warnings.filterwarnings('ignore')

# Fechar stderr para não poluir o terminal do C++
sys.stderr = open(os.devnull, 'w')

try:
    import google.generativeai as genai
except ImportError:
    sys.stderr = sys.__stderr__
    print(json.dumps({"status": "error", "response": "Erro: Biblioteca google-generativeai não instalada."}, ensure_ascii=False))
    sys.exit(1)

def sanitize_text(text):
    """Limpa caracteres que podem quebrar a exibição no terminal Windows"""
    replacements = {
        '\u2022': '-', '\u2013': '-', '\u2014': '-',
        '\u2018': "'", '\u2019': "'", '\u201c': '"', '\u201d': '"',
        '**': '', '*': '' # Remove negrito/itálico do markdown
    }
    for char, rep in replacements.items():
        text = text.replace(char, rep)
    return text

class GeminiAgent:
    def __init__(self, api_key):
        try:
            genai.configure(api_key=api_key)
            
            # --- CÉREBRO E REGRAS DE SEGURANÇA (GUARDRAILS) ---
            instrucao = """
            Você é um assistente ESTRITAMENTE focado em Viagens e Turismo.
            
            SUAS REGRAS ABSOLUTAS:
            1. SE O USUÁRIO PERGUNTAR SOBRE VIAGEM (Roteiros, Câmbio, Destinos, Malas, Pontos Turísticos):
               - Responda de forma completa, simpática e detalhada.
               - Crie roteiros dia-a-dia se solicitado.
               - Use emojis para deixar o texto leve.
            
            2. BLOQUEIO DE ASSUNTO (IMPORTANTE):
               - Se a pergunta NÃO for sobre viagens (ex: matemática, código, política, história geral, fofoca):
               - RECUSE A RESPOSTA. Diga apenas: "Desculpe, sou um especialista apenas em viagens! ✈️ Posso ajudar a planejar suas férias?"
               - NÃO responda a pergunta proibida em hipótese alguma.
            
            3. MEMÓRIA: Mantenha o contexto do destino escolhido nas mensagens anteriores.
            
            4. FORMATAÇÃO: Use listas com hífens (-) para tópicos. Não use Markdown complexo (** ou __).
            """
            
            self.model = genai.GenerativeModel(
                model_name='gemini-2.0-flash-lite', 
                system_instruction=instrucao
            )
        except Exception as e:
            raise Exception(f"Erro init: {str(e)}")

    def _load_history(self):
        """Lê o histórico do arquivo JSON para manter a conversa viva"""
        if os.path.exists(HISTORY_FILE):
            try:
                with open(HISTORY_FILE, 'r', encoding='utf-8') as f:
                    return json.load(f)
            except:
                return []
        return []

    def _save_history(self, history):
        """Salva o histórico atualizado no arquivo JSON"""
        try:
            with open(HISTORY_FILE, 'w', encoding='utf-8') as f:
                json.dump(history, f, ensure_ascii=False, indent=2)
        except:
            pass

    def send_message(self, message):
        try:
            # 1. Carrega memória
            history_data = self._load_history()
            
            # 2. Inicia chat com a memória
            chat = self.model.start_chat(history=history_data)
            
            # 3. Envia nova pergunta
            response = chat.send_message(message)
            sanitized = sanitize_text(response.text)
            
            # 4. Salva memória atualizada
            # Recriamos o histórico manualmente para garantir formato compatível com JSON
            new_history = []
            for msg in chat.history:
                new_history.append({
                    "role": msg.role,
                    "parts": [part.text for part in msg.parts]
                })
            
            self._save_history(new_history)
            
            return {"status": "success", "response": sanitized}
            
        except Exception as e:
            return {"status": "error", "response": f"Erro na IA: {str(e)}"}

    def clear_history(self):
        if os.path.exists(HISTORY_FILE):
            os.remove(HISTORY_FILE)
        return {"status": "success", "response": "Histórico de viagem apagado! Começando do zero."}

def main():
    try:
        if len(sys.argv) < 3: return
        
        api_key = sys.argv[1]
        command = sys.argv[2]
        agent = GeminiAgent(api_key)
        
        if command == "send":
            msg = sys.argv[3] if len(sys.argv) > 3 else ""
            print(json.dumps(agent.send_message(msg), ensure_ascii=False))
        elif command == "clear":
            print(json.dumps(agent.clear_history(), ensure_ascii=False))
            
    except Exception as e:
        print(json.dumps({"status": "error", "response": str(e)}, ensure_ascii=False))

if __name__ == "__main__":
    # CORREÇÃO CRÍTICA PARA ACENTOS NO WINDOWS
    if sys.stdout.encoding != 'utf-8':
        sys.stdout.reconfigure(encoding='utf-8')
    main()