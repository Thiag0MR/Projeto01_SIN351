// Grupo: Thiago Mendes - 5959 - T1
//        Gabriel Alves - 5988 - T2

#include <stdio.h>
#include <unistd.h>

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

int contar_argumentos (linha) {


}

void analisa_entrada (char* linha, char* comandos) {

    comandos[0][0] = linha[0];

    int k = 0;

    for (int i = 1; i < 512; i++) {
        char c = linha[i-1];

        if (c == ' ') {
            if (linha[i] != ' ') {
                j++;
                comandos[j][i] = linha[i];
            }
        } else {
            comandos[j][i] = linha[i];
        }

        if (linha[i] == '\0') {
            break;
        }
    }
}

ls -la

int main(int argc, char const *argv[]) {

    char linha[512];
    char comandos[512][512];

    while (1) {
        printf("meuShell@meuShell-pc:$ > ");
        pegar_entrada(linha);
        analisa_entrada(linha, comandos);


        printf("%s\n", linha);

        break;
    }

    return 0;
}
