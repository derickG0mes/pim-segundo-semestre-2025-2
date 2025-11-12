# 📚 Documentação Completa do Sistema de Gestão Acadêmica

**Versão:** 2.0 | **Data:** 12 de Novembro de 2025

---

## 📋 Índice

1. [Visão Geral do Projeto](#visão-geral-do-projeto)
2. [Arquitetura do Sistema](#arquitetura-do-sistema)
3. [Estrutura de Diretórios](#estrutura-de-diretórios)
4. [Documentação de Módulos](#documentação-de-módulos)
5. [Estruturas de Dados](#estruturas-de-dados)
6. [Arquivos de Configuração JSON](#arquivos-de-configuração-json)
7. [Fluxos de Usuário](#fluxos-de-usuário)
8. [Como Usar o Sistema](#como-usar-o-sistema)

---

## 🎯 Visão Geral do Projeto

### Descrição
Este é um **Sistema de Gestão Acadêmica Integrado** desenvolvido em **C** com suporte a **Python** para recursos de IA. O sistema permite o gerenciamento completo de alunos, professores, turmas, atividades, avaliações e análise de desempenho.

### Objetivos Principais
- ✅ Centralizar informações de alunos e professores
- ✅ Gerenciar turmas, atividades e avaliações
- ✅ Acompanhar desempenho acadêmico em tempo real
- ✅ Fornecer painéis inteligentes com análises de IA
- ✅ Automatizar sincronização de dados em JSON
- ✅ Manter logs detalhados de todas as atividades
- ✅ Oferecer interface amigável para administradores, professores e alunos

### Características Principais
- **Autenticação Multi-nível**: Admin, Professor, Aluno com credenciais únicas
- **Painel Inteligente com IA**: Análise automática de desempenho dos alunos
- **Sincronização JSON Automática**: Todos os dados persistidos em JSON
- **Sistema de Logs Completo**: Rastreamento de todas as ações no sistema
- **Relatórios Dinâmicos**: Gráficos e estatísticas em tempo real
- **Interface UTF-8**: Suporte completo a caracteres especiais

---

## 🏗️ Arquitetura do Sistema

### Padrão de Arquitetura

```
┌─────────────────────────────────────────────────────────┐
│                   INTERFACE DE USUÁRIO                  │
│  (Terminais C + Python para Visualizações Avançadas)   │
└──────────────────┬──────────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────────┐
│                  CAMADA DE NEGÓCIOS                      │
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────┐   │
│  │ Autenticação│  │ Gerenciamento│  │ Processamento│  │
│  │  de Usuários│  │ de Atividades│  │  de Dados    │  │
│  └─────────────┘  └──────────────┘  └─────────────┘   │
└──────────────────┬──────────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────────┐
│             CAMADA DE DADOS (auto_sync.c)               │
│  ┌──────────────────────────────────────────────────┐  │
│  │  Sincronização Automática de JSON               │  │
│  │  - Validação de dados                           │  │
│  │  - Retry automático                             │  │
│  │  - Fila de sincronização                        │  │
│  │  - Log de operações                             │  │
│  └──────────────────────────────────────────────────┘  │
└──────────────────┬──────────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────────┐
│                ARQUIVOS JSON (Persistência)             │
│  - Credenciais de Admin                                 │
│  - Cadastros de Alunos, Professores                     │
│  - Turmas e Informações                                 │
│  - Atividades e Avaliações                              │
│  - Desempenho dos Alunos                                │
│  - Logs de Atividade                                    │
└─────────────────────────────────────────────────────────┘
```

---

## 📁 Estrutura de Diretórios

### Raiz do Projeto
```
PIM-2-SEMESTRE/
├── codigos/                          # Código-fonte C e Python
│   ├── administrador/                # Módulos administrativos
│   ├── alunos/                       # Módulos para alunos
│   ├── professores/                  # Módulos para professores
│   ├── turmas/                       # Módulos de gerenciamento de turmas
│   ├── ferramentas/                  # Ferramentas e utilitários
│   │   ├── auto_sync.c              # Sistema de sincronização JSON
│   │   ├── filtro_de_busca/         # Filtro de busca em Python
│   │   ├── filtro_historico/        # Filtro histórico em Python
│   │   └── painel_inteligente/      # Painel IA em Python
│   └── dados/                        # Dados relativos aos códigos
│       └── relatorio_ia/            # Relatórios de IA
│
├── dados/                            # Arquivos de dados JSON
│   ├── adm-credencias/              # Credenciais de administradores
│   ├── alunos_cadastros/            # Cadastros e desempenho de alunos
│   ├── atividades_e_avaliacoes/    # Conteúdo de atividades e avaliações
│   ├── desempenho_alunos_ia/       # Dados de desempenho processados por IA
│   ├── professores_cadastros/      # Cadastros de professores
│   ├── turmas_cadastros/           # Cadastros de turmas
│   ├── turmas_informacoes/         # Informações de turmas
│   ├── relatorio_ia/               # Relatórios de análise de IA
│   └── logs_atividade.json         # Log central de atividades
│
├── .vscode/                         # Configurações do VS Code
└── DOCUMENTACAO_COMPLETA.md         # Este arquivo

```

---

## 📚 Documentação de Módulos

### 1️⃣ Módulo de Administrador

#### 📄 `administrador/login_adm.c`

**Responsabilidade**: Autenticação de administradores no sistema

**Funções Principais**:
- `encontrar_arquivo_credenciais()` - Localiza arquivo de credenciais
- `verificar_credenciais(email, senha)` - Valida credenciais do admin
- `auto_log()` - Registra login no arquivo de logs (via auto_sync)

**Fluxo**:
1. Solicita email e senha do administrador
2. Valida contra arquivo `adm_credenciais.json`
3. Se válido, registra no log e acessa painel administrativo
4. Se inválido, tenta novamente com limite de tentativas

**Estrutura JSON Esperada** (`adm_credenciais.json`):
```json
{
  "email": "admin@escola.com",
  "senha": "senha_segura_123",
  "nome": "Administrador Principal",
  "ativo": true
}
```

---

#### 📄 `administrador/painel_adm.c`

**Responsabilidade**: Painel de controle central para administradores

**Estruturas de Dados**:
```c
typedef struct {
    char ra_ou_usuario[50];
    char nome[150];
    char tipo_usuario[30];      // "Aluno", "Professor", "Admin"
    char tipo_atividade[50];    // "Login", "Criação", "Resposta"
    char data[20];
    char hora[20];
    char descricao[300];
} LogAtividade;

typedef struct {
    int total_alunos;
    int total_professores;
    int total_admins;
    int total_turmas;
    int atividades_postadas_hoje;
    int atividades_postadas_total;
    int logins_hoje;
    float taxa_uso_diaria;
    int crescimento_usuarios;
} Estatisticas;
```

**Funcionalidades Principais**:

| Função | Descrição |
|--------|-----------|
| `exibir_dashboard_principal()` | Mostra resumo geral do sistema |
| `exibir_log_completo()` | Lista todas as atividades registradas |
| `exibir_atividade_usuario_especifico()` | Busca atividades de um usuário |
| `exibir_atividades_dia()` | Mostra atividades do dia atual |
| `exibir_grafico_uso_horario()` | Gráfico de uso por hora |
| `menu_gerenciar_usuarios()` | Gerencia alunos e professores |
| `menu_gerenciar_atividades()` | Gerencia atividades |
| `calcular_estatisticas()` | Calcula métricas do sistema |

**Menu Principal**:
```
[1] Ver Log Completo de Atividades
[2] Ver Atividades de Um Usuário Específico
[3] Ver Atividades do Dia
[4] Ver Gráfico de Uso Horário
[5] Gerenciar Usuários
[6] Gerenciar Atividades
[7] 🤖 Painel Inteligente com IA
[0] Sair
```

---

### 2️⃣ Módulo de Alunos

#### 📄 `alunos/area_login_aluno.c`

**Responsabilidade**: Autenticação e gerenciamento de conta de alunos

**Funções Principais**:
- `localizar_arquivo_json(out_path, out_size)` - Encontra arquivo de cadastro
- `encontrar_senha_por_login(login, senha_out, tamanho)` - Busca senha do aluno
- `buscar_dados_aluno_por_login(login, dados_out)` - Busca dados do aluno
- `pedir_e_validar_senha(senha_esperada, max_tentativas)` - Valida senha
- `solicitar_redefinicao(login)` - Permite redefinir senha
- `criar_desempenho_aluno(dados, serie)` - Cria perfil de desempenho

**Estrutura de Dados**:
```c
typedef struct {
    char nome[200];
    char ra[20];
    char matricula[50];
} DadosAlunoSimples;
```

**Fluxo de Login**:
1. Solicita CPF, Matrícula, RA ou Email
2. Confirma o login digitado
3. Localiza senha no JSON
4. Valida senha com até 5 tentativas
5. Cria perfil de desempenho se necessário
6. Registra login no arquivo de logs
7. Acessa área do aluno

**Recursos de Segurança**:
- ✅ Validação de campos vazios
- ✅ Limite de 5 tentativas
- ✅ Opção de redefinir senha
- ✅ Código de verificação (simulado)
- ✅ Backup automático de arquivo

---

#### 📄 `alunos/area_aluno.c`

**Responsabilidade**: Dashboard e gerenciamento da área do aluno

**Estruturas de Dados**:
```c
typedef struct {
    char ra[20];
    char nome[MAX_NOME];
    char serie[MAX_SERIE];
    char matricula[MAX_MATRICULA];
    float media_geral;
    float frequencia;
    char situacao[30];
    int atividades_concluidas;
    int atividades_pendentes;
    int mensagens_nao_lidas;
} DadosAluno;

typedef struct {
    int id_atividade;
    char titulo[200];
    char disciplina[MAX_DISCIPLINA];
    char tipo[20];           // "Atividade" ou "Avaliacao"
    char status[20];         // "Pendente", "Concluída"
    char data_postagem[20];
    char data_vencimento[20];
    char professor[MAX_NOME];
    int respondido;
    char data_entrega[20];
    float pontuacao;
} AtividadeAluno;
```

**Funcionalidades**:

| Função | Descrição |
|--------|-----------|
| `exibir_dashboard_aluno()` | Mostra informações pessoais e desempenho |
| `carregar_dados_aluno(ra)` | Carrega dados do aluno do JSON |
| `carregar_atividades_aluno(ra)` | Carrega atividades do aluno |
| `exibir_todas_atividades()` | Lista todas as atividades |
| `exibir_atividades_pendentes()` | Lista apenas pendentes |
| `responder_atividade(ra, indice)` | Interface para responder atividade |
| `atualizar_desempenho_aluno()` | Atualiza nota e desempenho |
| `atualizar_status_atividade()` | Marca atividade como concluída |

**Menu Principal do Aluno**:
```
[1] Ver todas as atividades
[2] Ver atividades pendentes
[3] Ver mensagens
[4] Responder atividade
[5] Voltar
```

**Fluxo de Resposta de Atividade**:
1. Exibe questões da atividade
2. Aluno seleciona alternativas (A, B, C, etc.)
3. Sistema valida respostas
4. Calcula pontuação: (acertos × 10) / total de questões
5. Atualiza desempenho do aluno
6. Registra resposta no arquivo de logs
7. Atualiza desempenho na turma

---

#### 📄 `alunos/area_cadastro_aluno.c`

**Responsabilidade**: Gerenciamento de cadastro de novos alunos

*(Arquivo não detalhado, mas seguindo padrão similar)*

---

### 3️⃣ Módulo de Professores

#### 📄 `professores/area_login_professor.c`

**Responsabilidade**: Autenticação de professores

**Funções Principais** (similares aos alunos):
- `localizar_arquivo_json(out_path, out_size)` - Encontra cadastro
- `encontrar_senha_por_login(login, senha_out, tamanho)` - Busca senha
- `pedir_e_validar_senha(senha_esperada, max_tentativas)` - Valida
- `solicitar_redefinicao(login)` - Redefinir senha
- `lerEntradaSegura(destino, tamanho, mensagem)` - Leitura segura

**Fluxo**:
1. Solicita email ou matrícula
2. Valida senha contra arquivo
3. Permite até 3 tentativas
4. Se sucesso, registra no log
5. Acessa painel do professor

**Estrutura JSON** (`cadastro_professor.json`):
```json
{
  "nome": "João Silva",
  "email": "joao@escola.com",
  "ra": "PROF001",
  "senha": "senha_segura",
  "disciplinas": ["Matemática", "Física"],
  "ativo": true
}
```

---

#### 📄 `professores/painel_atividade_avalicoes.c`

**Responsabilidade**: Gerenciamento de atividades e avaliações por professores

*(Arquivo em desenvolvimento com funcionalidades avançadas)*

---

#### 📄 `professores/historico_atividades_avaliacoes.c`

**Responsabilidade**: Manutenção de histórico de atividades

*(Arquivo em desenvolvimento)*

---

### 4️⃣ Módulo de Turmas

#### 📄 `turmas/area_cadastro_turmas.c`

**Responsabilidade**: Gerenciamento de cadastro de turmas

*(Arquivo em desenvolvimento)*

---

### 5️⃣ Módulo de Ferramentas

#### 📄 `ferramentas/auto_sync.c` ⭐ **CRÍTICO**

**Responsabilidade**: Sistema de sincronização automática de JSON

**Objetivo Principal**: Garantir 100% de automação de envios para JSON sem risco de quebra

**Componentes Principais**:

```c
typedef struct {
    char arquivo[512];          // Caminho do arquivo JSON
    char dados[2048];           // Dados a enviar (JSON)
    int tipo_operacao;          // 0=criar, 1=adicionar, 2=atualizar
    int tentativas;             // Contador de tentativas
    int maximas_tentativas;     // Máximo de tentativas
    int sucesso;                // 1 se bem-sucedido
} FilaSync;
```

**Funções Principais**:

| Função | Descrição | Uso |
|--------|-----------|-----|
| `validar_json(json)` | Valida sintaxe JSON | Antes de salvar |
| `criar_diretorios_necessarios(caminho)` | Cria diretórios | Auto na sincronização |
| `auto_enviar_json()` | Sincroniza dados | Operação universal |
| `auto_log()` | Registra log | Auto em toda ação |
| `auto_atualizar_desempenho()` | Atualiza desempenho | Após responder atividade |
| `finalizar_sistema()` | Sincroniza dados pendentes | Antes de sair |

**Características**:
- ✅ **Validação JSON**: Verifica sintaxe antes de salvar
- ✅ **Fila de Sincronização**: Armazena dados se falhar
- ✅ **Retry Automático**: Tenta novamente até 5 vezes
- ✅ **Proteção contra Falhas**: Cria diretórios se necessário
- ✅ **Log de Operações**: Rastreia todas as sincronizações
- ✅ **Backup Automático**: Cria `.bak` antes de sobrescrever

**Exemplo de Uso**:
```c
// Registrar login no arquivo de logs
auto_log("dados/logs_atividade.json", "001", "Login", "Aluno realizou login");

// Enviar dados para JSON
const char *chaves[] = {"nome", "ra", "desempenho"};
const char *valores[] = {"João Silva", "001", "Excelente"};
auto_enviar_json("dados/turmas.json", chaves, valores, 3, 1);

// Atualizar desempenho na turma
auto_atualizar_desempenho("dados/turmas_informacoes.json", 
                          "001", "João Silva", "1º Ano", 8.5);

// Finalizar sistema antes de sair
finalizar_sistema();
```

---

#### 📄 `ferramentas/filtro_de_busca/FiltroDeBusca.py`

**Responsabilidade**: Filtro inteligente de busca em Python

**Funcionalidades**:
- Busca por múltiplos critérios
- Resultados ordenados e relevantes
- Integração com dados JSON

---

#### 📄 `ferramentas/filtro_historico/Filtro_historico.py`

**Responsabilidade**: Análise de histórico de atividades

**Funcionalidades**:
- Filtra atividades por período
- Análise de padrões
- Geração de estatísticas históricas

---

#### 📄 `ferramentas/painel_inteligente/painel_ia.py`

**Responsabilidade**: Painel inteligente com análises de IA

**Funcionalidades**:
- Análise automática de desempenho
- Predição de dificuldades
- Recomendações personalizadas
- Geração de relatórios de IA

---

## 📊 Estruturas de Dados

### Estrutura Log de Atividades
```c
typedef struct {
    char ra_ou_usuario[50];
    char nome[150];
    char tipo_usuario[30];      // "Aluno", "Professor", "Admin"
    char tipo_atividade[50];    // "Login", "Criação", "Resposta"
    char data[20];              // YYYY-MM-DD
    char hora[20];              // HH:MM:SS
    char descricao[300];
} LogAtividade;
```

### Estrutura Questão de Atividade
```c
typedef struct {
    char texto[500];
    char alternativas[5][100];
    int num_alternativas;
    char resposta_correta[2];   // "A", "B", "C", etc.
} Questao;
```

### Estrutura Atividade Completa
```c
typedef struct {
    int id;
    char titulo[200];
    char enunciado[500];
    int id_turma;
    char data_criacao[20];
    char data_expiracao[20];
    Questao questoes[50];
    int num_questoes;
} Atividade;
```

---

## 📝 Arquivos de Configuração JSON

### 1. `adm-credencias/adm_credenciais.json`

**Descrição**: Armazena credenciais de administradores

**Exemplo**:
```json
[
  {
    "email": "admin@escola.com",
    "senha": "admin123",
    "nome": "Administrador Principal",
    "ativo": true
  }
]
```

---

### 2. `alunos_cadastros/cadastro_aluno.json`

**Descrição**: Cadastro completo de alunos

**Exemplo**:
```json
[
  {
    "nome": "João Silva",
    "cpf": "123.456.789-00",
    "numeroMatricula": "2024001",
    "ra": "001",
    "email": "joao@escola.com",
    "senha": "senha123",
    "serie": "1º Ano",
    "turma": "A",
    "ativo": true,
    "data_cadastro": "2024-01-15"
  }
]
```

---

### 3. `alunos_cadastros/desempenho_alunos.json`

**Descrição**: Desempenho acadêmico de cada aluno

**Exemplo**:
```json
[
  {
    "ra": "001",
    "nome": "João Silva",
    "serie": "1º Ano",
    "matricula": "2024001",
    "media_geral": 7.5,
    "frequencia": 95.5,
    "situacao": "Ativo",
    "atividades_concluidas": 12,
    "atividades_pendentes": 3,
    "mensagens_nao_lidas": 2
  }
]
```

---

### 4. `alunos_cadastros/atividades_aluno.json`

**Descrição**: Atividades atribuídas a cada aluno

**Exemplo**:
```json
[
  {
    "id_aluno_ra": "001",
    "id_atividade": 1,
    "titulo": "Exercício de Linguagem C",
    "disciplina": "Programação",
    "tipo": "Atividade",
    "status": "Concluída",
    "data_postagem": "2024-11-01",
    "data_vencimento": "2024-11-08",
    "professor": "Prof. Carlos",
    "respondido": true,
    "data_entrega": "2024-11-06",
    "pontuacao": 8.5
  }
]
```

---

### 5. `atividades_e_avaliacoes/conteudo_atividades.json`

**Descrição**: Conteúdo das atividades (questões, enunciados, etc.)

**Exemplo**:
```json
[
  {
    "id": 1,
    "titulo": "Exercício de Linguagem C",
    "enunciado": "Resolva os exercícios propostos sobre ponteiros em C",
    "id_turma": 1,
    "data_criacao": "2024-11-01",
    "data_expiracao": "2024-11-08",
    "questoes": [
      {
        "numero": 1,
        "texto": "O que é um ponteiro?",
        "alternativas": ["A) Uma variável que armazena um endereço de memória", "B) Uma função"],
        "resposta_correta": "A"
      }
    ]
  }
]
```

---

### 6. `atividades_e_avaliacoes/conteudo_avaliacoes.json`

**Descrição**: Conteúdo das avaliações formais

---

### 7. `turmas_cadastros/cadastro_turma.json`

**Descrição**: Cadastro de turmas

**Exemplo**:
```json
[
  {
    "id": 1,
    "nome": "1º Ano A",
    "serie": "1º Ano",
    "professor_responsavel": "Prof. João",
    "periodo": "Vespertino",
    "total_alunos": 30,
    "data_criacao": "2024-01-10",
    "ativo": true
  }
]
```

---

### 8. `turmas_informacoes/turmas_informacoes.json`

**Descrição**: Informações de desempenho por turma

**Exemplo**:
```json
{
  "1º Ano A": [
    {
      "nome": "João Silva",
      "ra": "001",
      "desempenho": "Excelente"
    },
    {
      "nome": "Maria Santos",
      "ra": "002",
      "desempenho": "Bom"
    }
  ]
}
```

---

### 9. `logs_atividade.json`

**Descrição**: Log central de todas as atividades do sistema

**Exemplo**:
```json
[
  {
    "ra_ou_usuario": "001",
    "nome": "João Silva",
    "tipo_usuario": "Aluno",
    "tipo_atividade": "Login",
    "data": "2024-11-12",
    "hora": "14:30",
    "descricao": "Aluno realizou login na conta"
  },
  {
    "ra_ou_usuario": "001",
    "nome": "João Silva",
    "tipo_usuario": "Aluno",
    "tipo_atividade": "Resposta",
    "data": "2024-11-12",
    "hora": "14:35",
    "descricao": "Aluno respondeu atividade: Exercício C (Pontuação: 8.5)"
  }
]
```

---

### 10. `relatorio_ia/relatorio_analise.json` / `.txt`

**Descrição**: Relatórios de análise gerados por IA

---

## 🔄 Fluxos de Usuário

### 📍 Fluxo: Administrador

```
┌─────────────────────┐
│   INICIAR SISTEMA   │
└──────────┬──────────┘
           │
           ▼
┌──────────────────────┐
│ TELA DE LOGIN ADMIN  │
│ (email + senha)      │
└──────────┬───────────┘
           │
    ✅ Válido?
    │      │
    │      ▼ ❌
    │   ERRO
    │   (Retry max 5x)
    │
    ▼
┌──────────────────────────┐
│ PAINEL ADMINISTRATIVO    │
│ - Dashboard              │
│ - Ver Logs               │
│ - Gerenciar Usuários     │
│ - Gerenciar Atividades   │
│ - Painel IA              │
└──────────┬───────────────┘
           │
    ┌──────▼─────────┐
    │ Operação       │
    │ Selecionada    │
    └──────┬─────────┘
           │
    ┌──────▼─────────────────────────┐
    │ Sincronizar dados (auto_sync)  │
    │ Registrar no logs              │
    └──────┬──────────────────────────┘
           │
    ┌──────▼──────┐
    │ Sair?       │
    │ Sim/Não     │
    └──────┬──────┘
           │
     Não ──┴──► Voltar ao Menu
     │
     Sim
     │
     ▼
┌──────────────────────┐
│ SINCRONIZAR DADOS    │
│ FINALIZAR SISTEMA    │
└──────────────────────┘
```

---

### 📍 Fluxo: Aluno

```
┌─────────────────────────────────────┐
│   INICIAR SISTEMA - LOGIN ALUNO     │
└────────────────┬────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────┐
│ SOLICITAR LOGIN                     │
│ (CPF, Matrícula, RA ou Email)      │
└────────────────┬────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────┐
│ CONFIRMAR LOGIN                     │
└────────────────┬────────────────────┘
                 │
        ✅ Confirmado?
        │         │
        │         ▼ ❌
        │      CANCELAR
        │      Retry
        │
        ▼
┌─────────────────────────────────────┐
│ LOCALIZAR DADOS DO ALUNO            │
│ No arquivo cadastro_aluno.json      │
└────────────────┬────────────────────┘
                 │
        ✅ Encontrado?
        │         │
        │         ▼ ❌
        │      ERRO - Retry
        │
        ▼
┌─────────────────────────────────────┐
│ SOLICITAR SENHA                     │
│ (máx 5 tentativas)                 │
└────────────────┬────────────────────┘
                 │
        ✅ Válida?
        │         │
        │         ▼ ❌
        │      RETRY
        │      (até 5x)
        │
        ▼
┌─────────────────────────────────────┐
│ CARREGAR DADOS DE DESEMPENHO        │
│ CARREGAR ATIVIDADES                 │
└────────────────┬────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────┐
│ DASHBOARD DO ALUNO                  │
│ - Informações pessoais              │
│ - Desempenho acadêmico              │
│ - Atividades recentes               │
└────────────────┬────────────────────┘
                 │
        ┌────────▼────────────────┐
        │ Menu de Opções          │
        │ 1. Ver todas atividades │
        │ 2. Ver pendentes        │
        │ 3. Ver mensagens        │
        │ 4. Responder atividade  │
        │ 5. Voltar               │
        └────────┬────────────────┘
                 │
        ┌────────▼──────────┐
        │ Opção 4           │
        │ Responder         │
        │ Atividade         │
        └────────┬──────────┘
                 │
                 ▼
    ┌──────────────────────────────┐
    │ Listar atividades pendentes  │
    │ Aluno seleciona uma          │
    └──────────┬───────────────────┘
               │
               ▼
    ┌──────────────────────────────┐
    │ Exibir questões              │
    │ Aluno responde (A, B, C...)  │
    └──────────┬───────────────────┘
               │
               ▼
    ┌──────────────────────────────┐
    │ Calcular pontuação           │
    │ Acertos × 10 / Total         │
    └──────────┬───────────────────┘
               │
               ▼
    ┌──────────────────────────────┐
    │ Atualizar:                   │
    │ - Status atividade           │
    │ - Desempenho aluno           │
    │ - Desempenho turma           │
    │ - Log de atividade           │
    └──────────┬───────────────────┘
               │
               ▼
    ┌──────────────────────────────┐
    │ Sincronizar com JSON         │
    │ (auto_sync)                  │
    └──────────┬───────────────────┘
               │
               ▼
    ┌──────────────────────────────┐
    │ Exibir resultado             │
    │ Acertos, Erros, Pontuação   │
    └──────────┬───────────────────┘
```

---

## 🚀 Como Usar o Sistema

### Pré-requisitos
- **Compilador GCC** instalado (para código C)
- **Python 3.x** instalado (para módulos de IA)
- **Terminal/Prompt de Comando** com suporte a UTF-8
- **Estrutura de diretórios** conforme especificado

### Instalação

#### 1. Windows (MSYS2 com GCC)
```bash
# Se não tiver GCC, instale via MSYS2
# https://www.msys2.org/

# No terminal MSYS2:
pacman -S mingw-w64-ucrt64-gcc
```

#### 2. Linux
```bash
sudo apt-get install gcc python3
```

### Compilação

#### Login do Administrador
```bash
cd codigos/administrador
gcc -o login_adm login_adm.c
./login_adm
```

#### Login do Aluno
```bash
cd codigos/alunos
gcc -o login_aluno area_login_aluno.c
./login_aluno
```

#### Login do Professor
```bash
cd codigos/professores
gcc -o login_prof area_login_professor.c
./login_prof
```

### Execução

#### Via Terminal VS Code
```bash
# Build task já configurado
Ctrl + Shift + B  # Compila
F5                 # Executa com Debug
Ctrl + F5          # Executa sem Debug
```

#### Via Terminal Direto
```bash
# Compilar e executar
gcc -o programa arquivo.c && ./programa

# No Windows (PowerShell)
gcc -o programa arquivo.c; .\programa.exe
```

### Primeiro Acesso

#### Credenciais de Admin (Padrão)
```
Email: admin@escola.com
Senha: admin123
```

#### Dados de Teste (Alunos)
```
RA: 3123131 (ou outro RA no arquivo)
CPF/Matrícula/Email: conforme cadastro_aluno.json
Senha: [conforme cadastro_aluno.json]
```

---

## 🔐 Segurança

### Recursos de Segurança Implementados

✅ **Autenticação Multi-nível**
- Credenciais únicas por usuário
- Validação contra JSON
- Limite de tentativas (3-5)

✅ **Proteção de Dados**
- Backup automático de arquivos (`.bak`)
- Validação JSON antes de salvar
- Fila de sincronização com retry

✅ **Auditoria**
- Log completo de atividades
- Rastreamento de operações críticas
- Data/hora de cada ação

✅ **Tratamento de Erros**
- Validação de campos vazios
- Verificação de limites de arrays
- Overflow protetion com `strncpy`

### Recomendações de Segurança

⚠️ **Para Produção**:
1. Adicionar criptografia de senhas (hash SHA-256)
2. Implementar tokens JWT
3. Validação de entrada contra SQL Injection (mesmo com JSON)
4. HTTPS para transmissão de dados
5. Dupla autenticação (2FA)
6. Permissões de arquivo (chmod 600 para JSONs sensíveis)

---

## 📈 Análise de IA

### Painel Inteligente (`painel_ia.py`)

O painel IA oferece:
- **Análise de Desempenho**: Identifica alunos em risco
- **Predição**: Estima desempenho futuro
- **Recomendações**: Sugere ações personalizadas
- **Relatórios**: Gera análises automáticas

### Relatórios de IA

Armazenados em `dados/relatorio_ia/`:
- `relatorio_analise.json`: Dados estruturados de análise
- `relatorio_analise.txt`: Relatório legível por humanos

---

## 🐛 Troubleshooting

### Problema: "Arquivo não encontrado"
**Solução**: Verifique se os arquivos JSON existem em `dados/`

### Problema: "Erro ao abrir arquivo"
**Solução**: Ajuste os caminhos relativos conforme sua posição no terminal

### Problema: "Caracteres corrompidos"
**Solução**: Configure UTF-8: `chcp 65001` (Windows) ou `export LANG=pt_BR.UTF-8` (Linux)

### Problema: "Falha na sincronização JSON"
**Solução**: Verifique se o JSON é válido e se há espaço em disco

### Problema: "Senha não encontrada"
**Solução**: Verifique se o usuário existe em `cadastro_aluno.json` ou `cadastro_professor.json`

---

## 📞 Suporte e Contribuições

Para dúvidas, bugs ou sugestões:
1. Verifique a documentação
2. Consulte os comentários no código
3. Execute com debug mode
4. Verifique logs em `dados/logs_atividade.json`

---

## 📄 Licença e Informações

**Projeto**: Sistema de Gestão Acadêmica Integrado  
**Versão**: 2.0  
**Data de Criação**: 2024  
**Data de Última Atualização**: 12 de Novembro de 2025  
**Linguagens**: C, Python  
**Status**: Desenvolvimento Ativo

---

## 🎓 Conclusão

Este sistema oferece uma solução completa para gestão acadêmica com:
- ✅ Interface intuitiva para todos os usuários
- ✅ Sincronização automática de dados
- ✅ Análises inteligentes com IA
- ✅ Auditoria completa de atividades
- ✅ Escalabilidade para múltiplos usuários

Aproveite plenamente a plataforma para melhorar o acompanhamento acadêmico!

---

**Documentação Completa - v2.0 | 12 de Novembro de 2025**

