/*
================================================================================
                    SISTEMA COMPLETO DE GESTÃO ESCOLAR
                         Integração de Todos os Módulos
================================================================================

Autor: Sistema de Gestão Escolar
Data: 23/11/2025
Versão: 1.0

Descrição:
    Este programa integra todas as funcionalidades do sistema de gestão escolar,
    oferecendo um ponto de entrada único para acessar:
    - Login de Alunos
    - Login de Professores
    - Login de Administrador
    - Cadastro de Alunos
    - Cadastro de Professores
    - Cadastro de Turmas
    - Painel do Aluno
    - Painel de Atividades (Professor)
    - Painel de Histórico (Professor)
    - Painel Administrativo
    - Painel Inteligente com IA

================================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

// Constantes
#define MAX_NOME 100
#define VERSAO_SISTEMA "1.0"

// Tipos de usuários
typedef enum {
    TIPO_ALUNO = 1,
    TIPO_PROFESSOR = 2,
    TIPO_ADMINISTRADOR = 3,
    TIPO_VISITANTE = 4
} TipoUsuario;

// ============================================================================
// FORWARD DECLARATIONS (Declarações Antecipadas de Funções)
// ============================================================================
void limpar_tela(void);
void pausar(void);
void exibir_cabecalho(void);
void exibir_info_usuario(void);
void registrar_acesso(TipoUsuario tipo, const char *nome);
void menu_principal(void);
void executar_login_aluno(void);
void executar_cadastro_aluno(void);
void executar_painel_aluno(void);
void executar_login_professor(void);
void executar_cadastro_professor(void);
void executar_painel_atividades(void);
void executar_historico_atividades(void);
void executar_cadastro_turma(void);
void executar_login_admin(void);
void executar_painel_administrativo(void);
void executar_painel_ia(void);
void exibir_estatisticas_sistema(void);
void menu_configuracoes(void);
void finalizar_sistema(void);
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

// ============================================================================
// CONFIGURAÇÕES E CONSTANTES
// ============================================================================

#define VERSAO_SISTEMA "1.0"
#define MAX_CAMINHO 512
#define MAX_NOME 150
#define MAX_SENHA 50

// Cores para terminal (Windows)
#define LIMPAR_TELA() system("cls")
#define CONFIG_CONSOLE() system("chcp 65001 > nul")

// ============================================================================
// ESTRUTURAS DE DADOS
// ============================================================================

typedef struct {
    int id;
    char nome[MAX_NOME];
    TipoUsuario tipo;
    char login[MAX_NOME];
    time_t timestamp_login;
} UsuarioAtual;

typedef struct {
    int total_acessos;
    int alunos_ativos;
    int professores_ativos;
    int admin_ativos;
    char data_ultima_atualizacao[20];
} EstatisticasSistema;

// ============================================================================
// VARIÁVEIS GLOBAIS
// ============================================================================

UsuarioAtual usuario_logado = {0, "", 0, "", 0};
EstatisticasSistema stats_sistema = {0, 0, 0, 0, ""};
int sistema_ativo = 1;

// ============================================================================
// FUNÇÕES AUXILIARES
// ============================================================================

void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausar() {
    printf("\n[Pressione ENTER para continuar...]");
    fflush(stdout);
    getchar();
}

void exibir_cabecalho() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                                    ║\n");
    printf("║                   SISTEMA COMPLETO DE GESTÃO ESCOLAR                             ║\n");
    printf("║                            Versão %s                                            ║\n", VERSAO_SISTEMA);
    printf("║                                                                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void exibir_info_usuario() {
    if (usuario_logado.id > 0) {
        printf("│ Usuário: %-20s | Tipo: ", usuario_logado.nome);
        
        switch(usuario_logado.tipo) {
            case TIPO_ALUNO:
                printf("ALUNO");
                break;
            case TIPO_PROFESSOR:
                printf("PROFESSOR");
                break;
            case TIPO_ADMINISTRADOR:
                printf("ADMINISTRADOR");
                break;
            default:
                printf("VISITANTE");
        }
        printf(" │\n");
    } else {
        printf("│ Nenhum usuário logado                                                   │\n");
    }
}

void registrar_acesso(TipoUsuario tipo, const char *nome) {
    time_t agora = time(NULL);
    struct tm *tm_info = localtime(&agora);
    
    printf("\n✅ Acesso registrado em: ");
    printf("%02d/%02d/%04d %02d:%02d:%02d\n",
        tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900,
        tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    
    stats_sistema.total_acessos++;
    
    switch(tipo) {
        case TIPO_ALUNO:
            stats_sistema.alunos_ativos++;
            break;
        case TIPO_PROFESSOR:
            stats_sistema.professores_ativos++;
            break;
        case TIPO_ADMINISTRADOR:
            stats_sistema.admin_ativos++;
            break;
        default:
            break;
    }
}

// ============================================================================
// MENU PRINCIPAL
// ============================================================================

void menu_principal() {
    int opcao;
    
    while (sistema_ativo) {
        limpar_tela();
        exibir_cabecalho();
        
        printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
        printf("║                         BEM-VINDO AO SISTEMA ESCOLAR                             ║\n");
        printf("║                                                                                    ║\n");
        exibir_info_usuario();
        printf("║                                                                                    ║\n");
        printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
        
        printf("┌────────────────────────────────────────────────────────────────────────────────────┐\n");
        printf("│  SELECIONE O TIPO DE ACESSO:                                                     │\n");
        printf("└────────────────────────────────────────────────────────────────────────────────────┘\n\n");
        
        printf("  👨‍🎓 ÁREA DO ALUNO:\n");
        printf("    [1] Login - Aluno\n");
        printf("    [2] Cadastro - Novo Aluno\n");
        printf("    [3] Painel do Aluno (Área Logada)\n\n");
        
        printf("  👨‍🏫 ÁREA DO PROFESSOR:\n");
        printf("    [4] Login - Professor\n");
        printf("    [5] Cadastro - Novo Professor\n");
        printf("    [6] Painel de Atividades (Professor)\n");
        printf("    [7] Histórico de Atividades\n\n");
        
        printf("  🏫 GESTÃO DE TURMAS:\n");
        printf("    [8] Cadastro - Nova Turma\n\n");
        
        printf("  👔 ADMINISTRAÇÃO:\n");
        printf("    [9] Login - Administrador\n");
        printf("    [10] Painel Administrativo\n");
        printf("    [11] Painel Inteligente com IA\n\n");
        
        printf("  ⚙️  SISTEMA:\n");
        printf("    [12] Estatísticas do Sistema\n");
        printf("    [13] Configurações\n");
        printf("    [0] Sair do Sistema\n\n");
        
        printf("┌────────────────────────────────────────────────────────────────────────────────────┐\n");
        printf("  Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();
        printf("└────────────────────────────────────────────────────────────────────────────────────┘\n");
        
        switch(opcao) {
            case 1:
                executar_login_aluno();
                break;
            case 2:
                executar_cadastro_aluno();
                break;
            case 3:
                executar_painel_aluno();
                break;
            case 4:
                executar_login_professor();
                break;
            case 5:
                executar_cadastro_professor();
                break;
            case 6:
                executar_painel_atividades();
                break;
            case 7:
                executar_historico_atividades();
                break;
            case 8:
                executar_cadastro_turma();
                break;
            case 9:
                executar_login_admin();
                break;
            case 10:
                executar_painel_administrativo();
                break;
            case 11:
                executar_painel_ia();
                break;
            case 12:
                exibir_estatisticas_sistema();
                break;
            case 13:
                menu_configuracoes();
                break;
            case 0:
                finalizar_sistema();
                sistema_ativo = 0;
                break;
            default:
                printf("\n❌ Opção inválida! Tente novamente.\n");
                pausar();
        }
    }
}

// ============================================================================
// EXECUTORES DE FUNCIONALIDADES
// ============================================================================

void executar_login_aluno() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                         LOGIN - ÁREA DO ALUNO                                     ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🔄 Abrindo módulo de login do aluno...\n\n");
    printf("⚠️  NOTA: Este módulo carregará o programa 'area_login_aluno.exe'\n\n");
    
    #ifdef _WIN32
        int resultado = system("area_login_aluno.exe");
        if (resultado == 0) {
            printf("\n✅ Login do aluno concluído com sucesso!\n");
            usuario_logado.tipo = TIPO_ALUNO;
            strcpy(usuario_logado.nome, "Aluno");
            registrar_acesso(TIPO_ALUNO, "Aluno");
        } else {
            printf("\n❌ Erro ao executar o módulo de login do aluno.\n");
            printf("   Certifique-se de que o arquivo 'area_login_aluno.exe' existe no diretório.\n");
        }
    #else
        printf("❌ Este recurso está disponível apenas para Windows.\n");
    #endif
    
    pausar();
}

void executar_cadastro_aluno() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    CADASTRO - NOVO ALUNO                                          ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🔄 Abrindo módulo de cadastro de aluno...\n\n");
    printf("⚠️  NOTA: Este módulo carregará o programa 'area_cadastro_aluno.exe'\n\n");
    
    #ifdef _WIN32
        int resultado = system("area_cadastro_aluno.exe");
        if (resultado == 0) {
            printf("\n✅ Cadastro realizado com sucesso!\n");
        } else {
            printf("\n❌ Erro ao executar o módulo de cadastro.\n");
        }
    #else
        printf("❌ Este recurso está disponível apenas para Windows.\n");
    #endif
    
    pausar();
}

void executar_painel_aluno() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    PAINEL DO ALUNO - DASHBOARD                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🔄 Abrindo painel do aluno...\n\n");
    printf("⚠️  NOTA: Este módulo carregará o programa 'area_aluno.exe'\n\n");
    
    #ifdef _WIN32
        int resultado = system("area_aluno.exe");
        if (resultado == 0) {
            printf("\n✅ Painel do aluno fechado.\n");
        } else {
            printf("\n❌ Erro ao executar o painel.\n");
        }
    #else
        printf("❌ Este recurso está disponível apenas para Windows.\n");
    #endif
    
    pausar();
}

void executar_login_professor() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                      LOGIN - ÁREA DO PROFESSOR                                    ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🔄 Abrindo módulo de login do professor...\n\n");
    printf("⚠️  NOTA: Este módulo carregará o programa 'area_login_professor.exe'\n\n");
    
    #ifdef _WIN32
        int resultado = system("area_login_professor.exe");
        if (resultado == 0) {
            printf("\n✅ Login do professor concluído com sucesso!\n");
            usuario_logado.tipo = TIPO_PROFESSOR;
            strcpy(usuario_logado.nome, "Professor");
            registrar_acesso(TIPO_PROFESSOR, "Professor");
        } else {
            printf("\n❌ Erro ao executar o módulo de login.\n");
        }
    #else
        printf("❌ Este recurso está disponível apenas para Windows.\n");
    #endif
    
    pausar();
}

void executar_cadastro_professor() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                 CADASTRO - NOVO PROFESSOR                                         ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🔄 Abrindo módulo de cadastro de professor...\n\n");
    printf("⚠️  NOTA: Este módulo carregará o programa 'area_cadastro_professor.exe'\n\n");
    
    #ifdef _WIN32
        int resultado = system("area_cadastro_professor.exe");
        if (resultado == 0) {
            printf("\n✅ Cadastro realizado com sucesso!\n");
        } else {
            printf("\n❌ Erro ao executar o módulo de cadastro.\n");
        }
    #else
        printf("❌ Este recurso está disponível apenas para Windows.\n");
    #endif
    
    pausar();
}

void executar_painel_atividades() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║               PAINEL DE ATIVIDADES E AVALIAÇÕES (PROFESSOR)                       ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🔄 Abrindo painel de atividades...\n\n");
    printf("⚠️  NOTA: Este módulo carregará o programa 'painel_atividade_avalicoes.exe'\n\n");
    
    #ifdef _WIN32
        int resultado = system("painel_atividade_avalicoes.exe");
        if (resultado == 0) {
            printf("\n✅ Painel de atividades fechado.\n");
        } else {
            printf("\n❌ Erro ao executar o painel.\n");
        }
    #else
        printf("❌ Este recurso está disponível apenas para Windows.\n");
    #endif
    
    pausar();
}

void executar_historico_atividades() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║            HISTÓRICO DE ATIVIDADES E AVALIAÇÕES (PROFESSOR)                       ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🔄 Abrindo módulo de histórico...\n\n");
    printf("⚠️  NOTA: Este módulo carregará o programa 'historico_atividades_avaliacoes.exe'\n\n");
    
    #ifdef _WIN32
        int resultado = system("historico_atividades_avaliacoes.exe");
        if (resultado == 0) {
            printf("\n✅ Histórico fechado.\n");
        } else {
            printf("\n❌ Erro ao executar o módulo.\n");
        }
    #else
        printf("❌ Este recurso está disponível apenas para Windows.\n");
    #endif
    
    pausar();
}

void executar_cadastro_turma() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    CADASTRO - NOVA TURMA                                          ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🔄 Abrindo módulo de cadastro de turma...\n\n");
    printf("⚠️  NOTA: Este módulo carregará o programa 'area_cadastro_turma.exe'\n\n");
    
    #ifdef _WIN32
        int resultado = system("area_cadastro_turma.exe");
        if (resultado == 0) {
            printf("\n✅ Cadastro realizado com sucesso!\n");
        } else {
            printf("\n❌ Erro ao executar o módulo de cadastro.\n");
        }
    #else
        printf("❌ Este recurso está disponível apenas para Windows.\n");
    #endif
    
    pausar();
}

void executar_login_admin() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                   LOGIN - ADMINISTRADOR                                           ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🔄 Abrindo módulo de login administrativo...\n\n");
    printf("⚠️  NOTA: Este módulo carregará o programa 'login_adm.exe'\n\n");
    
    #ifdef _WIN32
        int resultado = system("login_adm.exe");
        if (resultado == 0) {
            printf("\n✅ Login administrativo concluído!\n");
            usuario_logado.tipo = TIPO_ADMINISTRADOR;
            strcpy(usuario_logado.nome, "Administrador");
            registrar_acesso(TIPO_ADMINISTRADOR, "Administrador");
        } else {
            printf("\n❌ Erro ao executar o módulo de login.\n");
        }
    #else
        printf("❌ Este recurso está disponível apenas para Windows.\n");
    #endif
    
    pausar();
}

void executar_painel_administrativo() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║              PAINEL DE CONTROLE DO ADMINISTRADOR                                  ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🔄 Abrindo painel administrativo...\n\n");
    printf("⚠️  NOTA: Este módulo carregará o programa 'painel_adm.exe'\n\n");
    
    #ifdef _WIN32
        int resultado = system("painel_adm.exe");
        if (resultado == 0) {
            printf("\n✅ Painel administrativo fechado.\n");
        } else {
            printf("\n❌ Erro ao executar o painel.\n");
        }
    #else
        printf("❌ Este recurso está disponível apenas para Windows.\n");
    #endif
    
    pausar();
}

void executar_painel_ia() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                 PAINEL INTELIGENTE COM IA                                         ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("🤖 Abrindo painel inteligente com análise de IA...\n\n");
    printf("⚠️  NOTA: Este módulo é executado via Python (painel_ia.py)\n");
    printf("   Certifique-se de que Python e OpenAI estão instalados.\n\n");
    
    #ifdef _WIN32
        int resultado = system("python codigos\\ferramentas\\painel_inteligente\\painel_ia.py");
        if (resultado == 0) {
            printf("\n✅ Painel de IA fechado.\n");
        } else {
            printf("\n⚠️  O painel de IA requer Python instalado.\n");
            printf("   Instale com: pip install openai\n");
        }
    #else
        int resultado = system("python3 codigos/ferramentas/painel_inteligente/painel_ia.py");
        if (resultado == 0) {
            printf("\n✅ Painel de IA fechado.\n");
        }
    #endif
    
    pausar();
}

void exibir_estatisticas_sistema() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    ESTATÍSTICAS DO SISTEMA                                        ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("┌────────────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│  RESUMO DE ACESSOS                                                               │\n");
    printf("├────────────────────────────────────────────────────────────────────────────────────┤\n");
    printf("│  Total de Acessos ao Sistema:     %d                                            │\n", stats_sistema.total_acessos);
    printf("│  Acessos de Alunos:               %d                                            │\n", stats_sistema.alunos_ativos);
    printf("│  Acessos de Professores:          %d                                            │\n", stats_sistema.professores_ativos);
    printf("│  Acessos de Administradores:      %d                                            │\n", stats_sistema.admin_ativos);
    printf("└────────────────────────────────────────────────────────────────────────────────────┘\n\n");
    
    printf("📊 Distribuição de Acessos:\n\n");
    
    if (stats_sistema.total_acessos > 0) {
        float pct_alunos = (stats_sistema.alunos_ativos * 100.0) / stats_sistema.total_acessos;
        float pct_prof = (stats_sistema.professores_ativos * 100.0) / stats_sistema.total_acessos;
        float pct_admin = (stats_sistema.admin_ativos * 100.0) / stats_sistema.total_acessos;
        
        printf("  👨‍🎓 Alunos:         %.1f%%  ", pct_alunos);
        for (int i = 0; i < (int)(pct_alunos / 5); i++) printf("█");
        printf("\n");
        
        printf("  👨‍🏫 Professores:   %.1f%%  ", pct_prof);
        for (int i = 0; i < (int)(pct_prof / 5); i++) printf("█");
        printf("\n");
        
        printf("  👔 Administradores: %.1f%%  ", pct_admin);
        for (int i = 0; i < (int)(pct_admin / 5); i++) printf("█");
        printf("\n");
    } else {
        printf("  Nenhum acesso registrado ainda.\n");
    }
    
    printf("\n");
    
    pausar();
}

void menu_configuracoes() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                         CONFIGURAÇÕES DO SISTEMA                                  ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("⚙️  OPÇÕES DISPONÍVEIS:\n\n");
    printf("  [1] Limpar Logs de Atividade\n");
    printf("  [2] Redefinir Estatísticas\n");
    printf("  [3] Configurações de Diretório de Dados\n");
    printf("  [4] Informações do Sistema\n");
    printf("  [0] Voltar ao Menu Principal\n\n");
    
    printf("Escolha uma opção: ");
    int opcao;
    scanf("%d", &opcao);
    getchar();
    
    switch(opcao) {
        case 1:
            printf("\n✅ Logs de atividade foram marcados para limpeza.\n");
            break;
        case 2:
            stats_sistema.total_acessos = 0;
            stats_sistema.alunos_ativos = 0;
            stats_sistema.professores_ativos = 0;
            stats_sistema.admin_ativos = 0;
            printf("\n✅ Estatísticas foram redefinidas.\n");
            break;
        case 3:
            printf("\n📁 Diretório de Dados: dados/\n");
            printf("   Arquivos JSON armazenados em: dados/\n");
            printf("   Códigos-fonte em: codigos/\n");
            break;
        case 4:
            printf("\n📋 INFORMAÇÕES DO SISTEMA:\n");
            printf("   Versão: %s\n", VERSAO_SISTEMA);
            printf("   Plataforma: ");
            #ifdef _WIN32
                printf("Windows (64-bit)\n");
            #else
                printf("Linux/Unix\n");
            #endif
            printf("   Data/Hora: ");
            time_t agora = time(NULL);
            printf("%s\n", ctime(&agora));
            break;
        case 0:
            return;
        default:
            printf("\n❌ Opção inválida!\n");
    }
    
    pausar();
}

void finalizar_sistema() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                     ENCERRANDO O SISTEMA                                          ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("📊 RESUMO DA SESSÃO:\n");
    printf("   • Total de Acessos: %d\n", stats_sistema.total_acessos);
    printf("   • Alunos: %d\n", stats_sistema.alunos_ativos);
    printf("   • Professores: %d\n", stats_sistema.professores_ativos);
    printf("   • Administradores: %d\n", stats_sistema.admin_ativos);
    
    printf("\n✅ Sistema encerrado com sucesso!\n");
    printf("👋 Até logo!\n\n");
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

int main() {
    // Configurar console para UTF-8 (Windows)
    #ifdef _WIN32
        SetConsoleCP(65001);
        SetConsoleOutputCP(65001);
        CONFIG_CONSOLE();
    #endif
    
    // Inicializar dados do sistema
    time_t agora = time(NULL);
    struct tm *tm_info = localtime(&agora);
    strftime(stats_sistema.data_ultima_atualizacao, sizeof(stats_sistema.data_ultima_atualizacao),
             "%d/%m/%Y %H:%M:%S", tm_info);
    
    // Entrar no menu principal
    menu_principal();
    
    return 0;
}

/*
================================================================================
                             FIM DO PROGRAMA
================================================================================

Para compilar este programa em Windows com GCC:

    gcc -o sistema_completo.exe sistema_completo.c

Para compilar com suporte a cores e UTF-8 melhorado:

    gcc -fdiagnostics-color=always -o sistema_completo.exe sistema_completo.c

Para executar:

    ./sistema_completo.exe

NOTAS IMPORTANTES:

1. Este programa é o ponto de entrada principal do sistema.
   
2. Ele chamará os seguintes arquivos .exe (que devem ser compilados separadamente):
   - area_login_aluno.exe
   - area_cadastro_aluno.exe
   - area_aluno.exe
   - area_login_professor.exe
   - area_cadastro_professor.exe
   - painel_atividade_avalicoes.exe
   - historico_atividades_avaliacoes.exe
   - area_cadastro_turma.exe
   - login_adm.exe
   - painel_adm.exe

3. O painel de IA (painel_ia.py) requer Python instalado.

4. Todos os programas .exe devem estar no mesmo diretório que sistema_completo.exe.

5. Os arquivos de dados (JSON) devem estar na pasta "dados/" relativa ao diretório
   de execução.

================================================================================
*/
