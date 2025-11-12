// Área do Aluno - Dashboard Principal
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#endif
#include <sys/stat.h>

// Incluir módulo de sincronização automática de JSON
#include "../ferramentas/auto_sync.c"

// Função auxiliar para configuração do console para UTF-8
void configurarConsole() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
}

#define MAX_NOME 150
#define MAX_MATRICULA 50
#define MAX_SERIE 100
#define MAX_DISCIPLINA 50
#define CAMINHO_MAX 512
#define MAX_ATIVIDADES 100

// Estruturas de dados
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
    char status[20];         // "Pendente", "Concluída", "Em andamento"
    char data_postagem[20];
    char data_vencimento[20];
    char professor[MAX_NOME];
    int respondido;
    char data_entrega[20];
    float pontuacao;
} AtividadeAluno;

typedef struct {
    char texto[500];
    char alternativas[5][100];
    int num_alternativas;
    char resposta_correta[2];
} Questao;

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

// Variáveis globais
DadosAluno aluno_atual;
AtividadeAluno atividades[MAX_ATIVIDADES];
int num_atividades = 0;

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

// Função para extrair valor do JSON
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
        
        // Lê até a vírgula ou fecha chave
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

// Carregar dados do aluno
int carregar_dados_aluno(const char *ra) {
    FILE *fp = fopen("../../dados/alunos_cadastros/desempenho_alunos.json", "r");
    if (!fp) {
        printf("❌ Erro ao abrir arquivo de desempenho\n");
        return 0;
    }
    
    // Ler arquivo
    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = (char *)malloc(tamanho + 1);
    fread(buffer, 1, tamanho, fp);
    buffer[tamanho] = '\0';
    fclose(fp);
    
    // Procurar pelo RA no JSON
    char procura[256];
    snprintf(procura, sizeof(procura), "\"ra\": \"%s\"", ra);
    
    if (!strstr(buffer, procura)) {
        printf("❌ Aluno não encontrado\n");
        free(buffer);
        return 0;
    }
    
    // Encontra o objeto do aluno
    const char *inicio = strstr(buffer, procura);
    const char *inicio_obj = inicio;
    while (inicio_obj > buffer && *inicio_obj != '{') {
        inicio_obj--;
    }
    
    const char *fim_obj = strchr(inicio, '}');
    size_t tamanho_obj = fim_obj - inicio_obj + 1;
    
    char obj_aluno[2048];
    strncpy(obj_aluno, inicio_obj, tamanho_obj);
    obj_aluno[tamanho_obj] = '\0';
    
    // Extrai valores
    strcpy(aluno_atual.ra, ra);
    extrair_valor_json(obj_aluno, "nome", aluno_atual.nome, sizeof(aluno_atual.nome));
    extrair_valor_json(obj_aluno, "serie", aluno_atual.serie, sizeof(aluno_atual.serie));
    extrair_valor_json(obj_aluno, "matricula", aluno_atual.matricula, sizeof(aluno_atual.matricula));
    
    char temp[50];
    extrair_valor_json(obj_aluno, "media_geral", temp, sizeof(temp));
    aluno_atual.media_geral = atof(temp);
    
    extrair_valor_json(obj_aluno, "frequencia", temp, sizeof(temp));
    aluno_atual.frequencia = atof(temp);
    
    extrair_valor_json(obj_aluno, "situacao", aluno_atual.situacao, sizeof(aluno_atual.situacao));
    
    extrair_valor_json(obj_aluno, "atividades_concluidas", temp, sizeof(temp));
    aluno_atual.atividades_concluidas = atoi(temp);
    
    extrair_valor_json(obj_aluno, "atividades_pendentes", temp, sizeof(temp));
    aluno_atual.atividades_pendentes = atoi(temp);
    
    extrair_valor_json(obj_aluno, "mensagens_nao_lidas", temp, sizeof(temp));
    aluno_atual.mensagens_nao_lidas = atoi(temp);
    
    free(buffer);
    return 1;
}

// Carregar atividades do aluno
int carregar_atividades_aluno(const char *ra) {
    FILE *fp = fopen("../../dados/alunos_cadastros/atividades_aluno.json", "r");
    if (!fp) {
        printf("⚠️ Arquivo de atividades não encontrado\n");
        return 0;
    }
    
    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = (char *)malloc(tamanho + 1);
    fread(buffer, 1, tamanho, fp);
    buffer[tamanho] = '\0';
    fclose(fp);
    
    // Conta atividades do aluno
    int count = 0;
    const char *pos = buffer;
    
    while ((pos = strstr(pos, "\"id_aluno_ra\"")) != NULL && count < MAX_ATIVIDADES) {
        // Encontra o valor do RA
        const char *valor_ra = strstr(pos, ": \"");
        if (valor_ra) {
            valor_ra += 3;
            char aluno_ra[20];
            sscanf(valor_ra, "%19[^\"]", aluno_ra);
            
            if (strcmp(aluno_ra, ra) == 0) {
                // Encontra o objeto completo
                const char *inicio_obj = pos;
                while (inicio_obj > buffer && *inicio_obj != '{') {
                    inicio_obj--;
                }
                
                const char *fim_obj = strchr(pos, '}');
                size_t tamanho_obj = fim_obj - inicio_obj + 1;
                
                char obj_atividade[1024];
                strncpy(obj_atividade, inicio_obj, tamanho_obj);
                obj_atividade[tamanho_obj] = '\0';
                
                // Extrai valores
                char temp[50];
                extrair_valor_json(obj_atividade, "id_atividade", temp, sizeof(temp));
                atividades[count].id_atividade = atoi(temp);
                
                extrair_valor_json(obj_atividade, "titulo", atividades[count].titulo, sizeof(atividades[count].titulo));
                extrair_valor_json(obj_atividade, "disciplina", atividades[count].disciplina, sizeof(atividades[count].disciplina));
                extrair_valor_json(obj_atividade, "tipo", atividades[count].tipo, sizeof(atividades[count].tipo));
                extrair_valor_json(obj_atividade, "status", atividades[count].status, sizeof(atividades[count].status));
                extrair_valor_json(obj_atividade, "data_postagem", atividades[count].data_postagem, sizeof(atividades[count].data_postagem));
                extrair_valor_json(obj_atividade, "data_vencimento", atividades[count].data_vencimento, sizeof(atividades[count].data_vencimento));
                extrair_valor_json(obj_atividade, "professor", atividades[count].professor, sizeof(atividades[count].professor));
                
                extrair_valor_json(obj_atividade, "respondido", temp, sizeof(temp));
                atividades[count].respondido = (strcmp(temp, "true") == 0) ? 1 : 0;
                
                extrair_valor_json(obj_atividade, "pontuacao", temp, sizeof(temp));
                atividades[count].pontuacao = atof(temp);
                
                count++;
            }
        }
        pos++;
    }
    
    free(buffer);
    num_atividades = count;
    return count;
}

// Função para retornar cor baseada na situação
void exibir_cor_situacao(const char *situacao) {
    if (strcmp(situacao, "Excelente") == 0) {
        printf("⭐ EXCELENTE");
    } else if (strcmp(situacao, "Boa") == 0) {
        printf("✅ BOA");
    } else if (strcmp(situacao, "Regular") == 0) {
        printf("⚠️ REGULAR");
    } else if (strcmp(situacao, "Crítico") == 0) {
        printf("🔴 CRÍTICO");
    } else if (strcmp(situacao, "Ruim") == 0) {
        printf("❌ RUIM");
    } else {
        printf("%s", situacao);
    }
}

// Exibir dashboard do aluno
void exibir_dashboard_aluno() {
    limpar_tela();
    
    printf("                              DASHBOARD DO ALUNO\n\n");
    
    // Informações básicas
    printf("========== INFORMAÇÕES PESSOAIS ==========\n");
    printf("Nome: %s\n", aluno_atual.nome);
    printf("Matrícula: %s\n", aluno_atual.matricula);
    printf("RA: %s\n", aluno_atual.ra);
    printf("Série/Ano: %s\n\n", aluno_atual.serie);
    
    // Desempenho acadêmico
    printf("========== DESEMPENHO ACADÊMICO ==========\n");
    printf("Média Geral: %.2f\n", aluno_atual.media_geral);
    printf("Frequência: %.1f%%\n", aluno_atual.frequencia);
    printf("Situação Acadêmica: ");
    exibir_cor_situacao(aluno_atual.situacao);
    printf("\n\n");
    
    // Resumo de atividades
    printf("========== RESUMO DE ATIVIDADES ==========\n");
    printf("✅ Atividades Concluídas: %d\n", aluno_atual.atividades_concluidas);
    printf("⏳ Atividades Pendentes: %d\n", aluno_atual.atividades_pendentes);
    printf("💬 Mensagens Não Lidas: %d\n\n", aluno_atual.mensagens_nao_lidas);
    
    // Atividades recentes
    if (num_atividades > 0) {
        printf("========== ATIVIDADES RECENTES (Últimas 5) ==========\n");
        
        int limite = (num_atividades > 5) ? 5 : num_atividades;
        for (int i = num_atividades - limite; i < num_atividades; i++) {
            if (i >= 0) {
                printf("\n📋 Título: %s\n", atividades[i].titulo);
                printf("   Disciplina: %s | Professor: %s\n", atividades[i].disciplina, atividades[i].professor);
                printf("   Tipo: %s | Data Vencimento: %s\n", atividades[i].tipo, atividades[i].data_vencimento);
                
                // Exibir status com ícone apropriado
                printf("   Status: ");
                if (strcmp(atividades[i].status, "Concluída") == 0) {
                    printf("✅ %s", atividades[i].status);
                    if (atividades[i].pontuacao > 0) {
                        printf(" | Pontuação: %.1f", atividades[i].pontuacao);
                    }
                } else if (strcmp(atividades[i].status, "Pendente") == 0) {
                    printf("⏳ %s", atividades[i].status);
                } else if (strcmp(atividades[i].status, "Em andamento") == 0) {
                    printf("🔄 %s", atividades[i].status);
                } else {
                    printf("%s", atividades[i].status);
                }
                printf("\n");
            }
        }
        printf("\n");
    }
    
    // Menu de opções
    printf("========== OPÇÕES ==========\n");
    printf("[1] Ver todas as atividades\n");
    printf("[2] Ver atividades pendentes\n");
    printf("[3] Ver mensagens\n");
    printf("[4] Responder atividade\n");
    printf("[5] Voltar\n\n");
}

// Exibir todas as atividades
void exibir_todas_atividades() {
    limpar_tela();
    printf("                           TODAS AS ATIVIDADES\n\n");
    
    if (num_atividades == 0) {
        printf("❌ Nenhuma atividade encontrada.\n");
        pausar();
        return;
    }
    
    printf("ID | Título                                | Disciplina      | Status            | Vencimento\n");
    printf("---+---------------------------------------+-----------------+-------------------+--------------------\n");
    
    for (int i = 0; i < num_atividades; i++) {
        printf("%2d | %-37s| %-15s | %-17s | %s\n",
               i + 1,
               (strlen(atividades[i].titulo) > 37) ? 
                   strncpy((char *)malloc(38), atividades[i].titulo, 37) : atividades[i].titulo,
               atividades[i].disciplina,
               atividades[i].status,
               atividades[i].data_vencimento);
    }
    
    printf("\n");
    pausar();
}

// Exibir atividades pendentes
void exibir_atividades_pendentes() {
    limpar_tela();
    printf("                        ATIVIDADES PENDENTES\n\n");
    
    int encontradas = 0;
    printf("ID | Título                                | Disciplina      | Vencimento\n");
    printf("---+---------------------------------------+-----------------+--------------------\n");
    
    for (int i = 0; i < num_atividades; i++) {
        if (strcmp(atividades[i].status, "Pendente") == 0 || strcmp(atividades[i].status, "Em andamento") == 0) {
            printf("%2d | %-37s| %-15s | %s\n",
                   encontradas + 1,
                   (strlen(atividades[i].titulo) > 37) ? 
                       strncpy((char *)malloc(38), atividades[i].titulo, 37) : atividades[i].titulo,
                   atividades[i].disciplina,
                   atividades[i].data_vencimento);
            encontradas++;
        }
    }
    
    if (encontradas == 0) {
        printf("\n✅ Parabéns! Você não tem atividades pendentes.\n");
    }
    
    printf("\n");
    pausar();
}

// Função para atualizar o desempenho do aluno após responder
int atualizar_desempenho_aluno(const char *ra, int id_atividade, float pontuacao, int total_questoes, int acertos) {
    FILE *fp = fopen("../../dados/alunos_cadastros/desempenho_alunos.json", "r+");
    if (!fp) return 0;
    
    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = (char *)malloc(tamanho + 1);
    fread(buffer, 1, tamanho, fp);
    buffer[tamanho] = '\0';
    
    // Procura pelo RA e atualiza dados
    char procura[256];
    snprintf(procura, sizeof(procura), "\"ra\": \"%s\"", ra);
    
    if (!strstr(buffer, procura)) {
        free(buffer);
        fclose(fp);
        return 0;
    }
    
    // Encontra o objeto do aluno
    const char *inicio = strstr(buffer, procura);
    const char *inicio_obj = inicio;
    while (inicio_obj > buffer && *inicio_obj != '{') {
        inicio_obj--;
    }
    
    const char *fim_obj = strchr(inicio, '}');
    
    // Extrai dados atuais
    char obj_aluno[2048];
    size_t tamanho_obj = fim_obj - inicio_obj + 1;
    strncpy(obj_aluno, inicio_obj, tamanho_obj);
    obj_aluno[tamanho_obj] = '\0';
    
    char temp[50];
    extrair_valor_json(obj_aluno, "atividades_concluidas", temp, sizeof(temp));
    int atividades_concluidas = atoi(temp) + 1;
    
    extrair_valor_json(obj_aluno, "atividades_pendentes", temp, sizeof(temp));
    int atividades_pendentes = atoi(temp) - 1;
    if (atividades_pendentes < 0) atividades_pendentes = 0;
    
    extrair_valor_json(obj_aluno, "media_geral", temp, sizeof(temp));
    float media_atual = atof(temp);
    float nova_media = (media_atual + pontuacao) / 2.0;
    
    // Criar novo objeto do aluno com valores atualizados
    char novo_obj[2048];
    snprintf(novo_obj, sizeof(novo_obj),
        "  {\n"
        "    \"ra\": \"%s\",\n",
        ra);
    
    extrair_valor_json(obj_aluno, "nome", temp, sizeof(temp));
    snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
        "    \"nome\": \"%s\",\n", temp);
    
    extrair_valor_json(obj_aluno, "serie", temp, sizeof(temp));
    snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
        "    \"serie\": \"%s\",\n", temp);
    
    extrair_valor_json(obj_aluno, "matricula", temp, sizeof(temp));
    snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
        "    \"matricula\": \"%s\",\n", temp);
    
    snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
        "    \"media_geral\": %.1f,\n", nova_media);
    
    extrair_valor_json(obj_aluno, "frequencia", temp, sizeof(temp));
    snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
        "    \"frequencia\": %s,\n", temp);
    
    snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
        "    \"situacao\": \"Ativo\",\n");
    
    snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
        "    \"atividades_concluidas\": %d,\n", atividades_concluidas);
    
    snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
        "    \"atividades_pendentes\": %d,\n", atividades_pendentes);
    
    extrair_valor_json(obj_aluno, "mensagens_nao_lidas", temp, sizeof(temp));
    snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
        "    \"mensagens_nao_lidas\": %s\n", temp);
    
    snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj), "  }");
    
    // Substituir no buffer
    char novo_buffer[50000];
    int pos_antes = inicio_obj - buffer;
    int pos_depois = fim_obj - buffer + 1;
    
    strncpy(novo_buffer, buffer, pos_antes);
    novo_buffer[pos_antes] = '\0';
    strcat(novo_buffer, novo_obj);
    strcat(novo_buffer, buffer + pos_depois);
    
    // Escrever de volta
    fseek(fp, 0, SEEK_SET);
    ftruncate(fileno(fp), 0);
    fwrite(novo_buffer, 1, strlen(novo_buffer), fp);
    fclose(fp);
    
    free(buffer);
    return 1;
}

// Função para atualizar o status da atividade do aluno
int atualizar_status_atividade(const char *ra, int id_atividade, float pontuacao, int acertos, int total) {
    FILE *fp = fopen("../../dados/alunos_cadastros/atividades_aluno.json", "r+");
    if (!fp) return 0;
    
    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = (char *)malloc(tamanho + 1);
    fread(buffer, 1, tamanho, fp);
    buffer[tamanho] = '\0';
    
    // Procura a atividade do aluno
    char procura[256];
    snprintf(procura, sizeof(procura), "\"id_aluno_ra\": \"%s\"", ra);
    
    const char *pos = buffer;
    while ((pos = strstr(pos, procura)) != NULL) {
        const char *pos_id = strstr(pos, "\"id_atividade\"");
        if (pos_id) {
            char id_str[20];
            sscanf(pos_id + 16, "%d", &id_atividade);
            
            if (id_atividade == id_atividade) {
                // Encontra o objeto completo
                const char *inicio_obj = pos;
                while (inicio_obj > buffer && *inicio_obj != '{') {
                    inicio_obj--;
                }
                
                const char *fim_obj = strchr(pos, '}');
                
                char obj_atividade[1024];
                size_t tamanho_obj = fim_obj - inicio_obj + 1;
                strncpy(obj_atividade, inicio_obj, tamanho_obj);
                obj_atividade[tamanho_obj] = '\0';
                
                // Criar novo objeto com status atualizado
                char novo_obj[1024];
                char temp[50];
                
                snprintf(novo_obj, sizeof(novo_obj),
                    "  {\n"
                    "    \"id_aluno_ra\": \"%s\",\n", ra);
                
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"id_atividade\": %d,\n", id_atividade);
                
                extrair_valor_json(obj_atividade, "titulo", temp, sizeof(temp));
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"titulo\": \"%s\",\n", temp);
                
                extrair_valor_json(obj_atividade, "disciplina", temp, sizeof(temp));
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"disciplina\": \"%s\",\n", temp);
                
                extrair_valor_json(obj_atividade, "tipo", temp, sizeof(temp));
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"tipo\": \"%s\",\n", temp);
                
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"status\": \"Concluída\",\n");
                
                extrair_valor_json(obj_atividade, "data_postagem", temp, sizeof(temp));
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"data_postagem\": \"%s\",\n", temp);
                
                extrair_valor_json(obj_atividade, "data_vencimento", temp, sizeof(temp));
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"data_vencimento\": \"%s\",\n", temp);
                
                extrair_valor_json(obj_atividade, "professor", temp, sizeof(temp));
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"professor\": \"%s\",\n", temp);
                
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"respondido\": true,\n");
                
                // Data de entrega de hoje
                time_t t = time(NULL);
                struct tm *tm_info = localtime(&t);
                char data_hoje[20];
                strftime(data_hoje, sizeof(data_hoje), "%Y-%m-%d", tm_info);
                
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"data_entrega\": \"%s\",\n", data_hoje);
                
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj),
                    "    \"pontuacao\": %.1f\n", pontuacao);
                
                snprintf(novo_obj + strlen(novo_obj), sizeof(novo_obj) - strlen(novo_obj), "  }");
                
                // Substituir no buffer
                char novo_buffer[50000];
                int pos_antes = inicio_obj - buffer;
                int pos_depois = fim_obj - buffer + 1;
                
                strncpy(novo_buffer, buffer, pos_antes);
                novo_buffer[pos_antes] = '\0';
                strcat(novo_buffer, novo_obj);
                strcat(novo_buffer, buffer + pos_depois);
                
                // Escrever de volta
                fseek(fp, 0, SEEK_SET);
                ftruncate(fileno(fp), 0);
                fwrite(novo_buffer, 1, strlen(novo_buffer), fp);
                fclose(fp);
                
                free(buffer);
                return 1;
            }
        }
        pos++;
    }
    
    free(buffer);
    fclose(fp);
    return 0;
}

// Função para atualizar desempenho do aluno na turma (turmas_informacoes.json)
int atualizar_desempenho_turma(const char *ra, const char *nome, const char *serie, float pontuacao) {
    // Tenta vários caminhos para encontrar o arquivo
    const char *caminhos[] = {
        "../../dados/turmas_informacoes/turmas_informacoes.json",
        "../dados/turmas_informacoes/turmas_informacoes.json",
        "dados/turmas_informacoes/turmas_informacoes.json",
        "../../../../dados/turmas_informacoes/turmas_informacoes.json"
    };
    
    FILE *fp = NULL;
    for (int i = 0; i < 4; i++) {
        fp = fopen(caminhos[i], "r+");
        if (fp) break;
    }
    
    if (!fp) return 0;
    
    fseek(fp, 0, SEEK_END);
    long tamanho = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char *buffer = (char *)malloc(tamanho + 1);
    if (!buffer) {
        fclose(fp);
        return 0;
    }
    
    fread(buffer, 1, tamanho, fp);
    buffer[tamanho] = '\0';
    
    // Procura a série
    char procura_serie[256];
    snprintf(procura_serie, sizeof(procura_serie), "\"%s\":", serie);
    
    char *pos_serie = strstr(buffer, procura_serie);
    if (!pos_serie) {
        free(buffer);
        fclose(fp);
        return 0;
    }
    
    // Procura o RA dentro da série
    char procura_ra[256];
    snprintf(procura_ra, sizeof(procura_ra), "\"ra\": \"%s\"", ra);
    
    char *pos_ra = strstr(pos_serie, procura_ra);
    if (!pos_ra) {
        free(buffer);
        fclose(fp);
        return 0;
    }
    
    // Encontra o objeto completo do aluno
    const char *inicio_obj = pos_ra;
    while (inicio_obj > buffer && *inicio_obj != '{') {
        inicio_obj--;
    }
    
    const char *fim_obj = strchr(pos_ra, '}');
    
    char obj_aluno[1024];
    size_t tamanho_obj = fim_obj - inicio_obj + 1;
    strncpy(obj_aluno, inicio_obj, tamanho_obj);
    obj_aluno[tamanho_obj] = '\0';
    
    // Extrai desempenho atual
    char desempenho_atual[100];
    extrair_valor_json(obj_aluno, "desempenho", desempenho_atual, sizeof(desempenho_atual));
    
    // Define novo desempenho baseado na pontuação
    const char *novo_desempenho;
    if (pontuacao >= 9.0) {
        novo_desempenho = "Excelente";
    } else if (pontuacao >= 7.0) {
        novo_desempenho = "Bom";
    } else if (pontuacao >= 5.0) {
        novo_desempenho = "Regular";
    } else if (pontuacao >= 3.0) {
        novo_desempenho = "Insuficiente";
    } else {
        novo_desempenho = "Crítico";
    }
    
    // Criar novo objeto com desempenho atualizado
    char novo_obj[1024];
    snprintf(novo_obj, sizeof(novo_obj),
        "    {\n"
        "      \"nome\": \"%s\",\n"
        "      \"ra\": \"%s\",\n"
        "      \"desempenho\": \"%s\"\n"
        "    }",
        nome, ra, novo_desempenho);
    
    // Substituir no buffer
    char novo_buffer[50000];
    int pos_antes = inicio_obj - buffer;
    int pos_depois = fim_obj - buffer + 1;
    
    strncpy(novo_buffer, buffer, pos_antes);
    novo_buffer[pos_antes] = '\0';
    strcat(novo_buffer, novo_obj);
    strcat(novo_buffer, buffer + pos_depois);
    
    // Escrever de volta
    fseek(fp, 0, SEEK_SET);
    ftruncate(fileno(fp), 0);
    fwrite(novo_buffer, 1, strlen(novo_buffer), fp);
    fclose(fp);
    
    free(buffer);
    return 1;
}

// Função para exibir questões e coletar respostas
void responder_atividade(const char *ra, int indice_atividade) {
    if (indice_atividade < 0 || indice_atividade >= num_atividades) {
        printf("❌ Atividade inválida!\n");
        pausar();
        return;
    }
    
    AtividadeAluno *ativ = &atividades[indice_atividade];
    
    if (ativ->respondido) {
        limpar_tela();
        printf("⚠️ Você já respondeu a esta atividade!\n");
        printf("Status: %s | Pontuação: %.1f\n", ativ->status, ativ->pontuacao);
        pausar();
        return;
    }
    
    limpar_tela();
    printf("                      RESPONDER ATIVIDADE\n\n");
    printf("Título: %s\n", ativ->titulo);
    printf("Disciplina: %s\n", ativ->disciplina);
    printf("Professor: %s\n", ativ->professor);
    printf("Data de Vencimento: %s\n\n", ativ->data_vencimento);
    printf("===============================================\n\n");
    
    // Simular questões da atividade (em uma versão real, isso viria do conteudo_atividades.json)
    Questao questoes[10];
    int num_questoes = 0;
    
    // Dados de exemplo para demonstração
    if (ativ->id_atividade == 1) {
        strcpy(questoes[0].texto, "A gente falou sobre Linguagem C?");
        strcpy(questoes[0].alternativas[0], "Sim");
        strcpy(questoes[0].alternativas[1], "Não");
        questoes[0].num_alternativas = 2;
        strcpy(questoes[0].resposta_correta, "A");
        
        strcpy(questoes[1].texto, "A gente falou sobre Java?");
        strcpy(questoes[1].alternativas[0], "Sim");
        strcpy(questoes[1].alternativas[1], "Não");
        questoes[1].num_alternativas = 2;
        strcpy(questoes[1].resposta_correta, "B");
        
        num_questoes = 2;
    } else {
        strcpy(questoes[0].texto, "Qual é o resultado de 2 + 2?");
        strcpy(questoes[0].alternativas[0], "3");
        strcpy(questoes[0].alternativas[1], "4");
        strcpy(questoes[0].alternativas[2], "5");
        questoes[0].num_alternativas = 3;
        strcpy(questoes[0].resposta_correta, "B");
        
        num_questoes = 1;
    }
    
    char respostas[10];
    int acertos = 0;
    
    // Mostrar e coletar respostas
    for (int i = 0; i < num_questoes; i++) {
        printf("Questão %d: %s\n", i + 1, questoes[i].texto);
        printf("───────────────────────────────────────\n");
        
        for (int j = 0; j < questoes[i].num_alternativas; j++) {
            printf("[%c] %s\n", 'A' + j, questoes[i].alternativas[j]);
        }
        
        char resposta[2];
        int resposta_valida = 0;
        while (!resposta_valida) {
            printf("Sua resposta (A-%c): ", 'A' + questoes[i].num_alternativas - 1);
            fgets(resposta, sizeof(resposta), stdin);
            resposta[0] = toupper(resposta[0]);
            getchar();
            
            if (resposta[0] >= 'A' && resposta[0] < 'A' + questoes[i].num_alternativas) {
                resposta_valida = 1;
                respostas[i] = resposta[0];
                
                if (resposta[0] == questoes[i].resposta_correta[0]) {
                    acertos++;
                    printf("✅ Correto!\n");
                } else {
                    printf("❌ Incorreto. A resposta correta é %s\n", questoes[i].resposta_correta);
                }
            } else {
                printf("❌ Resposta inválida!\n");
            }
        }
        printf("\n");
    }
    
    // Calcular pontuação
    float pontuacao = (acertos * 10.0) / num_questoes;
    
    limpar_tela();
    printf("                    RESULTADO FINAL\n");
    printf("===============================================\n\n");
    printf("Total de questões: %d\n", num_questoes);
    printf("Acertos: %d\n", acertos);
    printf("Erros: %d\n", num_questoes - acertos);
    printf("Pontuação: %.1f/10.0\n\n", pontuacao);
    
    // Atualizar status e desempenho
    if (atualizar_status_atividade(ra, ativ->id_atividade, pontuacao, acertos, num_questoes)) {
        if (atualizar_desempenho_aluno(ra, ativ->id_atividade, pontuacao, num_questoes, acertos)) {
            printf("✅ Respostas registradas com sucesso!\n");
            printf("✅ Seu desempenho foi atualizado!\n");
            
            // Atualizar a estrutura local
            ativ->respondido = 1;
            ativ->pontuacao = pontuacao;
            strcpy(ativ->status, "Concluída");
            
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            strftime(ativ->data_entrega, sizeof(ativ->data_entrega), "%Y-%m-%d", tm_info);
            
            // Atualizar desempenho na turma (AUTOMATIZADO via auto_sync)
            auto_atualizar_desempenho("dados/turmas_informacoes.json", ra, aluno_atual.nome, aluno_atual.serie, pontuacao);
            
            // Registrar resposta no arquivo de logs (AUTOMATIZADO via auto_sync)
            char descricao[300];
            snprintf(descricao, sizeof(descricao), "Aluno respondeu atividade: %s (Pontuação: %.1f)", ativ->titulo, pontuacao);
            auto_log("dados/logs_atividade.json", ra, "Resposta", descricao);
        } else {
            printf("⚠️ Erro ao atualizar desempenho\n");
        }
    } else {
        printf("⚠️ Erro ao registrar respostas\n");
    }
    
    printf("\n");
    pausar();
}

// Menu para selecionar atividade para responder
void menu_responder_atividade(const char *ra) {
    limpar_tela();
    printf("                    RESPONDER ATIVIDADE\n\n");
    
    int atividades_pendentes_count = 0;
    for (int i = 0; i < num_atividades; i++) {
        if (!atividades[i].respondido && 
            (strcmp(atividades[i].status, "Pendente") == 0 || 
             strcmp(atividades[i].status, "Em andamento") == 0)) {
            atividades_pendentes_count++;
        }
    }
    
    if (atividades_pendentes_count == 0) {
        printf("✅ Parabéns! Você não tem atividades pendentes para responder.\n");
        pausar();
        return;
    }
    
    printf("Selecione uma atividade para responder:\n\n");
    int opcao = 1;
    for (int i = 0; i < num_atividades; i++) {
        if (!atividades[i].respondido && 
            (strcmp(atividades[i].status, "Pendente") == 0 || 
             strcmp(atividades[i].status, "Em andamento") == 0)) {
            printf("[%d] %s (%s) - Vence em: %s\n", 
                   opcao, atividades[i].titulo, atividades[i].disciplina, atividades[i].data_vencimento);
            opcao++;
        }
    }
    printf("[0] Voltar\n\n");
    
    printf("Digite sua escolha: ");
    int escolha;
    scanf("%d", &escolha);
    getchar();
    
    if (escolha == 0) return;
    
    // Encontrar a atividade correspondente
    int contador = 1;
    for (int i = 0; i < num_atividades; i++) {
        if (!atividades[i].respondido && 
            (strcmp(atividades[i].status, "Pendente") == 0 || 
             strcmp(atividades[i].status, "Em andamento") == 0)) {
            if (contador == escolha) {
                responder_atividade(ra, i);
                return;
            }
            contador++;
        }
    }
    
    printf("❌ Opção inválida!\n");
    pausar();
}

// Menu principal da área do aluno
void menu_area_aluno(const char *ra) {
    if (!carregar_dados_aluno(ra)) {
        printf("❌ Erro ao carregar dados do aluno\n");
        return;
    }
    
    carregar_atividades_aluno(ra);
    
    int opcao;
    while (1) {
        exibir_dashboard_aluno();
        
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();
        
        switch (opcao) {
            case 1:
                exibir_todas_atividades();
                break;
            
            case 2:
                exibir_atividades_pendentes();
                break;
            
            case 3:
                limpar_tela();
                printf("📧 MENSAGENS\n\n");
                printf("Você tem %d mensagem(ns) não lida(s)\n", aluno_atual.mensagens_nao_lidas);
                if (aluno_atual.mensagens_nao_lidas > 0) {
                    printf("\n📨 Novas mensagens:\n");
                    printf("  1. Professor João Silva enviou um feedback sobre o Simulado\n");
                    printf("  2. Coordenação: Atualização sobre calendário acadêmico\n");
                }
                pausar();
                break;
            
            case 4:
                menu_responder_atividade(ra);
                break;
            
            case 5:
                return;
            
            default:
                printf("❌ Opção inválida!\n");
                pausar();
        }
    }
}

// Função principal (para teste)
int main() {
    configurarConsole();
    
    // Exemplo de uso: aluno com RA "3123131"
    menu_area_aluno("3123131");
    
    // Garantir que todos os dados pendentes sejam sincronizados antes de sair
    finalizar_sistema();
    
    return 0;
}
