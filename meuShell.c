// Grupo: Thiago Mendes - 5959 - T1
//        Gabriel Alves - 5988 - T2

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

void pegar_entrada(char* linha) {
    char c;
    for (int i = 0; i < 512; i++) {
        scanf("%c", &c);
        if (c == '\n') {
            linha[i] = '\0';
            break;
        } else {
            linha[i] = c;
        }
    }
}


void split_entrada (char* linha, char comandos[][512]) {
    int l = 0, inicio = 0, fim = 0;
    bool espaco = false;

    for (int i = 0; i < 512; i++) {
        if (i == 0) {
            inicio = i;
        }

        if (linha[i] == ' ' && espaco == false) {
            fim = i;
            espaco = true;
            int contador = 0;
            for (int j = inicio; j < fim; j++) {
                comandos[l][contador] = linha[j];
                contador++;
            }
            comandos[l][fim] ='\0';
            comandos[l+1][0] ='\0';
            l++;
        } else {
            if (espaco) {
                inicio = i;
            }
            espaco = false;
        }

        if (linha[i] == '\0') {
            int contador = 0;
            fim = i;
            for (int j = inicio; j < fim; j++) {
                comandos[l][contador] = linha[j];
                contador++;
            }
            comandos[l][fim] ='\0';
            comandos[l+1][0] ='\0';
            break;
        }
    }
}

void funcao_imprimir_comandos (char comandos[][512]) {

    int i = 0;
    while (comandos[i][0] != '\0') {
        int j = 0;
        while (comandos[i][j] != '\0') {
            printf("%c", comandos[i][j]);
            j++;
        }
        printf("\n");
        i++;
    }
}



int main(int argc, char const *argv[]) {

    char linha[512];
    char comandos[512][512];

    while (1) {
        printf("meuShell@meuShell-pc:$ > ");
        pegar_entrada(linha);

        split_entrada(linha, comandos);

        funcao_imprimir_comandos(comandos);


        break;
    }

    return 0;
}
