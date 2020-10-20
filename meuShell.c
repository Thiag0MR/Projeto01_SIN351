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
    int contador = 0;
    for (int i = 1; i < 512; i++) {
        char c = linha[i-1];

        if (c == ' ') {
            if (linha[i] != ' ') {
                contador++;
            }
        }

        if (linha[i] == '\0') {
            break;
        }
    }

    return contador;
}
char* analisa_entrada (char* linha) {

}


int main(int argc, char const *argv[]) {

    char linha[512];

    ls -la

    while (1) {
        printf("meuShell@meuShell-pc:$ > ");
        pegar_entrada(linha);
        // analisa_entrada(linha);

        printf("%s\n", linha);

        break;
    }

    return 0;
}
