// Painel de Atividades e Avaliações
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
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

#define MAX_TURMAS 50
#define MAX_ATIVIDADES 100
#define MAX_NOME 100
#define MAX_PROFESSOR 100
#define CAMINHO_MAX 512  // Alterado de MAX_PATH para evitar conflito com Windows.h
#define MAX_QUESTOES 50
#define MAX_ALTERNATIVAS 5
#define MAX_TEXTO 1000

// Estruturas de dados
typedef struct {
    char texto[MAX_TEXTO];
    char alternativas[MAX_ALTERNATIVAS][MAX_TEXTO];
    char resposta_correta;  // Letra da alternativa correta (A, B, C, D, E)
    int num_alternativas;
} Questao;

typedef struct {
    int id;
    char enunciado[MAX_TEXTO];
    Questao questoes[MAX_QUESTOES];
    int num_questoes;
} ConteudoAtividade;

typedef struct {
    int id;
    char serie_turma[MAX_NOME];
    char professor_responsavel[MAX_PROFESSOR];
    char horario[30];
    char dias_semana[100];
    char sala[20];
    char curso[50];
    char processo_letivo[10];
    int capacidade;
} Turma;

typedef struct {
    int id;
    char titulo[MAX_NOME];
    char tipo[20]; // "Atividade" ou "Avaliação"
    char status[30]; // "Expirado", "Respondido", "Nenhum aluno respondeu"
    int id_turma;
    char data_criacao[20];
    char data_expiracao[20];
    int num_respostas;
} Atividade;

// Variáveis globais
Turma turmas[MAX_TURMAS];
Atividade atividades[MAX_ATIVIDADES];
ConteudoAtividade conteudos[MAX_ATIVIDADES];
int num_turmas = 0;
int num_atividades = 0;
char nome_professor_global[MAX_PROFESSOR] = "";  // Variável global para armazenar nome do professor

// Função para localizar arquivo JSON de turmas
int localizar_arquivo_turmas(char *out_path, size_t out_size) {
    const char *candidatos[] = {
        "../../dados/turmas_cadastros/cadastro_turma.json"
    };
    size_t n = sizeof(candidatos) / sizeof(candidatos[0]);
    
    for (size_t i = 0; i < n; i++) {
        FILE *f = fopen(candidatos[i], "r");
        if (f) {
            fclose(f);
            snprintf(out_path, out_size, "%s", candidatos[i]);
            return 1;
        }
    }
    return 0;
}

// Função para extrair valor do JSON
int extrair_valor_json(const char *buffer, const char *chave, size_t start_pos, char *out, size_t out_size) {
    char procura[128];
    snprintf(procura, sizeof(procura), "\"%s\": \"", chave);
    const char *p = strstr(buffer + start_pos, procura);
    if (!p) {
        // Tenta buscar sem aspas (para números)
        snprintf(procura, sizeof(procura), "\"%s\": ", chave);
        p = strstr(buffer + start_pos, procura);
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

// Funções auxiliares
void limpar_tela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pausar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
    getchar();
}

// Carregar turmas do arquivo JSON
int carregar_turmas(const char *nome_professor) {
    char arquivo_path[CAMINHO_MAX];
    if (!localizar_arquivo_turmas(arquivo_path, sizeof(arquivo_path))) {
        printf("Erro: Arquivo de turmas não encontrado.\n");
        return 0;
    }
    
    // Ler o arquivo
    FILE *f = fopen(arquivo_path, "r");
    if (!f) return 0;
    
    char buffer[50000];
    size_t bytes = fread(buffer, 1, sizeof(buffer) - 1, f);
    fclose(f);
    buffer[bytes] = '\0';
    
    // Procurar turmas do professor
    num_turmas = 0;
    const char *pos = buffer;
    while ((pos = strstr(pos, "\"professor_responsavel\":")) != NULL && num_turmas < MAX_TURMAS) {
        size_t obj_pos = (size_t)(pos - buffer);
        char professor[MAX_PROFESSOR];
        
        if (extrair_valor_json(buffer, "professor_responsavel", obj_pos, professor, sizeof(professor)) &&
            strcmp(professor, nome_professor) == 0) {
            
            // Encontrou uma turma do professor, extrair dados
            char id_str[10];
            Turma *t = &turmas[num_turmas];
            
            // Gera ID sequencial
            t->id = num_turmas + 1;
            
            // Extrai os demais campos
            extrair_valor_json(buffer, "serie_turma", obj_pos, t->serie_turma, sizeof(t->serie_turma));
            extrair_valor_json(buffer, "professor_responsavel", obj_pos, t->professor_responsavel, sizeof(t->professor_responsavel));
            extrair_valor_json(buffer, "horario", obj_pos, t->horario, sizeof(t->horario));
            extrair_valor_json(buffer, "dias_semana", obj_pos, t->dias_semana, sizeof(t->dias_semana));
            extrair_valor_json(buffer, "sala", obj_pos, t->sala, sizeof(t->sala));
            extrair_valor_json(buffer, "curso", obj_pos, t->curso, sizeof(t->curso));
            extrair_valor_json(buffer, "processo_letivo", obj_pos, t->processo_letivo, sizeof(t->processo_letivo));
            
            char capacidade_str[10];
            if (extrair_valor_json(buffer, "capacidade", obj_pos, capacidade_str, sizeof(capacidade_str))) {
                t->capacidade = atoi(capacidade_str);
            }
            
            num_turmas++;
        }
        pos++;
    }
    
    return num_turmas > 0;
}

// Função para garantir que o diretório existe
void criar_diretorio_se_necessario(const char *path) {
    #ifdef _WIN32
        _mkdir("../../dados");
        _mkdir("../../dados/atividades_e_avaliacoes");
    #else
        mkdir("../../dados", 0755);
        mkdir("../../dados/atividades_e_avaliacoes", 0755);
    #endif
}

// Salvar atividades em arquivo JSON (DESCONTINUADO - usar apenas conteudo_atividades.json e conteudo_avaliacoes.json)
void salvar_atividades_arquivo() {
    // Esta função foi descontinuada
    // Os dados agora são salvos apenas em conteudo_atividades.json e conteudo_avaliacoes.json
    // dentro da pasta atividades_e_avaliacoes
}

// Exibir turmas disponíveis
void exibir_turmas() {
    printf("\nTURMAS RESPONSÁVEIS\n");

    for (int i = 0; i < num_turmas; i++) {
        printf("  [%d] %s\n", turmas[i].id, turmas[i].serie_turma);
        printf("      Curso: %s\n", turmas[i].curso);
        printf("      Horário: %s\n", turmas[i].horario);
        printf("      Dias: %s\n", turmas[i].dias_semana);
        printf("      Capacidade: %d alunos\n", turmas[i].capacidade);
        printf("      Processo Letivo: %s\n", turmas[i].processo_letivo);
        printf("\n");
    }
}

// Exibir atividades de uma turma específica
void exibir_atividades_turma(int id_turma) {
    int encontradas = 0;

    printf("ATIVIDADES E AVALIAÇÕES\n");

    printf("%-5s %-30s %-12s %-25s %-10s\n", "ID", "TÍTULO", "TIPO", "STATUS", "RESPOSTAS");
    printf("─────────────────────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < num_atividades; i++) {
        if (atividades[i].id_turma == id_turma) {
            printf("%-5d %-30s %-12s %-25s %-10d\n",
                   atividades[i].id,
                   atividades[i].titulo,
                   atividades[i].tipo,
                   atividades[i].status,
                   atividades[i].num_respostas);
            encontradas++;
        }
    }
    
    if (encontradas == 0) {
        printf("\n  Nenhuma atividade encontrada para esta turma.\n");
    }
    
    printf("\n");
}

// Salvar conteúdo das atividades em arquivo JSON separado
void salvar_conteudo_atividades() {
    criar_diretorio_se_necessario("../../dados/atividades_e_avaliacoes");
    
    // Arquivo para atividades
    char arquivo_atividades[CAMINHO_MAX];
    snprintf(arquivo_atividades, sizeof(arquivo_atividades), 
             "../../dados/atividades_e_avaliacoes/conteudo_atividades.json");
    
    // Arquivo para avaliações
    char arquivo_avaliacoes[CAMINHO_MAX];
    snprintf(arquivo_avaliacoes, sizeof(arquivo_avaliacoes), 
             "../../dados/atividades_e_avaliacoes/conteudo_avaliacoes.json");
    
    // Salvar atividades
    FILE *fp_atividades = fopen(arquivo_atividades, "w");
    FILE *fp_avaliacoes = fopen(arquivo_avaliacoes, "w");
    
    if (fp_atividades == NULL || fp_avaliacoes == NULL) {
        printf("Erro ao criar arquivos de conteúdo!\n");
        if (fp_atividades) fclose(fp_atividades);
        if (fp_avaliacoes) fclose(fp_avaliacoes);
        return;
    }
    
    // Início dos arrays JSON
    fprintf(fp_atividades, "[\n");
    fprintf(fp_avaliacoes, "[\n");
    
    int num_ativs = 0, num_avals = 0;
    
    // Para cada conteúdo
    for (int i = 0; i < num_atividades; i++) {
        // Decide qual arquivo usar baseado no tipo
        FILE *fp_atual;
        int *contador;
        
        if (strcmp(atividades[i].tipo, "Atividade") == 0) {
            fp_atual = fp_atividades;
            contador = &num_ativs;
        } else {
            fp_atual = fp_avaliacoes;
            contador = &num_avals;
        }
        
        // Se não é o primeiro item, adiciona vírgula
        if (*contador > 0) {
            fprintf(fp_atual, ",\n");
        }
        
        // Incrementa o contador apropriado
        (*contador)++;
        
        // Escreve o conteúdo
        fprintf(fp_atual, "  {\n");
        fprintf(fp_atual, "    \"id\": %d,\n", conteudos[i].id);
        fprintf(fp_atual, "    \"titulo\": \"%s\",\n", atividades[i].titulo);
        fprintf(fp_atual, "    \"enunciado\": \"%s\",\n", conteudos[i].enunciado);
        fprintf(fp_atual, "    \"id_turma\": %d,\n", atividades[i].id_turma);
        fprintf(fp_atual, "    \"data_criacao\": \"%s\",\n", atividades[i].data_criacao);
        fprintf(fp_atual, "    \"data_expiracao\": \"%s\",\n", atividades[i].data_expiracao);
        fprintf(fp_atual, "    \"questoes\": [\n");
        
        for (int j = 0; j < conteudos[i].num_questoes; j++) {
            fprintf(fp_atual, "      {\n");
            fprintf(fp_atual, "        \"texto\": \"%s\",\n", conteudos[i].questoes[j].texto);
            fprintf(fp_atual, "        \"alternativas\": [\n");
            
            for (int k = 0; k < conteudos[i].questoes[j].num_alternativas; k++) {
                fprintf(fp_atual, "          \"%s\"%s\n", 
                    conteudos[i].questoes[j].alternativas[k],
                    k < conteudos[i].questoes[j].num_alternativas - 1 ? "," : "");
            }
            
            fprintf(fp_atual, "        ],\n");
            fprintf(fp_atual, "        \"resposta_correta\": \"%c\"\n", conteudos[i].questoes[j].resposta_correta);
            fprintf(fp_atual, "      }%s\n", j < conteudos[i].num_questoes - 1 ? "," : "");
        }
        
        fprintf(fp_atual, "    ]\n");
        fprintf(fp_atual, "  }");
    }
    
    // Fechamento dos arrays JSON
    fprintf(fp_atividades, "\n]\n");
    fprintf(fp_avaliacoes, "\n]\n");
    
    fclose(fp_atividades);
    fclose(fp_avaliacoes);
}

// Registrar atividade no histórico
void registrar_atividade_no_historico(Atividade atividade, const char *professor, const char *disciplina, const char *nome_turma) {
    // Localiza o arquivo de histórico
    FILE *fp_historico = fopen("../../dados/atividades_e_avaliacoes/historico_atividades.json", "r");
    
    if (!fp_historico) {
        printf("⚠️  Aviso: Arquivo de histórico não encontrado. Criando novo...\n");
        // Se não existir, cria um arquivo vazio com estrutura JSON
        fp_historico = fopen("../../dados/atividades_e_avaliacoes/historico_atividades.json", "w");
        if (!fp_historico) {
            printf("❌ Erro ao criar arquivo de histórico\n");
            return;
        }
        fprintf(fp_historico, "[]");
        fclose(fp_historico);
        fp_historico = fopen("../../dados/atividades_e_avaliacoes/historico_atividades.json", "r");
    }
    
    // Lê o arquivo atual
    fseek(fp_historico, 0, SEEK_END);
    long tamanho = ftell(fp_historico);
    fseek(fp_historico, 0, SEEK_SET);
    
    char *buffer = (char *)malloc(tamanho + 1);
    fread(buffer, 1, tamanho, fp_historico);
    buffer[tamanho] = '\0';
    fclose(fp_historico);
    
    // Remove os últimos caracteres ] para adicionar o novo objeto
    char *pos_fecha = strrchr(buffer, ']');
    if (pos_fecha) {
        *pos_fecha = '\0';
    }
    
    // Obtém a data e hora atual
    time_t agora = time(NULL);
    struct tm *tm_info = localtime(&agora);
    char data_criacao[20];
    strftime(data_criacao, sizeof(data_criacao), "%Y-%m-%d", tm_info);
    
    // Calcula data de expiração (7 dias após a criação)
    time_t expiracao_time = agora + (7 * 24 * 60 * 60);
    struct tm *tm_expiracao = localtime(&expiracao_time);
    char data_expiracao[20];
    strftime(data_expiracao, sizeof(data_expiracao), "%Y-%m-%d", tm_expiracao);
    
    // Abre o arquivo para escrita
    fp_historico = fopen("../../dados/atividades_e_avaliacoes/historico_atividades.json", "w");
    if (!fp_historico) {
        printf("❌ Erro ao atualizar arquivo de histórico\n");
        free(buffer);
        return;
    }
    
    // Escreve o buffer anterior
    fprintf(fp_historico, "%s", buffer);
    
    // Se não era um array vazio (mais que apenas "["), adiciona vírgula
    if (strlen(buffer) > 1) {
        fprintf(fp_historico, ",\n");
    } else {
        fprintf(fp_historico, "\n");
    }
    
    // Adiciona o novo objeto de atividade
    fprintf(fp_historico, "  {\n");
    fprintf(fp_historico, "    \"id\": %d,\n", atividade.id);
    fprintf(fp_historico, "    \"titulo\": \"%s\",\n", atividade.titulo);
    fprintf(fp_historico, "    \"tipo\": \"%s\",\n", atividade.tipo);
    fprintf(fp_historico, "    \"status\": \"Ativo\",\n");
    fprintf(fp_historico, "    \"data_criacao\": \"%s\",\n", data_criacao);
    fprintf(fp_historico, "    \"data_expiracao\": \"%s\",\n", data_expiracao);
    fprintf(fp_historico, "    \"id_turma\": %d,\n", atividade.id_turma);
    fprintf(fp_historico, "    \"nome_turma\": \"%s\",\n", nome_turma);
    fprintf(fp_historico, "    \"professor\": \"%s\",\n", professor);
    fprintf(fp_historico, "    \"disciplina\": \"%s\",\n", disciplina);
    fprintf(fp_historico, "    \"total_alunos\": 0,\n");
    fprintf(fp_historico, "    \"total_respondido\": 0,\n");
    fprintf(fp_historico, "    \"respostas\": []\n");
    fprintf(fp_historico, "  }\n");
    fprintf(fp_historico, "]");  // Sem \n no final para evitar linhas vazias
    
    fclose(fp_historico);
    free(buffer);
    
    printf("✓ Atividade registrada no histórico!\n");
}

// Criar nova atividade
void criar_nova_atividade() {
    limpar_tela();
    printf("\nCRIAR NOVA ATIVIDADE/AVALIACAO\n");

    if (num_atividades >= MAX_ATIVIDADES) {
        printf("Limite de atividades atingido!\n");
        pausar();
        return;
    }
    
    Atividade nova;
    ConteudoAtividade novo_conteudo;
    nova.id = num_atividades + 1;
    novo_conteudo.id = num_atividades + 1;
    
    printf("Título: ");
    getchar();
    fgets(nova.titulo, MAX_NOME, stdin);
    nova.titulo[strcspn(nova.titulo, "\n")] = 0;
    
    printf("\nTipo:\n");
    printf("  [1] Atividade\n");
    printf("  [2] Avaliação\n");
    printf("Escolha: ");
    int tipo;
    scanf("%d", &tipo);
    strcpy(nova.tipo, tipo == 1 ? "Atividade" : "Avaliacao");
    getchar(); // Limpar o buffer
    
    printf("\nEnunciado geral da %s:\n", nova.tipo);
    fgets(novo_conteudo.enunciado, MAX_TEXTO, stdin);
    novo_conteudo.enunciado[strcspn(novo_conteudo.enunciado, "\n")] = 0;
    
    printf("\nQuantas questões deseja adicionar? (máx. %d): ", MAX_QUESTOES);
    scanf("%d", &novo_conteudo.num_questoes);
    getchar(); // Limpar o buffer
    
    if (novo_conteudo.num_questoes > MAX_QUESTOES) {
        novo_conteudo.num_questoes = MAX_QUESTOES;
        printf("Número ajustado para o máximo permitido: %d\n", MAX_QUESTOES);
    }
    
    // Para cada questão
    for (int i = 0; i < novo_conteudo.num_questoes; i++) {
        printf("\nQuestão %d:\n", i + 1);
        printf("Digite o texto da questão:\n");
        fgets(novo_conteudo.questoes[i].texto, MAX_TEXTO, stdin);
        novo_conteudo.questoes[i].texto[strcspn(novo_conteudo.questoes[i].texto, "\n")] = 0;
        
        printf("Quantas alternativas para esta questão? (2-5): ");
        scanf("%d", &novo_conteudo.questoes[i].num_alternativas);
        getchar(); // Limpar o buffer
        
        if (novo_conteudo.questoes[i].num_alternativas < 2) 
            novo_conteudo.questoes[i].num_alternativas = 2;
        if (novo_conteudo.questoes[i].num_alternativas > MAX_ALTERNATIVAS)
            novo_conteudo.questoes[i].num_alternativas = MAX_ALTERNATIVAS;
        
        // Para cada alternativa
        for (int j = 0; j < novo_conteudo.questoes[i].num_alternativas; j++) {
            printf("Digite a alternativa %c:\n", 'A' + j);
            fgets(novo_conteudo.questoes[i].alternativas[j], MAX_TEXTO, stdin);
            novo_conteudo.questoes[i].alternativas[j][strcspn(novo_conteudo.questoes[i].alternativas[j], "\n")] = 0;
        }
        
        do {
            printf("Qual é a alternativa correta (A-%c)? ", 'A' + novo_conteudo.questoes[i].num_alternativas - 1);
            scanf(" %c", &novo_conteudo.questoes[i].resposta_correta);
            novo_conteudo.questoes[i].resposta_correta = toupper(novo_conteudo.questoes[i].resposta_correta);
            getchar(); // Limpar o buffer
        } while (novo_conteudo.questoes[i].resposta_correta < 'A' || 
                novo_conteudo.questoes[i].resposta_correta >= 'A' + novo_conteudo.questoes[i].num_alternativas);
    }
    
    exibir_turmas();
    printf("\nSelecione a turma (ID): ");
    scanf("%d", &nova.id_turma);
    getchar(); // Limpar o buffer
    
    printf("\nDisciplina (ex: Matemática, Português, História): ");
    char disciplina[MAX_NOME];
    fgets(disciplina, sizeof(disciplina), stdin);
    disciplina[strcspn(disciplina, "\n")] = 0;
    
    printf("Data de expiração (DD/MM/AAAA): ");
    scanf("%s", nova.data_expiracao);
    getchar(); // Limpar o buffer
    
    strcpy(nova.data_criacao, "09/11/2025");
    strcpy(nova.status, "Nenhum aluno respondeu");
    nova.num_respostas = 0;
    
    atividades[num_atividades] = nova;
    conteudos[num_atividades] = novo_conteudo;
    num_atividades++;
    
    salvar_atividades_arquivo();
    salvar_conteudo_atividades();
    
    // Registra a atividade no histórico
    char nome_turma[MAX_NOME] = "";
    // Busca o nome da turma pelo ID
    for (int i = 0; i < num_turmas; i++) {
        if (turmas[i].id == nova.id_turma) {
            strcpy(nome_turma, turmas[i].serie_turma);
            break;
        }
    }
    // Se não encontrou nome, usa um padrão
    if (strlen(nome_turma) == 0) {
        snprintf(nome_turma, sizeof(nome_turma), "Turma %d", nova.id_turma);
    }
    
    registrar_atividade_no_historico(nova, nome_professor_global, disciplina, nome_turma);
    
    printf("\n✓ Atividade criada com sucesso!\n");
    pausar();
}

// Buscar atividades usando Python
void buscar_atividades() {
    limpar_tela();
    
    printf("BUSCAR ATIVIDADES\n");

    
    printf("O que você deseja buscar?\n");
    printf("  [1] Atividades (exercícios, trabalhos, listas)\n");
    printf("  [2] Avaliações (provas, simulados, testes)\n");
    printf("  [3] Todos os tipos\n\n");
    
    printf("Escolha (1-3): ");
    int tipo_escolha;
    scanf("%d", &tipo_escolha);
    getchar(); // Limpar o buffer
    
    char tipo_filtro[20] = "";
    if (tipo_escolha == 1) {
        strcpy(tipo_filtro, "Atividade");
    } else if (tipo_escolha == 2) {
        strcpy(tipo_filtro, "Avaliacao");
    }
    // Se escolher 3, tipo_filtro fica vazio (busca tudo)
    
    // Mostrar exemplos baseado no tipo escolhido
    printf("\nExemplos de busca:\n");
    if (tipo_escolha == 1) {
        printf("  • 'Lista de Exercícios'\n");
        printf("  • 'exercício matemática'\n");
        printf("  • 'trabalho grupo'\n");
        printf("  • 'atividade prática'\n");
    } else if (tipo_escolha == 2) {
        printf("  • 'Simulado'\n");
        printf("  • 'prova final'\n");
        printf("  • 'avaliação bimestral'\n");
        printf("  • 'teste diagnóstico'\n");
    } else {
        printf("  • Qualquer título de atividade ou avaliação\n");
        printf("  • Disciplina ou matéria\n");
    }
    
    printf("\nDigite o termo de busca (ou pressione ENTER para listar tudo): ");
    char termo[MAX_NOME];
    fgets(termo, MAX_NOME, stdin);
    termo[strcspn(termo, "\n")] = 0;
    
    // Construir o comando para executar o script Python
    char comando[1024];
    if (strlen(tipo_filtro) > 0) {
        snprintf(comando, sizeof(comando), 
                 "python \"../../codigos/ferramentas/filtro_de_busca/FiltroDeBusca.py\" \"%s\" \"%s\"", 
                 tipo_filtro, termo);
    } else {
        snprintf(comando, sizeof(comando), 
                 "python \"../../codigos/ferramentas/filtro_de_busca/FiltroDeBusca.py\" \"\" \"%s\"", 
                 termo);
    }
    
    printf("\nExecutando busca...\n\n");
    
    // Executar o script Python
    #ifdef _WIN32
        system(comando);
    #else
        system(comando);
    #endif
    
    pausar();
}

// Função para abrir o painel inteligente com IA
void abrir_painel_ia() {
    limpar_tela();
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                          ║\n");
    printf("║              🤖 ABRINDO PAINEL INTELIGENTE COM IA...                    ║\n");
    printf("║                                                                          ║\n");
    printf("╚══════════════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Iniciando análise inteligente com IA...\n");
    printf("Carregando dados de desempenho dos alunos...\n\n");
    
    // Comando para executar o script Python
    char comando[512];
    
    #ifdef _WIN32
        // Para Windows
        snprintf(comando, sizeof(comando), "python \"..\\..\\codigos\\ferramentas\\painel_inteligente\\painel_ia.py\"");
    #else
        // Para Linux/Mac
        snprintf(comando, sizeof(comando), "python3 \"../../codigos/ferramentas/painel_inteligente/painel_ia.py\"");
    #endif
    
    printf("Executando comando: %s\n\n", comando);
    printf("───────────────────────────────────────────────────────────────────────────\n\n");
    
    // Executa o script Python
    int resultado = system(comando);
    
    if (resultado != 0) {
        printf("\n❌ Erro ao executar o painel de IA\n");
        printf("   Verifique se Python está instalado e acessível no PATH\n");
        printf("   Verifique se o arquivo painel_ia.py existe em:\n");
        printf("   codigos/ferramentas/painel_inteligente/painel_ia.py\n");
    }
    
    printf("\n───────────────────────────────────────────────────────────────────────────\n");
    pausar();
}

// Menu principal
void menu_principal() {
    int opcao;
    int turma_selecionada = 0;
    
    while (1) {
        limpar_tela();
        printf("\nAREA DE ATIVIDADES E AVALIACOES\n");

        printf("  [1] Selecionar Turma\n");
        printf("  [2] Ver Atividades da Turma Selecionada\n");
        printf("  [3] Criar Nova Atividade/Avaliação\n");
        printf("  [4] Buscar Atividades (Python)\n");
        printf("  [5] Listar Todas as Turmas\n");
        printf("  [6] 🤖 Painel Inteligente com IA\n");
        printf("  [0] Sair\n\n");
        
        if (turma_selecionada > 0) {
            printf("  Turma selecionada: ");
            for (int i = 0; i < num_turmas; i++) {
                if (turmas[i].id == turma_selecionada) {
                    printf("%s - %s\n", turmas[i].serie_turma, turmas[i].curso);
                    break;
                }
            }
        } else {
            printf("  Nenhuma turma selecionada\n");
        }
        
        printf("\n  Escolha uma opção: ");
        scanf("%d", &opcao);
        
        switch (opcao) {
            case 1:
                limpar_tela();
                exibir_turmas();
                printf("Selecione o ID da turma: ");
                scanf("%d", &turma_selecionada);
                break;
                
            case 2:
                if (turma_selecionada > 0) {
                    limpar_tela();
                    exibir_atividades_turma(turma_selecionada);
                    pausar();
                } else {
                    printf("\nSelecione uma turma primeiro!\n");
                    pausar();
                }
                break;
                
            case 3:
                criar_nova_atividade();
                break;
                
            case 4:
                buscar_atividades();
                break;
                
            case 5:
                limpar_tela();
                exibir_turmas();
                pausar();
                break;
                
            case 6:
                abrir_painel_ia();
                break;
                
            case 0:
                printf("\nSaindo...\n");
                return;
                
            default:
                printf("\nOpção inválida!\n");
                pausar();
        }
    }
}

// Função para inicializar os dados
void inicializar_dados(const char *nome_professor) {
    // Limpa as estruturas de dados
    memset(turmas, 0, sizeof(turmas));
    memset(atividades, 0, sizeof(atividades));
    memset(conteudos, 0, sizeof(conteudos));
    num_turmas = 0;
    num_atividades = 0;
    
    // Carrega as turmas do professor
    if (!carregar_turmas(nome_professor)) {
        printf("Nenhuma turma encontrada para o professor: %s\n", nome_professor);
        return;
    }
    
    printf("\nDados inicializados com sucesso!\n");
}

int main() {
    configurarConsole();
    
    char nome_professor[MAX_PROFESSOR];
    printf("\nDigite o nome do professor: ");
    fgets(nome_professor, sizeof(nome_professor), stdin);
    nome_professor[strcspn(nome_professor, "\n")] = 0;
    
    // Armazena o nome na variável global para uso em outras funções
    strcpy(nome_professor_global, nome_professor);
    
    inicializar_dados(nome_professor);
    // NOTA: Não chamamos salvar_atividades_arquivo() aqui pois os dados
    // são salvos apenas em conteudo_atividades.json e conteudo_avaliacoes.json
    menu_principal();
    return 0;
}