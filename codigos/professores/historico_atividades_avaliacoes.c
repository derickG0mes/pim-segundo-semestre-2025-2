// Histórico de Atividades e Avaliações
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#endif
#include <sys/stat.h>

// Configuração do console para UTF-8
void configurarConsole() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
}

#define MAX_NOME 100
#define MAX_PROFESSOR 100
#define CAMINHO_MAX 512
#define MAX_TEXTO 1000

// Estrutura para armazenar resposta de aluno
typedef struct {
    int id_aluno;
    char nome_aluno[MAX_NOME];
    char data_resposta[20];
    int pontuacao;
    char status_resposta[30];
} RespostaAluno;

// Estrutura para atividade no histórico
typedef struct {
    int id;
    char titulo[MAX_TEXTO];
    char tipo[20];
    char status[30];
    char data_criacao[20];
    char data_expiracao[20];
    int id_turma;
    char nome_turma[MAX_NOME];
    char professor[MAX_PROFESSOR];
    char disciplina[MAX_NOME];
    RespostaAluno *respostas;
    int total_alunos;
    int total_respondido;
    int num_respostas;
} HistoricoAtividade;

// Variáveis globais
HistoricoAtividade *historico = NULL;
int num_atividades_historico = 0;

// Função auxiliar para limpar a tela
void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Função para pausar a execução
void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
    fflush(stdin);
}

// Função para chamar o filtro Python
void chamar_filtro_python(const char *tipo_filtro) {
    char comando[512];
    
    #ifdef _WIN32
        // Em Windows, o script está em: codigos/ferramentas/filtro_historico/
        // Quando chamado de codigos/professores/, precisa subir 1 nível e depois descer
        // Usa python diretamente (deve estar no PATH)
        if (tipo_filtro && strlen(tipo_filtro) > 0) {
            snprintf(comando, sizeof(comando), 
                    "python \"..\\ferramentas\\filtro_historico\\Filtro_historico.py\" \"%s\"",
                    tipo_filtro);
        } else {
            snprintf(comando, sizeof(comando), 
                    "python \"..\\ferramentas\\filtro_historico\\Filtro_historico.py\"");
        }
    #else
        if (tipo_filtro && strlen(tipo_filtro) > 0) {
            snprintf(comando, sizeof(comando), 
                    "python3 '../ferramentas/filtro_historico/Filtro_historico.py' '%s'",
                    tipo_filtro);
        } else {
            snprintf(comando, sizeof(comando), 
                    "python3 '../ferramentas/filtro_historico/Filtro_historico.py'");
        }
    #endif
    
    system(comando);
}

// Função para exibir o menu de histórico
void menu_historico() {
    int opcao;
    
    while (1) {
        limpar_tela();

        printf("HISTÓRICO DE ATIVIDADES E AVALIAÇÕES\n");
        printf("📋 OPÇÕES:\n\n");
        printf("  [1] Ver todas as atividades/avaliações\n");
        printf("  [2] Filtrar por data (intervalo)\n");
        printf("  [3] Filtrar por turma\n");
        printf("  [4] Filtrar por tipo (Atividade/Avaliação)\n");
        printf("  [5] Filtrar por status\n");
        printf("  [6] Filtro avançado (combinar múltiplos filtros)\n");
        printf("  [7] Voltar ao menu principal\n\n");
        
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao) {
            case 1:
                limpar_tela();
                printf("⏳ Carregando histórico de atividades...\n\n");
                chamar_filtro_python("");
                pausar();
                break;
            
            case 2:
                limpar_tela();
                printf("⏳ Abrindo filtro por data...\n\n");
                // O script Python gerenciará a entrada de datas
                chamar_filtro_python("");
                pausar();
                break;
            
            case 3:
                limpar_tela();
                printf("⏳ Abrindo filtro por turma...\n\n");
                chamar_filtro_python("");
                pausar();
                break;
            
            case 4:
                limpar_tela();
                printf("Escolha o tipo:\n");
                printf("  [1] Atividade\n");
                printf("  [2] Avaliação\n");
                printf("\nEscolha: ");
                int tipo_escolha;
                scanf("%d", &tipo_escolha);
                getchar();
                
                char tipo_filtro[30] = "";
                if (tipo_escolha == 1) {
                    strcpy(tipo_filtro, "Atividade");
                } else if (tipo_escolha == 2) {
                    strcpy(tipo_filtro, "Avaliacao");
                }
                
                limpar_tela();
                printf("⏳ Filtrando por tipo: %s...\n\n", tipo_filtro);
                chamar_filtro_python(tipo_filtro);
                pausar();
                break;
            
            case 5:
                limpar_tela();
                printf("⏳ Abrindo filtro por status...\n\n");
                chamar_filtro_python("");
                pausar();
                break;
            
            case 6:
                limpar_tela();
                printf("⏳ Abrindo filtro avançado...\n\n");
                printf("Você terá acesso a todos os filtros disponíveis:\n");
                printf("  • Data (início e fim)\n");
                printf("  • Turma\n");
                printf("  • Tipo\n");
                printf("  • Status\n\n");
                chamar_filtro_python("");
                pausar();
                break;
            
            case 7:
                return;
            
            default:
                printf("\n❌ Opção inválida! Tente novamente.\n");
                pausar();
        }
    }
}

// Função principal do módulo de histórico
int main() {
    configurarConsole();
    
    printf("Iniciando módulo de histórico...\n");
    menu_historico();
    
    return 0;
}