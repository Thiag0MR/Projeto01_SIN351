// Grupo: Thiago Mendes -  5959  -  T1
//        Gabriel Alves -  5988  -  T2

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio_ext.h>

#define TAM_ENTRADA 512
#define PIPE ','
#define p_char char*

// Retorna 1 se a entrada é maior que 512 caracteres e 0 caso contrário
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
// Retorna 1 se a entrada possuir um caractere válido ou 0 se a entrada for vazia ou possuir somente espaços
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


// Verifica se a entrada contém o comando 'quit', caso verdadeiro o retorno é 1, caso contrário o retorno é 0
// Qualquer entrada que contenha a string 'quit' é uma forma válida de encerra o shell 
int verificar_comando_saida (p_char** tab_comandos, int qtd_comandos) {
    for (int i = 0; i < qtd_comandos; i++) {
        if (strcmp (tab_comandos[i][0], "quit") == 0) {
            return 1;
        }
    }
    return 0;
}

// Libera a entrutura tab_comandos, a qual foi criada dinâmicamente para armazenar os comandos e seus respectivos argumentos
void liberar_tab_comandos (p_char** tab_comandos, int* qtd_espaco_comandos) {

    int i = 0;

    while (i < (*qtd_espaco_comandos)) {
        int j = 0;
        while (tab_comandos[i][j] != NULL) {
            free (tab_comandos[i][j]);
            j++;
        }
        free (tab_comandos[i]);
        i++;
    }
    free (tab_comandos);
}

// Retira espaços inúteis que podem eventualmente existir na entrada
// Necessário pois a lógica da função split_entrada ficaria mais complexa
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
    p_char** tab_comandos = (p_char**) calloc ((*qtd_espaco_comandos) , sizeof(p_char*));
    if (tab_comandos == NULL) {
        printf("Erro na alocação\n");
        exit (EXIT_FAILURE);
    }

    //Aloca espaço inicial para "qtd_espaco_args" argumentos
    for (int i = 0; i < (*qtd_espaco_comandos); i++) {
        tab_comandos[i] = (p_char*) calloc ((*qtd_espaco_args) , sizeof(p_char));
        if (tab_comandos[i] == NULL) {
            printf("Erro na alocação\n");
            exit (EXIT_FAILURE);
        }
    }

    return tab_comandos;
}

// Escreve a string delimitada pelas variáveis inicio e fim; o retorno é 
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

    // Inicialmente foi alocado espaço para 'qtd_espaco_args' argumentos, se o comando tiver mais argumentos é feito uma realocação de memória
    // com o dobro de espaços inicialmente alocado
    if (qual_argumento == *qtd_espaco_args) {
        tab_comandos[qual_comando] = (p_char*) realloc (tab_comandos[qual_comando], sizeof(p_char) * (*qtd_espaco_args << 1));
        if (tab_comandos[qual_comando] == NULL) {
            printf("Erro na realocação\n");
            exit (EXIT_FAILURE);
        }
        *qtd_espaco_args <<= 1;
    }
    tab_comandos[qual_comando][qual_argumento] = NULL;

    return qual_argumento;
}

// Divide os comandos e seus respectivos argumentos, armazenando os mesmos na estrutura tab_comandos; também verifica possível
// situação de erro, como qtd_pipes incorreta 
p_char** split_entrada (p_char entrada, int* qtd_pipes, int* qtd_espaco_comandos, int* qtd_espaco_args, int* qtd_comandos) {


    //Delimita o inicio e fim de um argumento
    int inicio = 0, fim = 0;

    bool aspas = false;

    int qual_comando = 0;
    int qual_argumento = 0;
    // Quando o bloco de memoria é realocado para um respectivo comando, a variável qtd_espaco_args é incrementada para a nova qtd de espaços vazios, logo é preciso 
    // armazenar a qtd de blocos vazios que foi inicialmente alocada para os outros comandos para que a variável qtd_espaco_args corresponda a qtd correta de espaço vazios 
    int qtd_espaco_args_inicial = *qtd_espaco_args;

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
                        inicio = i + 2;

                        // Vai verificar se tab_comandos[qual_comando][0] possui um comando válido antes de incrementar a variável qual_comando
                        // Necessário para verificar se o comando antes do PIPE é um comando válido, pois pode acontecer da entrada começar com PIPE
                        if (tab_comandos[qual_comando][0] != NULL) {
                            qual_comando++;
                            (*qtd_comandos)++;
                            qual_argumento = 0;
                            *qtd_espaco_args = qtd_espaco_args_inicial;
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
            }

            if (entrada[i] == '\0') {
                if (entrada[i - 1] == '\'' || entrada[i - 1] == '\"') {
                    fim = i - 1;
                } else {
                    fim = i;
                }
                if (fim > inicio) {
                    qual_argumento = escrever_string (tab_comandos, entrada, inicio, fim, qual_comando, qual_argumento, qtd_espaco_args);
                    (*qtd_comandos)++;
                }
                // Verifica se a quantidade de pipes faz sentido, pois o usuário pode entrar com uma entrada inválida com vários pipes
                if ((*qtd_pipes + 1) == *qtd_comandos) {
                    break;
                } else {
                    liberar_tab_comandos (tab_comandos, qtd_espaco_comandos);
                    return NULL;
                }
            }
        }
    }
    return tab_comandos;
}

void executar_comando (p_char** tab_comandos, int qtd_pipes) {
    int qtd_comandos = qtd_pipes + 1;

    if (qtd_comandos == 1) {

        pid_t pid;

        pid = fork();

        // Fork retorna 0 p/ o filho
        if (pid == 0) {

            if (execvp(tab_comandos[0][0], tab_comandos[0]) == -1) {
                // perror("Erro execvp");

                // perror mostra uma mensagem de erro referente a última chamada de sistema (nesse caso execvp)
                // de acordo com o valor da variável errno
                perror(tab_comandos[0][0]);
            }
            exit(EXIT_FAILURE);

        } else if (pid < 0) {
            // Fork retorna -1 em caso de erro
            perror("Erro fork");
        } else {
            // Fork retorna o id do processo filho no processo pai em caso de sucesso
            // Pai entra aqui e espera pelo filho
            waitpid(-1, NULL, 0);
        }
    } else if (qtd_comandos >= 2) {

        int** fd;
        pid_t pid;

        // Cria uma matriz de inteiros com "qtd_pipes" linhas e 2 colunas
        // C/ linha representa a leitura fd[linha][0] e escrita fd[linha][1] de um pipe
        fd = (int**) malloc (sizeof(int*) * qtd_pipes);
        if (fd == NULL) {
            printf("Erro na alocação\n");
            exit (EXIT_FAILURE);
        }
        for (int i = 0; i < qtd_pipes; i++) {
            fd[i] = (int*) malloc(sizeof(int) * 2);
            if (fd[i] == NULL) {
                printf("Erro na alocação\n");
                exit (EXIT_FAILURE);
            }
        }

        // Cria efetivamente o pipe
        for (int i = 0; i < qtd_pipes; i++) {
            if( pipe(fd[i]) < 0) {
                perror("Erro pipe") ;
                return;
            }
        }

        // Cria um processo filho para c/ comando
        for (int i = 0; i < qtd_comandos; i++) {
            pid = fork();

            if (pid == 0) {
                if (i == 0) {
                    // Se for o  primeiro filho
                    dup2(fd[i][1], STDOUT_FILENO);
                } else if (i != 0 && i != (qtd_comandos - 1)) {
                    // Entre o 1º e o último
                    dup2(fd[i - 1][0], STDIN_FILENO);
                    dup2(fd[i][1], STDOUT_FILENO);
                } else {
                    // Se for o último filho
                    dup2(fd[i - 1][0], STDIN_FILENO);
                }

                //Fecha todos os descritores
                for (int i = 0; i < qtd_pipes; i++) {
                    for (int j = 0; j < 2; j++) {
                        close(fd[i][j]);
                    }
                }

                // Carrega o executável na memória do processo filho
                if (execvp(tab_comandos[i][0], tab_comandos[i]) == -1) {
                    // perror("Erro execvp");
                    perror(tab_comandos[i][0]);
                }
                exit(EXIT_FAILURE);
            } else if (pid < 0) {
                perror("Erro fork");
            }
        }

        // O pai fecha todos os descritores, pois só assim o filho vai saber quando parar de ler
        for (int i = 0; i < qtd_pipes; i++) {
            for (int j = 0; j < 2; j++) {
                close(fd[i][j]);
            }
        }

        // O pai espera por todos os filhos terminarem
        for (int i = 0; i < qtd_comandos; i++) {
            waitpid(-1, NULL, 0);
        }

        // Desaloca a memória alocada para os descritores
        for (int i = 0; i < qtd_pipes; i++) {
            free(fd[i]);
        }
        free(fd);
    }
}

void imprimir_tab_comandos (p_char** tab_comandos, int qtd_espaco_comandos) {

    int i = 0;
    //Percorre os comandos
    while (i < qtd_espaco_comandos) {
        int j = 0;
        printf("tab_comando[%d]: ", i);
        //Percorre os argumentos
        while (tab_comandos[i][j] != NULL) {
            int l = 0;
            printf("tab_comando[%d][%d]: ", i, j);
            
            //Percorre os caracteres
            while (tab_comandos[i][j][l] != '\0') {
                printf("%c", tab_comandos[i][j][l]);
                l++;
            }
            printf("      ");
            j++;
        }
        printf("\n");
        i++;
    }
}


// Características do shell:
// Não suporta expansão de nome de arquivos (globbing), redirecionamento ('<', '<<', '>', '>>', ...)
// Única forma de concatenar comandos é através de pipes (comando utilizado é a ',')
// Interpreta argumentos entre aspas como um único argumento
// Não faz distinção entre aspas duplas ou simples 
// Se existir somente uma aspas, ele exerga tudo da 1ª até o final como algo único
// Para sair digitar o comando 'quit' ou uma entrada que contenha o comando 'quit' (Ex: 'echo 111111 , quit')


int main() {

    char entrada[TAM_ENTRADA];

    while (1) {
        int qtd_pipes = 0;
        int qtd_comandos = 0;
        int qtd_espaco_comandos = 4;
        int qtd_espaco_args = 4;

        // Estrutura usada para armazenar os comandos
        // tab_comandos[0][0] acessa o 1º arg do 1º comando (próprio comando), tab_comandos[0][1] acessa o 2º arg do 1º comando ...
        p_char** tab_comandos;

        if (pegar_entrada(entrada)) {
            if (verificar_entrada(entrada)) {
                tab_comandos = split_entrada(entrada, &qtd_pipes, &qtd_espaco_comandos, &qtd_espaco_args, &qtd_comandos);

                // if (tab_comandos != NULL) {
                //     imprimir_tab_comandos(tab_comandos, qtd_espaco_comandos);
                // }
                
                if (tab_comandos == NULL) {
                    printf("meuShell: erro de sintaxe próximo ao token %c\n", PIPE);
                } else {
                    if (verificar_comando_saida(tab_comandos, qtd_comandos)) {
                        liberar_tab_comandos (tab_comandos, &qtd_espaco_comandos);
                        break;
                    } else {
                        executar_comando (tab_comandos, qtd_pipes);
                    }
                    liberar_tab_comandos (tab_comandos, &qtd_espaco_comandos);
                }
            }
        }
    }

    return 0;
}
