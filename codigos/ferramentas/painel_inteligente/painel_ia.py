"""
Sistema Inteligente de Análise Escolar com IA
Integração com OpenAI para insights e predições
Autor: Sistema Painel IA
Versão: 1.0
"""

import os
import json
from datetime import datetime, timedelta
from typing import Dict, List, Any, Optional
import google.generativeai as genai
from dataclasses import dataclass, asdict
from enum import Enum


def carregar_dados_desempenho(caminho: str = None) -> List['DadosAluno']:
    """
    Carrega dados de desempenho dos alunos a partir do arquivo JSON
    
    Args:
        caminho: Caminho do arquivo. Se None, usa o padrão do sistema
        
    Returns:
        Lista de objetos DadosAluno
    """
    if caminho is None:
        # Caminho padrão: dados/desempenho_alunos_ia/desempenho.json
        script_dir = os.path.dirname(os.path.abspath(__file__))
        caminho = os.path.join(script_dir, "..", "..", "..", "dados", "desempenho_alunos_ia", "desempenho.json")
    
    caminho = os.path.abspath(caminho)
    
    try:
        if not os.path.exists(caminho):
            print(f"⚠️  Arquivo não encontrado: {caminho}")
            print("   Usando dados de exemplo...")
            return None
        
        with open(caminho, 'r', encoding='utf-8') as f:
            dados = json.load(f)
        
        alunos = []
        for item in dados:
            aluno = DadosAluno(
                id=item.get('ra', item.get('id', '')),
                nome=item.get('nome', ''),
                turma=item.get('turma', ''),
                notas=item.get('notas', {}),
                frequencia=item.get('frequencia', 0.0),
                risco_evasao=item.get('risco_evasao', 'baixo')
            )
            alunos.append(aluno)
        
        print(f"✅ {len(alunos)} alunos carregados de: {caminho}")
        return alunos
        
    except Exception as e:
        print(f"❌ Erro ao carregar dados: {e}")
        return None


class NivelRisco(Enum):
    """Níveis de risco de evasão"""
    BAIXO = "baixo"
    MEDIO = "medio"
    ALTO = "alto"


class TipoAlerta(Enum):
    """Tipos de alertas do sistema"""
    CRITICO = "critico"
    ATENCAO = "atencao"
    INFO = "info"


@dataclass
class DadosAluno:
    """Estrutura de dados do aluno"""
    id: str
    nome: str
    turma: str
    notas: Dict[str, float]
    frequencia: float
    risco_evasao: str
    
    def media_geral(self) -> float:
        """Calcula média geral do aluno"""
        if not self.notas:
            return 0.0
        return sum(self.notas.values()) / len(self.notas)


@dataclass
class MetricasGerais:
    """Métricas gerais da escola"""
    total_alunos: int
    media_geral: float
    taxa_frequencia: float
    risco_evasao: float
    periodo: str
    timestamp: str


@dataclass
class Alerta:
    """Estrutura de alerta"""
    tipo: str
    titulo: str
    descricao: str
    prioridade: int
    alunos_afetados: List[str]
    timestamp: str


@dataclass
class Recomendacao:
    """Estrutura de recomendação da IA"""
    titulo: str
    descricao: str
    categoria: str
    impacto_estimado: str
    acoes: List[str]
    prioridade: int


class AnalisadorIA:
    """
    Classe responsável pela análise inteligente usando Gemini
    """
    
    def __init__(self, api_key: str):
        """
        Inicializa o analisador com a chave da API Gemini
        
        Args:
            api_key: Chave da API Gemini (Google AI)
        """
        self.api_key = api_key
        self.model = None
        self.usar_ia = False
        
        # Tentar conectar à API
        if api_key and api_key != "sua-chave-aqui":
            try:
                genai.configure(api_key=api_key)
                self.model = genai.GenerativeModel('gemini-2.0-flash')
                self.usar_ia = True
                print("✅ Conexão com Gemini estabelecida!")
            except Exception as e:
                print(f"⚠️  Não foi possível conectar ao Gemini: {e}")
                print("📊 Usando modo análise inteligente simulada...")
                self.usar_ia = False
        else:
            print("📊 Modo análise inteligente simulada (sem API key)")
            self.usar_ia = False
        
    def analisar_desempenho_geral(self, metricas: MetricasGerais, 
                                   historico: List[MetricasGerais]) -> Dict[str, Any]:
        """
        Análise geral do desempenho escolar usando IA
        
        Args:
            metricas: Métricas atuais
            historico: Histórico de métricas
            
        Returns:
            Análise detalhada com insights
        """
        if not self.usar_ia or not self.model:
            return self._analise_inteligente_local(metricas, historico)
            
        prompt = f"""
        Você é um especialista em análise de dados educacionais. Analise os seguintes dados:
        
        DADOS ATUAIS:
        - Total de alunos: {metricas.total_alunos}
        - Média geral: {metricas.media_geral}
        - Taxa de frequência: {metricas.taxa_frequencia}%
        - Risco de evasão: {metricas.risco_evasao}%
        
        HISTÓRICO (últimos períodos):
        {json.dumps([asdict(m) for m in historico[-5:]], indent=2)}
        
        Forneça uma análise em formato JSON com:
        1. "resumo_executivo": resumo em 2-3 frases
        2. "tendencias": lista de tendências identificadas
        3. "pontos_criticos": lista de pontos que requerem atenção imediata
        4. "pontos_positivos": lista de aspectos positivos
        5. "previsoes": previsões para os próximos períodos
        
        Retorne APENAS o JSON, sem markdown ou explicações adicionais.
        """
        
        try:
            response = self.model.generate_content(prompt)
            # Limpar a resposta de possível markdown
            conteudo = response.text
            if conteudo.startswith("```json"):
                conteudo = conteudo[7:]
            if conteudo.startswith("```"):
                conteudo = conteudo[3:]
            if conteudo.endswith("```"):
                conteudo = conteudo[:-3]
            
            resultado = json.loads(conteudo.strip())
            return resultado
            
        except Exception as e:
            print(f"Erro na análise IA: {e}")
            return self._analise_inteligente_local(metricas, historico)
    
    def gerar_recomendacoes(self, alunos: List[DadosAluno], 
                           alertas: List[Alerta]) -> List[Recomendacao]:
        """
        Gera recomendações personalizadas usando IA
        
        Args:
            alunos: Lista de alunos
            alertas: Lista de alertas ativos
            
        Returns:
            Lista de recomendações priorizadas
        """
        if not self.usar_ia or not self.model:
            return self._recomendacoes_inteligente_local(alunos, alertas)
            
        # Preparar dados para análise
        alunos_risco = [a for a in alunos if a.risco_evasao in ["medio", "alto"]]
        disciplinas_criticas = self._identificar_disciplinas_criticas(alunos)
        
        prompt = f"""
        Como especialista educacional, gere recomendações práticas baseadas em:
        
        SITUAÇÃO ATUAL:
        - Total de alunos: {len(alunos)}
        - Alunos em risco: {len(alunos_risco)}
        - Alertas ativos: {len(alertas)}
        - Disciplinas críticas: {', '.join(disciplinas_criticas)}
        
        ALERTAS PRINCIPAIS:
        {json.dumps([{'titulo': a.titulo, 'descricao': a.descricao} for a in alertas[:5]], indent=2)}
        
        Gere 5 recomendações em formato JSON:
        {{
            "recomendacoes": [
                {{
                    "titulo": "Título da recomendação",
                    "descricao": "Descrição detalhada",
                    "categoria": "academico/comportamental/estrutural",
                    "impacto_estimado": "alto/medio/baixo",
                    "acoes": ["ação 1", "ação 2"],
                    "prioridade": 1-5
                }}
            ]
        }}
        
        Retorne APENAS o JSON válido.
        """
        
        try:
            response = self.model.generate_content(prompt)
            # Limpar a resposta de possível markdown
            conteudo = response.text
            if conteudo.startswith("```json"):
                conteudo = conteudo[7:]
            if conteudo.startswith("```"):
                conteudo = conteudo[3:]
            if conteudo.endswith("```"):
                conteudo = conteudo[:-3]
            
            resultado = json.loads(conteudo.strip())
            
            recomendacoes = []
            for rec in resultado.get("recomendacoes", []):
                recomendacoes.append(Recomendacao(
                    titulo=rec["titulo"],
                    descricao=rec["descricao"],
                    categoria=rec["categoria"],
                    impacto_estimado=rec["impacto_estimado"],
                    acoes=rec["acoes"],
                    prioridade=rec["prioridade"]
                ))
            
            return sorted(recomendacoes, key=lambda x: x.prioridade)
            
        except Exception as e:
            print(f"Erro ao gerar recomendações: {e}")
            return self._recomendacoes_inteligente_local(alunos, alertas)
    
    def prever_desempenho(self, aluno: DadosAluno, 
                         historico_notas: Dict[str, List[float]]) -> Dict[str, float]:
        """
        Prevê desempenho futuro do aluno
        
        Args:
            aluno: Dados do aluno
            historico_notas: Histórico de notas por disciplina
            
        Returns:
            Previsões por disciplina
        """
        prompt = f"""
        Analise o histórico de notas e preveja o desempenho futuro:
        
        ALUNO: {aluno.nome}
        TURMA: {aluno.turma}
        FREQUÊNCIA: {aluno.frequencia}%
        
        HISTÓRICO DE NOTAS:
        {json.dumps(historico_notas, indent=2)}
        
        NOTAS ATUAIS:
        {json.dumps(aluno.notas, indent=2)}
        
        Retorne previsões em JSON:
        {{
            "previsoes": {{
                "disciplina": nota_prevista,
                ...
            }},
            "tendencia_geral": "melhora/estavel/declinio",
            "confianca": 0-100,
            "fatores_influentes": ["fator1", "fator2"]
        }}
        
        Retorne APENAS JSON válido.
        """
        
        try:
            response = self.model.generate_content(prompt)
            # Limpar a resposta de possível markdown
            conteudo = response.text
            if conteudo.startswith("```json"):
                conteudo = conteudo[7:]
            if conteudo.startswith("```"):
                conteudo = conteudo[3:]
            if conteudo.endswith("```"):
                conteudo = conteudo[:-3]
            
            return json.loads(conteudo.strip())
            
        except Exception as e:
            print(f"Erro na previsão: {e}")
            return self._previsao_fallback(aluno, historico_notas)
    
    def _identificar_disciplinas_criticas(self, alunos: List[DadosAluno]) -> List[str]:
        """Identifica disciplinas com desempenho crítico"""
        medias_disciplinas = {}
        
        for aluno in alunos:
            for disciplina, nota in aluno.notas.items():
                if disciplina not in medias_disciplinas:
                    medias_disciplinas[disciplina] = []
                medias_disciplinas[disciplina].append(nota)
        
        criticas = []
        for disciplina, notas in medias_disciplinas.items():
            media = sum(notas) / len(notas)
            if media < 7.0:
                criticas.append(disciplina)
        
        return criticas
    
    def _analise_inteligente_local(self, metricas: MetricasGerais, 
                                    historico: List[MetricasGerais]) -> Dict[str, Any]:
        """Análise inteligente simulada baseada em regras (sem IA)"""
        
        # Determinar status geral
        if metricas.media_geral >= 8.0:
            status = "EXCELENTE"
            resumo = f"O sistema apresenta desempenho excelente com média geral de {metricas.media_geral}."
        elif metricas.media_geral >= 7.0:
            status = "BOM"
            resumo = f"O sistema apresenta bom desempenho com média geral de {metricas.media_geral}."
        elif metricas.media_geral >= 6.0:
            status = "ADEQUADO"
            resumo = f"O sistema apresenta desempenho adequado, mas requer melhorias (média {metricas.media_geral})."
        else:
            status = "CRÍTICO"
            resumo = f"O sistema está em situação crítica com média geral de {metricas.media_geral}."
        
        # Tendências
        tendencias = []
        if len(historico) > 1:
            ultima = historico[-1].media_geral
            penultima = historico[-2].media_geral
            if ultima > penultima:
                tendencias.append("📈 Tendência de melhora detectada")
            elif ultima < penultima:
                tendencias.append("📉 Tendência de declínio detectada")
            else:
                tendencias.append("➡️ Desempenho estável")
        
        # Pontos críticos
        pontos_criticos = []
        if metricas.risco_evasao > 30:
            pontos_criticos.append(f"Alto risco de evasão ({metricas.risco_evasao}%)")
        if metricas.taxa_frequencia < 75:
            pontos_criticos.append(f"Taxa de frequência baixa ({metricas.taxa_frequencia}%)")
        if metricas.media_geral < 7.0:
            pontos_criticos.append(f"Média geral abaixo do esperado ({metricas.media_geral})")
        
        # Pontos positivos
        pontos_positivos = []
        if metricas.media_geral >= 7.0:
            pontos_positivos.append("Média geral dentro dos padrões aceitáveis")
        if metricas.taxa_frequencia >= 85:
            pontos_positivos.append("Frequência dentro de padrões adequados")
        if metricas.risco_evasao < 20:
            pontos_positivos.append("Risco de evasão controlado")
        
        # Previsões
        previsoes = []
        if len(tendencias) > 0 and "melhora" in tendencias[0].lower():
            previsoes.append("Expectativa de melhora continuada nos próximos períodos")
        else:
            previsoes.append("Manutenção do nível atual esperado")
        
        if metricas.risco_evasao > 25:
            previsoes.append("Recomenda-se intervenção focada em alunos em risco")
        
        return {
            "resumo_executivo": resumo,
            "status": status,
            "tendencias": tendencias,
            "pontos_criticos": pontos_criticos,
            "pontos_positivos": pontos_positivos,
            "previsoes": previsoes,
            "modo": "Análise Inteligente Local (sem IA)"
        }
    
    def _recomendacoes_inteligente_local(self, alunos: List[DadosAluno], 
                                        alertas: List[Alerta]) -> List[Recomendacao]:
        """Recomendações inteligentes simuladas baseadas em regras (sem IA)"""
        recomendacoes = []
        
        # Análise de alunos em risco
        alunos_risco = [a for a in alunos if a.risco_evasao in ["medio", "alto"]]
        if len(alunos_risco) > 0:
            recomendacoes.append(Recomendacao(
                titulo="Programa de Acompanhamento para Alunos em Risco",
                descricao=f"Implementar programa intensivo de mentoria para {len(alunos_risco)} alunos identificados em risco de evasão",
                categoria="comportamental",
                impacto_estimado="alto",
                acoes=[
                    "Criar parecer de acompanhamento individual",
                    "Aumentar frequência de reuniões com responsáveis",
                    "Oferecer apoio psicossocial"
                ],
                prioridade=1
            ))
        
        # Disciplinas críticas
        disciplinas_criticas = self._identificar_disciplinas_criticas(alunos)
        if disciplinas_criticas:
            recomendacoes.append(Recomendacao(
                titulo="Reforço em Disciplinas com Baixo Desempenho",
                descricao=f"Implementar atividades de reforço em: {', '.join(disciplinas_criticas)}",
                categoria="academico",
                impacto_estimado="alto",
                acoes=[
                    "Aulas de reforço extras",
                    "Tutoria entre pares",
                    "Revisão de metodologia de ensino"
                ],
                prioridade=2
            ))
        
        # Frequência
        alunos_baixa_freq = [a for a in alunos if a.frequencia < 75]
        if alunos_baixa_freq:
            recomendacoes.append(Recomendacao(
                titulo="Programa de Combate à Absenteísmo",
                descricao=f"{len(alunos_baixa_freq)} alunos com frequência inferior a 75%",
                categoria="comportamental",
                impacto_estimado="medio",
                acoes=[
                    "Comunicação com familiares",
                    "Investigação de causas de ausência",
                    "Planejamento de recuperação"
                ],
                prioridade=3
            ))
        
        # Estratégia geral
        recomendacoes.append(Recomendacao(
            titulo="Monitoramento Contínuo de Indicadores",
            descricao="Implementar sistema de monitoramento em tempo real dos indicadores educacionais",
            categoria="estrutural",
            impacto_estimado="medio",
            acoes=[
                "Dashboard de acompanhamento",
                "Relatórios periódicos",
                "Alertas automatizados"
            ],
            prioridade=4
        ))
        
        # Melhoria geral
        recomendacoes.append(Recomendacao(
            titulo="Estratégia de Melhoria Contínua",
            descricao="Desenvolver plano de ação para incrementar qualidade educacional",
            categoria="estrutural",
            impacto_estimado="medio",
            acoes=[
                "Análise periódica de resultados",
                "Feedback de stakeholders",
                "Ajustes metodológicos"
            ],
            prioridade=5
        ))
        
        return sorted(recomendacoes, key=lambda x: x.prioridade)
    
    def _analise_fallback(self, metricas: MetricasGerais, 
                         historico: List[MetricasGerais]) -> Dict[str, Any]:
        """Análise básica sem IA em caso de erro"""
        return {
            "resumo_executivo": f"Sistema com {metricas.total_alunos} alunos e média {metricas.media_geral}",
            "tendencias": ["Análise básica - IA indisponível"],
            "pontos_criticos": [],
            "pontos_positivos": [],
            "previsoes": []
        }
    
    def _recomendacoes_fallback(self, alunos: List[DadosAluno], 
                               alertas: List[Alerta]) -> List[Recomendacao]:
        """Recomendações básicas sem IA"""
        return [
            Recomendacao(
                titulo="Análise Manual Necessária",
                descricao="Sistema IA temporariamente indisponível",
                categoria="sistema",
                impacto_estimado="medio",
                acoes=["Revisar dados manualmente"],
                prioridade=1
            )
        ]
    
    def _previsao_fallback(self, aluno: DadosAluno, 
                          historico_notas: Dict[str, List[float]]) -> Dict[str, Any]:
        """Previsão básica sem IA"""
        previsoes = {}
        for disciplina, notas in historico_notas.items():
            if notas:
                previsoes[disciplina] = sum(notas[-3:]) / len(notas[-3:])
        
        return {
            "previsoes": previsoes,
            "tendencia_geral": "estavel",
            "confianca": 50,
            "fatores_influentes": []
        }


class GerenciadorAlertas:
    """
    Gerencia alertas e detecção de problemas
    """
    
    def __init__(self):
        self.alertas_ativos: List[Alerta] = []
    
    def detectar_alertas(self, alunos: List[DadosAluno], 
                        metricas: MetricasGerais) -> List[Alerta]:
        """
        Detecta alertas baseado nos dados
        
        Args:
            alunos: Lista de alunos
            metricas: Métricas gerais
            
        Returns:
            Lista de alertas detectados
        """
        alertas = []
        timestamp = datetime.now().isoformat()
        
        # Alerta: Alto risco de evasão
        alunos_alto_risco = [a for a in alunos if a.risco_evasao == "alto"]
        if len(alunos_alto_risco) > 0:
            alertas.append(Alerta(
                tipo=TipoAlerta.CRITICO.value,
                titulo="Alto Risco de Evasão",
                descricao=f"{len(alunos_alto_risco)} alunos identificados com padrão de risco elevado",
                prioridade=1,
                alunos_afetados=[a.id for a in alunos_alto_risco],
                timestamp=timestamp
            ))
        
        # Alerta: Baixa frequência
        alunos_baixa_freq = [a for a in alunos if a.frequencia < 75]
        if len(alunos_baixa_freq) > 0:
            alertas.append(Alerta(
                tipo=TipoAlerta.ATENCAO.value,
                titulo="Frequência Crítica",
                descricao=f"{len(alunos_baixa_freq)} alunos com frequência abaixo de 75%",
                prioridade=2,
                alunos_afetados=[a.id for a in alunos_baixa_freq],
                timestamp=timestamp
            ))
        
        # Alerta: Desempenho em declínio
        disciplinas_problematicas = self._analisar_desempenho_disciplinas(alunos)
        if disciplinas_problematicas:
            alertas.append(Alerta(
                tipo=TipoAlerta.ATENCAO.value,
                titulo="Queda de Desempenho",
                descricao=f"Disciplinas com média abaixo de 7.0: {', '.join(disciplinas_problematicas)}",
                prioridade=2,
                alunos_afetados=[],
                timestamp=timestamp
            ))
        
        # Alerta: Melhoria detectada
        if metricas.media_geral >= 8.0:
            alertas.append(Alerta(
                tipo=TipoAlerta.INFO.value,
                titulo="Desempenho Positivo",
                descricao=f"Média geral em {metricas.media_geral} - mantendo padrão de excelência",
                prioridade=5,
                alunos_afetados=[],
                timestamp=timestamp
            ))
        
        self.alertas_ativos = alertas
        return alertas
    
    def _analisar_desempenho_disciplinas(self, alunos: List[DadosAluno]) -> List[str]:
        """Identifica disciplinas com problemas"""
        medias = {}
        for aluno in alunos:
            for disc, nota in aluno.notas.items():
                if disc not in medias:
                    medias[disc] = []
                medias[disc].append(nota)
        
        problematicas = []
        for disc, notas in medias.items():
            media = sum(notas) / len(notas)
            if media < 7.0:
                problematicas.append(disc)
        
        return problematicas


class AnalisadorDesempenho:
    """
    Analisa desempenho por turma e disciplina
    """
    
    @staticmethod
    def calcular_metricas_turma(alunos: List[DadosAluno], turma: str) -> Dict[str, float]:
        """
        Calcula métricas de uma turma específica
        
        Args:
            alunos: Lista de alunos
            turma: Nome da turma
            
        Returns:
            Dicionário com métricas
        """
        alunos_turma = [a for a in alunos if a.turma == turma]
        
        if not alunos_turma:
            return {}
        
        # Média por disciplina
        disciplinas = {}
        for aluno in alunos_turma:
            for disc, nota in aluno.notas.items():
                if disc not in disciplinas:
                    disciplinas[disc] = []
                disciplinas[disc].append(nota)
        
        metricas = {
            "turma": turma,
            "total_alunos": len(alunos_turma),
            "media_geral": sum(a.media_geral() for a in alunos_turma) / len(alunos_turma),
            "frequencia_media": sum(a.frequencia for a in alunos_turma) / len(alunos_turma),
        }
        
        # Adicionar médias por disciplina
        for disc, notas in disciplinas.items():
            metricas[f"media_{disc}"] = sum(notas) / len(notas)
        
        return metricas
    
    @staticmethod
    def gerar_mapa_calor(alunos: List[DadosAluno]) -> Dict[str, Dict[str, float]]:
        """
        Gera mapa de calor de desempenho
        
        Args:
            alunos: Lista de alunos
            
        Returns:
            Estrutura de mapa de calor
        """
        turmas = set(a.turma for a in alunos)
        mapa = {}
        
        for turma in turmas:
            alunos_turma = [a for a in alunos if a.turma == turma]
            disciplinas = {}
            
            for aluno in alunos_turma:
                for disc, nota in aluno.notas.items():
                    if disc not in disciplinas:
                        disciplinas[disc] = []
                    disciplinas[disc].append(nota)
            
            mapa[turma] = {
                disc: sum(notas) / len(notas) 
                for disc, notas in disciplinas.items()
            }
        
        return mapa
    
    @staticmethod
    def calcular_distribuicao_risco(alunos: List[DadosAluno]) -> Dict[str, float]:
        """
        Calcula distribuição de risco de evasão
        
        Args:
            alunos: Lista de alunos
            
        Returns:
            Percentuais por nível de risco
        """
        total = len(alunos)
        if total == 0:
            return {"baixo": 0, "medio": 0, "alto": 0}
        
        baixo = len([a for a in alunos if a.risco_evasao == "baixo"])
        medio = len([a for a in alunos if a.risco_evasao == "medio"])
        alto = len([a for a in alunos if a.risco_evasao == "alto"])
        
        return {
            "baixo": (baixo / total) * 100,
            "medio": (medio / total) * 100,
            "alto": (alto / total) * 100
        }


class SistemaPainelInteligente:
    """
    Sistema principal que integra todos os componentes
    """
    
    def __init__(self, openai_api_key: str):
        """
        Inicializa o sistema
        
        Args:
            openai_api_key: Chave da API OpenAI
        """
        self.analisador_ia = AnalisadorIA(openai_api_key)
        self.gerenciador_alertas = GerenciadorAlertas()
        self.analisador_desempenho = AnalisadorDesempenho()
        self.historico_metricas: List[MetricasGerais] = []
    
    def processar_dados(self, alunos: List[DadosAluno]) -> Dict[str, Any]:
        """
        Processa dados e gera análises completas
        
        Args:
            alunos: Lista de alunos
            
        Returns:
            Relatório completo com todas as análises
        """
        print("🔄 Iniciando processamento de dados...")
        
        # 1. Calcular métricas gerais
        metricas = self._calcular_metricas_gerais(alunos)
        self.historico_metricas.append(metricas)
        print(f"✅ Métricas calculadas: {metricas.total_alunos} alunos")
        
        # 2. Detectar alertas
        alertas = self.gerenciador_alertas.detectar_alertas(alunos, metricas)
        print(f"⚠️  {len(alertas)} alertas detectados")
        
        # 3. Análise com IA
        print("🤖 Gerando análise com IA...")
        analise_ia = self.analisador_ia.analisar_desempenho_geral(
            metricas, 
            self.historico_metricas
        )
        
        # 4. Gerar recomendações
        print("💡 Gerando recomendações...")
        recomendacoes = self.analisador_ia.gerar_recomendacoes(alunos, alertas)
        
        # 5. Análises por turma
        print("📊 Analisando desempenho por turma...")
        turmas = set(a.turma for a in alunos)
        analise_turmas = {
            turma: self.analisador_desempenho.calcular_metricas_turma(alunos, turma)
            for turma in turmas
        }
        
        # 6. Mapa de calor
        mapa_calor = self.analisador_desempenho.gerar_mapa_calor(alunos)
        
        # 7. Distribuição de risco
        dist_risco = self.analisador_desempenho.calcular_distribuicao_risco(alunos)
        
        # 8. Alunos que precisam acompanhamento
        alunos_atencao = [
            a for a in alunos 
            if a.risco_evasao in ["medio", "alto"] or a.frequencia < 75 or a.media_geral() < 6.0
        ]
        
        print("✅ Processamento concluído!")
        
        return {
            "timestamp": datetime.now().isoformat(),
            "metricas_gerais": asdict(metricas),
            "alertas": [asdict(a) for a in alertas],
            "analise_ia": analise_ia,
            "recomendacoes": [asdict(r) for r in recomendacoes],
            "analise_por_turma": analise_turmas,
            "mapa_calor": mapa_calor,
            "distribuicao_risco": dist_risco,
            "alunos_atencao": [asdict(a) for a in alunos_atencao],
            "total_alunos_processados": len(alunos)
        }
    
    def gerar_previsoes(self, aluno: DadosAluno, 
                       historico_notas: Dict[str, List[float]]) -> Dict[str, Any]:
        """
        Gera previsões para um aluno específico
        
        Args:
            aluno: Dados do aluno
            historico_notas: Histórico de notas
            
        Returns:
            Previsões detalhadas
        """
        print(f"🔮 Gerando previsões para {aluno.nome}...")
        previsoes = self.analisador_ia.prever_desempenho(aluno, historico_notas)
        print("✅ Previsões geradas!")
        return previsoes
    
    def exportar_relatorio(self, resultado: Dict[str, Any], 
                          formato: str = "json") -> str:
        """
        Exporta relatório em diferentes formatos
        
        Args:
            resultado: Resultado do processamento
            formato: Formato de saída (json, txt)
            
        Returns:
            String com o relatório
        """
        if formato == "json":
            return json.dumps(resultado, indent=2, ensure_ascii=False)
        
        elif formato == "txt":
            linhas = []
            linhas.append("=" * 80)
            linhas.append("RELATÓRIO DE ANÁLISE ESCOLAR")
            linhas.append("=" * 80)
            linhas.append(f"\nData: {resultado['timestamp']}")
            linhas.append(f"\nTotal de alunos: {resultado['total_alunos_processados']}")
            
            metricas = resultado['metricas_gerais']
            linhas.append(f"\n--- MÉTRICAS GERAIS ---")
            linhas.append(f"Média Geral: {metricas['media_geral']:.2f}")
            linhas.append(f"Taxa de Frequência: {metricas['taxa_frequencia']:.1f}%")
            linhas.append(f"Risco de Evasão: {metricas['risco_evasao']:.1f}%")
            
            linhas.append(f"\n--- ALERTAS ({len(resultado['alertas'])}) ---")
            for alerta in resultado['alertas']:
                linhas.append(f"\n[{alerta['tipo'].upper()}] {alerta['titulo']}")
                linhas.append(f"  {alerta['descricao']}")
            
            linhas.append(f"\n--- ANÁLISE IA ---")
            analise = resultado['analise_ia']
            linhas.append(f"\n{analise.get('resumo_executivo', 'N/A')}")
            
            linhas.append(f"\n--- RECOMENDAÇÕES ({len(resultado['recomendacoes'])}) ---")
            for i, rec in enumerate(resultado['recomendacoes'][:5], 1):
                linhas.append(f"\n{i}. {rec['titulo']}")
                linhas.append(f"   {rec['descricao']}")
            
            linhas.append("\n" + "=" * 80)
            
            return "\n".join(linhas)
        
        return ""
    
    def _calcular_metricas_gerais(self, alunos: List[DadosAluno]) -> MetricasGerais:
        """Calcula métricas gerais da escola"""
        if not alunos:
            return MetricasGerais(0, 0.0, 0.0, 0.0, "atual", datetime.now().isoformat())
        
        media_geral = sum(a.media_geral() for a in alunos) / len(alunos)
        freq_media = sum(a.frequencia for a in alunos) / len(alunos)
        taxa_risco = (len([a for a in alunos if a.risco_evasao == "alto"]) / len(alunos)) * 100
        
        return MetricasGerais(
            total_alunos=len(alunos),
            media_geral=round(media_geral, 2),
            taxa_frequencia=round(freq_media, 1),
            risco_evasao=round(taxa_risco, 1),
            periodo="atual",
            timestamp=datetime.now().isoformat()
        )


# ============================================================================
# EXEMPLO DE USO
# ============================================================================

def exemplo_uso():
    """
    Exemplo completo de como usar o sistema
    Carrega dados reais do sistema ou usa dados de exemplo
    """
    
    # Configurar chave da API (em produção, usar variável de ambiente)
    API_KEY = os.getenv("GOOGLE_API_KEY", "AIzaSyDp_w9MPuY9_5VPl6wunwsk9Ebii9ATII4")
    
    # Inicializar sistema
    print("\n" + "="*80)
    print("INICIALIZANDO SISTEMA PAINEL INTELIGENTE")
    print("="*80 + "\n")
    sistema = SistemaPainelInteligente(API_KEY)
    
    # Tentar carregar dados reais
    print("Carregando dados...\n")
    alunos_exemplo = carregar_dados_desempenho()
    
    # Se não conseguir carregar dados reais, usar dados de exemplo
    if alunos_exemplo is None:
        print("Usando dados de exemplo padrão...\n")
        alunos_exemplo = [
            DadosAluno(
                id="001",
                nome="Bruno Santos",
                turma="3º Ano A",
                notas={"matematica": 6.8, "portugues": 7.2, "ciencias": 6.5},
                frequencia=87.0,
                risco_evasao="medio"
            ),
            DadosAluno(
                id="002",
                nome="Carlos Lima",
                turma="3º Ano A",
                notas={"matematica": 5.2, "portugues": 5.8, "ciencias": 5.0},
                frequencia=72.0,
                risco_evasao="alto"
            ),
            DadosAluno(
                id="003",
                nome="Ana Silva",
                turma="3º Ano B",
                notas={"matematica": 9.0, "portugues": 8.8, "ciencias": 9.2},
                frequencia=95.0,
                risco_evasao="baixo"
            ),
            DadosAluno(
                id="004",
                nome="Beatriz Costa",
                turma="3º Ano B",
                notas={"matematica": 8.5, "portugues": 8.9, "ciencias": 8.2},
                frequencia=92.0,
                risco_evasao="baixo"
            ),
            DadosAluno(
                id="005",
                nome="Diego Pereira",
                turma="3º Ano A",
                notas={"matematica": 4.8, "portugues": 5.0, "ciencias": 4.5},
                frequencia=65.0,
                risco_evasao="alto"
            ),
        ]
    
    # Processar dados
    print("Processando dados...\n")
    resultado = sistema.processar_dados(alunos_exemplo)
    
    # Exibir resultados em texto
    print("\n" + "="*80)
    print("RELATORIO COMPLETO DE ANALISE")
    print("="*80)
    relatorio_txt = sistema.exportar_relatorio(resultado, "txt")
    print(relatorio_txt)
    
    # Exibir análise IA detalhada
    print("\n" + "="*80)
    print("ANALISE IA DETALHADA")
    print("="*80)
    print(json.dumps(resultado['analise_ia'], indent=2, ensure_ascii=False))
    
    # Exibir recomendações
    print("\n" + "="*80)
    print("RECOMENDACOES DO SISTEMA")
    print("="*80)
    for i, rec in enumerate(resultado['recomendacoes'], 1):
        print(f"\n{i}. [{rec['prioridade']}] {rec['titulo']}")
        print(f"   Categoria: {rec['categoria'].upper()}")
        print(f"   Impacto: {rec['impacto_estimado'].upper()}")
        print(f"   Descricao: {rec['descricao']}")
        print(f"   Acoes:")
        for acao in rec['acoes']:
            print(f"      - {acao}")
    
    # Exibir distribuição de risco
    print("\n" + "="*80)
    print("DISTRIBUICAO DE RISCO")
    print("="*80)
    dist = resultado['distribuicao_risco']
    for nivel, percentual in dist.items():
        barra = "=" * int(percentual / 5)
        print(f"   {nivel.upper():6} {barra} {percentual:.1f}%")
    
    # Exibir alunos em atenção
    print("\n" + "="*80)
    print("ALUNOS QUE REQUEREM ATENCAO")
    print("="*80)
    alunos_atencao = resultado['alunos_atencao']
    if alunos_atencao:
        for aluno in alunos_atencao:
            print(f"\n   - {aluno['nome']} (ID: {aluno['id']})")
            print(f"     Turma: {aluno['turma']}")
            print(f"     Frequencia: {aluno['frequencia']:.1f}%")
            print(f"     Risco: {aluno['risco_evasao'].upper()}")
    else:
        print("   OK - Nenhum aluno requer atencao imediata")
    
    # Salvar em arquivo
    print("\n" + "="*80)
    print("SALVANDO RELATORIOS")
    print("="*80)
    
    # Definir caminho de saída
    script_dir = os.path.dirname(os.path.abspath(__file__))
    caminho_relatorios = os.path.join(script_dir, "..", "..", "dados", "relatorio_ia")
    caminho_relatorios = os.path.abspath(caminho_relatorios)
    
    # Criar pasta se não existir
    os.makedirs(caminho_relatorios, exist_ok=True)
    
    # Salvar JSON
    caminho_json = os.path.join(caminho_relatorios, "relatorio_analise.json")
    with open(caminho_json, "w", encoding="utf-8") as f:
        json.dump(resultado, f, indent=2, ensure_ascii=False)
    print(f"OK - Relatorio JSON salvo")
    
    # Salvar TXT
    caminho_txt = os.path.join(caminho_relatorios, "relatorio_analise.txt")
    with open(caminho_txt, "w", encoding="utf-8") as f:
        f.write(relatorio_txt)
    print(f"OK - Relatorio TXT salvo")
    
    print("\n" + "="*80)
    print("PROCESSAMENTO CONCLUIDO COM SUCESSO!")
    print("="*80 + "\n")
    
    return resultado


if __name__ == "__main__":
    try:
        exemplo_uso()
    except Exception as e:
        print(f"ERRO: {e}")
        import traceback
        traceback.print_exc()