// Área de Cadastro de Professores
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <locale.h>
#include <ctype.h>
#ifdef _WIN32
#include <direct.h>
#endif

// Protótipo da função formatarTelefone
void formatarTelefone(char *telefone);

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

// Função auxiliar para verificar se um caractere UTF-8 é válido para nomes
int ehCaractereNomeValido(const unsigned char *c) {
    if (*c < 128) {
        // ASCII básico
        return isalpha(*c) || *c == ' ' || *c == '-' || *c == '\'';
    } else if ((*c & 0xE0) == 0xC0) {
        // 2 bytes UTF-8
        return 1;
    } else if ((*c & 0xF0) == 0xE0) {
        // 3 bytes UTF-8
        return 1;
    }
    return 0;
}

// Função para validar o nome (aceita letras, espaços e caracteres UTF-8)
void validarNome(char *nome, size_t tamanho) {
    int nomeValido = 0;
    
    while (!nomeValido) {
        nomeValido = 1;
        
        for (int i = 0; nome[i] != '\0';) {
            unsigned char c = (unsigned char)nome[i];
            
            if (!ehCaractereNomeValido((const unsigned char *)&nome[i])) {
                printf("Nome invalido! Use apenas letras, espacos, hifen ou apostrofo.\n");
                lerEntradaSegura(nome, tamanho, "Digite o nome do professor novamente: ");
                nomeValido = 0;
                break;
            }
            
            // Avança o ponteiro baseado no número de bytes do caractere UTF-8
            if (c < 128) i++; // ASCII básico
            else if ((c & 0xE0) == 0xC0) i += 2; // 2 bytes UTF-8
            else if ((c & 0xF0) == 0xE0) i += 3; // 3 bytes UTF-8
            else i++; // Caractere inválido, avança um byte
        }
    }
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
                printf("RA invalido! Use apenas numeros.\n");
                lerEntradaSegura(ra, tamanho, "Digite o RA do professor novamente: ");
                raValido = 0;
                break;
            }
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
            printf("Email invalido! Deve conter '@' e '.'.\n");
            lerEntradaSegura(email, tamanho, "Digite o email do professor novamente: ");
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
            printf("Data de nascimento invalida! Use o formato DD/MM/AAAA.\n");
            lerEntradaSegura(dataNascimento, tamanho, "Digite a data de nascimento do professor novamente: ");
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
                printf("Numero de telefone invalido! Use apenas numeros (pode usar parenteses, espacos e tracos).\n");
                lerEntradaSegura(numeroTelefone, tamanho, "Digite o numero de telefone do professor novamente: ");
                telefoneValido = 0;
                break;
            }
        }

        if (telefoneValido && contadorDigitos != 10 && contadorDigitos != 11) {
            printf("Numero de telefone invalido! Deve conter 10 ou 11 digitos.\n");
            lerEntradaSegura(numeroTelefone, tamanho, "Digite o numero de telefone do professor novamente: ");
            telefoneValido = 0;
        }
    }
    
    // Formata o telefone após validação
    formatarTelefone(numeroTelefone);
}


// Função para validar o tempo de atuação (apenas números)
void validarTempoAtuacao(char *tempoAtuacao, size_t tamanho) {
    int tempoValido = 0;

    while (!tempoValido) {
        tempoValido = 1;

        for (int i = 0; tempoAtuacao[i] != '\0'; i++) {
            char caractere = tempoAtuacao[i];

            int ehDigito = (caractere >= '0' && caractere <= '9');

            if (!ehDigito) {
                printf("Tempo de atuacao invalido! Use apenas numeros.\n");
                lerEntradaSegura(tempoAtuacao, tamanho, "Digite o tempo de atuacao do professor novamente: ");
                tempoValido = 0;
                break;
            }
        }
    }
}

// Função para validar a disciplina, onde o usuário deve escolher entre opções pré-definidas
void validarDisciplina(char *disciplina, size_t tamanho) {
    const char *disciplinasValidas[] = {
        "Matematica",
        "Portugues",
        "Historia",
        "Geografia",
        "Ciencias",
        "Fisica",
        "Quimica",
        "Biologia",
        "Ingles",
        "Educacao Fisica"
    };
    size_t numDisciplinas = sizeof(disciplinasValidas) / sizeof(disciplinasValidas[0]);
    int disciplinaValida = 0;

    while (!disciplinaValida) {
        disciplinaValida = 0;

        for (size_t i = 0; i < numDisciplinas; i++) {
            if (strcmp(disciplina, disciplinasValidas[i]) == 0) {
                disciplinaValida = 1;
                break;
            }
        }

        if (!disciplinaValida) {
            printf("Disciplina invalida! Escolha entre as seguintes opcoes:\n");
            for (size_t i = 0; i < numDisciplinas; i++) {
                printf("- %s\n", disciplinasValidas[i]);
            }
            lerEntradaSegura(disciplina, tamanho, "Digite a disciplina do professor novamente: ");
        }
    }
}

// Função para validar o periodo de trabalho, onde o usuário deve escolher entre opções pré-definidas
void validarPeriodoTrabalho(char *periodoTrabalho, size_t tamanho) {
    const char *periodosValidos[] = {
        "Matutino",
        "Vespertino",
        "Noturno",
        "Integral"
    };
    size_t numPeriodos = sizeof(periodosValidos) / sizeof(periodosValidos[0]);
    int periodoValido = 0;

    while (!periodoValido) {
        periodoValido = 0;

        for (size_t i = 0; i < numPeriodos; i++) {
            if (strcmp(periodoTrabalho, periodosValidos[i]) == 0) {
                periodoValido = 1;
                break;
            }
        }

        if (!periodoValido) {
            printf("Periodo de trabalho invalido! Escolha entre as seguintes opcoes:\n");
            for (size_t i = 0; i < numPeriodos; i++) {
                printf("- %s\n", periodosValidos[i]);
            }
            lerEntradaSegura(periodoTrabalho, tamanho, "Digite o periodo de trabalho do professor novamente: ");
        }
    }
}

// Função para validar data de inicio de contrato (formato DD/MM/AAAA)
void validarDataInicioContrato(char *dataInicioContrato, size_t tamanho) {
    int dataValida = 0;

    while (!dataValida) {
        dataValida = 1;

        if (strlen(dataInicioContrato) != 10 ||
            dataInicioContrato[2] != '/' || dataInicioContrato[5] != '/') {
            dataValida = 0;
        } else {
            for (int i = 0; i < 10; i++) {
                if (i == 2 || i == 5) continue;
                if (dataInicioContrato[i] < '0' || dataInicioContrato[i] > '9') {
                    dataValida = 0;
                    break;
                }
            }
        }

        if (!dataValida) {
            printf("Data de inicio de contrato invalida! Use o formato DD/MM/AAAA.\n");
            lerEntradaSegura(dataInicioContrato, tamanho, "Digite a data de inicio de contrato do professor novamente: ");
        }
    }
}

// Função para converter data em número para comparação
int dataParaNumero(const char *data) {
    int dia = (data[0] - '0') * 10 + (data[1] - '0');
    int mes = (data[3] - '0') * 10 + (data[4] - '0');
    int ano = (data[6] - '0') * 1000 + (data[7] - '0') * 100 + 
              (data[8] - '0') * 10 + (data[9] - '0');
    return ano * 10000 + mes * 100 + dia;
}

// Função para validar data de fim de contrato (formato DD/MM/AAAA)
void validarDataFimContrato(char *dataFimContrato, size_t tamanho, const char *dataInicio) {
    int dataValida = 0;

    while (!dataValida) {
        dataValida = 1;

        if (strlen(dataFimContrato) != 10 ||
            dataFimContrato[2] != '/' || dataFimContrato[5] != '/') {
            dataValida = 0;
        } else {
            for (int i = 0; i < 10; i++) {
                if (i == 2 || i == 5) continue;
                if (dataFimContrato[i] < '0' || dataFimContrato[i] > '9') {
                    dataValida = 0;
                    break;
                }
            }
            
            // Verifica se a data de fim é posterior à data de início
            if (dataValida && dataParaNumero(dataFimContrato) <= dataParaNumero(dataInicio)) {
                printf("Data de fim deve ser posterior a data de inicio (%s).\n", dataInicio);
                dataValida = 0;
            }
        }

        if (!dataValida) {
            printf("Data de fim de contrato invalida! Use o formato DD/MM/AAAA.\n");
            lerEntradaSegura(dataFimContrato, tamanho, "Digite a data de fim de contrato do professor novamente: ");
        }
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

// Função para validar senha (esconder a senha entre asteriscos)
void validarSenha(char *senha, size_t tamanho) {
    while (strlen(senha) < 6) {
        printf("Senha muito curta! A senha deve ter pelo menos 6 caracteres.\n");
        lerEntradaSegura(senha, tamanho, "Digite a senha novamente: ");
    }
    
    // Esconder a senha entre asteriscos
    char senhaOculta[100];
    for (int i = 0; i < strlen(senha); i++) {
        senhaOculta[i] = '*';
    }
    senhaOculta[strlen(senha)] = '\0';
    printf("Senha cadastrada com sucesso! (Senha: %s)\n", senhaOculta);
}

// Função para confirmar senha
void confirmarSenha(char *senha, size_t tamanho) {
    char senhaConfirmada[100];
    int senhaCorreta = 0;
    
    while (!senhaCorreta) {
        lerEntradaSegura(senhaConfirmada, tamanho, "Confirme sua senha: ");
        if (strcmp(senha, senhaConfirmada) == 0) {
            printf("Senha confirmada com sucesso!\n");
            senhaCorreta = 1;
        } else {
            printf("Senha nao confere. Tente novamente.\n");
        }
    }
}

// Função para confirmar o cadastro
void confirmarCadastro() {
    char confirmacao[4];
    lerEntradaSegura(confirmacao, sizeof(confirmacao), "Confirma o cadastro? (sim/nao): ");

    while (strcmp(confirmacao, "sim") != 0 && strcmp(confirmacao, "nao") != 0) {
        printf("Resposta invalida! Digite 'sim' ou 'nao': ");
        lerEntradaSegura(confirmacao, sizeof(confirmacao), "");
    }

    if (strcmp(confirmacao, "sim") == 0) {
        printf("Cadastro confirmado!\n");
    } else {
        printf("Cadastro cancelado.\n");
        exit(0);
    }
}

// Função limpar todos os campos (definir strings como vazias)
void limparCampos(char *campos[], size_t numCampos) {
    for (size_t i = 0; i < numCampos; i++) {
        campos[i][0] = '\0';
    }
}


// Inicio do programa
int main() {
    setlocale(LC_ALL, "Portuguese");
    system("chcp 65001");

    printf("\nArea de Cadastro do Professor\n");
    printf("===================================\n");

    char nome[100];
    lerEntradaSegura(nome, sizeof(nome), "Digite o nome do professor: ");

    validarNome(nome, sizeof(nome));

    char ra[20];
    lerEntradaSegura(ra, sizeof(ra), "Digite o RA do professor (apenas numeros): ");

    validarRA(ra, sizeof(ra));

    char dataNascimento[11];
    lerEntradaSegura(dataNascimento, sizeof(dataNascimento), "Digite a data de nascimento do professor (DD/MM/AAAA): ");

    validarDataNascimento(dataNascimento, sizeof(dataNascimento));

    // Campo matéria
    char materia[50];
    lerEntradaSegura(materia, sizeof(materia), "Escolha a materia que o professor leciona (Matematica/Portugues/Historia/Geografia/Ciencias/Fisica/Quimica/Biologia/Ingles/Educacao Fisica): ");

    validarDisciplina(materia, sizeof(materia));

    char tempoAtuacao[10];
    lerEntradaSegura(tempoAtuacao, sizeof(tempoAtuacao), "Digite o tempo de atuacao do professor (em anos): ");

    validarTempoAtuacao(tempoAtuacao, sizeof(tempoAtuacao));

    char periodoTrabalho[20];
    lerEntradaSegura(periodoTrabalho, sizeof(periodoTrabalho), "Escolha o periodo de trabalho do professor (Matutino/Vespertino/Noturno/Integral): ");

    validarPeriodoTrabalho(periodoTrabalho, sizeof(periodoTrabalho));

    char inicioContrato[11];
    lerEntradaSegura(inicioContrato, sizeof(inicioContrato), "Digite a data de inicio do contrato (DD/MM/AAAA): ");

    validarDataInicioContrato(inicioContrato, sizeof(inicioContrato));

    char fimContrato[11];
    lerEntradaSegura(fimContrato, sizeof(fimContrato), "Digite a data de fim do contrato (DD/MM/AAAA): ");

    validarDataFimContrato(fimContrato, sizeof(fimContrato), inicioContrato);

    char email[100];
    lerEntradaSegura(email, sizeof(email), "Digite o email do professor: ");

    validarEmail(email, sizeof(email));

    char telefone[20];
    lerEntradaSegura(telefone, sizeof(telefone), "Digite o numero de telefone do professor: ");
    validarNumeroTelefone(telefone, sizeof(telefone));

    char senha[100];
    lerEntradaSegura(senha, sizeof(senha), "Digite a senha do professor (minimo 6 caracteres): ");
    senha[strcspn(senha, "\n")] = 0;

    validarSenha(senha, sizeof(senha));

    char confirmacaoSenha[100];
    confirmarSenha(senha, sizeof(confirmacaoSenha));

    char confirmacaoCadastro[4];
    confirmarCadastro();

    char Limpar[10];
    lerEntradaSegura(Limpar, sizeof(Limpar), "Deseja limpar todos os campos? (sim/nao): ");

    if (strcmp(Limpar, "sim") == 0) {
        char *campos[] = {
            nome, ra, dataNascimento, materia, tempoAtuacao,
            periodoTrabalho, inicioContrato, fimContrato,
            email, telefone, senha
        };
        limparCampos(campos, sizeof(campos) / sizeof(campos[0]));
        printf("Todos os campos foram limpos.\n");
    } else {
        printf("Os campos nao foram limpos.\n");
    }

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
        "../dados/professores_cadastros/cadastro_professor.json",
        "../../dados/professores_cadastros/cadastro_professor.json",
        "../../../dados/professores_cadastros/cadastro_professor.json",
        "./dados/professores_cadastros/cadastro_professor.json"
    };
    size_t nc = sizeof(candidatos) / sizeof(candidatos[0]);
    for (size_t i = 0; i < nc && !encontrado; i++) {
        FILE *f = fopen(candidatos[i], "r");
        if (f) {
            fclose(f);
            snprintf(arquivo_path, sizeof(arquivo_path), "%s", candidatos[i]);
            encontrado = 1;
            break;
        }
    }

    if (!encontrado) {
        // Usar pasta padrão professores_cadastros
        const char *padrao_dir = "professores_cadastros";
        const char *padrao = "professores_cadastros/cadastro_professor.json";
        
        // Criar diretório se necessário
#ifdef _WIN32
        _mkdir(padrao_dir);
#else
        mkdir(padrao_dir, 0755);
#endif
        snprintf(arquivo_path, sizeof(arquivo_path), "%s", padrao);
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
        fprintf(arquivo, "    \"ra\": \"%s\",\n", ra);
        fprintf(arquivo, "    \"dataNascimento\": \"%s\",\n", dataNascimento);
        fprintf(arquivo, "    \"materia\": \"%s\",\n", materia);
        fprintf(arquivo, "    \"tempoAtuacao\": \"%s\",\n", tempoAtuacao);
        fprintf(arquivo, "    \"periodoTrabalho\": \"%s\",\n", periodoTrabalho);
        fprintf(arquivo, "    \"inicioContrato\": \"%s\",\n", inicioContrato);
        fprintf(arquivo, "    \"fimContrato\": \"%s\",\n", fimContrato);
        fprintf(arquivo, "    \"email\": \"%s\",\n", email);
        fprintf(arquivo, "    \"telefone\": \"%s\",\n", telefone);
        fprintf(arquivo, "    \"senha\": \"%s\"\n", senha);
        fprintf(arquivo, "  }\n]");
        fclose(arquivo);
        printf("Cadastro realizado com sucesso! Arquivo salvo em: %s\n", arquivo_path);
    } else {
        printf("Erro ao finalizar o cadastro. Nao foi possivel abrir '%s' para escrita.\n", arquivo_path);
    }

    printf("\nPressione qualquer tecla para continuar...");
    system("pause > nul");
    return 0;
}