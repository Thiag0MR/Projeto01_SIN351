// Grupo: Thiago Mendes - 5959 - T1
//        Gabriel Alves - 5988 - T2

#include <stdio.h>
#include <unistd.h>

void pegar_entrada(char* linha) {
    char c;
    int i =0;
    for ( i ; i < 512; i++) {
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

void analisa_entrada (char* linha, char comandos[512][512]) {

	int i = 1;
    int k = 0;
    int final = 0;
    int comeco = 0;
	int col = 0;
    int lin = 0;
    char c = 0;
    int espaco = 0;
    
    for ( i ; i < 512; i++) {
        c = linha[i-1];
		final = i;
        if (c == ' ') {
        	espaco++;
            if (linha[i] != ' ') {
            	espaco ++;
            	final -= (espaco + 1);
            	
            	
            	for(col ; col < 10;){
            		for(lin ; lin < final; lin++){
            			comandos[col][lin]= linha[comeco];
            			comeco++;
					}
				}
				col++;
				comeco += (final + (espaco + 1));
				
                
            }else if (linha[i] == '\0'){
            	break;
			}
    	} 
	}


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
