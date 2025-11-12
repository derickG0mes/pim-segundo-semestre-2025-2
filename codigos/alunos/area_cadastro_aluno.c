#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif

// Incluir módulo de sincronização automática de JSON
#include "../ferramentas/auto_sync.c"

// Função para formatar CPF (###.###.###-##)
void formatarCPF(char *cpf) {
    char cpfFormatado[15];
    int j = 0;
    
    // Remove formatação anterior se houver
    char cpfLimpo[12];
    int k = 0;
    for (int i = 0; cpf[i] != '\0' && k < 11; i++) {
        if (cpf[i] >= '0' && cpf[i] <= '9') {
            cpfLimpo[k++] = cpf[i];
        }
    }
    cpfLimpo[k] = '\0';
    
    // Formata o CPF
    if (strlen(cpfLimpo) == 11) {
        snprintf(cpfFormatado, sizeof(cpfFormatado), "%c%c%c.%c%c%c.%c%c%c-%c%c",
                 cpfLimpo[0], cpfLimpo[1], cpfLimpo[2],
                 cpfLimpo[3], cpfLimpo[4], cpfLimpo[5],
                 cpfLimpo[6], cpfLimpo[7], cpfLimpo[8],
                 cpfLimpo[9], cpfLimpo[10]);
        strcpy(cpf, cpfFormatado);
    }
}

// Função para formatar RG (##.###.###-#)
void formatarRG(char *rg) {
    char rgFormatado[14];
    
    // Remove formatação anterior se houver
    char rgLimpo[10];
    int k = 0;
    for (int i = 0; rg[i] != '\0' && k < 9; i++) {
        if (rg[i] >= '0' && rg[i] <= '9') {
            rgLimpo[k++] = rg[i];
        }
    }
    rgLimpo[k] = '\0';
    
    // Formata o RG
    if (strlen(rgLimpo) == 9) {
        snprintf(rgFormatado, sizeof(rgFormatado), "%c%c.%c%c%c.%c%c%c-%c",
                 rgLimpo[0], rgLimpo[1],
                 rgLimpo[2], rgLimpo[3], rgLimpo[4],
                 rgLimpo[5], rgLimpo[6], rgLimpo[7],
                 rgLimpo[8]);
        strcpy(rg, rgFormatado);
    }
}

// Função para formatar Telefone ((##) #####-####)
void formatarTelefone(char *telefone) {
    char telefoneFormatado[16];
    
    // Remove formatação anterior se houver
    char telefoneLimpo[12];
    int k = 0;
    for (int i = 0; telefone[i] != '\0' && k < 11; i++) {
        if (telefone[i] >= '0' && telefone[i] <= '9') {
            telefoneLimpo[k++] = telefone[i];
        }
    }
    telefoneLimpo[k] = '\0';
    
    // Formata o telefone
    if (strlen(telefoneLimpo) == 11) {
        snprintf(telefoneFormatado, sizeof(telefoneFormatado), "(%c%c) %c%c%c%c%c-%c%c%c%c",
                 telefoneLimpo[0], telefoneLimpo[1],
                 telefoneLimpo[2], telefoneLimpo[3], telefoneLimpo[4], telefoneLimpo[5], telefoneLimpo[6],
                 telefoneLimpo[7], telefoneLimpo[8], telefoneLimpo[9], telefoneLimpo[10]);
        strcpy(telefone, telefoneFormatado);
    } else if (strlen(telefoneLimpo) == 10) {
        // Formato antigo (##) ####-####
        snprintf(telefoneFormatado, sizeof(telefoneFormatado), "(%c%c) %c%c%c%c-%c%c%c%c",
                 telefoneLimpo[0], telefoneLimpo[1],
                 telefoneLimpo[2], telefoneLimpo[3], telefoneLimpo[4], telefoneLimpo[5],
                 telefoneLimpo[6], telefoneLimpo[7], telefoneLimpo[8], telefoneLimpo[9]);
        strcpy(telefone, telefoneFormatado);
    }
}

// Função para validar o nome (aceita apenas letras e espaços)
void validarNome(char *nome, size_t tamanho) {
    int nomeValido = 0;
    
    while (!nomeValido) {
        nomeValido = 1;
        
        for (int i = 0; nome[i] != '\0'; i++) {
            char letra = nome[i];
            
            int ehLetraMaiuscula = (letra >= 'A' && letra <= 'Z');
            int ehLetraMinuscula = (letra >= 'a' && letra <= 'z');
            int ehEspaco = (letra == ' ');
            
            if (!ehLetraMaiuscula && !ehLetraMinuscula && !ehEspaco) {
                printf("Nome inválido! Use apenas letras e espaços.\n");
                lerEntradaSegura(nome, tamanho, "Digite o nome do aluno novamente: ");
                
                nomeValido = 0;
                break;
            }
        }
    }
}

// Função para validar o CPF (apenas números, aceita com ou sem formatação)
void validarCPF(char *cpf, size_t tamanho) {
    int cpfValido = 0;
    
    while (!cpfValido) {
        cpfValido = 1;
        int contadorDigitos = 0;
        
        // Conta quantos dígitos tem
        for (int i = 0; cpf[i] != '\0'; i++) {
            char caractere = cpf[i];
            
            int ehDigito = (caractere >= '0' && caractere <= '9');
            int ehPonto = (caractere == '.');
            int ehTraco = (caractere == '-');
            
            if (ehDigito) {
                contadorDigitos++;
            } else if (!ehPonto && !ehTraco) {
                printf("CPF inválido! Use apenas números (pode usar pontos e traços).\n");
                lerEntradaSegura(cpf, tamanho, "Digite o CPF do aluno novamente: ");
                
                cpfValido = 0;
                break;
            }
        }
        
        if (cpfValido && contadorDigitos != 11) {
            printf("CPF inválido! Deve conter exatamente 11 dígitos.\n");
            lerEntradaSegura(cpf, tamanho, "Digite o CPF do aluno novamente: ");
            
            cpfValido = 0;
        }
    }
    
    // Formata o CPF após validação
    formatarCPF(cpf);
}

// Função para validar o RG (apenas números, aceita com ou sem formatação)
void validarRG(char *rg, size_t tamanho) {
    int rgValido = 0;

    while (!rgValido) {
        rgValido = 1;
        int contadorDigitos = 0;

        for (int i = 0; rg[i] != '\0'; i++) {
            char caractere = rg[i];

            int ehDigito = (caractere >= '0' && caractere <= '9');
            int ehPonto = (caractere == '.');
            int ehTraco = (caractere == '-');

            if (ehDigito) {
                contadorDigitos++;
            } else if (!ehPonto && !ehTraco) {
                printf("RG inválido! Use apenas números (pode usar pontos e traços).\n");
                lerEntradaSegura(rg, tamanho, "Digite o RG do aluno novamente: ");

                rgValido = 0;
                break;
            }
        }

        if (rgValido && contadorDigitos != 9) {
            printf("RG inválido! Deve conter exatamente 9 dígitos.\n");
            lerEntradaSegura(rg, tamanho, "Digite o RG do aluno novamente: ");

            rgValido = 0;
        }
    }
    
    // Formata o RG após validação
    formatarRG(rg);
}

// Função para validar o RA (apenas números)
void validarRA(char *ra, size_t tamanho) {
    int raValido = 0;

    while (!raValido) {
        raValido = 1;

        for (int i = 0; ra[i] != '\0'; i++) {
            char caractere = ra[i];

            int ehDigito = (caractere >= '0' && caractere <= '9');

            if (!ehDigito) {
                printf("RA inválido! Use apenas números.\n");
                lerEntradaSegura(ra, tamanho, "Digite o RA do aluno novamente: ");

                raValido = 0;
                break;
            }
        }
    }
}

// Função para validar a data de nascimento (formato DD/MM/AAAA)
void validarDataNascimento(char *dataNascimento, size_t tamanho) {
    int dataValida = 0;

    while (!dataValida) {
        dataValida = 1;

        if (strlen(dataNascimento) != 10 ||
            dataNascimento[2] != '/' || dataNascimento[5] != '/') {
            dataValida = 0;
        } else {
            for (int i = 0; i < 10; i++) {
                if (i == 2 || i == 5) continue;
                if (dataNascimento[i] < '0' || dataNascimento[i] > '9') {
                    dataValida = 0;
                    break;
                }
            }
        }

        if (!dataValida) {
            printf("Data de nascimento inválida! Use o formato DD/MM/AAAA.\n");
            lerEntradaSegura(dataNascimento, tamanho, "Digite a data de nascimento do aluno novamente: ");
        }
    }
}

// Função para validar o endereço completo (não vazio)
void validarEnderecoCompleto(char *enderecoCompleto, size_t tamanho) {
    int enderecoValido = 0;

    while (!enderecoValido) {
        enderecoValido = 1;

        if (strlen(enderecoCompleto) == 0) {
            enderecoValido = 0;
        }

        if (!enderecoValido) {
            printf("Endereço completo inválido! Não pode estar vazio.\n");
            lerEntradaSegura(enderecoCompleto, tamanho, "Digite o endereço completo do aluno novamente: ");
        }
    }
}

// Função para validar o número da matricula (apenas números e uma letra qualquer)
void validarNumeroMatricula(char *numeroMatricula, size_t tamanho) {
    int matriculaValida = 0;

    while (!matriculaValida) {
        matriculaValida = 1;

        for (int i = 0; numeroMatricula[i] != '\0'; i++) {
            char caractere = numeroMatricula[i];

            int ehDigito = (caractere >= '0' && caractere <= '9');
            int ehLetra = (caractere >= 'A' && caractere <= 'Z') || (caractere >= 'a' && caractere <= 'z');

            if (!ehDigito && !ehLetra) {
                printf("Número de matrícula inválido! Use apenas números e letras.\n");
                lerEntradaSegura(numeroMatricula, tamanho, "Digite o número de matrícula do aluno novamente: ");

                matriculaValida = 0;
                break;
            }
        }
    }
}

// Função para validar o nível escolaridade
void validarNivelEscolaridade(char *nivelEscolaridade, size_t tamanho) {
    int nivelValido = 0;

    while (!nivelValido) {
        nivelValido = 1;

        if (strlen(nivelEscolaridade) == 0) {
            nivelValido = 0;
        }

        if (!nivelValido) {
            printf("Nível de escolaridade inválido! Não pode estar vazio.\n");
            lerEntradaSegura(nivelEscolaridade, tamanho, "Digite o nível de escolaridade do aluno novamente: ");
        }
    }
}

// Função para selecionar série/turma
void selecionarSerieTurma(char *serie, size_t tamanho) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║              SELEÇÃO DE SÉRIE/TURMA/SEMESTRE               ║\n");
    printf("║                                                            ║\n");
    printf("║ Escolha a série/turma do aluno:                            ║\n");
    printf("║                                                            ║\n");
    printf("║  ENSINO FUNDAMENTAL (1º-9º Ano):                           ║\n");
    printf("║  [1] 1º Ano   [2] 2º Ano   [3] 3º Ano   [4] 4º Ano         ║\n");
    printf("║  [5] 5º Ano   [6] 6º Ano   [7] 7º Ano   [8] 8º Ano         ║\n");
    printf("║  [9] 9º Ano                                                ║\n");
    printf("║                                                            ║\n");
    printf("║  ENSINO MÉDIO (1º-3º Ano):                                 ║\n");
    printf("║  [10] 1º Ano Médio   [11] 2º Ano Médio   [12] 3º Médio    ║\n");
    printf("║                                                            ║\n");
    printf("║  SEMESTRES/BIMESTRES (1º-4º):                              ║\n");
    printf("║  [13] 1º Semestre/Bimestre                                 ║\n");
    printf("║  [14] 2º Semestre/Bimestre                                 ║\n");
    printf("║  [15] 3º Semestre/Bimestre                                 ║\n");
    printf("║  [16] 4º Semestre/Bimestre                                 ║\n");
    printf("║                                                            ║\n");
    printf("║  [0] Outro (Digite manualmente)                            ║\n");
    printf("║                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    const char *opcoes[] = {
        "1º Ano - Ensino Fundamental",
        "2º Ano - Ensino Fundamental",
        "3º Ano - Ensino Fundamental",
        "4º Ano - Ensino Fundamental",
        "5º Ano - Ensino Fundamental",
        "6º Ano - Ensino Fundamental",
        "7º Ano - Ensino Fundamental",
        "8º Ano - Ensino Fundamental",
        "9º Ano - Ensino Fundamental",
        "1º Ano - Ensino Médio",
        "2º Ano - Ensino Médio",
        "3º Ano - Ensino Médio",
        "1º Semestre/Bimestre",
        "2º Semestre/Bimestre",
        "3º Semestre/Bimestre",
        "4º Semestre/Bimestre"
    };
    
    char escolha[4];
    int opcao_valida = 0;
    
    while (!opcao_valida) {
        lerEntradaSegura(escolha, sizeof(escolha), "Digite sua escolha (0-16): ");
        int opcao = atoi(escolha);
        
        if (opcao >= 1 && opcao <= 16) {
            strcpy(serie, opcoes[opcao - 1]);
            opcao_valida = 1;
            printf("✓ Série/Turma selecionada: %s\n", serie);
        } else if (opcao == 0) {
            lerEntradaSegura(serie, tamanho, "Digite a série/turma manualmente: ");
            if (strlen(serie) > 0) {
                opcao_valida = 1;
                printf("✓ Série/Turma registrada: %s\n", serie);
            } else {
                printf("❌ Série/Turma não pode estar vazia!\n");
            }
        } else {
            printf("❌ Opção inválida! Digite um número entre 0 e 16.\n");
        }
    }
}

// Função para validar email
void validarEmail(char *email, size_t tamanho) {
    int emailValido = 0;

    while (!emailValido) {
        emailValido = 1;

        if (strchr(email, '@') == NULL || strchr(email, '.') == NULL) {
            emailValido = 0;
        }

        if (!emailValido) {
            printf("Email inválido! Deve conter '@' e '.'.\n");
            lerEntradaSegura(email, tamanho, "Digite o email do aluno novamente: ");
        }
    }
}

// Função para validar o número de telefone (aceita com ou sem formatação)
void validarNumeroTelefone(char *numeroTelefone, size_t tamanho) {
    int telefoneValido = 0;

    while (!telefoneValido) {
        telefoneValido = 1;
        int contadorDigitos = 0;

        for (int i = 0; numeroTelefone[i] != '\0'; i++) {
            char caractere = numeroTelefone[i];

            int ehDigito = (caractere >= '0' && caractere <= '9');
            int ehParentese = (caractere == '(' || caractere == ')');
            int ehEspaco = (caractere == ' ');
            int ehTraco = (caractere == '-');

            if (ehDigito) {
                contadorDigitos++;
            } else if (!ehParentese && !ehEspaco && !ehTraco) {
                printf("Número de telefone inválido! Use apenas números (pode usar parênteses, espaços e traços).\n");
                lerEntradaSegura(numeroTelefone, tamanho, "Digite o número de telefone do aluno novamente: ");

                telefoneValido = 0;
                break;
            }
        }

        if (telefoneValido && contadorDigitos != 10 && contadorDigitos != 11) {
            printf("Número de telefone inválido! Deve conter 10 ou 11 dígitos.\n");
            lerEntradaSegura(numeroTelefone, tamanho, "Digite o número de telefone do aluno novamente: ");

            telefoneValido = 0;
        }
    }
    
    // Formata o telefone após validação
    formatarTelefone(numeroTelefone);
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

void confirmarCadastro() {
    char confirmacao[4];
    lerEntradaSegura(confirmacao, sizeof(confirmacao), "Confirma o cadastro? (sim/não): ");

    while (strcmp(confirmacao, "sim") != 0 && strcmp(confirmacao, "não") != 0) {
        printf("Resposta inválida. Por favor, responda com 'sim' ou 'não'.\n");
        lerEntradaSegura(confirmacao, sizeof(confirmacao), "Confirma o cadastro? (sim/não): ");
    }

    if (strcmp(confirmacao, "sim") == 0) {
        printf("Cadastro confirmado!\n");
    } else {
        printf("Cadastro cancelado.\n");
        exit(0);
    }
}

// Função para adicionar aluno à turma no arquivo turmas_informacoes.json
void adicionarAlunoAturma(const char *nome, const char *ra, const char *serie) {
    // Cria o diretório se não existir
    #ifdef _WIN32
        _mkdir("dados");
        _mkdir("dados\\turmas_informacoes");
        const char *arquivo_path = "dados\\turmas_informacoes\\turmas_informacoes.json";
    #else
        mkdir("dados", 0777);
        mkdir("dados/turmas_informacoes", 0777);
        const char *arquivo_path = "dados/turmas_informacoes/turmas_informacoes.json";
    #endif
    
    // Verifica se o arquivo existe
    FILE *arquivo = fopen(arquivo_path, "r");
    
    if (arquivo == NULL) {
        // Arquivo não existe, cria um novo
        arquivo = fopen(arquivo_path, "w");
        if (arquivo != NULL) {
            fprintf(arquivo, "{\n");
            fprintf(arquivo, "  \"%s\": [\n", serie);
            fprintf(arquivo, "    {\n");
            fprintf(arquivo, "      \"nome\": \"%s\",\n", nome);
            fprintf(arquivo, "      \"ra\": \"%s\",\n", ra);
            fprintf(arquivo, "      \"desempenho\": \"A Iniciar\"\n");
            fprintf(arquivo, "    }\n");
            fprintf(arquivo, "  ]\n");
            fprintf(arquivo, "}\n");
            fclose(arquivo);
        }
        return;
    }
    
    // Arquivo existe, lê o conteúdo
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    fseek(arquivo, 0, SEEK_SET);
    
    char *conteudo = (char *)malloc(tamanho + 1);
    if (conteudo == NULL) {
        fclose(arquivo);
        return;
    }
    
    fread(conteudo, 1, tamanho, arquivo);
    conteudo[tamanho] = '\0';
    fclose(arquivo);
    
    // Remove o último } do JSON
    if (conteudo[tamanho - 2] == '\n') {
        conteudo[tamanho - 2] = '\0';
    } else if (conteudo[tamanho - 1] == '}') {
        conteudo[tamanho - 1] = '\0';
    }
    
    // Abre o arquivo para escrita
    arquivo = fopen(arquivo_path, "w");
    if (arquivo != NULL) {
        // Verifica se a série já existe no JSON
        char busca[256];
        snprintf(busca, sizeof(busca), "\"%s\":", serie);
        
        if (strstr(conteudo, busca) != NULL) {
            // Série já existe, adiciona aluno à série existente
            char novo_aluno[512];
            snprintf(novo_aluno, sizeof(novo_aluno),
                     ",\n    {\n      \"nome\": \"%s\",\n      \"ra\": \"%s\",\n      \"desempenho\": \"A Iniciar\"\n    }",
                     nome, ra);
            
            // Encontra a posição para inserir (antes do ] da série)
            char *pos = strstr(conteudo, busca);
            if (pos != NULL) {
                char *fim_serie = strchr(pos, ']');
                if (fim_serie != NULL) {
                    *fim_serie = '\0';
                    fprintf(arquivo, "%s%s%s\n}\n", conteudo, novo_aluno, "]");
                } else {
                    fprintf(arquivo, "%s\n}\n", conteudo);
                }
            }
        } else {
            // Série não existe, cria uma nova
            // Remove a última } e adiciona a nova série
            fprintf(arquivo, "%s,\n  \"%s\": [\n", conteudo, serie);
            fprintf(arquivo, "    {\n");
            fprintf(arquivo, "      \"nome\": \"%s\",\n", nome);
            fprintf(arquivo, "      \"ra\": \"%s\",\n", ra);
            fprintf(arquivo, "      \"desempenho\": \"A Iniciar\"\n");
            fprintf(arquivo, "    }\n");
            fprintf(arquivo, "  ]\n");
            fprintf(arquivo, "}\n");
        }
        fclose(arquivo);
    }
    
    free(conteudo);
}

int main() {
    system("chcp 65001");

    printf("Área de Cadastro do aluno\n");
    printf("===================================\n");

    char nome[100];
    lerEntradaSegura(nome, sizeof(nome), "Digite o nome do aluno: ");

    while (strlen(nome) == 0) {
        printf("Nome inválido.\n");
        lerEntradaSegura(nome, sizeof(nome), "Digite o nome do aluno: ");
    }

    validarNome(nome, sizeof(nome));

    char cpf[20];
    lerEntradaSegura(cpf, sizeof(cpf), "Digite o CPF do aluno (apenas números ou formato ###.###.###-##): ");
    validarCPF(cpf, sizeof(cpf));

    char rg[20];
    lerEntradaSegura(rg, sizeof(rg), "Digite o RG do aluno (apenas números ou formato ##.###.###-#): ");
    validarRG(rg, sizeof(rg));

    char ra[10];
    lerEntradaSegura(ra, sizeof(ra), "Digite o RA do aluno (Registro Acadêmico): ");
    validarRA(ra, sizeof(ra));

    char dataNascimento[11];
    lerEntradaSegura(dataNascimento, sizeof(dataNascimento), "Digite a data de nascimento do aluno (DD/MM/AAAA): ");
    validarDataNascimento(dataNascimento, sizeof(dataNascimento));

    char enderecoCompleto[300];
    lerEntradaSegura(enderecoCompleto, sizeof(enderecoCompleto), "Digite o endereço completo do aluno: ");
    validarEnderecoCompleto(enderecoCompleto, sizeof(enderecoCompleto));

    char numeroMatricula[20];
    lerEntradaSegura(numeroMatricula, sizeof(numeroMatricula), "Digite o número de matrícula do aluno: ");
    validarNumeroMatricula(numeroMatricula, sizeof(numeroMatricula));

    char nivelEscolaridade[50];
    lerEntradaSegura(nivelEscolaridade, sizeof(nivelEscolaridade), "Digite o nível de escolaridade do aluno: ");
    validarNivelEscolaridade(nivelEscolaridade, sizeof(nivelEscolaridade));

    char email[100];
    lerEntradaSegura(email, sizeof(email), "Digite o email do aluno: ");
    validarEmail(email, sizeof(email));

    char numeroTelefone[20];
    lerEntradaSegura(numeroTelefone, sizeof(numeroTelefone), "Digite o número de telefone do aluno (10 ou 11 dígitos): ");
    validarNumeroTelefone(numeroTelefone, sizeof(numeroTelefone));
    
    // Selecionar série/turma
    char serie[100];
    selecionarSerieTurma(serie, sizeof(serie));
    
    char senha[32];
    // Senha Randomica gerada a partir do RA
    snprintf(senha, sizeof(senha), "Senha@%s", ra);

    confirmarCadastro();

    // Localizar/definir caminho do arquivo JSON
    char arquivo_path[512];
    // verificar variável de ambiente CADASTRO_JSON primeiro
    int encontrado = 0;
    const char *env_path = getenv("CADASTRO_JSON");
    if (env_path) {
        FILE *fe = fopen(env_path, "r");
        if (fe) {
            fclose(fe);
            snprintf(arquivo_path, sizeof(arquivo_path), "%s", env_path);
            encontrado = 1;
        }
    }

    // caminhos candidatos
    const char *candidatos[] = {
        "../dados/alunos_cadastros/cadastro_aluno.json",
        "../../dados/alunos_cadastros/cadastro_aluno.json",
        "../../../dados/alunos_cadastros/cadastro_aluno.json",
        "./dados/alunos_cadastros/cadastro_aluno.json"
    };
    size_t nc = sizeof(candidatos) / sizeof(candidatos[0]);
    for (size_t i = 0; i < nc; i++) {
        FILE *f = fopen(candidatos[i], "r");
        if (f) {
            fclose(f);
            snprintf(arquivo_path, sizeof(arquivo_path), "%s", candidatos[i]);
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        // Pergunta ao usuário onde salvar ou usa padrão
        char resposta[512];
        const char *padrao_dir = "alunos_cadastros";
        const char *padrao = "alunos_cadastros/cadastro_aluno.json";
        printf("Arquivo 'cadastro_aluno.json' não encontrado em locais padrão.\n");
        char mensagem[600];
        snprintf(mensagem, sizeof(mensagem), "Pressione Enter para criar em '%s' ou digite caminho completo para salvar: ", padrao);
        lerEntradaSegura(resposta, sizeof(resposta), mensagem);
        if (strlen(resposta) == 0) {
            // cria diretório padrão se necessário
#ifdef _WIN32
            _mkdir(padrao_dir);
#else
            mkdir(padrao_dir, 0755);
#endif
            snprintf(arquivo_path, sizeof(arquivo_path), "%s", padrao);
        } else {
            snprintf(arquivo_path, sizeof(arquivo_path), "%s", resposta);
            // tenta criar o arquivo para validar o caminho
            FILE *ft = fopen(arquivo_path, "w");
            if (!ft) {
                printf("Não foi possível criar o arquivo em '%s'. Cancelando cadastro.\n", arquivo_path);
                return 1;
            }
            fclose(ft);
        }
    }

    // Ler o conteúdo existente do arquivo
    char conteudo_atual[50000] = "["; // Inicia com colchete para array JSON
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
        fprintf(arquivo, "    \"nome\": \"%s\",\n", nome);
        fprintf(arquivo, "    \"cpf\": \"%s\",\n", cpf);
        fprintf(arquivo, "    \"rg\": \"%s\",\n", rg);
        fprintf(arquivo, "    \"ra\": \"%s\",\n", ra);
        fprintf(arquivo, "    \"dataNascimento\": \"%s\",\n", dataNascimento);
        fprintf(arquivo, "    \"enderecoCompleto\": \"%s\",\n", enderecoCompleto);
        fprintf(arquivo, "    \"numeroMatricula\": \"%s\",\n", numeroMatricula);
        fprintf(arquivo, "    \"nivelEscolaridade\": \"%s\",\n", nivelEscolaridade);
        fprintf(arquivo, "    \"email\": \"%s\",\n", email);
        fprintf(arquivo, "    \"numeroTelefone\": \"%s\",\n", numeroTelefone);
        fprintf(arquivo, "    \"serie\": \"%s\",\n", serie);
        fprintf(arquivo, "    \"senha\": \"%s\"\n", senha);
        fprintf(arquivo, "  }\n]");
        fclose(arquivo);
        printf("Cadastro realizado com sucesso! Arquivo salvo em: %s\n", arquivo_path);
        
        // Adiciona o aluno à turma (AUTOMATIZADO via auto_sync)
        auto_atualizar_desempenho("dados/turmas_informacoes.json", ra, nome, serie, 0.0);
        printf("Aluno adicionado à turma em turmas_informacoes.json\n");
    } else {
        printf("Erro ao finalizar o cadastro. Nao foi possivel abrir '%s' para escrita.\n", arquivo_path);
    }

    printf("\nPressione qualquer tecla para continuar...");
    system("pause > nul");
    
    // Garantir que todos os dados pendentes sejam sincronizados antes de sair
    finalizar_sistema();
    
    return 0;
}