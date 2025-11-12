// Área de cadastro de turmas

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#endif

// Configuração do console para UTF-8
void configurarConsole() {
#ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
#endif
}

// Função para ler entrada com segurança
void lerEntradaSegura(char *destino, size_t tamanho, const char *mensagem) {
    printf("%s", mensagem);
    
    if (fgets(destino, tamanho, stdin) != NULL) {
        size_t len = strcspn(destino, "\n");
        if (len < tamanho - 1) {
            destino[len] = '\0';
        } else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    } else {
        destino[0] = '\0';
    }
}

// Verifica se uma string está vazia
int validarCampoVazio(const char *campo) {
    if (campo == NULL || strlen(campo) == 0) {
        printf("Campo vazio! Por favor, preencha todos os campos.\n");
        return 0;
    }
    return 1;
}

// Função para validar o formato do inicio do horário (HH:MM)
int validarInicioHorario(const char *horario) {
    if (strlen(horario) != 5) return 0;
    if (horario[2] != ':') return 0;
    for (int i = 0; i < strlen(horario); i++) {
        if (i == 2) continue;
        if (horario[i] < '0' || horario[i] > '9') return 0;
    }
    int h = (horario[0] - '0') * 10 + (horario[1] - '0');
    int m = (horario[3] - '0') * 10 + (horario[4] - '0');
    if (h < 0 || h > 23 || m < 0 || m > 59) return 0;
    return 1;
}

// Função para validar o formato do fim do horário (HH:MM)
int validarFimHorario(const char *horario) {
    if (strlen(horario) != 5) return 0;
    if (horario[2] != ':') return 0;
    for (int i = 0; i < strlen(horario); i++) {
        if (i == 2) continue;
        if (horario[i] < '0' || horario[i] > '9') return 0;
    }
    int h = (horario[0] - '0') * 10 + (horario[1] - '0');
    int m = (horario[3] - '0') * 10 + (horario[4] - '0');
    if (h < 0 || h > 23 || m < 0 || m > 59) return 0;
    return 1;
}

// Função para validar se o horário final é maior que o inicial
int validarIntervaloHorario(const char *inicio, const char *fim) {
    int h1 = (inicio[0] - '0') * 10 + (inicio[1] - '0');
    int m1 = (inicio[3] - '0') * 10 + (inicio[4] - '0');
    int h2 = (fim[0] - '0') * 10 + (fim[1] - '0');
    int m2 = (fim[3] - '0') * 10 + (fim[4] - '0');

    // Converte para minutos para facilitar a comparação
    int minutos1 = h1 * 60 + m1;
    int minutos2 = h2 * 60 + m2;

    return minutos2 > minutos1;
}

// Função para validar os dias da semana
int validarDiasSemana(const char *dias) {
    char diasValidos[][10] = {"Segunda", "Terca", "Quarta", "Quinta", "Sexta", "Sabado", "Domingo"};
    char temp[100];
    strcpy(temp, dias);
    char *token = strtok(temp, ",");
    
    while (token != NULL) {
        int diaValido = 0;
        for (int i = 0; i < 7; i++) {
            if (strcmp(token, diasValidos[i]) == 0) {
                diaValido = 1;
                break;
            }
        }
        if (!diaValido) return 0;
        token = strtok(NULL, ",");
    }
    return 1;
}

// Funções auxiliares para manipulação de JSON
// Se encontrar, escreve o caminho em out_path e retorna 1. Caso contrário,
// solicita ao usuário digitar o caminho completo e valida.
int localizar_arquivo_json(char *out_path, size_t out_size) {
    const char *candidatos[] = {
        "../dados/professores_cadastros/cadastro_professor.json",
        "../../dados/professores_cadastros/cadastro_professor.json",
        "../../../dados/professores_cadastros/cadastro_professor.json",
        "./dados/professores_cadastros/cadastro_professor.json"
    };
    size_t n = sizeof(candidatos) / sizeof(candidatos[0]);
    
    // 1) verificar variável de ambiente CADASTRO_JSON
    const char *env = getenv("CADASTRO_JSON");
    if (env) {
        FILE *fe = fopen(env, "r");
        if (fe) {
            fclose(fe);
            snprintf(out_path, out_size, "%s", env);
            return 1;
        }
    }
    
    // 2) tentar caminhos candidatos
    for (size_t i = 0; i < n; i++) {
        FILE *f = fopen(candidatos[i], "r");
        if (f) {
            fclose(f);
            snprintf(out_path, out_size, "%s", candidatos[i]);
            return 1;
        }
    }

    // Se não encontrou, pede ao usuário
    printf("Arquivo 'cadastro_professor.json' não encontrado nos locais padrão.\n");
    lerEntradaSegura(out_path, out_size, "Digite o caminho completo para 'cadastro_professor.json' ou Enter para cancelar: ");
    if (strlen(out_path) == 0) return 0;
    
    FILE *f = fopen(out_path, "r");
    if (!f) {
        printf("Não foi possível abrir '%s'.\n", out_path);
        return 0;
    }
    fclose(f);
    return 1;
}

// Funções auxiliares para arquivo e JSON
int ler_arquivo(const char *caminho, char *buffer, size_t tamanho_buffer) {
    FILE *f = fopen(caminho, "r");
    if (!f) return 0;
    size_t l = fread(buffer, 1, tamanho_buffer - 1, f);
    buffer[l] = '\0';
    fclose(f);
    return 1;
}

int extrair_valor_json(const char *buffer, const char *chave, size_t start_pos, char *out, size_t out_size) {
    char procura[128];
    snprintf(procura, sizeof(procura), "\"%s\": \"", chave);
    const char *p = strstr(buffer + start_pos, procura);
    if (!p) return 0;
    p += strlen(procura);
    const char *fim = strchr(p, '"');
    if (!fim) return 0;
    size_t len = (size_t)(fim - p);
    if (len >= out_size) len = out_size - 1;
    strncpy(out, p, len);
    out[len] = '\0';
    return 1;
}

// Funções específicas para verificação e validação
int verificar_professor_responsavel(const char *nome_professor) {
    char arquivo_path[512];
    if (!localizar_arquivo_json(arquivo_path, sizeof(arquivo_path))) {
        printf("Erro: Não foi possível localizar o arquivo de cadastro de professores.\n");
        return 0;
    }
    
    char conteudo[50000];
    if (!ler_arquivo(arquivo_path, conteudo, sizeof(conteudo))) {
        printf("Erro: Não foi possível ler o arquivo de cadastro de professores.\n");
        return 0;
    }

    // Procura o início do array
    const char *inicio_array = strchr(conteudo, '[');
    if (!inicio_array) {
        printf("Erro: Formato de arquivo inválido.\n");
        return 0;
    }

    // Para cada objeto no array
    const char *pos = inicio_array;
    while ((pos = strchr(pos, '{')) != NULL) {
        char nome_cadastrado[100];
        if (extrair_valor_json(conteudo, "nome", (size_t)(pos - conteudo), nome_cadastrado, sizeof(nome_cadastrado))) {
            if (strcmp(nome_cadastrado, nome_professor) == 0) {
                return 1; // Professor encontrado
            }
        }
        pos++; // Avança para o próximo caractere
    }

    return 0; // Professor não encontrado
}

// Função para validar turma (1A, 2B, etc.)
int validarTurma(const char *turma) {
    // Verifica se a turma está no formato correto (agora sem o ª)
    return (strlen(turma) == 2 && isdigit(turma[0]) && isupper(turma[1]));
}

// Função para validar o Processo Letivo (exemplo: 2025/2)
int validarProcessoLetivo(const char *processo) {
    // Verifica o tamanho da string
    if (strlen(processo) != 6) return 0;
    
    // Verifica se os primeiros 4 caracteres são dígitos (ano)
    for (int i = 0; i < 4; i++) {
        if (!isdigit(processo[i])) return 0;
    }
    
    // Verifica se tem a barra
    if (processo[4] != '/') return 0;
    
    // Verifica se o último caractere é 1 ou 2 (semestre)
    if (processo[5] != '1' && processo[5] != '2') return 0;
    
    // Extrai e valida o ano
    int ano;
    sscanf(processo, "%d", &ano);
    if (ano < 2023 || ano > 2100) return 0; // Aceita anos entre 2023 e 2100
    
    return 1;
}

// Função para validar cursos
int validarCurso(const char *curso) {
    char cursosValidos[][50] = {"Matematica", "Portugues", "Historia", "Geografia", "Ciencias", "Ingles", "Educacao Fisica"};
    for (int i = 0; i < 7; i++) {
        if (strcmp(curso, cursosValidos[i]) == 0) {
            return 1; // Curso válido
        }
    }
    return 0; // Curso inválido
}

// Função para validar capacidade máxima da turma
int validarCapacidade(int capacidade) {
    return (capacidade > 0 && capacidade <= 40); // Exemplo: capacidade entre 1 e 40
}

// Função para confirmar o cadastro
void confirmarCadastro() {
    char confirmacao[4];
    lerEntradaSegura(confirmacao, sizeof(confirmacao), "Confirma o cadastro? (sim/nao): ");

    while (strcmp(confirmacao, "sim") != 0 && strcmp(confirmacao, "nao") != 0) {
        printf("Resposta inválida! Digite 'sim' ou 'nao'.\n");
        lerEntradaSegura(confirmacao, sizeof(confirmacao), "Confirma o cadastro? (sim/nao): ");
    }

    if (strcmp(confirmacao, "sim") == 0) {
        printf("Cadastro confirmado!\n");
    } else {
        printf("Cadastro cancelado.\n");
        exit(0);
    }
}

// Programa principal
int main() {
    configurarConsole();

    printf("\nÁrea de Cadastro de Turmas\n");
    printf("===================================\n");

    // Entrada e validação do professor responsável
    char nome_responsavel[100];
    lerEntradaSegura(nome_responsavel, sizeof(nome_responsavel), "Digite o nome do professor responsável pela turma: ");

    if (!validarCampoVazio(nome_responsavel)) {
        printf("É necessário informar o nome do professor responsável.\n");
        return 1;
    }

    while (!verificar_professor_responsavel(nome_responsavel)) {
        printf("Professor responsável não encontrado.\n");
        lerEntradaSegura(nome_responsavel, sizeof(nome_responsavel), "Digite o nome do professor responsável pela turma novamente (ou Enter para cancelar): ");
        
        if (strlen(nome_responsavel) == 0) {
            printf("Cadastro cancelado pelo usuário.\n");
            return 1;
        }
    }

    verificar_professor_responsavel(nome_responsavel);
    
    // Entrada e validação do nome da turma
    char serie_turma[100];
    lerEntradaSegura(serie_turma, sizeof(serie_turma), "Digite o nome da turma (exemplo: 1ªA): ");

    if (!validarCampoVazio(serie_turma)) {
        printf("É necessário informar o nome da turma.\n");
        return 1;
    }

    while (!validarTurma(serie_turma)) {
        printf("Nome da turma invalido! Use o formato correto (exemplo: 1A, 2B).\n");
        lerEntradaSegura(serie_turma, sizeof(serie_turma), "Digite o nome da turma (exemplo: 1A): ");
    }

    // Entrada e validação do horário
    char InicioHorario[20];
    lerEntradaSegura(InicioHorario, sizeof(InicioHorario), "Digite o horário de início da turma (formato HH:MM): ");

    if (!validarCampoVazio(InicioHorario)) {
        printf("É necessário informar o horário de início.\n");
        return 1;
    }

    while (!validarInicioHorario(InicioHorario)) {
        printf("Horário de início inválido! Use o formato HH:MM.\n");
        lerEntradaSegura(InicioHorario, sizeof(InicioHorario), "Digite o horário de início da turma (formato HH:MM): ");
    }

    // Entrada e validação do horário de término
    char FimHorario[20];
    lerEntradaSegura(FimHorario, sizeof(FimHorario), "Digite o horário de término da turma (formato HH:MM): ");

    if (!validarCampoVazio(FimHorario)) {
        printf("É necessário informar o horário de término.\n");
        return 1;
    }

    while (!validarFimHorario(FimHorario) || !validarIntervaloHorario(InicioHorario, FimHorario)) {
        if (!validarFimHorario(FimHorario)) {
            printf("Horário de término inválido! Use o formato HH:MM.\n");
        } else {
            printf("Horário de término deve ser maior que o horário de início!\n");
        }
        lerEntradaSegura(FimHorario, sizeof(FimHorario), "Digite o horário de término da turma (formato HH:MM): ");
    }

    // Combina os horários de início e término
    char horario[50];
    snprintf(horario, sizeof(horario), "%s - %s", InicioHorario, FimHorario);

    // Entrada e validação do Processo Letivo
    char processo_letivo[10];
    lerEntradaSegura(processo_letivo, sizeof(processo_letivo), "Digite o Processo Letivo (exemplo: 2025/2): ");

    if (!validarCampoVazio(processo_letivo)) {
        printf("É necessário informar o Processo Letivo.\n");
        return 1;
    }

    while (!validarProcessoLetivo(processo_letivo)) {
        printf("Processo Letivo inválido! Use o formato correto (exemplo: 2025/2).\n");
        lerEntradaSegura(processo_letivo, sizeof(processo_letivo), "Digite o Processo Letivo (exemplo: 2025/2): ");
    }

    // Entrada e validação de maximo de alunos
    char capacidade_str[10];
    lerEntradaSegura(capacidade_str, sizeof(capacidade_str), "Digite a capacidade máxima de alunos (1-40): ");

    if (!validarCampoVazio(capacidade_str)) {
        printf("É necessário informar a capacidade máxima de alunos.\n");
        return 1;
    }

    int capacidade = atoi(capacidade_str);
    while (!validarCapacidade(capacidade)) {
        printf("Capacidade máxima inválida! Use um valor entre 1 e 40.\n");
        lerEntradaSegura(capacidade_str, sizeof(capacidade_str), "Digite a capacidade máxima de alunos (1-40): ");
        capacidade = atoi(capacidade_str);
    }

    // Entrada e validação do curso
    char curso[50];
    lerEntradaSegura(curso, sizeof(curso), "Digite o curso (Matematica, Portugues, Historia, Geografia, Ciencias, Ingles, Educacao Fisica): ");

    if (!validarCampoVazio(curso)) {
        printf("É necessário informar o curso.\n");
        return 1;
    }

    while (!validarCurso(curso)) {
        printf("Curso inválido! Digite um dos cursos disponíveis.\n");
        lerEntradaSegura(curso, sizeof(curso), "Digite o curso: ");
    }

    // Entrada e validação dos dias da semana
    char dias_semana[100];
    lerEntradaSegura(dias_semana, sizeof(dias_semana), "Digite os dias da semana (Segunda, Terca, Quarta, Quinta, Sexta): ");

    if (!validarCampoVazio(dias_semana)) {
        printf("É necessário informar os dias da semana.\n");
        return 1;
    }

    while (!validarDiasSemana(dias_semana)) {
        printf("Dias inválidos! Use os dias: Segunda, Terca, Quarta, Quinta, Sexta, Sabado, Domingo (separados por vírgula)\n");
        lerEntradaSegura(dias_semana, sizeof(dias_semana), "Digite os dias da semana: ");
    }

    // Array de possíveis caminhos para o diretório de turmas
    const char *candidatos_dir[] = {
        "../dados/turmas_cadastros",
        "../../dados/turmas_cadastros",
        "./dados/turmas_cadastros",
        "dados/turmas_cadastros"
    };
    
    // Encontrar ou criar o diretório correto
    const char *dir = NULL;
    for (size_t i = 0; i < sizeof(candidatos_dir) / sizeof(candidatos_dir[0]); i++) {
        struct stat st = {0};
        if (stat(candidatos_dir[i], &st) == 0) {
            // Diretório existe
            dir = candidatos_dir[i];
            break;
        }
    }
    
    // Se não encontrou, criar no caminho padrão
    if (dir == NULL) {
        dir = "../dados/turmas_cadastros";
        #ifdef _WIN32
            _mkdir("../dados");
            _mkdir(dir);
        #else
            mkdir("../dados", 0755);
            mkdir(dir, 0755);
        #endif
    }

    char arquivo_path[512];
    snprintf(arquivo_path, sizeof(arquivo_path), "%s/cadastro_turma.json", dir);
    
    // Verificar se já existe uma turma com os mesmos dados
    char buffer_verificacao[50000];
    FILE *arquivo_existente = fopen(arquivo_path, "r");
    if (arquivo_existente) {
        size_t bytes_lidos = fread(buffer_verificacao, 1, sizeof(buffer_verificacao) - 1, arquivo_existente);
        buffer_verificacao[bytes_lidos] = '\0';

        // Procura por turma com mesmo nome, professor e horário
        char *pos = buffer_verificacao;
        while ((pos = strstr(pos, "serie_turma")) != NULL) {
            char turma_cadastrada[100];
            char prof_cadastrado[100];
            char horario_cadastrado[100];
            
            // Extrai os valores do JSON
            if (extrair_valor_json(buffer_verificacao, "serie_turma", pos - buffer_verificacao, turma_cadastrada, sizeof(turma_cadastrada)) &&
                extrair_valor_json(buffer_verificacao, "professor_responsavel", pos - buffer_verificacao, prof_cadastrado, sizeof(prof_cadastrado)) &&
                extrair_valor_json(buffer_verificacao, "horario", pos - buffer_verificacao, horario_cadastrado, sizeof(horario_cadastrado))) {
                
                // Compara com os dados atuais
                if (strcmp(turma_cadastrada, serie_turma) == 0 &&
                    strcmp(prof_cadastrado, nome_responsavel) == 0 &&
                    strcmp(horario_cadastrado, horario) == 0) {
                    printf("\nATENÇÃO: Já existe uma turma cadastrada com os mesmos dados!\n");
                    printf("Turma: %s\n", turma_cadastrada);
                    printf("Professor: %s\n", prof_cadastrado);
                    printf("Horário: %s\n", horario_cadastrado);
                    printf("\nNão é possível cadastrar uma turma duplicada.\n");
                    return 1;
                }
            }
            pos++;
        }
    }
    
    // Ler o conteúdo existente do arquivo
    char conteudo_atual[50000] = "[";
    int primeiro_registro = 1;
    
    FILE *fexist = fopen(arquivo_path, "r");
    if (fexist) {
        // Se o arquivo existe, lê seu conteúdo
        char buffer[50000];
        size_t bytes_lidos = fread(buffer, 1, sizeof(buffer) - 1, fexist);
        buffer[bytes_lidos] = '\0';
        
        // Se o arquivo já contém dados, copia o conteúdo existente
        if (bytes_lidos > 0) {
            // Remove colchetes existentes se houver
            if (buffer[0] == '[') {
                memmove(buffer, buffer + 1, strlen(buffer));
            }
            if (buffer[strlen(buffer)-1] == ']') {
                buffer[strlen(buffer)-1] = '\0';
            }
            strcat(conteudo_atual, buffer);
            primeiro_registro = 0;
        }
        fclose(fexist);
    }

    confirmarCadastro();

    // Abre o arquivo para escrita
    FILE *arquivo = fopen(arquivo_path, "w");
    if (arquivo) {
        // Se não é o primeiro registro, adiciona uma vírgula
        if (!primeiro_registro) {
            fprintf(arquivo, "%s,\n", conteudo_atual);
        } else {
            fprintf(arquivo, "[\n");
        }
        
        // Adiciona o novo registro
        fprintf(arquivo, "  {\n");
        fprintf(arquivo, "    \"serie_turma\": \"%s\",\n", serie_turma);
        fprintf(arquivo, "    \"professor_responsavel\": \"%s\",\n", nome_responsavel);
        fprintf(arquivo, "    \"horario\": \"%s\",\n", horario);
        fprintf(arquivo, "    \"dias_semana\": \"%s\",\n", dias_semana);
        fprintf(arquivo, "    \"curso\": \"%s\",\n", curso);
        fprintf(arquivo, "    \"processo_letivo\": \"%s\",\n", processo_letivo);
        fprintf(arquivo, "    \"capacidade\": %d\n", capacidade);
        fprintf(arquivo, "  }\n]");
        fclose(arquivo);
        printf("Cadastro realizado com sucesso! Arquivo salvo em: %s\n", arquivo_path);
    } else {
        printf("Erro ao finalizar o cadastro. Não foi possível abrir '%s' para escrita.\n", arquivo_path);
        return 1;
    }

    printf("\nPressione qualquer tecla para continuar...");
    system("pause > nul");
    return 0;
}