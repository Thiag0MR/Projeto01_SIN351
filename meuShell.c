// Grupo: Thiago Mendes -  5959  -  T1
//        Gabriel Alves -  5988  -  T2

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define TAM_ENTRADA 512
#define PIPE ','
#define p_char char*

int pegar_entrada(char* entrada) {

    // Para imprimir colocorido
    printf("\033[1;32m");
    printf("meuShell@meuShell-pc:$ > ");
    // Volta a cor default
    printf("\033[0m");

    char c;
    for (int i = 0; i < 1024; i++) {
        if (i < 512) {
            scanf("%c", &c);
            if (c == '\n') {
                entrada[i] = '\0';
                return 1;
            } else {
                entrada[i] = c;
            }
        } else {
            printf("Erro: Entrada com mais de 512 caracteres\n");
            __fpurge(stdin);
            break;
        }
    }
    return 0;
}
// Retorna 1 se a entrada conter um caractere válido ou 0 se a entrada for vazia ou conter somente espaços
int verificar_entrada (char* entrada) {

    for (int i = 0; i < TAM_ENTRADA; i++) {
        if (entrada[i] == '\0') {
            break;
        } else {
            if (entrada[i] != ' ') {
                return 1;
            }
        }
    }
    return 0;
}

// Retira espaços inúteis
void retirar_espacos (char* entrada) {
    int j = 0;
    bool proxEspaco = false;
    bool entreAspas = false;

    for (int i = 0; i < TAM_ENTRADA; i++) {
         if (entrada[i] == '\'' || entrada[i] == '\"') {
            if (entreAspas == false) {
                entreAspas = true;
            } else {
                entreAspas = false;
            }
            if (i != j) {
                entrada[j] = entrada[i];
            }
            j++;
            proxEspaco = true; 
        } else {
            if (entreAspas == false) {
                if (entrada[i] != ' ') {
                    if (i != j) {
                        entrada[j] = entrada[i];
                    }
                    if (entrada[i] == '\0') {
                        break;
                    } 
                    j++;
                    proxEspaco = true;
                } else {
                    if (proxEspaco && entrada[i] == ' ') {
                        entrada[j++] = ' ';
                        proxEspaco = false;
                    }
                }
            } else {
                if (i != j) {
                    entrada[j] = entrada[i];
                }
                if (entrada[i] == '\0') {
                    break;
                } 
                j++;
                proxEspaco = true; 
            }
        }
    }
    if (entrada[j - 1] == ' ') {
        entrada[j - 1] = '\0';
    }
}

p_char** alocar_memoria (int* qtd_espaco_comandos, int* qtd_espaco_args) {
    //Aloca espaço inicial para "qtd_espaco_comandos" comandos
    p_char** tab_comandos = (p_char**) calloc (*qtd_espaco_comandos , sizeof(p_char*));
    if (tab_comandos == NULL) {
        printf("Erro na alocação\n");
        exit (EXIT_FAILURE);
    }

    //Aloca espaço inicial para  "qtd_espaco_args" argumentos
    for (int i = 0; i < (*qtd_espaco_comandos); i++) {
        tab_comandos[i] = (p_char*) calloc ((*qtd_espaco_args) , sizeof(p_char));
        if (tab_comandos[i] == NULL) {
            printf("Erro na alocação\n");
            exit (EXIT_FAILURE);
        }
    }

    return tab_comandos;
}

p_char** alocar_memoria (int* qtd_espaco_comandos, int* qtd_espaco_args) {
    //Aloca espaço inicial para "qtd_espaco_comandos" comandos
    p_char** tab_comandos = (p_char**) calloc (*qtd_espaco_comandos , sizeof(p_char*));
    if (tab_comandos == NULL) {
        printf("Erro na alocação\n");
        exit (EXIT_FAILURE);
    }

    //Aloca espaço inicial para  "qtd_espaco_args" argumentos
    for (int i = 0; i < (*qtd_espaco_comandos); i++) {
        tab_comandos[i] = (p_char*) calloc ((*qtd_espaco_args) , sizeof(p_char));
        if (tab_comandos[i] == NULL) {
            printf("Erro na alocação\n");
            exit (EXIT_FAILURE);
        }
    }

    return tab_comandos;
}

// Escreve a string delimitada pelas variáveis inicio e fim
int escrever_string (p_char** tab_comandos, char* entrada, int inicio, int fim, int qual_comando, int qual_argumento, int* qtd_espaco_args) {

    tab_comandos[qual_comando][qual_argumento] = (char*) malloc (sizeof(char) * ((fim - inicio) + 1));
    if (tab_comandos[qual_comando][qual_argumento] == NULL) {
        printf("Erro na alocação\n");
        exit (EXIT_FAILURE);
    }
    int contador = 0;
    for (int j = inicio; j < fim; j++) {
        tab_comandos[qual_comando][qual_argumento][contador++] = entrada[j];
    }
    tab_comandos[qual_comando][qual_argumento++][contador] = '\0';

    if (qual_argumento == *qtd_espaco_args) {
        tab_comandos[qual_comando] = realloc (tab_comandos[qual_comando], sizeof(p_char) * (*qtd_espaco_args << 1));
        if (tab_comandos[qual_comando] == NULL) {
            printf("Erro na realocação\n");
            exit (EXIT_FAILURE);
        }
        *qtd_espaco_args <<= 1;
    }
    tab_comandos[qual_comando][qual_argumento] = NULL;

    return qual_argumento;
}

p_char** split_entrada (p_char entrada, int* qtd_pipes, int* qtd_espaco_comandos, int* qtd_espaco_args) {


    //Delimita o inicio e fim de um argumento
    int inicio = 0, fim = 0;

    bool aspas = false;

    int qual_comando = 0;
    int qual_argumento = 0;

    p_char** tab_comandos = alocar_memoria (qtd_espaco_comandos, qtd_espaco_args);

    retirar_espacos (entrada);

    //Percorre a entrada
    for (int i = 0; i < TAM_ENTRADA; i++) {
        if (entrada[i] == '\'' || entrada[i] == '\"') {
            if (aspas == false) {
                aspas = true;
                inicio = i + 1;
            } else {
                aspas = false;
            }
        } else {
            if (aspas == false) {
                if (entrada[i] == ' ') {
                    if (entrada[i - 1] != PIPE) {
                        if (entrada[i - 1] == '\'' || entrada[i - 1] == '\"') {
                            fim = i - 1;
                        } else {
                            fim = i;
                        }
                        qual_argumento = escrever_string (tab_comandos, entrada, inicio, fim, qual_comando, qual_argumento, qtd_espaco_args);
                        inicio = i + 1;
                    }
                } else {
                    if (entrada[i] == PIPE) {
                        (*qtd_pipes)++;
                        qual_comando++;
                        qual_argumento = 0;
                        inicio = i + 2;
                        if (qual_comando == *qtd_espaco_comandos) {
                            tab_comandos = realloc (tab_comandos, sizeof(p_char*) * (*qtd_espaco_comandos << 1));
                            if (tab_comandos == NULL) {
                                printf("Erro na realocação\n");
                                exit (EXIT_FAILURE);
                            }
                            *qtd_espaco_comandos <<= 1;
                            //Aloca espaço para os novos espaços criados
                            for (int j = qual_comando; j < (*qtd_espaco_comandos); j++) {
                                tab_comandos[j] = (p_char*) calloc ((*qtd_espaco_args) , sizeof(p_char));
                                if (tab_comandos[j] == NULL) {
                                    printf("Erro na alocação\n");
                                    exit (EXIT_FAILURE);
                                }
                            }
                        }
                    }
                }
            }

            if (entrada[i] == '\0') {
                if (entrada[i - 1] == '\'' || entrada[i - 1] == '\"') {
                    fim = i - 1;
                } else {
                    fim = i;
                }
                qual_argumento = escrever_string (tab_comandos, entrada, inicio, fim, qual_comando, qual_argumento, qtd_espaco_args);
                break;
            }
        }
    }
    return tab_comandos;
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
