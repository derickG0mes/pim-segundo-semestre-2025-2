# Estrutura de Dados - Sistema Painel Inteligente

## 📁 Pastas e Arquivos

### dados/desempenho_alunos_ia/
Pasta contendo dados de desempenho dos alunos em formato JSON.

**Arquivo: desempenho.json**
```json
[
  {
    "ra": "3123131",                    // RA do aluno
    "nome": "Rivaldo",                  // Nome completo
    "turma": "1 ano, Ensino Superior",  // Turma/série
    "notas": {                          // Notas por disciplina
      "matematica": 8.0,
      "portugues": 7.5,
      "ciencias": 7.8,
      "programacao": 9.0
    },
    "frequencia": 85.0,                 // Percentual de presença
    "risco_evasao": "baixo",            // Nível de risco: baixo, medio, alto
    "atividades_concluidas": 2,         // Atividades completadas
    "atividades_pendentes": 1,          // Atividades não entregues
    "media_atividades": 9.25            // Média das atividades
  }
]
```

### dados/relatorio_ia/
Pasta contendo relatórios gerados pelo sistema de análise.

**Arquivos gerados:**
- `relatorio_analise.json` - Dados estruturados completos
- `relatorio_analise.txt` - Relatório formatado para leitura
- `README.md` - Documentação dos relatórios

## 🔄 Fluxo de Dados

```
desempenho_alunos_ia/desempenho.json
            ↓
    painel_ia.py (carregar_dados_desempenho)
            ↓
    SistemaPainelInteligente.processar_dados()
            ↓
    Gera análise, alertas, recomendações
            ↓
    relatorio_ia/ (salva resultados)
```

## 📊 Como Adicionar Novos Alunos

Edite `dados/desempenho_alunos_ia/desempenho.json` e adicione um novo objeto na lista:

```json
{
  "ra": "2024004",
  "nome": "Novo Aluno",
  "turma": "1 ano, Ensino Superior",
  "notas": {
    "matematica": 7.0,
    "portugues": 7.5,
    "ciencias": 7.8,
    "programacao": 8.0
  },
  "frequencia": 90.0,
  "risco_evasao": "baixo",
  "atividades_concluidas": 3,
  "atividades_pendentes": 0,
  "media_atividades": 9.0
}
```

## 🎯 Executar Análise

```bash
cd codigos\ferramentas\painel_inteligente
python painel_ia.py
```

O sistema irá:
1. Carregar dados de `dados/desempenho_alunos_ia/desempenho.json`
2. Processar e analisar
3. Gerar alertas e recomendações
4. Salvar relatórios em `dados/relatorio_ia/`

## 📝 Campos Opcionais

Nem todos os campos são obrigatórios:
- `atividades_concluidas` - Padrão: 0
- `atividades_pendentes` - Padrão: 0
- `media_atividades` - Padrão: 0.0

## ⚠️ Níveis de Risco Válidos

- `"baixo"` - Aluno com bom desempenho e frequência
- `"medio"` - Aluno com desempenho ou frequência intermediária
- `"alto"` - Aluno com risco iminente de evasão

---

**Versão**: 1.0  
**Data**: 12/11/2025
