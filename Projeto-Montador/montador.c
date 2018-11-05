#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MAX 32


/* = VARIÁVEIS GLOBAIS = */

/*Mnemonicos*/
char mnemonico[16][4] = { "INC", "DEC", "RET", "HLT", "CMP", "ADD", "SUB", "MOV","JC", "JNC", "JZ", "JNZ", "JBE", "JA", "JMP", "CALL"};

/*struct dos rótulos*/
typedef struct
{
    char nome[MAX], valor[3];
    int EQU;
} identificacao;
/*Inicialização do struct como id, para salvar todos os rótulos, seus valores e identificar se trata-se de uma diretiva ou não.*/
identificacao id[MAX];
/*r contabiliza a quantidade de rótulos*/
int r = 0;

// Inicia a posição logo no começo do programa, para ser registrada na main e na função de rotulação.
int posicao = 0;

// rotulo valido => caso encontremos algum rotulo nao registrado, essa variável permitirá encerrar a leitura do arquivo
int rotulovalido = 1;
char rotuloinvalido[MAX];

/*Rotina para verificar se o rótulo existe e substituí-lo caso existir.*/
void verificarotulo(char verificar[MAX])
{
    int i, valido = 0;

    for (i = 0; i < r; i++)
        if(strcmp(verificar,(id[i]).nome) == 0) // Verifica se o rótulo está na tabela, substitui e registra a validação
        {
            strcpy(verificar, (id[i]).valor);
            valido = 1;
        }

    if (!valido)    // Quando não registrada a validação, exibe a mensagem de erro ao sair da leitura da linha.
    {
        rotulovalido = 0;
        strcpy(rotuloinvalido, verificar);
    }
}

/*Rotina que cria tabela de rotulos (struct id)*/
void rotula(char frase[MAX])
{
    int rotulo = 0, i;

    /*Pega a primeira parte da frase contendo conteudo e salva em tok*/
    char * tok = strtok(frase, " \n");

    /*Se o tok for nulo aqui, significa apenas uma linha vazia*/
    if (tok == NULL)
        return;

    /*Se o primeiro tok for algum mnemonico, significa que ele nao é um rotulo*/
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
    /*Se for o primeiro mnemonico do programa, posição 0.*/
    if (posicao != 0)
        posicao++;

    for (i = 0; i < 16; i++)
    {
        if (strcmp(tok, mnemonico[i]) == 0)
        {
            /*Os mnemonicos nas posições de 0 a 3 são de nível 1, necessitando registrar apenas 1 posição*/
            if (i < 4)
            return;

            /*Os mnemonicos nas posições de 7 a 15 são de nível 2, necessitando registrar outra posição*/
            if (i > 7)
            {
                posicao++;
                return;
            }

            /*Os outros casos precisam ser analisados: */
            tok = strtok(NULL, " ,[]\n");
            while (tok != NULL)
            {
                /*Se em algum momento eles não forem A, B ou [B], significa que são nível 2, registrando outra posição*/
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
    
    /*Iniciamos verificando se trata-se de um mnemonico, uma diretiva ou um rótulo*/

    // Se for um mnemonico, esse for resolve.
    for (i = 0; i < 16; i++)
        if(strcmp(tok, mnemonico[i]) == 0)
            sair = 0;
    
    // Se não for uma diretiva, esse outro for que analisa os rótulos, resolve e pula a parte com o nome do rótulo
    for (i = 0; i < r; i++)
        if((strcmp(tok, (id[i]).nome) == 0) && (!(id[i]).EQU))
        {
            sair = 0;
            tok = strtok(NULL, " ");
        }
    
    // Se até aqui a variável sair ainda não estiver resetada, significa que trata-se de uma diretiva e, portanto, não registra opcode na saída.
    if(sair)
        return;

    /*Inicia a análise do mnemonico*/
    for (i = 0; i < 16; i++)
        if(strcmp(tok,mnemonico[i]) == 0)
            /*Como os mnemonicos estão registrados num vetor, um switch contendo sua posição correspondente resolve*/
            switch(i)
            {
                case 0:
                    //INC => verifica se incrementa A ou B e registra opcode.
                    tok = strtok(NULL," \n");
                    if (strcmp(tok,"A") == 0)
                        fputs("40h\n", arquivosaida);
                    else
                        fputs("41h", arquivosaida);
                    break;
                case 1:
                    //DEC => verifica se decrementa A ou B e registra opcode.
                    tok = strtok(NULL," \n");
                    if (strcmp(tok,"A") == 0)
                        fputs("42h\n", arquivosaida);
                    else
                        fputs("43h", arquivosaida);
                    break;
                case 2:
                    //RET => registra opcode
                    fputs("c3h\n", arquivosaida);
                    break;
                case 3:
                    //HLT => registra opcode
                    fputs("f4h", arquivosaida);
                    break;
                case 4:
                    //CMP => Verifica qual CMP se trata e registra opcode.
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
                    //ADD => Verifica qual ADD se trata e registra opcode.
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
                    //SUB => Verifica qual SUB se trata e registra opcode.
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
                    //MOV => Aparentemente o mais complicado, possui mais verificações por poder fazer alterações no B, mas após as verificações, registra opcodes.
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
                    //JC => Registra opcode e substitui rótulo
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("72h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 9:
                    //JNC => Registra opcode e substitui rótulo
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("73h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 10:
                    //JZ => Registra opcode e substitui rótulo
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("74h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 11:
                    //JNZ => Registra opcode e substitui rótulo
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("75h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 12:
                    //JBE => Registra opcode e substitui rótulo
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("76h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 13:
                    //JA => Registra opcode e substitui rótulo
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("77h\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 14:
                    //JMP => Registra opcode e substitui rótulo
                    tok = strtok(NULL, " \n");
                    verificarotulo(tok);
                    fputs("ebh\n", arquivosaida);
                    fputs(tok, arquivosaida);
                    fputs("\n", arquivosaida);
                    break;
                case 15:
                    //CALL => Registra opcode e substitui rótulo
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
    
    /*Variáveis para criação de arquivos*/
    FILE *arquivo;
    FILE *arquivosaida;

    // variável que vai ler linha do código.
    char texto[MAX];
    int i;

    /*Abertura de arquivo somente para leitura, incluso em teste para verificar êxito*/
    if ((arquivo = fopen("codigo.txt", "r")) == NULL)
    {
        printf("Erro na abertura do arquivo do código!");
        exit(1);
    }

    /*Ler linha do código e chamar rotina de criar tabela de rotulos*/
    while(fgets(texto, MAX, arquivo) != NULL)
        rotula(texto);

    /*Fecha arquivo*/
    fclose(arquivo);

    /*Abertura de arquivo somente para leitura, incluso em teste para verificar êxito*/
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

    //o arquivo foi encerrado e reaberto para voltar ao seu início
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
