//Área de login do professor

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>

// Incluir módulo de sincronização automática de JSON
#include "../ferramentas/auto_sync.c"

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

// Localiza o arquivo cadastro_professor.json em caminhos comuns.
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

// Verifica se uma string está vazia
int validarCampoVazio(const char *campo) {
    if (campo == NULL || strlen(campo) == 0) {
        printf("Campo vazio! Por favor, preencha todos os campos.\n");
        return 0;
    }
    return 1;
}

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

    const char *chaves[] = {"email", "ra"};
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
    char conteudo[50000];
    if (!ler_arquivo(arquivo_path, conteudo, sizeof(conteudo))) {
        return 0;
    }

    const char *chaves[] = {"email", "ra"};
    size_t n = sizeof(chaves) / sizeof(chaves[0]);

    // Procura o início do array
    const char *inicio_array = strchr(conteudo, '[');
    if (!inicio_array) return 0;

    // Para cada objeto no array
    const char *pos = inicio_array;
    const char *inicio_objeto = NULL;
    const char *fim_objeto = NULL;
    int encontrado = 0;

    while ((pos = strchr(pos, '{')) != NULL && !encontrado) {
        // Para cada chave que queremos verificar
        for (size_t i = 0; i < n; i++) {
            char padrao[256];
            snprintf(padrao, sizeof(padrao), "\"%s\": \"%s\"", chaves[i], login);
            
            // Procura até o próximo objeto ou fim do array
            const char *prox_obj = strchr(pos + 1, '{');
            const char *fim_array = strchr(pos, ']');
            const char *fim = prox_obj ? prox_obj : fim_array;
            
            if (!fim) fim = conteudo + strlen(conteudo);
            
            if (strstr(pos, padrao)) {
                inicio_objeto = pos;
                fim_objeto = fim;
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) pos++;
    }

    if (!encontrado || !inicio_objeto || !fim_objeto) return 0;

    // Encontrar o campo senha no objeto
    const char *p_senha = strstr(inicio_objeto, "\"senha\": \"");
    if (!p_senha || p_senha > fim_objeto) return 0;
    
    const char *inicio_val = p_senha + strlen("\"senha\": \"");
    const char *fim_val = strchr(inicio_val, '"');
    if (!fim_val || fim_val > fim_objeto) return 0;

    // Construir novo conteúdo
    char novo[50000];
    size_t parte1_len = p_senha - conteudo + strlen("\"senha\": \"");
    strncpy(novo, conteudo, parte1_len);
    
    // Adicionar nova senha
    strcpy(novo + parte1_len, nova_senha);
    
    // Adicionar resto do arquivo
    strcat(novo, fim_val);

    // Salvar no arquivo
    FILE *f = fopen(arquivo_path, "w");
    if (!f) return 0;
    fputs(novo, f);
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
    
    char codigo[16];
    lerEntradaSegura(codigo, sizeof(codigo), "Digite o código recebido: ");
    if (strcmp(codigo, "123456") != 0) {
        printf("Código incorreto. Redefinição cancelada.\n");
        return;
    }

    char nova[128];
    char confirmar[128];
    lerEntradaSegura(nova, sizeof(nova), "Digite a nova senha: ");
    lerEntradaSegura(confirmar, sizeof(confirmar), "Confirme a nova senha: ");
    
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
        lerEntradaSegura(senha, sizeof(senha), "Digite a senha: ");
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

// Incio do programa de login
int main() {
    system("chcp 65001");
    printf("Área de Login do Professor\n");
    printf("===================================\n");

    char login[128];
    lerEntradaSegura(login, sizeof(login), "Digite o email ou número de matrícula: ");

    if (!validarCampoVazio(login)) {
        return 1;
    }

    char senha_esperada[128];
    if (!encontrar_senha_por_login(login, senha_esperada, sizeof(senha_esperada))) {
        printf("Login não encontrado para '%s'.\n", login);
        return 1;
    }

    if (pedir_e_validar_senha(senha_esperada, 3)) {
        printf("Login bem-sucedido! Bem-vindo, professor '%s'.\n", login);
        
        // Registrar login no arquivo de logs (AUTOMATIZADO via auto_sync)
        auto_log("dados/logs_atividade.json", login, "Login", "Professor realizou login na conta");
    } else {
        printf("Falha no login após várias tentativas.\n");
        char resposta[4];
        lerEntradaSegura(resposta, sizeof(resposta), "Deseja redefinir a senha? (sim/nao): ");
        if (strcmp(resposta, "sim") == 0) {
            solicitar_redefinicao(login);
        } else {
            printf("Redefinição de senha cancelada.\n");
        }
    }
    
    printf("\nPressione qualquer tecla para continuar...");
    system("pause > nul");
    
    // Garantir que todos os dados pendentes sejam sincronizados antes de sair
    finalizar_sistema();
    
    return 0;
}