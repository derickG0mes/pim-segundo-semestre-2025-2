// Painel para Administradores - Centro de Controle
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#endif
#include <sys/stat.h>

// Incluir módulo de sincronização automática de JSON
#include "../ferramentas/auto_sync.c"

// Configuração do console para UTF-8
void configurarConsole() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
}

#define MAX_LOGS 1000
#define MAX_USUARIOS 500

// Estrutura para logs de atividade
typedef struct {
    char ra_ou_usuario[50];
    char nome[150];
    char tipo_usuario[30];      // "Aluno", "Professor", "Admin"
    char tipo_atividade[50];    // "Login", "Criação", "Resposta", etc
    char data[20];
    char hora[20];
    char descricao[300];
} LogAtividade;

// Estrutura para estatísticas
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

// Variáveis globais
LogAtividade logs[MAX_LOGS];
int num_logs = 0;
char nome_admin_logado[150] = "";

// Função para limpar tela
void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Função para pausar
void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
    fflush(stdin);
}

// Função para extrair valor JSON
int extrair_valor_json(const char *buffer, const char *chave, char *out, size_t out_size) {
    char procura[256];
    snprintf(procura, sizeof(procura), "\"%s\": \"", chave);
    const char *p = strstr(buffer, procura);
    
    if (!p) {
        // Tenta buscar sem aspas (para números)
        snprintf(procura, sizeof(procura), "\"%s\": ", chave);
        p = strstr(buffer, procura);
        if (!p) return 0;
        p += strlen(procura);
        
        const char *fim = strpbrk(p, ",}");
        if (!fim) return 0;
        size_t len = (size_t)(fim - p);
        if (len >= out_size) len = out_size - 1;
        strncpy(out, p, len);
        out[len] = '\0';
        return 1;
    }
    
    p += strlen(procura);
    const char *fim = strchr(p, '"');
    if (!fim) return 0;
    size_t len = (size_t)(fim - p);
    if (len >= out_size) len = out_size - 1;
    strncpy(out, p, len);
    out[len] = '\0';
    return 1;
}

// Carregar logs de atividade
int carregar_logs() {
    FILE *fp = fopen("../../dados/logs_atividade.json", "r");
    if (!fp) {
        // Tentar caminho alternativo
        fp = fopen("../../../dados/logs_atividade.json", "r");
        if (!fp) {
            printf("⚠️ Arquivo de logs não encontrado\n");
            return 0;
        }
    }
    
    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = (char *)malloc(tamanho + 1);
    fread(buffer, 1, tamanho, fp);
    buffer[tamanho] = '\0';
    fclose(fp);
    
    // Parsear JSON simples
    const char *pos = buffer;
    int count = 0;
    
    while ((pos = strstr(pos, "\"ra_ou_usuario\"")) != NULL && count < MAX_LOGS) {
        const char *inicio_obj = pos;
        while (inicio_obj > buffer && *inicio_obj != '{') {
            inicio_obj--;
        }
        
        const char *fim_obj = strchr(pos, '}');
        size_t tamanho_obj = fim_obj - inicio_obj + 1;
        
        char obj_log[1024];
        strncpy(obj_log, inicio_obj, tamanho_obj);
        obj_log[tamanho_obj] = '\0';
        
        extrair_valor_json(obj_log, "ra_ou_usuario", logs[count].ra_ou_usuario, sizeof(logs[count].ra_ou_usuario));
        extrair_valor_json(obj_log, "nome", logs[count].nome, sizeof(logs[count].nome));
        extrair_valor_json(obj_log, "tipo_usuario", logs[count].tipo_usuario, sizeof(logs[count].tipo_usuario));
        extrair_valor_json(obj_log, "tipo_atividade", logs[count].tipo_atividade, sizeof(logs[count].tipo_atividade));
        extrair_valor_json(obj_log, "data", logs[count].data, sizeof(logs[count].data));
        extrair_valor_json(obj_log, "hora", logs[count].hora, sizeof(logs[count].hora));
        extrair_valor_json(obj_log, "descricao", logs[count].descricao, sizeof(logs[count].descricao));
        
        count++;
        pos++;
    }
    
    free(buffer);
    num_logs = count;
    return count;
}

// Contar usuários por tipo
Estatisticas calcular_estatisticas() {
    Estatisticas stats = {0};
    
    FILE *fp_alunos = fopen("../../dados/alunos_cadastros/cadastro_aluno.json", "r");
    FILE *fp_professores = fopen("../../dados/professores_cadastros/cadastro_professor.json", "r");
    FILE *fp_turmas = fopen("../../dados/turmas_cadastros/turmas.json", "r");
    
    // Contar alunos
    if (fp_alunos) {
        char linha[256];
        while (fgets(linha, sizeof(linha), fp_alunos)) {
            if (strstr(linha, "\"nome\"")) stats.total_alunos++;
        }
        fclose(fp_alunos);
    }
    
    // Contar professores
    if (fp_professores) {
        char linha[256];
        while (fgets(linha, sizeof(linha), fp_professores)) {
            if (strstr(linha, "\"nome\"")) stats.total_professores++;
        }
        fclose(fp_professores);
    }
    
    // Contar turmas
    if (fp_turmas) {
        char linha[256];
        while (fgets(linha, sizeof(linha), fp_turmas)) {
            if (strstr(linha, "\"id\"")) stats.total_turmas++;
        }
        fclose(fp_turmas);
    }
    
    stats.total_admins = 1; // Assumindo 1 admin para o exemplo
    
    // Contar atividades e logins
    time_t agora = time(NULL);
    struct tm *info_data = localtime(&agora);
    char data_hoje[20];
    strftime(data_hoje, sizeof(data_hoje), "%Y-%m-%d", info_data);
    
    for (int i = 0; i < num_logs; i++) {
        if (strcmp(logs[i].data, data_hoje) == 0) {
            if (strcmp(logs[i].tipo_atividade, "Login") == 0) {
                stats.logins_hoje++;
            }
            if (strcmp(logs[i].tipo_atividade, "Criação") == 0) {
                stats.atividades_postadas_hoje++;
            }
        }
        if (strcmp(logs[i].tipo_atividade, "Criação") == 0) {
            stats.atividades_postadas_total++;
        }
    }
    
    // Calcular taxa de uso
    stats.taxa_uso_diaria = (stats.logins_hoje > 0) ? ((float)stats.logins_hoje / (stats.total_alunos + stats.total_professores)) * 100 : 0;
    
    // Crescimento (simular como 5% ao dia)
    stats.crescimento_usuarios = 5;
    
    return stats;
}

// Exibir cabeçalho do painel
void exibir_cabecalho() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                                              ║\n");
    printf("║                         PAINEL DE CONTROLE DO ADMINISTRADOR                                 ║\n");
    printf("║                                                                                              ║\n");
    printf("║                                Admin Logado: %-60s║\n", nome_admin_logado);
    printf("║                                                                                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

// Exibir dashboard principal
void exibir_dashboard_principal() {
    limpar_tela();
    exibir_cabecalho();
    
    Estatisticas stats = calcular_estatisticas();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                              RESUMO DO SISTEMA                                                ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    // Seção de Usuários
    printf("👥 USUÁRIOS DO SISTEMA\n");
    printf("────────────────────────────────────────────────────────────────\n");
    printf("  Total de Alunos Cadastrados:         %3d\n", stats.total_alunos);
    printf("  Total de Professores Cadastrados:    %3d\n", stats.total_professores);
    printf("  Total de Administradores:            %3d\n", stats.total_admins);
    printf("  ────────────────────────────────────────────────────────────\n");
    printf("  TOTAL DE USUÁRIOS:                   %3d\n\n", stats.total_alunos + stats.total_professores + stats.total_admins);
    
    // Seção de Turmas e Atividades
    printf("📚 TURMAS E ATIVIDADES\n");
    printf("────────────────────────────────────────────────────────────────\n");
    printf("  Turmas Ativas no Sistema:            %3d\n", stats.total_turmas);
    printf("  Atividades Postadas (Total):         %3d\n", stats.atividades_postadas_total);
    printf("  Atividades Postadas Hoje:            %3d\n\n", stats.atividades_postadas_hoje);
    
    // Seção de Uso e Crescimento
    printf("📊 MÉTRICAS DE DESEMPENHO\n");
    printf("────────────────────────────────────────────────────────────────\n");
    printf("  Logins Realizados Hoje:              %3d\n", stats.logins_hoje);
    printf("  Taxa de Uso Diária:                  %.2f%%\n", stats.taxa_uso_diaria);
    printf("  Crescimento de Usuários (Dia):       +%d%%\n\n", stats.crescimento_usuarios);
    
    // Menu de opções
    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                  MENU PRINCIPAL                                              ║\n");
    printf("║                                                                                              ║\n");
    printf("║  [1] Ver Log Completo de Atividades                                                         ║\n");
    printf("║  [2] Ver Atividades de Um Usuário Específico                                                ║\n");
    printf("║  [3] Ver Atividades do Dia                                                                  ║\n");
    printf("║  [4] Ver Gráfico de Uso Horário                                                             ║\n");
    printf("║  [5] Gerenciar Usuários                                                                      ║\n");
    printf("║  [6] Gerenciar Atividades                                                                    ║\n");
    printf("║  [7] 🤖 Painel Inteligente com IA                                                            ║\n");
    printf("║  [0] Sair                                                                                    ║\n");
    printf("║                                                                                              ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
}

// Exibir log completo
void exibir_log_completo() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                          LOG COMPLETO DE ATIVIDADES DO SISTEMA                               ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    if (num_logs == 0) {
        printf("❌ Nenhuma atividade registrada.\n");
        pausar();
        return;
    }
    
    printf("ID  | Data        | Hora       | Usuário                | Tipo Atividade | Descrição\n");
    printf("────┼─────────────┼────────────┼────────────────────────┼────────────────┼────────────────────────────\n");
    
    for (int i = 0; i < num_logs; i++) {
        printf("%3d | %s | %s | %-22s | %-14s | %s\n",
               i + 1,
               logs[i].data,
               logs[i].hora,
               logs[i].nome,
               logs[i].tipo_atividade,
               logs[i].descricao);
    }
    
    printf("\n");
    pausar();
}

// Exibir atividades de usuário específico
void exibir_atividade_usuario_especifico() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    BUSCAR ATIVIDADES DE USUÁRIO ESPECÍFICO                                   ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    char nome_busca[150];
    printf("Digite o nome do usuário (ou RA/ID): ");
    fgets(nome_busca, sizeof(nome_busca), stdin);
    nome_busca[strcspn(nome_busca, "\n")] = 0;
    
    if (strlen(nome_busca) == 0) {
        printf("❌ Nome vazio!\n");
        pausar();
        return;
    }
    
    printf("\n");
    printf("ID  | Data        | Hora       | Tipo Atividade | Descrição\n");
    printf("────┼─────────────┼────────────┼────────────────┼────────────────────────────────────────\n");
    
    int encontrados = 0;
    for (int i = 0; i < num_logs; i++) {
        if (strstr(logs[i].nome, nome_busca) || strstr(logs[i].ra_ou_usuario, nome_busca)) {
            printf("%3d | %s | %s | %-14s | %s\n",
                   i + 1,
                   logs[i].data,
                   logs[i].hora,
                   logs[i].tipo_atividade,
                   logs[i].descricao);
            encontrados++;
        }
    }
    
    if (encontrados == 0) {
        printf("\n❌ Nenhuma atividade encontrada para '%s'.\n", nome_busca);
    } else {
        printf("\n✅ Total de atividades encontradas: %d\n", encontrados);
    }
    
    printf("\n");
    pausar();
}

// Exibir atividades do dia
void exibir_atividades_dia() {
    limpar_tela();
    exibir_cabecalho();
    
    time_t agora = time(NULL);
    struct tm *info_data = localtime(&agora);
    char data_hoje[20];
    strftime(data_hoje, sizeof(data_hoje), "%Y-%m-%d", info_data);
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                     ATIVIDADES REGISTRADAS HOJE (%s)                                  ║\n", data_hoje);
    printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("ID  | Hora       | Usuário                | Tipo Usuário  | Tipo Atividade | Descrição\n");
    printf("────┼────────────┼────────────────────────┼───────────────┼────────────────┼──────────────────────────\n");
    
    int encontrados = 0;
    for (int i = 0; i < num_logs; i++) {
        if (strcmp(logs[i].data, data_hoje) == 0) {
            printf("%3d | %s | %-22s | %-13s | %-14s | %s\n",
                   encontrados + 1,
                   logs[i].hora,
                   logs[i].nome,
                   logs[i].tipo_usuario,
                   logs[i].tipo_atividade,
                   logs[i].descricao);
            encontrados++;
        }
    }
    
    if (encontrados == 0) {
        printf("\n❌ Nenhuma atividade registrada para hoje.\n");
    } else {
        printf("\n✅ Total de atividades hoje: %d\n", encontrados);
    }
    
    printf("\n");
    pausar();
}

// Exibir gráfico de uso horário
void exibir_grafico_uso_horario() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        GRÁFICO DE USO HORÁRIO DO SISTEMA                                     ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    // Contar atividades por hora
    int uso_por_hora[24] = {0};
    for (int i = 0; i < num_logs; i++) {
        int hora = atoi(logs[i].hora);
        if (hora >= 0 && hora < 24) {
            uso_por_hora[hora]++;
        }
    }
    
    printf("Horário | Atividades | Gráfico de Barras\n");
    printf("────────┼────────────┼──────────────────────────────────────────────────────────\n");
    
    int max_atividades = 0;
    for (int i = 0; i < 24; i++) {
        if (uso_por_hora[i] > max_atividades) max_atividades = uso_por_hora[i];
    }
    
    for (int i = 0; i < 24; i++) {
        printf("%02d:00   | %10d | ", i, uso_por_hora[i]);
        
        int barra_tamanho = (max_atividades > 0) ? (uso_por_hora[i] * 40) / max_atividades : 0;
        for (int j = 0; j < barra_tamanho; j++) {
            printf("█");
        }
        printf("\n");
    }
    
    printf("\n");
    pausar();
}

// Menu de gerenciamento de usuários
void menu_gerenciar_usuarios() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        GERENCIAMENTO DE USUÁRIOS                                             ║\n");
    printf("║                                                                                              ║\n");
    printf("║  [1] Listar Todos os Alunos                                                                 ║\n");
    printf("║  [2] Listar Todos os Professores                                                            ║\n");
    printf("║  [3] Desativar Usuário                                                                      ║\n");
    printf("║  [4] Ver Estatísticas de Usuários                                                           ║\n");
    printf("║  [0] Voltar                                                                                  ║\n");
    printf("║                                                                                              ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Escolha uma opção: ");
    int opcao;
    scanf("%d", &opcao);
    getchar();
    
    switch (opcao) {
        case 1: {
            limpar_tela();
            exibir_cabecalho();
            printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
            printf("║                              TODOS OS ALUNOS CADASTRADOS                                     ║\n");
            printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
            
            FILE *fp = fopen("../../dados/alunos_cadastros/cadastro_aluno.json", "r");
            if (fp) {
                char linha[512];
                int count = 0;
                printf("ID | Nome\n");
                printf("───┼────────────────────────────────────────────────────────────────────────\n");
                while (fgets(linha, sizeof(linha), fp)) {
                    if (strstr(linha, "\"nome\"")) {
                        count++;
                        printf("%2d | (Carregado do JSON)\n", count);
                    }
                }
                fclose(fp);
                printf("\n✅ Total de alunos: %d\n", count);
            }
            pausar();
            break;
        }
        
        case 2: {
            limpar_tela();
            exibir_cabecalho();
            printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
            printf("║                         TODOS OS PROFESSORES CADASTRADOS                                     ║\n");
            printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
            
            FILE *fp = fopen("../../dados/professores_cadastros/cadastro_professor.json", "r");
            if (fp) {
                char linha[512];
                int count = 0;
                printf("ID | Nome\n");
                printf("───┼────────────────────────────────────────────────────────────────────────\n");
                while (fgets(linha, sizeof(linha), fp)) {
                    if (strstr(linha, "\"nome\"")) {
                        count++;
                        printf("%2d | (Carregado do JSON)\n", count);
                    }
                }
                fclose(fp);
                printf("\n✅ Total de professores: %d\n", count);
            }
            pausar();
            break;
        }
        
        case 0:
            return;
        
        default:
            printf("❌ Opção inválida!\n");
            pausar();
    }
}

// Menu de gerenciamento de atividades
void menu_gerenciar_atividades() {
    limpar_tela();
    exibir_cabecalho();
    
    printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                      GERENCIAMENTO DE ATIVIDADES                                             ║\n");
    printf("║                                                                                              ║\n");
    printf("║  [1] Listar Todas as Atividades                                                             ║\n");
    printf("║  [2] Remover Atividade                                                                      ║\n");
    printf("║  [3] Ver Estatísticas de Atividades                                                         ║\n");
    printf("║  [0] Voltar                                                                                  ║\n");
    printf("║                                                                                              ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Escolha uma opção: ");
    int opcao;
    scanf("%d", &opcao);
    getchar();
    
    switch (opcao) {
        case 1: {
            limpar_tela();
            exibir_cabecalho();
            printf("╔════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
            printf("║                       TODAS AS ATIVIDADES DO SISTEMA                                         ║\n");
            printf("╚════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n");
            
            FILE *fp = fopen("../../dados/atividades_e_avaliacoes/conteudo_atividades.json", "r");
            if (fp) {
                fseek(fp, 0, SEEK_END);
                long tamanho = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                
                char *buffer = (char *)malloc(tamanho + 1);
                fread(buffer, 1, tamanho, fp);
                buffer[tamanho] = '\0';
                fclose(fp);
                
                int count = 0;
                const char *pos = buffer;
                while ((pos = strstr(pos, "\"titulo\"")) != NULL) {
                    count++;
                    pos++;
                }
                
                printf("ID | Titulo | Data Criação | Data Expiração\n");
                printf("───┼────────┼──────────────┼────────────────\n");
                printf("(Carregado do JSON - Total: %d atividades)\n", count);
                
                free(buffer);
            }
            pausar();
            break;
        }
        
        case 0:
            return;
        
        default:
            printf("❌ Opção inválida!\n");
            pausar();
    }
}

// Menu principal do painel administrativo
void menu_painel_administrativo(const char *admin_name) {
    strcpy(nome_admin_logado, admin_name);
    carregar_logs();
    
    int opcao;
    while (1) {
        exibir_dashboard_principal();
        
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao) {
            case 1:
                exibir_log_completo();
                break;
            
            case 2:
                exibir_atividade_usuario_especifico();
                break;
            
            case 3:
                exibir_atividades_dia();
                break;
            
            case 4:
                exibir_grafico_uso_horario();
                break;
            
            case 5:
                menu_gerenciar_usuarios();
                break;
            
            case 6:
                menu_gerenciar_atividades();
                break;
            
            case 7:
                menu_painel_inteligente();
                break;
            
            case 0:
                printf("✅ Encerrando painel administrativo...\n");
                return;
            
            default:
                printf("❌ Opção inválida!\n");
                pausar();
        }
    }
}

// Função principal
int main() {
    configurarConsole();
    
    // Exemplo: admin "admin" logado
    menu_painel_administrativo("Administrador Principal");
    
    return 0;
}