// Área de Login do Administrador
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// Incluir módulo de sincronização automática de JSON
#include "../ferramentas/auto_sync.c"

// Declaração da função do painel (será linkada)
void menu_painel_administrativo(const char *admin_name);

// Encontrar o arquivo "adm_credenciais.json"
FILE* encontrar_arquivo_credenciais() {
    const char *caminhos[] = {
        "../dados/adm-credencias/adm_credenciais.json",
        "../../dados/adm-credencias/adm_credenciais.json",
        "../../../dados/adm-credencias/adm_credenciais.json",
        "./dados/adm-credencias/adm_credenciais.json"
    };
    FILE *arquivo = NULL;
    for (int i = 0; i < 4; i++) {
        arquivo = fopen(caminhos[i], "r");
        if (arquivo != NULL) {
            break;
        }
    }
    if (arquivo == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo de credenciais.\n");
        exit(1);
    }
    return arquivo;
}

//Verificar se está funcionando
int verificar_credenciais(const char *email, const char *senha) {
    FILE *arquivo = encontrar_arquivo_credenciais();
    char linha[512];
    char email_armazenado[100];
    char senha_armazenada[100];
    char nome_armazenado[100];
    int ativo;

    while (fgets(linha, sizeof(linha), arquivo)) {
        // Procurar por "email": no JSON
        if (strstr(linha, "\"email\":")) {
            sscanf(linha, " {\"email\":\"%99[^\"]\",\"senha\":\"%99[^\"]\"", email_armazenado, senha_armazenada);
            
            if (strcmp(email, email_armazenado) == 0 && strcmp(senha, senha_armazenada) == 0) {
                fclose(arquivo);
                
                // Registrar login do admin (AUTOMATIZADO via auto_sync)
                auto_log("dados/logs_atividade.json", email, "Login", "Administrador realizou login no painel");
                
                return 1; // Credenciais válidas
            }
        }
    }

    fclose(arquivo);
    printf("\nEmail ou senha incorretos!\n");
    printf("Pressione qualquer tecla para continuar...");
    system("pause > nul");
    return 0; // Credenciais inválidas
}