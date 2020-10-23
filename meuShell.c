// Grupo: Thiago Mendes - 5959 - T1
//        Gabriel Alves - 5988 - T2

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define TAM_ENTRADA 512
#define p_char char*

void pegar_entrada(p_char entrada) {

    printf("meuShell@meuShell-pc:$ > ");

    char c;
    for (int i = 0; i < TAM_ENTRADA; i++) {
        scanf("%c", &c);
        if (c == '\n') {
            entrada[i] = '\0';
            break;
        } else {
            entrada[i] = c;
        }
    }
}


p_char** split_entrada (p_char entrada, int* qtd_pipes, int* qtd_espaco_comandos, int* qtd_espaco_args) {

    p_char** lista_args;

    //Delimita o inicio e fim de um argumento
    int inicio = 0, fim = 0;

    //Usado para ignorar espaço e pipes
    bool espacoOuPipe = false;

    int qual_comando = 0;
    int qual_argumento = 0;

    //Aloca espaço inicial para "qtd_espaco_comandos" comandos
    lista_args = (p_char**) malloc (*qtd_espaco_comandos * sizeof(p_char*));
    if (lista_args == NULL) {
        printf("Erro na alocação\n");
        exit (EXIT_FAILURE);
    }

    //Aloca espaço inicial para  "qtd_espaco_args" argumentos
    for (int i = 0; i < (*qtd_espaco_comandos); i++) {
        lista_args[i] = (p_char*) malloc ((*qtd_espaco_args) * sizeof(p_char));
        if (lista_args[i] == NULL) {
            printf("Erro na alocação\n");
            exit (EXIT_FAILURE);
        }
    }

    //Percorre a entrada
    for (int i = 0; i < TAM_ENTRADA; i++) {
        if (i == 0) {
            inicio = i;
        }

        //Entra se hover um espaço depois de um caracter válido
        if ((entrada[i] == ' ')  && espacoOuPipe == false) {
            fim = i;
            espacoOuPipe = true;
            int contador = 0;
            lista_args[qual_comando][qual_argumento] = (char*) malloc (sizeof(char) * ((fim - inicio) + 1));
            if (lista_args[qual_comando][qual_argumento] == NULL) {
                printf("Erro na alocação\n");
                exit (EXIT_FAILURE);
            }
            for (int j = inicio; j < fim; j++) {
                lista_args[qual_comando][qual_argumento][contador++] = entrada[j];
            }
            lista_args[qual_comando][qual_argumento++][contador] = '\0';

            if (qual_argumento == *qtd_espaco_args) {
                lista_args[qual_comando] = realloc (lista_args[qual_comando], sizeof(p_char) * (*qtd_espaco_args << 1));
                if (lista_args[qual_comando] == NULL) {
                    printf("Erro na realocação\n");
                    exit (EXIT_FAILURE);
                }
                *qtd_espaco_args <<= 1;
            }
            lista_args[qual_comando][qual_argumento] = NULL;
        } else {
            if (espacoOuPipe) {
                inicio = i;
                if (entrada[i] == '|') {
                    (*qtd_pipes)++;
                    qual_comando++;
                    qual_argumento = 0;
                    if (qual_comando == *qtd_espaco_comandos) {
                        lista_args = realloc (lista_args, sizeof(p_char*) * (*qtd_espaco_comandos << 1));
                        if (lista_args == NULL) {
                            printf("Erro na realocação\n");
                            exit (EXIT_FAILURE);
                        }
                        *qtd_espaco_comandos <<= 1;
                        //Aloca espaço para os novos espaços criados
                        for (int j = qual_comando; j < (*qtd_espaco_comandos); j++) {
                            lista_args[j] = (p_char*) malloc ((*qtd_espaco_args) * sizeof(p_char));
                            if (lista_args[j] == NULL) {
                                printf("Erro na alocação\n");
                                exit (EXIT_FAILURE);
                            }
                        }
                    }
                    espacoOuPipe = true;
                } else {
                    if (entrada[i] == ' ') {
                        espacoOuPipe = true;
                    } else {
                        espacoOuPipe = false;
                    }
                }
            }
        }

        if (entrada[i] == '\0') {
            int contador = 0;
            fim = i;
            lista_args[qual_comando][qual_argumento] = (char*) malloc (sizeof(char) * ((fim - inicio) + 1));
            for (int j = inicio; j < fim; j++) {
                lista_args[qual_comando][qual_argumento][contador++] = entrada[j];
            }
            lista_args[qual_comando][qual_argumento++][contador] = '\0';

            if (qual_argumento == *qtd_espaco_args) {
                lista_args[qual_comando] = realloc (lista_args[qual_comando], sizeof(p_char) * (*qtd_espaco_args << 1));
                if (lista_args[qual_comando] == NULL) {
                    printf("Erro na realocação\n");
                    exit (EXIT_FAILURE);
                }
                *qtd_espaco_args <<= 1;
            }
            lista_args[qual_comando][qual_argumento] = NULL;

            break;
        }
    }
    return lista_args;
}


void funcao_imprimir_lista_args (p_char** lista_args, int qtd_espaco_comandos) {

    int i = 0;
    //Percorre os comandos
    while (i < qtd_espaco_comandos) {
        int j = 0;
        printf("%dº comando: ", i + 1);
        //Percorre os argumentos
        while (lista_args[i][j] != NULL) {
            int l = 0;
            //Percorre os caracteres
            while (lista_args[i][j][l] != '\0') {
                printf("%c", lista_args[i][j][l]);
                l++;
            }
            printf(" ");
            j++;
        }
        printf("\n");
        i++;
    }
}

void liberar_lista_args (p_char** lista_args, int* qtd_espaco_comandos) {

    int i = 0;

    while (i < (*qtd_espaco_comandos)) {
        int j = 0;
        while (lista_args[i][j] != NULL) {
            free (lista_args[i][j]);
            j++;
        }
        free (lista_args[i]);
        i++;
    }
    free (lista_args);
}


int main() {

    char entrada[TAM_ENTRADA];
    p_char** lista_args;
    int qtd_espaco_comandos = 1;
    int qtd_espaco_args = 1;
    pid_t pid;


    while (1) {
        int qtd_pipes = 0;

        pegar_entrada(entrada);

        lista_args = split_entrada(entrada, &qtd_pipes, &qtd_espaco_comandos, &qtd_espaco_args);
        funcao_imprimir_lista_args(lista_args, qtd_espaco_comandos);


        liberar_lista_args (lista_args, &qtd_espaco_comandos);
    }


    return 0;
}
