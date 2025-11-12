// Área de Login do Aluno
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>

// Incluir módulo de sincronização automática de JSON
#include "../ferramentas/auto_sync.c"

// Localiza o arquivo cadastro_aluno.json em caminhos comuns.
// Se encontrar, escreve o caminho em out_path e retorna 1. Caso contrário,
// solicita ao usuário digitar o caminho completo e valida.
int localizar_arquivo_json(char *out_path, size_t out_size) {
    const char *candidatos[] = {
        "../dados/alunos_cadastros/cadastro_aluno.json",
        "../../dados/alunos_cadastros/cadastro_aluno.json",
        "../../../dados/alunos_cadastros/cadastro_aluno.json",
        "./dados/alunos_cadastros/cadastro_aluno.json"
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
    for (size_t i = 0; i < n; i++) {
        FILE *f = fopen(candidatos[i], "r");
        if (f) {
            fclose(f);
            snprintf(out_path, out_size, "%s", candidatos[i]);
            return 1;
        }
    }

    // Se não encontrou, pede ao usuário
    printf("Arquivo 'cadastro_aluno.json' não encontrado nos locais padrão.\n");
    printf("Digite o caminho completo para 'cadastro_aluno.json' ou Enter para cancelar: ");
    if (!fgets(out_path, (int)out_size, stdin)) return 0;
    out_path[strcspn(out_path, "\n")] = 0;
    if (strlen(out_path) == 0) return 0;
    FILE *f = fopen(out_path, "r");
    if (!f) {
        printf("Não foi possível abrir '%s'.\n", out_path);
        return 0;
    }
    fclose(f);
    return 1;
}

// Verifica se uma string está vazia
int validarCampoVazio(const char *campo) {
    if (campo == NULL || strlen(campo) == 0) {
        printf("Campo vazio! Por favor, preencha todos os campos.\n");
        return 0;
    }
    return 1;
}

// Estructura para armazenar dados do aluno
typedef struct {
    char nome[200];
    char ra[20];
    char matricula[50];
} DadosAlunoSimples;

// Lê todo o conteúdo de um arquivo para um buffer (retorna 1 em sucesso)
int ler_arquivo(const char *caminho, char *buffer, size_t tamanho_buffer) {
    FILE *f = fopen(caminho, "r");
    if (!f) return 0;
    size_t l = fread(buffer, 1, tamanho_buffer - 1, f);
    buffer[l] = '\0';
    fclose(f);
    return 1;
}

// Extrai o valor da chave JSON simples: procura "chave": "valor"
// Procura a partir de start_pos. Retorna 1 se encontrou e copia em out.
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

// Busca informações do aluno (nome, RA, matrícula) pelo login
// Retorna 1 se encontrou, 0 caso contrário
int buscar_dados_aluno_por_login(const char *login, DadosAlunoSimples *dados_out) {
    char arquivo_path[512];
    if (!localizar_arquivo_json(arquivo_path, sizeof(arquivo_path))) {
        return 0;
    }
    char conteudo[50000];
    if (!ler_arquivo(arquivo_path, conteudo, sizeof(conteudo))) {
        return 0;
    }

    const char *chaves[] = {"cpf", "numeroMatricula", "ra", "email"};
    size_t n = sizeof(chaves) / sizeof(chaves[0]);

    const char *inicio_array = strchr(conteudo, '[');
    if (!inicio_array) return 0;

    const char *pos = inicio_array;
    while ((pos = strchr(pos, '{')) != NULL) {
        for (size_t i = 0; i < n; i++) {
            char padrao[256];
            snprintf(padrao, sizeof(padrao), "\"%s\": \"%s\"", chaves[i], login);
            
            const char *prox_obj = strchr(pos + 1, '{');
            const char *fim_array = strchr(pos, ']');
            size_t busca_len;
            
            if (prox_obj && (!fim_array || prox_obj < fim_array)) {
                busca_len = prox_obj - pos;
            } else if (fim_array) {
                busca_len = fim_array - pos;
            } else {
                busca_len = strlen(pos);
            }
            
            char temp[5000];
            if (busca_len >= sizeof(temp)) busca_len = sizeof(temp) - 1;
            strncpy(temp, pos, busca_len);
            temp[busca_len] = '\0';
            
            if (strstr(temp, padrao)) {
                // Encontrou o login, extrai nome, RA e matrícula
                if (extrair_valor_json(temp, "nome", 0, dados_out->nome, sizeof(dados_out->nome)) &&
                    extrair_valor_json(temp, "ra", 0, dados_out->ra, sizeof(dados_out->ra)) &&
                    extrair_valor_json(temp, "numeroMatricula", 0, dados_out->matricula, sizeof(dados_out->matricula))) {
                    return 1;
                }
            }
        }
        pos++;
    }
    return 0;
}

// Procura um registro no JSON (arquivo cadastro_aluno.json) que tenha
// cpf, numeroMatricula, ra ou email igual a login. Se encontrado, preenche senha_out
// e retorna 1, caso contrário retorna 0.
int encontrar_senha_por_login(const char *login, char *senha_out, size_t senha_size) {
    char arquivo_path[512];
    if (!localizar_arquivo_json(arquivo_path, sizeof(arquivo_path))) {
        return 0;
    }
    char conteudo[50000];
    if (!ler_arquivo(arquivo_path, conteudo, sizeof(conteudo))) {
        return 0;
    }

    const char *chaves[] = {"cpf", "numeroMatricula", "ra", "email"};
    size_t n = sizeof(chaves) / sizeof(chaves[0]);

    // Procura o início do array
    const char *inicio_array = strchr(conteudo, '[');
    if (!inicio_array) return 0;

    // Para cada objeto no array
    const char *pos = inicio_array;
    while ((pos = strchr(pos, '{')) != NULL) {
        // Para cada chave que queremos verificar
        for (size_t i = 0; i < n; i++) {
            char padrao[256];
            snprintf(padrao, sizeof(padrao), "\"%s\": \"%s\"", chaves[i], login);
            
            // Procura até o próximo objeto ou fim do array
            const char *prox_obj = strchr(pos + 1, '{');
            const char *fim_array = strchr(pos, ']');
            size_t busca_len;
            
            if (prox_obj && (!fim_array || prox_obj < fim_array)) {
                busca_len = prox_obj - pos;
            } else if (fim_array) {
                busca_len = fim_array - pos;
            } else {
                busca_len = strlen(pos);
            }
            
            // Cria uma cópia temporária do objeto atual para busca
            char temp[5000];
            if (busca_len >= sizeof(temp)) busca_len = sizeof(temp) - 1;
            strncpy(temp, pos, busca_len);
            temp[busca_len] = '\0';
            
            if (strstr(temp, padrao)) {
                // Encontrou o login, procura a senha neste objeto
                if (extrair_valor_json(temp, "senha", 0, senha_out, senha_size)) {
                    return 1;
                }
            }
        }
        pos++; // Move para o próximo caractere
    }
    return 0;
}

// Atualiza o campo "senha" do registro correspondente ao login no arquivo JSON.
// Retorna 1 em sucesso, 0 caso não encontre ou erro.
int atualizar_senha_no_json(const char *login, const char *nova_senha) {
    char arquivo_path[512];
    if (!localizar_arquivo_json(arquivo_path, sizeof(arquivo_path))) {
        return 0;
    }
    char conteudo[20000];
    if (!ler_arquivo(arquivo_path, conteudo, sizeof(conteudo))) {
        return 0;
    }

    const char *chaves[] = {"cpf", "numeroMatricula", "ra", "email"};
    size_t n = sizeof(chaves) / sizeof(chaves[0]);
    const char *pos = NULL;
    size_t offset = 0;

    for (size_t i = 0; i < n; i++) {
        char padrao[256];
        snprintf(padrao, sizeof(padrao), "\"%s\": \"%s\"", chaves[i], login);
        pos = strstr(conteudo, padrao);
        if (pos) {
            offset = (size_t)(pos - conteudo);
            break;
        }
    }

    if (!pos) return 0; // login não encontrado

    // A partir de offset, procurar o campo "senha"
    const char *p_senha = strstr(conteudo + offset, "\"senha\": \"");
    if (!p_senha) return 0;
    const char *inicio_val = p_senha + strlen("\"senha\": \"");
    const char *fim_val = strchr(inicio_val, '"');
    if (!fim_val) return 0;

    // Construir novo conteúdo substituindo o valor da senha
    char novo[22000];
    size_t antes_len = (size_t)(inicio_val - conteudo);
    if (antes_len >= sizeof(novo)) return 0;
    memcpy(novo, conteudo, antes_len);

    // Inserir nova senha
    int written = snprintf(novo + antes_len, sizeof(novo) - antes_len, "%s", nova_senha);
    if (written < 0) return 0;
    size_t pos_depois = antes_len + (size_t)written;

    // Copiar o restante do conteúdo após a senha antiga
    const char *resto = fim_val + 1; // após a aspa final
    size_t resto_len = strlen(resto);
    if (pos_depois + resto_len >= sizeof(novo)) return 0;
    memcpy(novo + pos_depois, resto, resto_len + 1); // inclui \0

    // Escrever de volta no arquivo
    // Criar backup antes de sobrescrever
    char backup_path[520];
    snprintf(backup_path, sizeof(backup_path), "%s.bak", arquivo_path);
    FILE *fb = fopen(backup_path, "w");
    if (fb) {
        fwrite(conteudo, 1, strlen(conteudo), fb);
        fclose(fb);
    }

    FILE *f = fopen(arquivo_path, "w");
    if (!f) return 0;
    fwrite(novo, 1, strlen(novo), f);
    fclose(f);
    return 1;
}

// Fluxo interativo para redefinir senha para o login fornecido
void solicitar_redefinicao(const char *login) {
    char senha_atual[128];
    if (!encontrar_senha_por_login(login, senha_atual, sizeof(senha_atual))) {
        printf("Não foi possível encontrar a conta para '%s'.\n", login);
        return;
    }

    printf("Iniciando redefinição de senha para '%s'.\n", login);
    printf("Enviamos um código para o e-mail/SMS cadastrado (simulado).\n");
    printf("Digite o código recebido: ");
    char codigo[16];
    if (!fgets(codigo, sizeof(codigo), stdin)) return;
    codigo[strcspn(codigo, "\n")] = 0;
    if (strcmp(codigo, "123456") != 0) {
        printf("Código incorreto. Redefinição cancelada.\n");
        return;
    }

    char nova[128];
    char confirmar[128];
    printf("Digite a nova senha: ");
    if (!fgets(nova, sizeof(nova), stdin)) return;
    nova[strcspn(nova, "\n")] = 0;
    printf("Confirme a nova senha: ");
    if (!fgets(confirmar, sizeof(confirmar), stdin)) return;
    confirmar[strcspn(confirmar, "\n")] = 0;
    if (strcmp(nova, confirmar) != 0) {
        printf("As senhas não conferem. Redefinição cancelada.\n");
        return;
    }

    if (atualizar_senha_no_json(login, nova)) {
        printf("Senha atualizada com sucesso para '%s'.\n", login);
    } else {
        printf("Falha ao atualizar a senha no arquivo.\n");
    }
}

// Pergunta senha ao usuário com até max_tentativas tentativas
int pedir_e_validar_senha(const char *senha_esperada, int max_tentativas) {
    char senha[128];
    for (int t = 1; t <= max_tentativas; t++) {
        printf("Digite a senha do aluno: ");
        fflush(stdout);
        if (!fgets(senha, sizeof(senha), stdin)) return 0;
        senha[strcspn(senha, "\n")] = 0;
        if (!validarCampoVazio(senha)) {
            continue;
        }
        if (strcmp(senha, senha_esperada) == 0) {
            return 1; // senha correta
        } else {
            printf("Senha incorreta. Tentativa %d de %d.\n", t, max_tentativas);
        }
    }
    return 0; // falhou
}

// Localiza o arquivo desempenho_alunos.json
int localizar_arquivo_desempenho(char *out_path, size_t out_size) {
    const char *candidatos[] = {
        "../dados/alunos_cadastros/desempenho_alunos.json",
        "../../dados/alunos_cadastros/desempenho_alunos.json",
        "../../../dados/alunos_cadastros/desempenho_alunos.json",
        "./dados/alunos_cadastros/desempenho_alunos.json"
    };
    size_t n = sizeof(candidatos) / sizeof(candidatos[0]);
    
    const char *env = getenv("DESEMPENHO_JSON");
    if (env) {
        FILE *fe = fopen(env, "r");
        if (fe) {
            fclose(fe);
            snprintf(out_path, out_size, "%s", env);
            return 1;
        }
    }
    
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

// Verifica se um aluno já tem desempenho registrado
int aluno_tem_desempenho(const char *ra) {
    char arquivo_path[512];
    if (!localizar_arquivo_desempenho(arquivo_path, sizeof(arquivo_path))) {
        return 0;
    }
    
    char conteudo[50000];
    if (!ler_arquivo(arquivo_path, conteudo, sizeof(conteudo))) {
        return 0;
    }
    
    char procura[128];
    snprintf(procura, sizeof(procura), "\"ra\": \"%s\"", ra);
    
    return strstr(conteudo, procura) != NULL ? 1 : 0;
}

// Cria um novo registro de desempenho para o aluno
int criar_desempenho_aluno(const DadosAlunoSimples *dados, const char *serie) {
    char arquivo_path[512];
    if (!localizar_arquivo_desempenho(arquivo_path, sizeof(arquivo_path))) {
        return 0;
    }
    
    // Lê o arquivo existente
    char conteudo[50000] = "[";
    int primeiro_registro = 1;
    
    FILE *fexist = fopen(arquivo_path, "r");
    if (fexist) {
        char buffer[50000];
        size_t bytes_lidos = fread(buffer, 1, sizeof(buffer) - 1, fexist);
        buffer[bytes_lidos] = '\0';
        fclose(fexist);
        
        if (bytes_lidos > 0) {
            // Remove colchetes existentes
            if (buffer[0] == '[') {
                memmove(buffer, buffer + 1, strlen(buffer));
            }
            if (buffer[strlen(buffer)-1] == ']') {
                buffer[strlen(buffer)-1] = '\0';
            }
            strcat(conteudo, buffer);
            primeiro_registro = 0;
        }
    }
    
    // Abre arquivo para escrita
    FILE *f = fopen(arquivo_path, "w");
    if (!f) return 0;
    
    // Se não é o primeiro registro, adiciona vírgula
    if (!primeiro_registro) {
        fprintf(f, "%s,\n", conteudo);
    } else {
        fprintf(f, "[\n");
    }
    
    // Adiciona novo registro com valores iniciais
    fprintf(f, "  {\n");
    fprintf(f, "    \"ra\": \"%s\",\n", dados->ra);
    fprintf(f, "    \"nome\": \"%s\",\n", dados->nome);
    fprintf(f, "    \"serie\": \"%s\",\n", serie);
    fprintf(f, "    \"matricula\": \"%s\",\n", dados->matricula);
    fprintf(f, "    \"media_geral\": 0.0,\n");
    fprintf(f, "    \"frequencia\": 0,\n");
    fprintf(f, "    \"situacao\": \"Ativo\",\n");
    fprintf(f, "    \"atividades_concluidas\": 0,\n");
    fprintf(f, "    \"atividades_pendentes\": 0,\n");
    fprintf(f, "    \"mensagens_nao_lidas\": 0\n");
    fprintf(f, "  }\n]");
    
    fclose(f);
    return 1;
}

// Lógica principal do login
int main(void) {
    system("chcp 65001");
    char login[200];
    char senha_esperada[128];
    const int max_attempts = 5; // número total de tentativas permitidas
    int attempts_left = max_attempts;

    while (attempts_left > 0) {
        // Pede o login
        printf("Digite o CPF, Número de Matrícula, RA ou Email do aluno: ");
        if (!fgets(login, sizeof(login), stdin)) return 1;
        login[strcspn(login, "\n")] = 0;

        if (!validarCampoVazio(login)) continue; // não consome tentativa

        // Pergunta se o usuário confirma o login digitado
        char confirma[8];
        printf("Você digitou '%s'. Confirmar (S/N): ", login);
        if (!fgets(confirma, sizeof(confirma), stdin)) return 1;
        confirma[strcspn(confirma, "\n")] = 0;
        if (!(confirma[0] == 'S' || confirma[0] == 's')) {
            // Usuário não confirmou, volta a pedir o login sem consumir tentativa
            continue;
        }

        // Tenta localizar a senha para esse login
        if (!encontrar_senha_por_login(login, senha_esperada, sizeof(senha_esperada))) {
            attempts_left--;
            printf("Login não encontrado. Tentativas restantes: %d\n", attempts_left);
            if (attempts_left == 0) {
                // Perguntar se deseja redefinir a senha
                char resposta[8];
                printf("Suas %d tentativas foram esgotadas. Deseja redefinir a senha? (S/N): ", max_attempts);
                if (!fgets(resposta, sizeof(resposta), stdin)) return 1;
                resposta[strcspn(resposta, "\n")] = 0;
                if (resposta[0] == 'S' || resposta[0] == 's') {
                    solicitar_redefinicao(login);
                } else {
                    printf("Operação cancelada. Saindo.\n");
                }
                return 0;
            }
            continue;
        }

        // Se encontrou o registro, pede a senha (cada tentativa consome 1 das 5)
        if (pedir_e_validar_senha(senha_esperada, 1)) {
            printf("Login realizado com sucesso!\n");
            
            // Busca dados do aluno para criar desempenho se necessário
            DadosAlunoSimples dados;
            if (buscar_dados_aluno_por_login(login, &dados)) {
                // Verifica se aluno já tem desempenho registrado
                if (!aluno_tem_desempenho(dados.ra)) {
                    // Define a série (pode ser melhorado com mais informações)
                    const char *serie = "1º Ano - Ensino Superior";
                    
                    if (criar_desempenho_aluno(&dados, serie)) {
                        printf("✓ Perfil de desempenho criado com sucesso!\n");
                    }
                }
                
                // Registrar login no arquivo de logs (AUTOMATIZADO via auto_sync)
                auto_log("dados/logs_atividade.json", dados.ra, "Login", "Aluno realizou login na conta");
            }
            
            return 0;
        } else {
            attempts_left--;
            printf("Senha incorreta. Tentativas restantes: %d\n", attempts_left);
            if (attempts_left == 0) {
                // Perguntar se deseja redefinir a senha
                char resposta[8];
                printf("Suas %d tentativas foram esgotadas. Deseja redefinir a senha? (S/N): ", max_attempts);
                if (!fgets(resposta, sizeof(resposta), stdin)) return 1;
                resposta[strcspn(resposta, "\n")] = 0;
                if (resposta[0] == 'S' || resposta[0] == 's') {
                    solicitar_redefinicao(login);
                } else {
                    printf("Operação cancelada. Saindo.\n");
                }
                return 0;
            }
            // continuar para próxima tentativa
        }
    }

    printf("\nPressione qualquer tecla para continuar...");
    system("pause > nul");
    
    // Garantir que todos os dados pendentes sejam sincronizados antes de sair
    finalizar_sistema();
    
    return 0;
}
