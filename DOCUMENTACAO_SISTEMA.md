# 📚 DOCUMENTAÇÃO COMPLETA - SISTEMA DE GESTÃO ESCOLAR

**Versão:** 1.0  
**Data:** 23 de Novembro de 2025  
**Status:** ✅ Produção

---

## 📑 Índice

1. [Visão Geral](#visão-geral)
2. [Arquitetura do Sistema](#arquitetura-do-sistema)
3. [Módulos Disponíveis](#módulos-disponíveis)
4. [Como Iniciar](#como-iniciar)
5. [Guia de Uso por Perfil](#guia-de-uso-por-perfil)
6. [Estrutura de Dados](#estrutura-de-dados)
7. [Fluxos de Funcionamento](#fluxos-de-funcionamento)
8. [Troubleshooting](#troubleshooting)

---

## 🎯 Visão Geral

### O que é o Sistema?

O **Sistema de Gestão Escolar** é uma aplicação integrada desenvolvida em **C** para Windows que oferece uma solução completa para gerenciar:

- ✅ Cadastro e login de alunos
- ✅ Cadastro e login de professores
- ✅ Cadastro e gerenciamento de turmas
- ✅ Criação de atividades e avaliações
- ✅ Histórico de atividades com filtros
- ✅ Painel administrativo com estatísticas
- ✅ Análise inteligente de desempenho (IA)

### Características Principais

| Feature | Status | Descrição |
|---------|--------|-----------|
| Menu Unificado | ✅ | 13 opções em um único ponto de entrada |
| Validação de Dados | ✅ | CPF, RG, Email, Data, etc |
| Banco de Dados JSON | ✅ | Armazenamento em arquivos JSON |
| Rastreamento de Usuário | ✅ | Logs automáticos de atividades |
| Suporte UTF-8 | ✅ | Acentuação completa |
| Cross-platform | ✅ | Windows, Linux (compatível) |
| Estatísticas | ✅ | Dashboard com gráficos |
| IA Integrada | ✅ | Análise com OpenAI |

---

## 🏗️ Arquitetura do Sistema

### Diagrama da Arquitetura

```
┌─────────────────────────────────────────────────────────────┐
│                   SISTEMA COMPLETO (Main)                  │
│                  sistema_completo.exe                       │
│                                                             │
│  Menu Principal (13 opções) com Rastreamento de Usuário   │
└────────────────┬────────────────────────────────────────────┘
                 │
     ┌───────────┼───────────┐
     │           │           │
     ▼           ▼           ▼
┌─────────┐  ┌─────────┐  ┌─────────┐
│ ALUNOS  │  │PROFESSORES│  │ ADMIN  │
│         │  │           │  │        │
│ 3 módulos  4 módulos    2 módulos   
└─────────┘  └─────────┘  └─────────┘
     │           │           │
     ├─────┬─────┼─────┬─────┤
     │     │     │     │     │
     ▼     ▼     ▼     ▼     ▼
   Login  Cad  Painel Atv  Hist
```

### Componentes

```
SISTEMA
├── MÓDULO ALUNO
│   ├── area_login_aluno.exe
│   ├── area_cadastro_aluno.exe
│   └── area_aluno.exe
│
├── MÓDULO PROFESSOR
│   ├── area_login_professor.exe
│   ├── area_cadastro_professor.exe
│   ├── painel_atividade_avalicoes.exe
│   └── historico_atividades_avaliacoes.exe
│
├── MÓDULO TURMA
│   └── area_cadastro_turma.exe
│
├── MÓDULO ADMIN
│   ├── login_adm.exe
│   └── painel_adm.exe
│
└── ORQUESTRADOR
    └── sistema_completo.exe
```

### Fluxo de Dados

```
Usuário
  │
  ▼
Menu Principal (sistema_completo.exe)
  │
  ├─► Seleciona Opção
  │
  ▼
Valida Entrada
  │
  ├─► Inválida? ─► Mostra Erro ─► Volta ao Menu
  │
  └─► Válida?
      │
      ▼
   Chama Subprocess (area_*.exe)
      │
      ├─► Executa Módulo
      │
      ├─► Valida Dados
      │
      ├─► Lê/Escreve JSON
      │
      ├─► Registra Log
      │
      └─► Retorna ao Menu Principal
```

---

## 📦 Módulos Disponíveis

### 1️⃣ Módulo de Aluno

#### 1.1 - area_login_aluno.exe
**Objetivo:** Autenticar aluno no sistema

**Fluxo:**
```
Inicia
  ↓
Exibe tela de login
  ↓
Solicita entrada (CPF/RA/Email)
  ↓
Valida entrada
  ↓
Busca em cadastro_aluno.json
  ↓
Encontrou? → SIM → Registra log → Abre painel
           → NÃO → Mensagem erro → Tenta novamente
```

**Validações:**
- ✅ CPF: 11 dígitos (com ou sem formatação)
- ✅ RA: Número inteiro
- ✅ Email: Formato válido

**Arquivo de Dados:**
```
dados/alunos_cadastros/cadastro_aluno.json
```

---

#### 1.2 - area_cadastro_aluno.exe
**Objetivo:** Registrar novo aluno

**Campos Obrigatórios:**
```
├── Nome Completo (letras e espaços)
├── Data de Nascimento (DD/MM/AAAA)
├── CPF (11 dígitos)
├── RG (9 dígitos)
├── Email (formato válido)
├── Telefone (10-11 dígitos)
├── Endereço Completo
├── CEP (8 dígitos)
└── Turma (vinculação)
```

**Formatações Aplicadas:**
```
CPF:      123.456.789-00
RG:       12.345.678-9
Telefone: (11) 98765-4321
```

**Fluxo:**
```
Abre formulário
  ↓
Preenche campos (com validação em tempo real)
  ↓
Formata dados
  ↓
Salva em JSON
  ↓
Gera ID único
  ↓
Registra log
  ↓
Confirma sucesso
```

**Arquivo de Dados:**
```
dados/alunos_cadastros/cadastro_aluno.json
```

---

#### 1.3 - area_aluno.exe
**Objetivo:** Painel pessoal do aluno

**Funcionalidades:**
```
Menu:
  1. Ver meus dados
  2. Ver atividades pendentes
  3. Ver minhas notas
  4. Ver histórico de atividades
  5. Ver desempenho
  6. Editar perfil
  0. Sair
```

**Dados Exibidos:**
```
Nome: [Nome do Aluno]
RA: [Número RA]
Turma: [Turma vinculada]
Email: [Email registrado]

Atividades Pendentes: [Número]
Média Geral: [Média calculada]
Taxa de Conclusão: [Percentual]
```

**Arquivos de Dados:**
```
dados/alunos_cadastros/cadastro_aluno.json
dados/alunos_cadastros/atividades_aluno.json
dados/alunos_cadastros/desempenho_alunos.json
dados/atividades_e_avaliacoes/historico_atividades.json
```

---

### 2️⃣ Módulo de Professor

#### 2.1 - area_login_professor.exe
**Objetivo:** Autenticar professor

**Fluxo:**
```
Inicia
  ↓
Solicita email e senha
  ↓
Valida credenciais
  ↓
Busca em cadastro_professor.json
  ↓
Encontrou? → SIM → Abre painel
           → NÃO → Erro → Tenta novamente
```

**Arquivo de Dados:**
```
dados/professores_cadastros/cadastro_professor.json
```

---

#### 2.2 - area_cadastro_professor.exe
**Objetivo:** Registrar novo professor

**Campos Obrigatórios:**
```
├── Nome Completo
├── Email (único)
├── CPF (único)
├── RG
├── Telefone
├── Data de Contratação
├── Disciplina(s)
├── Turmas
└── Senha
```

**Fluxo:**
```
Formulário
  ↓
Valida campos
  ↓
Verifica duplicatas (email/CPF)
  ↓
Gera ID professor
  ↓
Salva em JSON
  ↓
Registra log
```

**Arquivo de Dados:**
```
dados/professores_cadastros/cadastro_professor.json
```

---

#### 2.3 - painel_atividade_avalicoes.exe
**Objetivo:** Gerenciar atividades e avaliações

**Menu:**
```
1. Criar nova atividade
2. Criar avaliação
3. Editar atividade
4. Adicionar questões
5. Gerenciar pontuação
6. Ver respostas de alunos
0. Sair
```

**Estrutura de Atividade:**
```json
{
  "id_atividade": "ATI001",
  "titulo": "Exercício de Matemática",
  "descricao": "Resolva os problemas de geometria",
  "turma": "TURMA_A",
  "disciplina": "Matemática",
  "data_criacao": "2025-11-23",
  "data_entrega": "2025-11-30",
  "pontuacao_maxima": 100,
  "questoes": [
    {
      "numero": 1,
      "pergunta": "Qual é a área de um quadrado?",
      "tipo": "discursiva",
      "pontos": 10
    }
  ]
}
```

**Arquivo de Dados:**
```
dados/atividades_e_avaliacoes/conteudo_atividades.json
dados/atividades_e_avaliacoes/conteudo_avaliacoes.json
```

---

#### 2.4 - historico_atividades_avaliacoes.exe
**Objetivo:** Consultar histórico com filtros

**Filtros Disponíveis:**
```
1. Filtrar por data
2. Filtrar por aluno
3. Filtrar por turma
4. Filtrar por disciplina
5. Filtrar por status (completa/pendente)
6. Gerar relatório completo
```

**Exemplo de Relatório:**
```
╔═══════════════════════════════════════════╗
║        HISTÓRICO DE ATIVIDADES            ║
╠═══════════════════════════════════════════╣
║ Turma: TURMA_A                            ║
║ Data: 2025-11-01 até 2025-11-23          ║
╠═══════════════════════════════════════════╣
║ Atividade: Exercício de Matemática       ║
║ Data: 2025-11-23                         ║
║ Alunos: 30/30 completas                  ║
║ Status: ✓ Concluída                      ║
╚═══════════════════════════════════════════╝
```

**Arquivo de Dados:**
```
dados/atividades_e_avaliacoes/historico_atividades.json
dados/logs_atividade.json
```

---

### 3️⃣ Módulo de Turma

#### 3.1 - area_cadastro_turma.exe
**Objetivo:** Registrar novas turmas

**Campos Obrigatórios:**
```
├── Nome da Turma (ex: 3º Ano A)
├── Série (1º, 2º, 3º)
├── Professor Responsável
├── Quantidade de Alunos
├── Período (Manhã/Tarde/Noite)
├── Sala
└── Ano Letivo
```

**Validações:**
- ✅ Verifica se professor existe
- ✅ Verifica duplicata de turma
- ✅ Valida período

**Fluxo:**
```
Formulário
  ↓
Valida professor
  ↓
Cria turma
  ↓
Atribui alunos
  ↓
Salva em JSON
  ↓
Registra log
```

**Arquivo de Dados:**
```
dados/turmas_cadastros/cadastro_turma.json
dados/turmas_informacoes/turmas_informacoes.json
```

---

### 4️⃣ Módulo de Administrador

#### 4.1 - login_adm.exe
**Objetivo:** Autenticar administrador

**Credenciais Padrão:**
```
Email: admin@escola.com
Senha: admin123
```

**Fluxo:**
```
Solicita email e senha
  ↓
Valida contra JSON
  ↓
Registra log de login
  ↓
Abre painel administrativo
```

**Arquivo de Dados:**
```
dados/adm-credencias/adm_credenciais.json
dados/logs_atividade.json
```

---

#### 4.2 - painel_adm.exe
**Objetivo:** Painel de controle administrativo

**Menu Principal:**
```
1. Ver Estatísticas de Uso
2. Gerenciar Usuários
3. Gerenciar Turmas
4. Ver Logs de Atividade
5. Gerenciar Atividades
6. Gerar Relatórios
7. Acessar Painel de IA
0. Sair
```

**1. Estatísticas de Uso:**
```
┌─────────────────────────────────────┐
│    ESTATÍSTICAS DO SISTEMA          │
├─────────────────────────────────────┤
│ Total de Alunos:        30          │
│ Total de Professores:   5           │
│ Total de Turmas:        3           │
│ Atividades Criadas:     15          │
│ Atividades Completas:   12          │
│ Taxa de Conclusão:      80%         │
│ Logins Hoje:            25          │
│ Taxa de Uso Diária:     95%         │
└─────────────────────────────────────┘
```

**2. Gerenciar Usuários:**
```
├── Listar todos os usuários
├── Buscar usuário específico
├── Editar dados de usuário
├── Desativar usuário
└── Reativar usuário
```

**3. Gerenciar Turmas:**
```
├── Ver todas as turmas
├── Editar turma
├── Adicionar alunos
├── Remover alunos
└── Deletar turma
```

**4. Ver Logs:**
```
├── Logs de login
├── Logs de cadastro
├── Logs de atividades
├── Filtrar por data
└── Exportar relatório
```

**5. Gerenciar Atividades:**
```
├── Ver todas as atividades
├── Deletar atividade
├── Modificar atividade
└── Ver respostas
```

**6. Gerar Relatórios:**
```
├── Relatório de desempenho
├── Relatório de presença
├── Relatório de atividades
└── Exportar em PDF
```

**7. Painel de IA:**
```
├── Análise de desempenho
├── Recomendações personalizadas
├── Previsão de evasão
└── Insights automáticos
```

**Arquivos de Dados:**
```
dados/logs_atividade.json
dados/adm-credencias/adm_credenciais.json
Acesso a todos os outros arquivos JSON
```

---

### 5️⃣ Sistema Completo (Orquestrador)

#### sistema_completo.exe
**Objetivo:** Menu principal integrado

**Menu (13 opções):**
```
╔════════════════════════════════════════════╗
║   SISTEMA DE GESTÃO ESCOLAR - Menu        ║
║   Versão 1.0                              ║
╠════════════════════════════════════════════╣
║                                            ║
║ 👨‍🎓 ÁREA DO ALUNO                          ║
║   1. Login do Aluno                        ║
║   2. Cadastro do Aluno                     ║
║   3. Painel do Aluno                       ║
║                                            ║
║ 👨‍🏫 ÁREA DO PROFESSOR                       ║
║   4. Login do Professor                    ║
║   5. Cadastro do Professor                 ║
║   6. Painel de Atividades                  ║
║   7. Histórico de Atividades               ║
║                                            ║
║ 📚 GERENCIAMENTO                           ║
║   8. Cadastro de Turmas                    ║
║                                            ║
║ 🔐 ADMINISTRAÇÃO                           ║
║   9. Login Administrador                   ║
║  10. Painel Administrativo                 ║
║                                            ║
║ 🤖 INTELIGÊNCIA ARTIFICIAL                 ║
║  11. Painel Inteligente (IA)               ║
║                                            ║
║ ⚙️  SISTEMA                                 ║
║  12. Configurações                         ║
║  13. Estatísticas do Sistema               ║
║   0. Sair                                  ║
║                                            ║
╚════════════════════════════════════════════╝
```

**Funcionalidades:**
```
├── Rastreamento de usuário logado
├── Exibição de estatísticas
├── Menu de configurações
├── Limpeza de logs
├── Reset de estatísticas
├── Informações do sistema
└── Encerramento gracioso
```

---

## 🚀 Como Iniciar

### Opção 1: Usando o Script de Inicialização (RECOMENDADO)

1. Abra o Windows Explorer
2. Navegue até: `c:\Users\decko\OneDrive\Área de Trabalho\pim-backup\codigos\build`
3. Duplo clique em: **INICIAR.bat**
4. O menu principal abrirá automaticamente

### Opção 2: Executar Diretamente

1. Abra PowerShell ou CMD
2. Execute:
```powershell
cd c:\Users\decko\OneDrive\Área de Trabalho\pim-backup\codigos\build
.\sistema_completo.exe
```

### Opção 3: Executar Módulo Individual

```powershell
cd c:\Users\decko\OneDrive\Área de Trabalho\pim-backup\codigos\build

# Para login de aluno
.\area_login_aluno.exe

# Para painel de professor
.\area_login_professor.exe

# Para painel de admin
.\login_adm.exe
```

---

## 👥 Guia de Uso por Perfil

### 👨‍🎓 ALUNO

#### Fluxo Completo:

**1. Primeiro Acesso (Novo Aluno):**
```
Menu Principal
  ↓
Seleciona: 2. Cadastro do Aluno
  ↓
Preenche formulário com:
  - Nome Completo
  - Data de Nascimento
  - CPF
  - RG
  - Email
  - Telefone
  - Endereço
  - Turma
  ↓
Confirma cadastro
  ↓
ID do aluno é gerado
  ↓
Volta ao Menu
```

**2. Login (Próximos Acessos):**
```
Menu Principal
  ↓
Seleciona: 1. Login do Aluno
  ↓
Escolhe tipo de entrada:
  - CPF
  - RA (Número do aluno)
  - Email
  ↓
Digita credencial
  ↓
Sistema valida
  ↓
Acesso ao painel pessoal
```

**3. No Painel do Aluno:**
```
Menu:
  ├─ 1. Ver meus dados
  │   └─ Exibe: Nome, RA, Turma, Email, Média
  │
  ├─ 2. Ver atividades pendentes
  │   └─ Lista: Título, Data, Disciplina
  │
  ├─ 3. Ver minhas notas
  │   └─ Mostra: Notas por disciplina/atividade
  │
  ├─ 4. Ver histórico
  │   └─ Atividades completas com datas
  │
  ├─ 5. Ver desempenho
  │   └─ Gráfico de desempenho
  │
  ├─ 6. Editar perfil
  │   └─ Atualiza dados pessoais
  │
  └─ 0. Sair
```

**Dados de Teste (Alunos):**
```
1. Maria Silva
   Email: maria@escola.com
   CPF: 123.456.789-00
   RA: 2021001

2. João Santos
   Email: joao@escola.com
   CPF: 987.654.321-00
   RA: 2021002

3. Ana Costa
   Email: ana@escola.com
   CPF: 456.789.123-00
   RA: 2021003
```

---

### 👨‍🏫 PROFESSOR

#### Fluxo Completo:

**1. Primeiro Acesso (Novo Professor):**
```
Menu Principal
  ↓
Seleciona: 5. Cadastro do Professor
  ↓
Preenche:
  - Nome Completo
  - Email (único)
  - CPF (único)
  - RG
  - Telefone
  - Data de Contratação
  - Disciplina(s)
  - Turmas que leciona
  - Cria senha
  ↓
Confirma cadastro
  ↓
ID professor é gerado
```

**2. Login:**
```
Menu Principal
  ↓
Seleciona: 4. Login do Professor
  ↓
Digita:
  - Email
  - Senha
  ↓
Sistema valida
  ↓
Acesso ao painel
```

**3. Criar Atividade:**
```
Painel do Professor
  ↓
Seleciona: 6. Painel de Atividades
  ↓
Escolhe: 1. Criar nova atividade
  ↓
Preenche:
  - Título
  - Descrição
  - Turma alvo
  - Disciplina
  - Data de entrega
  - Pontuação máxima
  ↓
Adiciona questões:
  - Número
  - Pergunta
  - Tipo (múltipla escolha/discursiva)
  - Pontos
  ↓
Salva atividade
  ↓
Alunos recebem notificação
```

**4. Ver Histórico:**
```
Painel do Professor
  ↓
Seleciona: 7. Histórico de Atividades
  ↓
Escolhe filtro:
  - Por data
  - Por aluno
  - Por turma
  - Por disciplina
  - Por status
  ↓
Sistema exibe relatório
  ↓
Pode exportar dados
```

**Dados de Teste (Professores):**
```
1. Prof. Carlos Mendes
   Email: carlos@escola.com
   ID: PROF001

2. Prof. Beatriz Lima
   Email: beatriz@escola.com
   ID: PROF002
```

---

### 🔐 ADMINISTRADOR

#### Fluxo Completo:

**1. Login:**
```
Menu Principal
  ↓
Seleciona: 9. Login Administrador
  ↓
Digita:
  - Email: admin@escola.com
  - Senha: admin123
  ↓
Sistema valida
  ↓
Acesso ao painel administrativo
```

**2. Painel Administrativo:**
```
Seleciona opção:

1️⃣ Ver Estatísticas
  └─ Dashboard com números do sistema
  
2️⃣ Gerenciar Usuários
  ├─ Listar todos
  ├─ Buscar específico
  ├─ Editar dados
  ├─ Desativar
  └─ Reativar

3️⃣ Gerenciar Turmas
  ├─ Listar todas
  ├─ Editar turma
  ├─ Adicionar alunos
  ├─ Remover alunos
  └─ Deletar turma

4️⃣ Ver Logs
  ├─ Logs de login
  ├─ Logs de cadastro
  ├─ Logs de atividades
  ├─ Filtrar por data
  └─ Exportar

5️⃣ Gerenciar Atividades
  ├─ Ver todas
  ├─ Deletar
  ├─ Modificar
  └─ Ver respostas

6️⃣ Gerar Relatórios
  ├─ Desempenho
  ├─ Presença
  ├─ Atividades
  └─ Exportar PDF

7️⃣ Painel de IA
  ├─ Análise de desempenho
  ├─ Recomendações
  ├─ Previsão de evasão
  └─ Insights
```

**Credenciais de Admin:**
```
Email: admin@escola.com
Senha: admin123
```

---

## 🗄️ Estrutura de Dados

### Arquivos JSON e Conteúdo

#### 1. adm_credenciais.json
```json
[
  {
    "id": 1,
    "email": "admin@escola.com",
    "senha": "admin123",
    "nome": "Administrador Sistema",
    "ativo": true,
    "data_criacao": "2025-01-01",
    "ultimo_acesso": "2025-11-23"
  }
]
```

#### 2. cadastro_aluno.json
```json
[
  {
    "id": 1,
    "nome": "Maria Silva",
    "ra": 2021001,
    "cpf": "123.456.789-00",
    "rg": "12.345.678-9",
    "email": "maria@escola.com",
    "telefone": "(11) 98765-4321",
    "data_nascimento": "2007-05-15",
    "endereco": "Rua A, 123",
    "cep": "01234-567",
    "turma": "TURMA_A",
    "data_cadastro": "2025-01-15",
    "ativo": true
  }
]
```

#### 3. cadastro_professor.json
```json
[
  {
    "id": 1,
    "nome": "Prof. Carlos Mendes",
    "id_professor": "PROF001",
    "email": "carlos@escola.com",
    "cpf": "111.222.333-44",
    "rg": "11.222.333-4",
    "telefone": "(11) 99999-8888",
    "data_contratacao": "2020-02-01",
    "disciplinas": ["Matemática", "Física"],
    "turmas": ["TURMA_A", "TURMA_B"],
    "ativo": true
  }
]
```

#### 4. cadastro_turma.json
```json
[
  {
    "id": 1,
    "nome": "3º Ano A",
    "serie": 3,
    "professor_responsavel": "PROF001",
    "quantidade_alunos": 30,
    "periodo": "Manhã",
    "sala": "301",
    "ano_letivo": 2025,
    "alunos": [2021001, 2021002, 2021003],
    "data_criacao": "2025-01-01"
  }
]
```

#### 5. conteudo_atividades.json
```json
[
  {
    "id_atividade": "ATI001",
    "titulo": "Exercício de Matemática",
    "descricao": "Resolva os problemas de geometria",
    "turma": "TURMA_A",
    "disciplina": "Matemática",
    "professor": "PROF001",
    "data_criacao": "2025-11-23",
    "data_entrega": "2025-11-30",
    "pontuacao_maxima": 100,
    "questoes": [
      {
        "numero": 1,
        "pergunta": "Qual é a área de um quadrado com lado 5?",
        "tipo": "discursiva",
        "pontos": 10,
        "resposta_correta": "25"
      }
    ]
  }
]
```

#### 6. historico_atividades.json
```json
[
  {
    "id_historico": "HIST001",
    "id_atividade": "ATI001",
    "id_aluno": 2021001,
    "data_entrega": "2025-11-25",
    "status": "completa",
    "pontos_obtidos": 85,
    "tempo_resolucao": 45,
    "respostas": [
      {
        "questao": 1,
        "resposta_aluno": "25",
        "correta": true
      }
    ]
  }
]
```

#### 7. logs_atividade.json
```json
[
  {
    "id_log": 1,
    "usuario": "maria@escola.com",
    "tipo_usuario": "Aluno",
    "tipo_atividade": "Login",
    "descricao": "Aluno realizou login no sistema",
    "data": "2025-11-23",
    "hora": "14:30:45",
    "ip": "192.168.1.100"
  }
]
```

---

## 🔄 Fluxos de Funcionamento

### Fluxo 1: Cadastro Completo de um Aluno

```
┌─────────────────────────────────────────────────────┐
│ 1. Novo Aluno acessa menu                           │
└──────────────┬──────────────────────────────────────┘
               │
┌──────────────▼──────────────────────────────────────┐
│ 2. Seleciona "Cadastro do Aluno"                   │
└──────────────┬──────────────────────────────────────┘
               │
┌──────────────▼──────────────────────────────────────┐
│ 3. Abre formulário de cadastro                      │
└──────────────┬──────────────────────────────────────┘
               │
┌──────────────▼──────────────────────────────────────┐
│ 4. Aluno preenche dados:                            │
│    - Nome, Data, CPF, RG, Email, Telefone          │
└──────────────┬──────────────────────────────────────┘
               │
┌──────────────▼──────────────────────────────────────┐
│ 5. Sistema valida cada campo:                       │
│    - Nome: apenas letras e espaços                 │
│    - CPF: 11 dígitos, sem duplicata               │
│    - Email: formato válido, único                  │
│    - Data: formato DD/MM/AAAA                      │
└──────────────┬──────────────────────────────────────┘
               │
        ┌──────▼──────┐
        │ Válido?      │
        └──┬────────┬──┘
       SIM │        │ NÃO
          │         └────────────────────┐
          │                              │
    ┌─────▼────┐                  ┌──────▼──────┐
    │ Formata  │                  │ Mostra erro │
    │ dados    │                  └──────┬──────┘
    └─────┬────┘                         │
          │                    ┌─────────▼────┐
    ┌─────▼────┐                │ Tenta novamente
    │ Salva em │                │ ou cancela
    │   JSON   │                └────┬─────────┘
    └─────┬────┘                     │
          │         ┌────────────────┘
    ┌─────▼────┐    │
    │ Gera ID  │◄───┘
    │ único    │
    └─────┬────┘
          │
    ┌─────▼────┐
    │ Registra │
    │ log      │
    └─────┬────┘
          │
    ┌─────▼────────────────┐
    │ Exibe confirmação    │
    │ "Cadastro realizado" │
    │ ID: [novo_id]       │
    └─────┬────────────────┘
          │
    ┌─────▼────────────────┐
    │ Volta ao menu        │
    └──────────────────────┘
```

---

### Fluxo 2: Criar e Entregar Atividade

```
┌───────────────────────────────────────────┐
│ Professor Acessa: Painel de Atividades    │
└──────────────┬────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Seleciona: 1. Criar nova atividade         │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Preenche:                                   │
│  - Título                                   │
│  - Descrição                                │
│  - Turma alvo                               │
│  - Data de entrega                          │
│  - Pontuação máxima                         │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Adiciona questões (loop):                  │
│  1. Pergunta                                │
│  2. Tipo (múltipla/discursiva)             │
│  3. Pontos por questão                     │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Confirma e salva em JSON                   │
└──────────────┬──────────────────────────────┘
               │
               │ ◄─ Sistema notifica alunos
               │
┌──────────────▼─────────────────────────────┐
│ Aluno recebe notificação de nova atividade │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Aluno acessa: Painel → Ver atividades      │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Seleciona atividade para resolver          │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Responde questões:                         │
│  - Lê cada pergunta                        │
│  - Seleciona/digita resposta               │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Clica "Enviar"                             │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Sistema:                                    │
│  - Valida respostas                        │
│  - Calcula pontos                          │
│  - Salva em historico_atividades.json      │
│  - Atualiza nota do aluno                  │
│  - Registra log                            │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Exibe resultado:                            │
│  "Pontos: 85/100"                          │
│  "Status: Completa"                        │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼─────────────────────────────┐
│ Professor visualiza:                        │
│ Histórico → Ver resultado do aluno         │
└──────────────────────────────────────────────┘
```

---

### Fluxo 3: Login e Acesso ao Sistema

```
┌──────────────────────────────────┐
│ Usuário executa sistema_completo │
└────────────┬─────────────────────┘
             │
┌────────────▼─────────────────────┐
│ Exibe menu com 13 opções        │
└────────────┬─────────────────────┘
             │
┌────────────▼─────────────────────┐
│ Usuário seleciona opção         │
│ (1-13 ou 0)                      │
└────────────┬─────────────────────┘
             │
      ┌──────▼──────┐
      │ Valida      │
      │ entrada?    │
      └──┬──────┬───┘
         │      │
    SIM  │      │ NÃO
        │       └─────────────────┐
        │                         │
┌───────▼────────┐      ┌─────────▼──────┐
│ Chama módulo   │      │ Erro: "Opção   │
│ (subprocess)   │      │ inválida"      │
└───────┬────────┘      └─────────┬──────┘
        │                         │
        │          ┌──────────────┘
        │          │
        │    ┌─────▼────────────┐
        │    │ Volta ao menu    │
        │    └──────────────────┘
        │
┌───────▼─────────────────────────┐
│ Módulo inicia:                  │
│  - Exibe interface              │
│  - Solicita dados               │
│  - Valida entradas              │
│  - Processa operação            │
│  - Atualiza JSON                │
│  - Registra log                 │
└───────┬─────────────────────────┘
        │
┌───────▼─────────────────────────┐
│ Módulo encerra                  │
│ Retorna ao menu principal       │
└───────┬─────────────────────────┘
        │
┌───────▼─────────────────────────┐
│ Atualiza usuário logado         │
│ Exibe estatísticas              │
└───────┬─────────────────────────┘
        │
┌───────▼─────────────────────────┐
│ Menu exibido novamente          │
│ Usuário pode:                   │
│  - Selecionar outra opção       │
│  - Sair do sistema (0)          │
└──────────────────────────────────┘
```

---

## 🔧 Troubleshooting

### Problema 1: "Arquivo não encontrado"
```
Erro: erro: arquivo nao foi encontrado
Causa: Pasta dados/ não existe ou estrutura incorreta

Solução:
1. Verifique se pasta dados/ existe em:
   c:\Users\decko\OneDrive\Área de Trabalho\pim-backup\

2. Verifique subpastas:
   dados/
   ├── adm-credencias/
   ├── alunos_cadastros/
   ├── atividades_e_avaliacoes/
   ├── desempenho_alunos_ia/
   ├── logs_atividade.json
   ├── professores_cadastros/
   ├── turmas_cadastros/
   └── turmas_informacoes/

3. Se faltar, crie com:
   mkdir dados\adm-credencias
   mkdir dados\alunos_cadastros
   etc...
```

### Problema 2: "Caracteres estranhos na tela"
```
Erro: Texto com caracteres como: Ôò├ ù

Causa: Encoding incorreto do console

Solução:
1. O sistema já configura automaticamente para UTF-8
2. Se ainda assim falhar, abra:
   - PowerShell como Admin
   - Acesse: Settings → Terminal → Encoding
   - Escolha: UTF-8
```

### Problema 3: "Login não funciona"
```
Erro: Email ou senha incorretos

Causas possíveis:
1. Credencial errada - Verifique dados de teste
2. JSON corrompido - Verifique arquivo JSON
3. Caminho de arquivo incorreto - Verifique path

Solução:
1. Verifique arquivo JSON em:
   dados/adm-credencias/adm_credenciais.json

2. Deve conter:
   {
     "email": "admin@escola.com",
     "senha": "admin123"
   }

3. Se corrompido, recrie com dados válidos
```

### Problema 4: "Módulo não inicia"
```
Erro: Erro ao abrir programa

Causas:
1. .exe não compilado
2. .exe com erro de compilação
3. Arquivo corrompido

Solução:
1. Verifique se existe em:
   c:\...\codigos\build\area_*.exe

2. Se não existir, recompile:
   - Abra pasta codigos/
   - Execute: compilar.bat

3. Se compilar com erro, verifique
   código-fonte .c para syntaxerrors
```

### Problema 5: "Dados não salvam"
```
Erro: Cadastro feito, mas dados não aparecem depois

Causa: Arquivo JSON protegido ou permissão negada

Solução:
1. Verifique permissões:
   - Clique direito em dados/ → Propriedades
   - Abas → Segurança
   - Editar → Seu usuário → Controle total

2. Teste salvamento:
   - Tente criar novo cadastro
   - Verifique JSON foi atualizado

3. Se falhar, recrie arquivo JSON manualmente
```

### Problema 6: "Painel de IA não funciona"
```
Erro: Python não reconhecido ou OpenAI não instalado

Causa: Python ou biblioteca OpenAI não instalada

Solução:
1. Instale Python:
   - Baixe em python.org
   - Marque "Add to PATH"
   - Instale

2. Instale OpenAI:
   pip install openai

3. Teste:
   python --version
   python -c "import openai; print('OK')"

4. Se usar variável de ambiente:
   set OPENAI_API_KEY=sua_chave_aqui
```

---

## 📞 Contato e Suporte

Para mais informações, consulte:
- **Documentação:** `DOCUMENTACAO_COMPLETA.md`
- **Testes:** `RELATORIO_TESTES.md`
- **README:** `codigos/build/README.md`

---

**Documento criado:** 23 de Novembro de 2025  
**Versão:** 1.0  
**Status:** ✅ Completo
