# Projeto_SIN351

#### Grupo: 
* Thiago Mendes - 5959 - T1
* Gabriel Alves - 5988 - T2
       
       
#### Objetivo:
O projeto consiste em desenvolver um interpretador de comandos para a disciplina de Sistemas Operacionais - SIN351


#### Características do shell:
* Não suporta expansão de nome de arquivos (globbing), redirecionamento ('<', '<<', '>', '>>', ...)
* Única forma de concatenar comandos é através de pipes (comando utilizado é a ',')
* Interpreta argumentos entre aspas como um único argumento
* Não faz distinção entre aspas duplas ou simples 
* Se existir somente uma aspas, ele exerga tudo da 1ª até o final como algo único
* Para sair digitar o comando 'quit' ou uma entrada que contenha o comando 'quit' (Ex: 'echo 111111 , quit')

#### Compilando o projeto:

```bash
make meuShell
```

#### Executando:

```bash
./meuShell
```

#### Link repositório: [https://github.com/Thiag0MR/Projeto_SIN351](https://github.com/Thiag0MR/Projeto_SIN351) 