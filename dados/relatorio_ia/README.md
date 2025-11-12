# Sistema Painel Inteligente IA - Relatórios

Esta pasta contém os relatórios gerados pelo Sistema Painel Inteligente de Análise Escolar com IA.

## 📁 Estrutura

```
dados/
├── relatorio_ia/
│   ├── relatorio_analise.json    # Dados estruturados em JSON
│   ├── relatorio_analise.txt     # Relatório formatado em texto
│   └── README.md                 # Este arquivo
```

## 📄 Arquivos

### relatorio_analise.json
Arquivo com todos os dados da análise em formato JSON estruturado:
- **Métricas gerais**: Total de alunos, média geral, taxa de frequência, risco de evasão
- **Alertas**: Alertas críticos, de atenção e informativos
- **Análise IA**: Resumo executivo, tendências, pontos críticos e positivos, previsões
- **Recomendações**: 5 recomendações priorizadas com ações específicas
- **Análise por turma**: Métricas desagregadas por turma
- **Mapa de calor**: Visualização de desempenho por turma e disciplina
- **Distribuição de risco**: Percentual de alunos por nível de risco
- **Alunos em atenção**: Lista de alunos que requerem acompanhamento

### relatorio_analise.txt
Relatório formatado em texto simples para fácil leitura:
- Resumo das métricas gerais
- Lista de alertas
- Análise IA
- Recomendações principais (top 5)

## 🔄 Como usar

### Executar análise completa
```bash
cd codigos\ferramentas\painel_inteligente
python teste_completo.py
```

### Testar estruturas básicas
```bash
python teste_painel.py
```

## 🤖 Modos de Funcionamento

### Modo IA (com créditos OpenAI)
```powershell
$env:OPENAI_API_KEY = "sua-chave-aqui"
python teste_completo.py
```

### Modo Local (sem custos)
```powershell
python teste_completo.py
# Usa análise inteligente simulada baseada em regras
```

## 📊 Dados Inclusos

Os relatórios analisam:

- **Alunos**: ID, nome, turma, notas por disciplina, frequência, risco de evasão
- **Disciplinas**: Matemática, Português, Ciências (personalizável)
- **Turmas**: Agrupamento por série/turma
- **Períodos**: Histórico de métricas para análise de tendências

## 🎯 Uso Recomendado

1. **Geradores**: Use os relatórios JSON para integração com sistemas
2. **Diretores**: Use o TXT para discussões em reuniões
3. **Professores**: Consulte para planejar interventções focadas
4. **Gestores**: Monitore tendências ao longo do tempo

## 📝 Atualização

Os relatórios são sobrescritos a cada execução. Para manter histórico:
```powershell
# Copie para pasta com timestamp
cp relatorio_analise.json "relatorio_analise_$(Get-Date -f 'yyyyMMdd_HHmmss').json"
```

## ⚙️ Configuração

Para personalizar:
- Edite `teste_completo.py` para adicionar mais alunos
- Modifique `painel_ia.py` para ajustar regras de análise
- Configure disciplinas no objeto `DadosAluno`

---

**Versão**: 1.0  
**Data**: 12/11/2025  
**Modo**: Análise Inteligente Local
