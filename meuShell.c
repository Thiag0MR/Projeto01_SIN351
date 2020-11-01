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
