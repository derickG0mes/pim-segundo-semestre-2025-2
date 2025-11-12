#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import json
from datetime import datetime
from pathlib import Path
import traceback

# Ajusta o caminho relativo para funcionar independente de onde o script é chamado
script_dir = Path(__file__).resolve().parent
# De filtro_historico (codigos/ferramentas/filtro_historico) para dados precisa subir 3 níveis
# ../../.. = até a raiz do projeto
project_root = script_dir.parent.parent.parent

# Função para encontrar o arquivo correto
def localizar_arquivo(nome_arquivo):
    """Tenta localizar um arquivo nos possíveis caminhos"""
    caminhos_possiveis = [
        # Caminho absoluto via project_root
        project_root / "dados" / "atividades_e_avaliacoes" / nome_arquivo,
        # Caminhos relativos (para diferentes pontos de execução)
        Path(__file__).resolve().parent.parent.parent / "dados" / "atividades_e_avaliacoes" / nome_arquivo,
        # Alternativa com acentuação diferente
        project_root / "dados" / "atividades_e_avalicoes" / nome_arquivo,
        Path(__file__).resolve().parent.parent.parent / "dados" / "atividades_e_avalicoes" / nome_arquivo,
    ]
    
    for caminho in caminhos_possiveis:
        if caminho.exists():
            print(f"✓ Arquivo encontrado em: {caminho}")
            return caminho
    
    # Se não encontrou, imprime os caminhos tentados para debug
    print(f"❌ Arquivo '{nome_arquivo}' não encontrado. Caminhos tentados:")
    for caminho in caminhos_possiveis:
        print(f"   - {caminho} (existe: {caminho.exists()})")
    
    return None

class HistoricoAtividade:
    """Classe para representar uma atividade no histórico"""
    def __init__(self, id, titulo, tipo, status, data_criacao, data_expiracao, 
                 id_turma, nome_turma, professor, disciplina, respostas, total_alunos, total_respondido):
        self.id = int(id)
        self.titulo = titulo
        self.tipo = tipo
        self.status = status
        self.data_criacao = data_criacao
        self.data_expiracao = data_expiracao
        self.id_turma = int(id_turma)
        self.nome_turma = nome_turma
        self.professor = professor
        self.disciplina = disciplina
        self.respostas = respostas
        self.total_alunos = int(total_alunos)
        self.total_respondido = int(total_respondido)
        
        # Calcula porcentagem de respondidos
        self.percentual_respondido = (self.total_respondido / self.total_alunos * 100) if self.total_alunos > 0 else 0
    
    def __str__(self):
        return f"ID: {self.id:3d} | {self.titulo:40s} | {self.tipo:12s} | {self.status:12s}"

def carregar_historico():
    """Carrega o histórico de atividades e avaliações"""
    historico = []
    
    arquivo_historico = localizar_arquivo("historico_atividades.json")
    
    try:
        if arquivo_historico and arquivo_historico.exists():
            with open(arquivo_historico, 'r', encoding='utf-8') as f:
                dados = json.load(f)
                for item in dados:
                    atividade = HistoricoAtividade(
                        id=item['id'],
                        titulo=item['titulo'],
                        tipo=item['tipo'],
                        status=item['status'],
                        data_criacao=item['data_criacao'],
                        data_expiracao=item['data_expiracao'],
                        id_turma=item['id_turma'],
                        nome_turma=item['nome_turma'],
                        professor=item['professor'],
                        disciplina=item['disciplina'],
                        respostas=item.get('respostas', []),
                        total_alunos=item['total_alunos'],
                        total_respondido=item['total_respondido']
                    )
                    historico.append(atividade)
                print(f"✓ Carregadas {len(dados)} atividades/avaliações do histórico")
        else:
            print("⚠️ Arquivo historico_atividades.json não encontrado")
        
        print(f"📊 Total no histórico: {len(historico)} itens\n")
    
    except json.JSONDecodeError as e:
        print(f"❌ Erro ao decodificar JSON: {e}")
        return []
    except Exception as e:
        print(f"❌ Erro ao carregar histórico: {e}")
        traceback.print_exc()
        return []
    
    return historico

def converter_data(data_str):
    """Converte string de data para objeto datetime"""
    try:
        return datetime.strptime(data_str, "%Y-%m-%d")
    except:
        return None

def filtrar_por_data(historico, data_inicio=None, data_fim=None):
    """Filtra atividades por intervalo de data"""
    if not data_inicio and not data_fim:
        return historico
    
    resultados = []
    for atividade in historico:
        data_criacao = converter_data(atividade.data_criacao)
        
        if data_criacao:
            if data_inicio and data_fim:
                inicio = converter_data(data_inicio)
                fim = converter_data(data_fim)
                if inicio <= data_criacao <= fim:
                    resultados.append(atividade)
            elif data_inicio:
                inicio = converter_data(data_inicio)
                if data_criacao >= inicio:
                    resultados.append(atividade)
            elif data_fim:
                fim = converter_data(data_fim)
                if data_criacao <= fim:
                    resultados.append(atividade)
    
    return resultados

def filtrar_por_turma(historico, id_turma):
    """Filtra atividades por turma"""
    if not id_turma:
        return historico
    
    try:
        id_turma = int(id_turma)
        return [a for a in historico if a.id_turma == id_turma]
    except:
        return historico

def filtrar_por_tipo(historico, tipo):
    """Filtra atividades por tipo (Atividade ou Avaliação)"""
    if not tipo:
        return historico
    
    tipo = tipo.strip().lower()
    if tipo in ["atividade", "atividades"]:
        return [a for a in historico if a.tipo.lower() == "atividade"]
    elif tipo in ["avaliacao", "avaliacoes"]:
        return [a for a in historico if a.tipo.lower() == "avaliacao"]
    
    return historico

def filtrar_por_status(historico, status):
    """Filtra atividades por status"""
    if not status:
        return historico
    
    status = status.strip().lower()
    return [a for a in historico if a.status.lower() == status]

def exibir_resultados(resultados, filtros_aplicados):
    """Exibe os resultados formatados"""
    linha = "═" * 130
    print(f"\n{linha}")
    print(f"   HISTÓRICO DE ATIVIDADES E AVALIAÇÕES")
    print(f"{linha}\n")
    
    if filtros_aplicados:
        print("📋 FILTROS APLICADOS:")
        for filtro, valor in filtros_aplicados.items():
            if valor:
                print(f"   • {filtro}: {valor}")
        print()
    
    if not resultados:
        print("  ❌ Nenhuma atividade encontrada com os filtros especificados.")
        print(f"\n{linha}\n")
        return
    
    print(f"  ✓ Total encontrado: {len(resultados)} item(ns)\n")
    
    # Cabeçalho da tabela
    header = f"{'ID':<5} {'TÍTULO':<45} {'TIPO':<12} {'STATUS':<12} {'DATA':<12} {'TURMA':<12} {'RESPOSTAS':<15}"
    print(header)
    print("─" * 130)
    
    # Imprime cada atividade
    for item in resultados:
        titulo_truncado = item.titulo[:42] + "..." if len(item.titulo) > 42 else item.titulo
        respostas_info = f"{item.total_respondido}/{item.total_alunos} ({item.percentual_respondido:.0f}%)"
        print(f"{item.id:<5} {titulo_truncado:<45} {item.tipo:<12} {item.status:<12} {item.data_criacao:<12} {item.nome_turma:<12} {respostas_info:<15}")
    
    print(f"\n{linha}\n")

def exibir_detalhes_atividade(atividade):
    """Exibe detalhes completos de uma atividade"""
    linha = "═" * 100
    print(f"\n{linha}")
    print(f"   DETALHES DA ATIVIDADE/AVALIAÇÃO")
    print(f"{linha}\n")
    
    print(f"📌 ID: {atividade.id}")
    print(f"📝 Título: {atividade.titulo}")
    print(f"📂 Tipo: {atividade.tipo}")
    print(f"🔔 Status: {atividade.status}")
    print(f"👨‍🏫 Professor: {atividade.professor}")
    print(f"📚 Disciplina: {atividade.disciplina}")
    print(f"👥 Turma: {atividade.nome_turma} (ID: {atividade.id_turma})")
    print(f"📅 Data de Criação: {atividade.data_criacao}")
    print(f"⏰ Data de Expiração: {atividade.data_expiracao}")
    print(f"\n📊 RESPOSTAS:")
    print(f"   Total de Alunos: {atividade.total_alunos}")
    print(f"   Total de Respondidos: {atividade.total_respondido}")
    print(f"   Percentual: {atividade.percentual_respondido:.1f}%")
    
    if atividade.respostas:
        print(f"\n📋 LISTA DE ALUNOS QUE RESPONDERAM:")
        print("─" * 100)
        print(f"{'ID Aluno':<10} {'Nome':<40} {'Data Resposta':<15} {'Pontuação':<12} {'Status':<15}")
        print("─" * 100)
        
        for resposta in atividade.respostas:
            nome_truncado = resposta['nome_aluno'][:37] + "..." if len(resposta['nome_aluno']) > 37 else resposta['nome_aluno']
            print(f"{resposta['id_aluno']:<10} {nome_truncado:<40} {resposta['data_resposta']:<15} {resposta['pontuacao']:<12} {resposta['status_resposta']:<15}")
    else:
        print(f"\n⚠️ Nenhum aluno respondeu ainda.")
    
    print(f"\n{linha}\n")

def menu_filtro_avancado():
    """Interface interativa para filtro avançado"""
    historico = carregar_historico()
    
    if not historico:
        return
    
    while True:
        print("\n" + "=" * 80)
        print("FILTRO DE HISTÓRICO DE ATIVIDADES E AVALIAÇÕES")
        print("=" * 80)
        print("\nOpções de filtro:")
        print("  [1] Filtrar por data (intervalo)")
        print("  [2] Filtrar por turma")
        print("  [3] Filtrar por tipo (Atividade/Avaliação)")
        print("  [4] Filtrar por status (Ativo/Expirado/Concluído)")
        print("  [5] Filtro avançado (combinar múltiplos filtros)")
        print("  [6] Listar tudo (sem filtros)")
        print("  [7] Voltar")
        
        opcao = input("\nEscolha uma opção: ").strip()
        
        if opcao == '1':
            print("\n→ FILTRAR POR DATA")
            print("  Digite a data no formato: YYYY-MM-DD (ex: 2025-11-05)")
            data_inicio = input("  Data de início (deixe em branco para ignorar): ").strip()
            data_fim = input("  Data de fim (deixe em branco para ignorar): ").strip()
            
            resultados = filtrar_por_data(historico, data_inicio if data_inicio else None, data_fim if data_fim else None)
            filtros = {}
            if data_inicio:
                filtros['Data Início'] = data_inicio
            if data_fim:
                filtros['Data Fim'] = data_fim
            exibir_resultados(resultados, filtros)
        
        elif opcao == '2':
            print("\n→ FILTRAR POR TURMA")
            id_turma = input("  Digite o ID da turma (ex: 1, 2, 3): ").strip()
            
            resultados = filtrar_por_turma(historico, id_turma)
            filtros = {'Turma ID': id_turma} if id_turma else {}
            exibir_resultados(resultados, filtros)
        
        elif opcao == '3':
            print("\n→ FILTRAR POR TIPO")
            print("  [1] Atividade")
            print("  [2] Avaliação")
            tipo_escolha = input("\nEscolha (1 ou 2): ").strip()
            tipo = "Atividade" if tipo_escolha == '1' else "Avaliacao" if tipo_escolha == '2' else ""
            
            resultados = filtrar_por_tipo(historico, tipo)
            filtros = {'Tipo': tipo} if tipo else {}
            exibir_resultados(resultados, filtros)
        
        elif opcao == '4':
            print("\n→ FILTRAR POR STATUS")
            print("  [1] Ativo")
            print("  [2] Expirado")
            print("  [3] Concluído")
            status_escolha = input("\nEscolha (1-3): ").strip()
            status_map = {
                '1': 'Ativo',
                '2': 'Expirado',
                '3': 'Concluído'
            }
            status = status_map.get(status_escolha, '')
            
            resultados = filtrar_por_status(historico, status)
            filtros = {'Status': status} if status else {}
            exibir_resultados(resultados, filtros)
        
        elif opcao == '5':
            print("\n→ FILTRO AVANÇADO (Combinar múltiplos filtros)")
            
            # Coleta os filtros
            data_inicio = input("  Data de início (YYYY-MM-DD, deixe em branco para ignorar): ").strip()
            data_fim = input("  Data de fim (YYYY-MM-DD, deixe em branco para ignorar): ").strip()
            id_turma = input("  ID da turma (deixe em branco para ignorar): ").strip()
            
            print("  Tipo:")
            print("    [1] Atividade  [2] Avaliação  [0] Todos")
            tipo_escolha = input("  Escolha: ").strip()
            tipo_map = {'1': 'Atividade', '2': 'Avaliacao'}
            tipo = tipo_map.get(tipo_escolha, '')
            
            print("  Status:")
            print("    [1] Ativo  [2] Expirado  [3] Concluído  [0] Todos")
            status_escolha = input("  Escolha: ").strip()
            status_map = {'1': 'Ativo', '2': 'Expirado', '3': 'Concluído'}
            status = status_map.get(status_escolha, '')
            
            # Aplica os filtros
            resultados = historico
            if data_inicio or data_fim:
                resultados = filtrar_por_data(resultados, data_inicio if data_inicio else None, data_fim if data_fim else None)
            if id_turma:
                resultados = filtrar_por_turma(resultados, id_turma)
            if tipo:
                resultados = filtrar_por_tipo(resultados, tipo)
            if status:
                resultados = filtrar_por_status(resultados, status)
            
            # Exibe os filtros aplicados
            filtros = {}
            if data_inicio:
                filtros['Data Início'] = data_inicio
            if data_fim:
                filtros['Data Fim'] = data_fim
            if id_turma:
                filtros['Turma ID'] = id_turma
            if tipo:
                filtros['Tipo'] = tipo
            if status:
                filtros['Status'] = status
            
            exibir_resultados(resultados, filtros)
        
        elif opcao == '6':
            exibir_resultados(historico, {})
        
        elif opcao == '7':
            break
        
        else:
            print("\n❌ Opção inválida!")
        
        input("\nPressione ENTER para continuar...")

def main():
    """Função principal"""
    try:
        # Configura a codificação do terminal para UTF-8
        if sys.platform == 'win32':
            try:
                import locale
                locale.setlocale(locale.LC_ALL, 'pt_BR.UTF-8')
            except:
                pass
            try:
                sys.stdout.reconfigure(encoding='utf-8')
            except:
                pass
        
        # Se foi chamado com argumentos (do programa C)
        if len(sys.argv) >= 2:
            tipo_filtro = sys.argv[1].strip() if len(sys.argv) > 1 else ""
            
            historico = carregar_historico()
            if not historico:
                print("\n❌ Nenhuma atividade encontrada no histórico.")
                return
            
            # Filtra por tipo se especificado
            if tipo_filtro:
                historico = filtrar_por_tipo(historico, tipo_filtro)
            
            exibir_resultados(historico, {'Tipo': tipo_filtro} if tipo_filtro else {})
        
        # Se foi executado diretamente (modo interativo)
        else:
            menu_filtro_avancado()
    
    except KeyboardInterrupt:
        print("\n\n⚠️  Operação cancelada pelo usuário.")
    except Exception as e:
        print(f"\n❌ Erro inesperado: {str(e)}")
        print("   Por favor, contate o suporte técnico.")
        traceback.print_exc()

if __name__ == "__main__":
    main()
