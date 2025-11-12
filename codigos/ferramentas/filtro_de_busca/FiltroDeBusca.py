import sys
import os
import json
from datetime import datetime
from pathlib import Path
import unicodedata
import traceback

# Ajusta o caminho relativo para funcionar independente de onde o script é chamado
script_dir = Path(__file__).resolve().parent
project_root = script_dir.parent.parent.parent

# Função para encontrar o arquivo correto
def localizar_arquivo(nome_arquivo):
    """Tenta localizar um arquivo nos possíveis caminhos"""
    # Prioriza caminhos com 'dados' em minúsculas (mais comum)
    caminhos_possiveis = [
        project_root / "dados" / "atividades_e_avaliacoes" / nome_arquivo,
        project_root / "dados" / "atividades_e_avalicoes" / nome_arquivo,
        Path("../../dados/atividades_e_avaliacoes") / nome_arquivo,
        Path("../../dados/atividades_e_avalicoes") / nome_arquivo,
        # Também tenta com 'DADOS' em maiúsculas por compatibilidade
        project_root / "DADOS" / "atividades_e_avaliacoes" / nome_arquivo,
        project_root / "DADOS" / "atividades_e_avalicoes" / nome_arquivo,
        Path("../../DADOS/atividades_e_avaliacoes") / nome_arquivo,
        Path("../../DADOS/atividades_e_avalicoes") / nome_arquivo
    ]
    
    for caminho in caminhos_possiveis:
        if caminho.exists():
            return caminho
    
    return None

class Atividade:
    def __init__(self, id, titulo, tipo, enunciado):
        self.id = int(id)
        self.titulo = titulo
        self.tipo = tipo
        self.enunciado = enunciado
        self.questoes = []
    
    def __str__(self):
        return f"ID: {self.id:3d} | {self.titulo:30s} | {self.tipo:12s}"

def carregar_atividades():
    """Carrega atividades e avaliações direto dos arquivos de conteúdo"""
    atividades = []
    
    # Verificar estrutura de diretórios
    pasta_dados = project_root / "dados"
    if not pasta_dados.exists():
        print("❌ Pasta 'dados' não encontrada!")
        return atividades

    pasta_atividades = None
    for pasta in ["atividades_e_avaliacoes", "atividades_e_avalicoes"]:
        if (pasta_dados / pasta).exists():
            pasta_atividades = pasta_dados / pasta
            break
    
    if pasta_atividades is None:
        print("❌ Pasta de atividades não encontrada dentro de 'dados'!")
        return atividades

    # Localizar e carregar atividades
    arquivo_atividades = localizar_arquivo("conteudo_atividades.json")
    arquivo_avaliacoes = localizar_arquivo("conteudo_avaliacoes.json")
    
    try:
        # Carregar ATIVIDADES
        if arquivo_atividades and arquivo_atividades.exists():
            with open(arquivo_atividades, 'r', encoding='utf-8') as f:
                dados_atividades = json.load(f)
                for item in dados_atividades:
                    atividade = Atividade(
                        id=item['id'],
                        titulo=item['titulo'],
                        tipo="Atividade",
                        enunciado=item.get('enunciado', '')
                    )
                    atividade.questoes = item.get('questoes', [])
                    atividades.append(atividade)
                print(f"✓ Carregadas {len(dados_atividades)} atividades")
        else:
            print("⚠️ Arquivo conteudo_atividades.json não encontrado")

        # Carregar AVALIAÇÕES
        if arquivo_avaliacoes and arquivo_avaliacoes.exists():
            with open(arquivo_avaliacoes, 'r', encoding='utf-8') as f:
                dados_avaliacoes = json.load(f)
                for item in dados_avaliacoes:
                    atividade = Atividade(
                        id=item['id'],
                        titulo=item['titulo'],
                        tipo="Avaliacao",
                        enunciado=item.get('enunciado', '')
                    )
                    atividade.questoes = item.get('questoes', [])
                    atividades.append(atividade)
                print(f"✓ Carregadas {len(dados_avaliacoes)} avaliações")
        else:
            print("⚠️ Arquivo conteudo_avaliacoes.json não encontrado")
        
        print(f"\n📊 Total carregado: {len(atividades)} itens\n")
    
    except json.JSONDecodeError as e:
        print(f"❌ Erro ao decodificar JSON: {e}")
        return []
    except Exception as e:
        print(f"❌ Erro ao carregar atividades: {e}")
        traceback.print_exc()
        return []
    
    return atividades

def buscar_atividades(termo, atividades):
    """Busca atividades por termo no título, enunciado, questões e alternativas"""
    if not termo:
        return atividades
    
    print(f"\n🔍 Buscando por: '{termo}'")
    
    # Normalização: remove acentos e converte para minúsculas
    def normalize(text):
        if not text:
            return ""
        if not isinstance(text, str):
            text = str(text)
        nfkd = unicodedata.normalize('NFKD', text)
        only_ascii = nfkd.encode('ASCII', 'ignore').decode('ASCII')
        return only_ascii.lower().strip()

    termo_norm = normalize(termo)
    termos = [t for t in termo_norm.split() if t]
    resultados = []
    
    for atividade in atividades:
        encontrado = False
        
        # Normalizar campos básicos
        titulo_norm = normalize(atividade.titulo)
        tipo_norm = normalize(atividade.tipo)
        enunciado_norm = normalize(atividade.enunciado)
        
        # 1. BUSCA NO TÍTULO
        for t in termos:
            if t in titulo_norm:
                resultados.append(atividade)
                encontrado = True
                break
        
        if encontrado:
            continue
        
        # 2. BUSCA NO ENUNCIADO
        for t in termos:
            if t in enunciado_norm:
                resultados.append(atividade)
                encontrado = True
                break
        
        if encontrado:
            continue
        
        # 3. BUSCA NAS QUESTÕES E ALTERNATIVAS
        for questao in atividade.questoes:
            if encontrado:
                break
            
            texto_questao = normalize(questao.get('texto', ''))
            alternativas = [normalize(alt) for alt in questao.get('alternativas', [])]
            
            for t in termos:
                if (t in texto_questao or
                    any(t in alt for alt in alternativas)):
                    resultados.append(atividade)
                    encontrado = True
                    break
    
    print(f"✓ Encontrados {len(resultados)} resultado(s)\n")
    return resultados

def filtrar_por_tipo(tipo, atividades):
    """Filtra atividades por tipo (Atividade ou Avaliação)"""
    return [a for a in atividades if a.tipo.lower() == tipo.lower()]

def filtrar_por_status(status, atividades):
    """Filtra atividades por status"""
    return [a for a in atividades if status.lower() in a.status.lower()]

def filtrar_por_turma(id_turma, atividades):
    """Filtra atividades por ID da turma"""
    return [a for a in atividades if a.id_turma == id_turma]

def exibir_resultados(resultados, termo):
    """Exibe os resultados da busca formatados"""
    linha = "═" * 100
    print(f"\n{linha}")
    print(f"   RESULTADOS DA BUSCA: '{termo}'")
    print(f"{linha}\n")
    
    if not resultados:
        print("  ❌ Nenhuma atividade encontrada.")
        print(f"\n{linha}\n")
        return
    
    # Agrupa resultados por tipo
    atividades = [r for r in resultados if r.tipo == "Atividade"]
    avaliacoes = [r for r in resultados if r.tipo == "Avaliacao"]
    
    print(f"  ✓ Total encontrado: {len(resultados)} item(ns)")
    print(f"    • Atividades: {len(atividades)}")
    print(f"    • Avaliações: {len(avaliacoes)}\n")
    
    # Cabeçalho da tabela
    header = f"{'ID':<5} {'TÍTULO':<50} {'TIPO':<12} {'QUESTÕES':<10}"
    print(header)
    print("─" * 100)
    
    # Função auxiliar para formatação
    def print_item(item):
        titulo_truncado = item.titulo[:47] + "..." if len(item.titulo) > 47 else item.titulo
        num_questoes = len(item.questoes)
        print(f"{item.id:<5} {titulo_truncado:<50} {item.tipo:<12} {num_questoes:>3} quest.")
    
    # Imprime primeiro as atividades
    if atividades:
        print("\n➤ ATIVIDADES:")
        for item in atividades:
            print_item(item)
    
    # Depois imprime as avaliações
    if avaliacoes:
        print("\n➤ AVALIAÇÕES:")
        for item in avaliacoes:
            print_item(item)
    
    print(f"\n{linha}\n")

def busca_avancada():
    """Interface de busca avançada interativa"""
    atividades = carregar_atividades()
    
    if not atividades:
        return

    print("BUSCA AVANÇADA DE ATIVIDADES")
    print("\n⚠️  IMPORTANTE: Digite o nome da atividade de forma COMPLETA para melhores resultados!")
    print("   Exemplo: se procura 'Simulado de Matemática', use o nome exato ou uma parte significativa.\n")
    
    print("Opções de filtro:")
    print("  [1] Buscar por termo geral")
    print("      → Exemplos: 'prova', 'exercício', 'lista', 'matemática', etc")
    print("      → Busca no título, tipo e status das atividades\n")
    
    print("  [2] Filtrar por tipo")
    print("      → Tipos disponíveis: 'Atividade' ou 'Avaliação'\n")
    
    print("  [3] Filtrar por status")
    print("      → Status possíveis:")
    print("        • Expirado")
    print("        • Respondido")
    print("        • Nenhum aluno respondeu\n")
    
    print("  [4] Filtrar por turma")
    print("      → Digite o número ID da turma (ex: 1, 2, 3, etc)\n")
    
    print("  [5] Listar todas as atividades")
    
    try:
        opcao = input("\nEscolha uma opção: ").strip()
        
        if opcao == '1':
            print("\n→ BUSCA POR TERMO")
            print("  Você pode buscar por palavras no título, tipo ou status")
            print("  Exemplos: 'prova final', 'exercício', 'matemática', etc")
            print("\n  💡 DICA: Use o nome COMPLETO da atividade para encontrar com precisão!")
            print("     Ex: 'Simulado de Matemática' em vez de apenas 'Simulado'")
            termo = input("\nDigite o termo de busca: ").strip()
            resultados = buscar_atividades(termo, atividades)
            exibir_resultados(resultados, termo)
        
        elif opcao == '2':
            print("\n→ FILTRAR POR TIPO")
            print("  [1] Atividade  - Exercícios, trabalhos, listas de exercícios")
            print("  [2] Avaliação  - Provas, testes, exames")
            tipo_escolha = input("\nEscolha o tipo (1 ou 2): ").strip()
            tipo = "Atividade" if tipo_escolha == '1' else "Avaliação"
            resultados = filtrar_por_tipo(tipo, atividades)
            exibir_resultados(resultados, f"Tipo: {tipo}")
        
        elif opcao == '3':
            print("\n→ FILTRAR POR STATUS")
            print("  [1] Expirado            - Prazo de entrega passou")
            print("  [2] Respondido          - Pelo menos um aluno respondeu")
            print("  [3] Nenhum aluno respondeu - Aguardando respostas")
            status_escolha = input("\nEscolha o status (1-3): ").strip()
            status_map = {
                '1': 'Expirado',
                '2': 'Respondido',
                '3': 'Nenhum aluno respondeu'
            }
            status = status_map.get(status_escolha, '')
            resultados = filtrar_por_status(status, atividades)
            exibir_resultados(resultados, f"Status: {status}")
        
        elif opcao == '4':
            print("\n→ FILTRAR POR TURMA")
            print("  Digite o número identificador da turma")
            print("  Exemplo: Para 'Turma 1', digite apenas '1'")
            id_turma = int(input("\nDigite o ID da turma: ").strip())
            resultados = filtrar_por_turma(id_turma, atividades)
            exibir_resultados(resultados, f"Turma ID: {id_turma}")
        
        elif opcao == '5':
            print("\n→ LISTANDO TODAS AS ATIVIDADES")
            exibir_resultados(atividades, "Todas as atividades")
        
        else:
            print("\n❌ Opção inválida!")
    
    except Exception as e:
        print(f"\n❌ Erro: {e}")

def main():
    """Função principal - pode ser chamada do C com argumentos"""
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
        # Argumento 1: tipo_filtro ("Atividade", "Avaliacao" ou vazio)
        # Argumento 2: termo_busca (string de busca ou vazio)
        if len(sys.argv) >= 2:
            tipo_filtro = sys.argv[1].strip() if len(sys.argv) > 1 else ""
            termo_busca = sys.argv[2].strip() if len(sys.argv) > 2 else ""
            
            print("=" * 80)
            print("⚠️  IMPORTANTE: O nome da atividade deve estar COMPLETO para encontrar!")
            print("   Digite o nome exato ou uma parte significativa do título.")
            print("=" * 80)
            if tipo_filtro:
                print(f"Buscando em: {tipo_filtro}s")
            else:
                print("Buscando em: Todos os tipos")
            if termo_busca:
                print(f"Termo: {termo_busca}")
            print("=" * 80)
            
            atividades = carregar_atividades()
            if not atividades:
                print("\n❌ Nenhuma atividade encontrada no sistema.")
                return
            
            # 1. Filtrar por tipo se especificado
            if tipo_filtro and tipo_filtro.lower() in ["atividade", "avaliacao"]:
                atividades = filtrar_por_tipo(tipo_filtro, atividades)
                print(f"\nTotal após filtro de tipo: {len(atividades)}")
            
            # 2. Buscar por termo
            if termo_busca:
                resultados = buscar_atividades(termo_busca, atividades)
                exibir_resultados(resultados, f"{tipo_filtro} - {termo_busca}" if tipo_filtro else termo_busca)
            else:
                # Se não tem termo, mostra tudo do tipo selecionado
                exibir_resultados(atividades, f"Todas as {tipo_filtro}s" if tipo_filtro else "Todas as atividades")
        
        # Se foi executado diretamente (modo interativo)
        else:
            busca_avancada()
    
    except KeyboardInterrupt:
        print("\n\n⚠️  Busca cancelada pelo usuário.")
    except Exception as e:
        print(f"\n❌ Erro inesperado: {str(e)}")
        print("   Por favor, contate o suporte técnico.")
        traceback.print_exc()

if __name__ == "__main__":
    main()