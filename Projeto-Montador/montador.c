#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX 32


/*VARIAVEIS GLOBAIS*/

/*Mnemonicos*/
char mnemonico[16][4] = { "INC", "DEC", "RET", "HLT", "CMP", "ADD", "SUB", "MOV","JC", "JNC", "JZ", "JNZ", "JBE", "JA", "JMP", "CALL"};
/*struct dos rótulos*/
typedef struct
{
    char nome[MAX], valor[3];
    int EQU;
} identificacao;

identificacao id[MAX];
int r = 0;

// marca a posição
int posicao = 0;

// rotulo valido => caso encontremos algum rotulo nao registrado, essa variável permitirá encerrar a leitura do arquivo
int rotulovalido = 1;
char rotuloinvalido[MAX];

void verificarotulo(char verificar[MAX])
{
    int i, valido = 0;

    for (i = 0; i < r; i++)
        if(strcmp(verificar,(id[i]).nome) == 0)
        {
            strcpy(verificar, (id[i]).valor);
            valido = 1;
        }

    if (!valido)
    {
        rotulovalido = 0;
        strcpy(rotuloinvalido, verificar);
    }
}

/*Rotina que cria tabela de rotulos (struct id)*/
void rotula(char frase[MAX])
{
    int rotulo = 0, i;

    /*Pega a primeira parte da frase contendo conteudo*/
    char * tok = strtok(frase, " \n");

    /*Se o tok for nulo aqui, significa apenas uma linha vazia*/
    if (tok == NULL)
        return;

    /*Se o primeiro tok for algum mnemonico, significa que ele nao eh um rotulo*/
    rotulo = 1;
    for (i = 0; i < 16; i++)
        if(strcmp(tok, mnemonico[i]) == 0)
            rotulo = 0;

    /*Se for rotulo, faz*/
    if(rotulo)    
    {
        /*Salva seu nome em id nome*/
        strcpy((id[r]).nome, tok);

        /*Pega outra parte*/
        tok = strtok(NULL, " ");

        /*Se a proxima parte for um EQU*/
        if (strcmp(tok, "EQU") == 0)
        {
            /*Registra*/
            (id[r]).EQU = 1;
            /*E salva o valor da proxima parte, sem precisar guardar posicao*/
            tok = strtok(NULL, " \n");
            strcpy((id[r++]).valor, tok);
            return;
        }
        else
        {   
            /*Se nao for, salva a posicao do proximo mnemonico e segue para registro de posicao*/
            (id[r]).EQU = 0;
            if (posicao == 0)
                itoa(posicao, (id[r++]).valor, 10);
            else
                itoa(posicao+1, (id[r++]).valor, 10);
        }
	}

    if (posicao != 0)
        posicao++;

    for (i = 0; i < 16; i++)
    {
        if (strcmp(tok, mnemonico[i]) == 0)
        {
            if (i < 4)
            return;

            if (i > 7)
            {
                posicao++;
                return;
            }

            tok = strtok(NULL, " ,[]\n");
            while (tok != NULL)
            {
                if (!((strcmp(tok,"A") == 0) || (strcmp(tok,"B") == 0)))
                {
                    posicao++;
                    return;
                }
                tok = strtok(NULL, " ,[]\n");
            }
            return;
        }

    }
}

/*Rotina que converte o texto em mnemonicos*/
void converte(char frase[MAX], FILE *arquivosaida)
{
    int i, j, sair = 1;

    char * tok = strtok(frase, " \n");

    /*Se o tok for nulo aqui, significa apenas uma linha vazia*/
    if (tok == NULL)
        return;
    
    for (i = 0; i < 16; i++)
        if(strcmp(tok, mnemonico[i]) == 0)
            sair = 0;
    
    for (i = 0; i < r; i++)
        if((strcmp(tok, (id[i]).nome) == 0) && (!(id[i]).EQU))
        {
            sair = 0;
            tok = strtok(NULL, " ");
        }
    
    if(sair)
        return;

    for (i = 0; i < 16; i++)
        if(strcmp(tok,mnemonico[i]) == 0)
            switch(i)
            {
                case 0:
                    //INC
                    tok = strtok(NULL," \n");
                    if (strcmp(tok,"A") == 0)
                        fputs("40h\n", arquivosaida);
                    else
                        fputs("41h", arquivosaida);
                    break;
                case 1:
                    //DEC
                    tok = strtok(NULL," \n");
                    if (strcmp(tok,"A") == 0)
                        fputs("42h\n", arquivosaida);
                    else
                        fputs("43h", arquivosaida);
                    break;
                case 2:
                    //RET
                    fputs("c3h\n", arquivosaida);
                    break;
                case 3:
                    //HLT
                    fputs("f4h", arquivosaida);
                    break;
                case 4:
                    //CMP
                    tok = strtok(NULL, " A,");
                    if(tok[0] == '[')
                    {
                        tok = strtok(tok, "[]\n");
                        
                        if (strcmp(tok,"B") == 0)
                            fputs("3bh\n", arquivosaida);
                        else
                        {
                            verificarotulo(tok);
                            fputs("3ah\n", arquivosaida);
                            fputs(tok, arquivosaida);
                            fputs("\n", arquivosaida);
                        }
                    }
                    else
                    {
                        fputs("3ch\n", arquivosaida);
                        tok = strtok(tok," \n");
                        fputs(tok, arquivosaida);
                        fputs("\n", arquivosaida);
                    }

                    break;
                case 5:
                    //ADD
                    tok = strtok(NULL, " A,");
                    if(tok[0] == '[')
                    {
                        tok = strtok(tok, "[]\n");
                        
                        if (strcmp(tok,"B") == 0)
                            fputs("03h\n", arquivosaida);
                        else
                        {
                            verificarotulo(tok);
                            fputs("02h\n", arquivosaida);
                            fputs(tok, arquivosaida);
                            fputs("\n", arquivosaida);
                        }
                    }
                    else
                    {
                        fputs("04h\n", arquivosaida);
                        tok = strtok(tok," \n");
                        fputs(tok, arquivosaida);
                        fputs("\n", arquivosaida);
                    }
                    break;
                case 6:
                    //SUB
                    tok = strtok(NULL, " A,");
                    if(tok[0] == '[')
                    {
                        tok = strtok(tok, "[]\n");
                        
                        if (strcmp(tok,"B") == 0)
                            fputs("2bh\n", arquivosaida);
                        else
                        {
                            verificarotulo(tok);
                            fputs("2ah\n", arquivosaida);
                            fputs(tok, arquivosaida);
                            fputs("\n", arquivosaida);
                        }
                    }
                    else
                    {
                        fputs("2ch\n", arquivosaida);
                        tok = strtok(tok," \n");
                        fputs(tok, arquivosaida);
                        fputs("\n", arquivosaida);
                    }
                    break;
                case 7:
                    //MOV
                    tok = strtok(NULL, " ,");

                    if (strcmp(tok, "B") == 0)
                    {
                        fputs("88h\n", arquivosaida);
                        break;
                    }

                    if (strcmp(tok, "A") == 0)
                    {
                        tok = strtok(NULL, " ,\n");
                        if(strcmp(tok, "B") == 0)
                        {
                            fputs("8ah\n", arquivosaida);
                            break;
                        }

                        if(tok[0] == '[')
                        {
                            tok = strtok(tok, "[]\n");
                            
                            if (strcmp(tok,"B") == 0)
                            {
                                fputs("a1h\n", arquivosaida);
                                break;
                            }
                            else
                            {
                                verificarotulo(tok);
                                fputs("a0h\n", arquivosaida);
                                fputs(tok, arquivosaida);
                                fputs("\n", arquivosaida);
                                break;
                            }
                        }
                        else
                        {
                            fputs("b0h\n", arquivosaida);
                            tok = strtok(tok," \n");
                            fputs(tok, arquivosaida);
                            fputs("\n", arquivosaida);
                            break;
                        }
                    }

                    tok = strtok(tok, "[]\n");
                    
                    if (strcmp(tok,"B") == 0)
                    {
                        fputs("a3h\n", arquivosaida);
                        break;
                    }
                    else
                    {
                        verificarotulo(tok);
                        fputs("a2h\n", arquivosaida);
                        fputs(tok, arquivosaida);
                        fputs("\n", arquivosaida);
                    }

                    break;
                case 8:
                    //JC
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("72h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 9:
                    //JNC
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("73h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 10:
                    //JZ
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("74h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 11:
                    //JNZ
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("75h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 12:
                    //JBE
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("76h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 13:
                    //JA
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("77h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 14:
                    //JMP
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("ebh\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 15:
                    //CALL
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("e8h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
            }
}

int main()
{
    setlocale(LC_ALL, "portuguese");
    
    /*Variáveis*/
    FILE *arquivo;
    FILE *arquivosaida;
    char texto[MAX]; // variável que vai ler linha do código.
    int i;

    /*Abertura de arquivo somente para leitura, incluso em teste para verificar êxito*/
    if ((arquivo = fopen("codigo.txt", "r")) == NULL)
    {
        printf("Erro na abertura do arquivo do código!");
        exit(1);
    }

    /*Ler linha do cÃ³digo e chamar rotina de criar tabela de rotulos*/
    while(fgets(texto, MAX, arquivo) != NULL)
        rotula(texto);

    /*Fecha arquivo*/
    fclose(arquivo);

    /*Abertura de arquivo somente para leitura, incluso em teste para verificar Ãªxito*/
    if ((arquivo = fopen("codigo.txt", "r")) == NULL)
    {
        printf("Erro na abertura do arquivo do código!");
        exit(1);
    } 

    if((arquivosaida = fopen("codigoconvertido.txt", "w")) == NULL)
    {
        printf("Não foi possível criar o arquivo de saída.");
        exit(1);
    }

    //o arquivo foi encerrado e reaberto para voltar ao seu inÃ­cio
    int linha = 0;
    while ((fgets(texto,MAX,arquivo) !=  NULL) && rotulovalido) 
    {
        linha++;
        converte(texto,arquivosaida);
    }

    if(!rotulovalido)
        printf("Erro na linha %d, rótulo %s", linha, rotuloinvalido);
    else
        printf("Montagem concluída com êxito!");

    /*Fecha arquivo*/
    fclose(arquivo);
    fclose(arquivosaida);

    /*Encerra programa*/
    system("pause>>null"); // pausa programa
    return 0;
}
