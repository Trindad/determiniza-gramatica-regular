/**
*Copyright {2014} {Silvana Trindade,Maurício André Cinelli}
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define TAM_CARACTERE 80
#define TAM_ALFABETO 256
#define TAM_OPCOES 100
#define TAM_PRODUCOES 210
/* estruturas */

typedef struct __opcao
{
	char producao[TAM_PRODUCOES];
	short int ehEpsilon;
} Opcao;

typedef struct __estado
{
	char identificador[TAM_CARACTERE]; //caractere
	int nOpcoes;
	Opcao *opcoes[TAM_OPCOES];
	short int ehFinal;
} Estado;

typedef struct __gramatica
{
	char alfabeto[TAM_ALFABETO];
	int numEstados;
	Estado *estados[TAM_OPCOES];
} Gramatica;

/**
 * vetor de caracteres permitidos para o estado simbolo
 */

char caracteres[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','Y','W','Z'} ;
/**
 * funções de inicialização e verificação da gramatica
 */
Gramatica *leGramatica();
int pertenceAlfabeto(char *producao, char *alfabeto);
void leAlfabeto(Gramatica *gramatica);
int ehRegular(Gramatica *gramatica);
int ehIndeterministica(Gramatica *gramatica);
char *agrupaOpcoesPorSimbolo(Estado *estado,char caractere);
Estado *mesclaEstados(Gramatica *gramatica, Gramatica *novaGramatica, char *producao);
void imprimeGramatica(Gramatica *gramatica);

Estado * buscaEstadoPorIdentificador(Gramatica *gramatica, char *estado);

/**
 * Funções para determinização da gramatica
 */
Gramatica *gramaticaDetermizada(Gramatica *gramatica);


/**
 * funções
 */
int main(int argc, char *argv[]) {

	Gramatica *gramatica = leGramatica();
	imprimeGramatica(gramatica);printf("\n\n\n\n");

	int regular = ehRegular(gramatica);

	if (regular == 0)
	{
		printf("Esta gramática não é regular\n");
		exit(1);
	}

	int indeterministica = ehIndeterministica(gramatica);

	if (indeterministica == 0)
	{
		printf("Esta gramática não é indeterministica então não será necessário determiniza-la.\n");
		exit(1);
	}

	Gramatica *novaGramatica = gramaticaDetermizada(gramatica);
	imprimeGramatica(novaGramatica);
	novaGramatica->numEstados += 0;

	free(gramatica);
	free(novaGramatica);
	return 0;
}

/**
 * Le gramática
 * retorna vetor do tipo Estado 
 */
Gramatica *leGramatica(){


	Gramatica *gramatica = (Gramatica *) malloc (sizeof(Gramatica));


	if ( gramatica == NULL )
	{
		printf("Erro na Alocação de memória\n");
		return NULL;
	}

	leAlfabeto(gramatica);

	if (gramatica->alfabeto == NULL)
	{
		printf("Você não digitou o alfabeto.\n");
		exit(1);
	}


	gramatica->numEstados = 0;
	gramatica->estados[gramatica->numEstados] = (Estado *) malloc (sizeof(Estado));

	if (gramatica->estados[gramatica->numEstados] == NULL)
	{
		printf("Erro ao alocar memória\n");
		exit(1);
	}

	gramatica->estados[gramatica->numEstados]->nOpcoes = 0;

	char temporario[TAM_ALFABETO];


	while(fgets(temporario,TAM_ALFABETO,stdin) != NULL) {

		if (strcmp(temporario,"$$") == 0)
		{
			break;
		}

		/**
		 * identifica linha digitada pelo usuário
		 */
		char auxiliar[TAM_ALFABETO*2];
		strcpy(auxiliar,temporario);
		//printf("auxiliar: %s \ntemporario:%s\n",auxiliar,temporario );
		char *token = strtok(auxiliar,":|");
		int count = 0;
		
		while(token) {

			char auxToken[200];
			strcpy(auxToken, token);

			if (strcmp(auxToken,"X") == 0)
			{
				printf("Você utilizou um simbolo não-terminal reservado\n");
				exit(1);
			}

			printf("auxToken:%s\n", auxToken);

			int tamanho = 0;

			tamanho = strlen(auxToken);

			if (auxToken[tamanho-1] == '\n')
			{
				auxToken[tamanho-1] = '\0';
				
			}

			//verificar identificador da gramatica
			if (count == 0)
			{
				strcpy(gramatica->estados[gramatica->numEstados]->identificador,auxToken);
			}
			else
			{

				Opcao *opcao = (Opcao*) malloc (sizeof(Opcao));

				if (opcao == NULL)
				{
					printf("Erro na alocação de memória\n");
					exit(1);
				}

				if (strcmp(auxToken,"epsilon") == 0)
				{
					gramatica->estados[gramatica->numEstados]->ehFinal = 1;
					opcao->ehEpsilon = 1;
				}
				else
				{
					gramatica->estados[gramatica->numEstados]->ehFinal = 0;
					opcao->ehEpsilon = 0;

					if (strlen(auxToken) == 1 && pertenceAlfabeto(auxToken, gramatica->alfabeto))
					{
						strcat(auxToken,"X");
					}
					strcpy(opcao->producao,auxToken);
					printf("opcao->producao:%s aux %s\n",opcao->producao,auxToken);
				}
				// //printf("token:%s\n", token);
				gramatica->estados[gramatica->numEstados]->opcoes[gramatica->estados[gramatica->numEstados]->nOpcoes] = opcao;
				gramatica->estados[gramatica->numEstados]->nOpcoes++;
			}
			token = strtok(NULL,":|");
			//printf("nova leitura :%s\n",token);
			count++;
		}
		//printf("------------%s-------------\n",token);
		gramatica->numEstados++;
		gramatica->estados[gramatica->numEstados] = (Estado *) malloc(sizeof(Estado));

		if (gramatica->estados[gramatica->numEstados] == NULL)
		{
			printf("Erro na Alocação de memória\n");
			exit(1);
		}

		gramatica->estados[gramatica->numEstados]->nOpcoes = 0;
	}

	//Cria estado final

	Estado *estadoX = (Estado*) malloc (sizeof(Estado));

	if (estadoX == NULL)
	{
		printf("Erro na Alocação de memória\n");
		exit(1);
	}

	estadoX->nOpcoes = 0;
	estadoX->ehFinal = 1;
	strcpy(estadoX->identificador, "X");

	gramatica->estados[gramatica->numEstados++] = estadoX;

	printf("numEstados: %d\n", gramatica->numEstados);

	int y = 0;
	for (y = 0; y < gramatica->numEstados; y++)
	{
		if (gramatica->estados[y]->ehFinal == 1) {
			printf("*");
		}
		printf("%s: ", gramatica->estados[y]->identificador);

		int n;
		for (n = 0; n < gramatica->estados[y]->nOpcoes; n++)
		{
			if (gramatica->estados[y]->opcoes[n]->ehEpsilon == 1) 
			{
				printf("ε | ");
			} else {
				printf("%s | ", gramatica->estados[y]->opcoes[n]->producao);
			}
			
		}

		printf("\n");
	}
	return gramatica;
}

/**
 * Faz a leitura do alfabeto
 * Recebe como parametro uma gramatica
 */
void leAlfabeto(Gramatica *gramatica) {

	char temporario[TAM_ALFABETO];

	if(fgets(temporario,TAM_ALFABETO,stdin) != NULL) {

		/**
		 * identifica linha digitada pelo usuário
		 */
		char *token;
		token = strtok(temporario,",");

		int count = 0;

		while(token != NULL) {

			gramatica->alfabeto[count] = token[0];
			count++;
			token = strtok(NULL,",");
		}
		gramatica->alfabeto[count] = '\0';
	}
}
/**
 * Linguagem Regular :
 *  		- não pode ter um simbolo do alfabeto no lado esquerdo;
 *  		- não pode ter somente simbolos do alfabeto no lado esquerdo;
 *  		- não pode ter dois simbolos do alfabeto no mesma opção
 * 
 */
int ehRegular(Gramatica *gramatica) {

	int i,j;

	for (i = 0; i < gramatica->numEstados;i++)
	{
		int numpertence = pertenceAlfabeto(gramatica->estados[i]->identificador,gramatica->alfabeto);

		// verifica simbolos não terminal, onde não pode ocorrer mais de 1 
		if (strlen(gramatica->estados[i]->identificador) >= 2 || numpertence >= 1)
		{
			//printf("verifica simbolos não terminal, onde não pode ocorrer mais de 1 \n" );
			return 0; // gramatica não é regular
		}
		
		for (j = 0; j < gramatica->estados[i]->nOpcoes; j++)
		{
			if (gramatica->estados[i]->opcoes[j]->ehEpsilon == 1) {

				continue;
			}

			int tamanho = strlen(gramatica->estados[i]->opcoes[j]->producao);

			if ( tamanho > 2)
			{
				//printf("Tamanho é maior que dois\n");
				return 0; // gramatica não é regular
			}
			else
			{
				char subString[4];
				strncpy(subString,gramatica->estados[i]->opcoes[j]->producao+1,2);
				
				int pertence = pertenceAlfabeto(subString,gramatica->alfabeto);

				if (pertence == 1)
				{
					return 0;
				}
			}
			/**
			 * Verifica se só tem um caractere como opção
			 * Verifica se o simbolo pertence ao alfabeto
			 * Se o simbolo pertence então esta dentro do padrão de linguagem regular
			 * Do contrario não pertence, então retorna 0 (zero)
			 */
			
			if (tamanho <= 2)
			{
				int numpertence = pertenceAlfabeto(gramatica->estados[i]->opcoes[j]->producao,gramatica->alfabeto);

				if (numpertence  != 1)
				{
					//printf("Não pertence ao alfabeto\n");
					return 0;
				}

			}
		}
		
	}
	return 1;

}

/**
 * Pertence ao Alfabeto
 * Verica se um caractere passado como parametro pertence ao alfabeto
 */
int pertenceAlfabeto(char *producao, char *alfabeto) {

	int i,j,count = 0;

	char auxiliar[TAM_ALFABETO];
	strcpy(auxiliar,producao);

	for (i = 0; i < strlen(auxiliar); i++)
	{
		for (j = 0; j < strlen(alfabeto); j++)
		{
			if (alfabeto[j] == auxiliar[i])
			{
				//printf("%d\n",count );
				count++;
			}
		}
	}

	return count;
}

/**
 * Verifica determinismo
 * passa uma gramatica como parametro
 * retorna 1(um) caso for indeterminitica
 * do contrário retorna 0 (zero)
 */
int ehIndeterministica(Gramatica *gramatica) {


	int i,j,k,w;

	for (i = 0; i < gramatica->numEstados;i++)
	{
		
		int count[strlen(gramatica->alfabeto)];

		for (j = 0; j < strlen(gramatica->alfabeto); j++)
		{
			count[j] = 0;
		}
		
		for (j = 0; j < gramatica->estados[i]->nOpcoes; j++)
		{	
			char *producao = gramatica->estados[i]->opcoes[j]->producao;
		
			if (gramatica->estados[i]->opcoes[j]->ehEpsilon == 1) {
				continue;
			}


			for (k = 0; k < strlen(producao); k++)
			{
			
				for (w = 0; w < strlen(gramatica->alfabeto); w++)
				{
					if (gramatica->alfabeto[w] == '\0')
					{
						continue;
					}
					if (gramatica->alfabeto[w] == producao[k])
					{
						count[w] += 1;
						break;
					}
				}
			}
		}
		
		for (j = 0; j < strlen(gramatica->alfabeto); j++)
		{
			if (count[j] > 1)
			{
				
				return 1; //ser indeterministica
			}
		}
	}
	return 0;
}

/**
 *  busca estado -- se estiver contido na gramatica retorna o estado 
 *  do contrario NULL
 */

Estado * buscaEstadoPorIdentificador(Gramatica *gramatica, char *estado) {

	int i;

	for (i = 0; i < gramatica->numEstados; i++)
	{
		if (strcmp(gramatica->estados[i]->identificador,estado) == 0	)
		{
			return gramatica->estados[i];
		}
	}

	return NULL;
}

/**
 * Faz o agrupamento dos simbolos para a determinização
 */

char *agrupaOpcoesPorSimbolo(Estado *estado, char caractere) {

	int i;
	char *producao = (char*) malloc (sizeof(char)*TAM_CARACTERE);
	strcpy(producao, "");

	if (producao == NULL)
	{
		printf("Erro na Alocação de memória\n");
		exit(1);
	}
	for (i = 0; i < estado->nOpcoes; i++)
	{
		if (estado->opcoes[i]->producao[0] == caractere)
		{
			producao = strncat(producao,&estado->opcoes[i]->producao[1],1);
		}
	}

	
	return producao;
}

/**
 * Determinização da gramatica
 * passa como parametro uma gramatica indeterministica
 * Retorna a gramatica determinizada
 */
Gramatica *gramaticaDetermizada(Gramatica *gramatica) {


	Gramatica *novaGramatica = (Gramatica*) malloc (sizeof(Gramatica));

	Estado *ordenado[500];

	int t;
	for (t = 0; t < 500; t++)
	{
		ordenado[t] = NULL;
	}

	if (novaGramatica == NULL)
	{
		printf("Erro de Alocação\n");
		return NULL;
	}

	int i;
	int count = -1, quantosJaTem = 0;

	novaGramatica->numEstados = 0;
	ordenado[0] = gramatica->estados[0];
	quantosJaTem++;

	strcpy(novaGramatica->alfabeto,gramatica->alfabeto);

	while (ordenado[count+1] != NULL) {

		count++;

		Estado *estadoAtual = ordenado[count];

		Estado *novoEstado = (Estado*) malloc(sizeof(Estado));

		novoEstado->nOpcoes = 0;
		novoEstado->ehFinal = estadoAtual->ehFinal;
		strcpy(novoEstado->identificador, estadoAtual->identificador);

		printf("attualll  %s\n", estadoAtual->identificador);

		for (i = 0; i < strlen(gramatica->alfabeto); i++)
		{
			char *novaProducao = agrupaOpcoesPorSimbolo(estadoAtual,gramatica->alfabeto[i]);
			if (novaProducao != NULL && strlen(novaProducao) > 0)
			{
				Opcao *novaOpcao = (Opcao*) malloc (sizeof(Opcao));

				if (novaOpcao == NULL)
				{
					printf("Erro ao alocar memória\n");
					exit(1);
				}

				char t[10];
				t[0] = gramatica->alfabeto[i];
				t[1] = '\0';

				strcat(novaOpcao->producao, t);
				strcat(novaOpcao->producao,novaProducao);
				novoEstado->opcoes[novoEstado->nOpcoes] = novaOpcao;
				novoEstado->nOpcoes++;

				Estado *proximoEstado = NULL;
				int g, existe = 0;

				for (g = 0; g < gramatica->numEstados; g++)
				{
					if (strcmp(novaProducao, gramatica->estados[g]->identificador) == 0) {
						existe = 1;
						proximoEstado = gramatica->estados[g];
						break;
					}
				}


				if (!existe) {
					proximoEstado = mesclaEstados(gramatica, novaGramatica, novaProducao);
					printf("------------------------\n");
					printf("%s: ", proximoEstado->identificador);

					int n;
					for (n = 0; n < proximoEstado->nOpcoes; n++)
					{
						printf("%s | ", proximoEstado->opcoes[n]->producao);
					}

					printf("\n");
					printf("------------------------\n");
				}

				int r, achou = 0;

				for (r = 0; r < 500 && ordenado[r]; r++)
				{

					if (strcmp(novaProducao, ordenado[r]->identificador) == 0)
					{
						achou = 1;	
					}
				}

				if (!achou) {
					ordenado[quantosJaTem++] = proximoEstado;
					//printf("nao achou, %s\n", proximoEstado->identificador);
				} else {
					// printf("------------------------\n");
					// printf("%s: ", proximoEstado->identificador);

					// int n;
					// for (n = 0; n < proximoEstado->nOpcoes; n++)
					// {
					// 	printf("%s | ", proximoEstado->opcoes[n]->producao);
					// }

					// printf("\n");
					// printf("------------------------\n");
				}
			}

			free(novaProducao);
		}

		//printf("novoooooo, %s\n", novoEstado->identificador);

		novaGramatica->estados[novaGramatica->numEstados++] = novoEstado;
	}

	// int y = 0;
	// for (y = 0; y < novaGramatica->numEstados; y++)
	// {
	// 	if (novaGramatica->estados[y]->ehFinal == 1) {
	// 		printf("*");
	// 	}
	// 	printf("%s: ", novaGramatica->estados[y]->identificador);

	// 	int n;
	// 	for (n = 0; n < novaGramatica->estados[y]->nOpcoes; n++)
	// 	{
	// 		if (novaGramatica->estados[y]->opcoes[n]->ehEpsilon == 1) 
	// 		{
	// 			printf("ε | ");
	// 		} else {
	// 			printf("%s | ", novaGramatica->estados[y]->opcoes[n]->producao);
	// 		}
			
	// 	}

	// 	printf("\n");
	// }

	return novaGramatica;
}

Estado *mesclaEstados(Gramatica *gramatica, Gramatica *novaGramatica, char *producao) {

	Estado *novoEstado = (Estado*) malloc (sizeof(Estado));

	if (novoEstado == NULL)
	{
		printf("Erro na Alocação de memória\n");
		return NULL;
	}
	novoEstado->nOpcoes = 0;
	strcpy(novoEstado->identificador, producao);
	//printf("----------------\n");

	int i, j, k, l, m, u;

	for (i = 0; i < strlen(novaGramatica->alfabeto); i++)
	{
		Opcao *novaOpcao = (Opcao*) malloc (sizeof(Opcao));

		if (novaOpcao == NULL)
		{
			printf("Erro na alocação de memória\n");
			exit(1);
		}
		int len = 0;
		novaOpcao->producao[len++] = novaGramatica->alfabeto[i];

		for (j = 0; j < strlen(producao); j++)
		{
			int contem = 0;
			for (k = 0; k < novaGramatica->numEstados; k++)
			{

				for (u = 0; u < strlen(novaGramatica->estados[k]->identificador); u++)
				{
					if (producao[j] == novaGramatica->estados[k]->identificador[u]) {
						contem = 1;
					}
				}

				if (contem) {

					if (novaGramatica->estados[k]->ehFinal == 1) {
						novoEstado->ehFinal = 1;
					}

					for (l = 0; l < novaGramatica->estados[k]->nOpcoes; l++)
					{
						if (novaGramatica->estados[k]->opcoes[l]->producao[0] == novaGramatica->alfabeto[i]) {
							int achou = 0;


							for (m = 0; m < strlen(novaOpcao->producao); m++)
							{
								if (novaOpcao->producao[m] == novaGramatica->estados[k]->opcoes[l]->producao[1]) {
									achou = 1;
									break;
								}
							}

							if (!achou) {
								novaOpcao->producao[len++] = novaGramatica->estados[k]->opcoes[l]->producao[1];
							}
						}
					}
				}
			}

			if (!contem) {
				int contemNaVelha = 0;

				for (k = 0; k < gramatica->numEstados; k++)
				{

					if (producao[j] == gramatica->estados[k]->identificador[0]) {
						contemNaVelha = 1;
					}

					if (contemNaVelha) {

						if (gramatica->estados[k]->ehFinal == 1) {
							novoEstado->ehFinal = 1;
						}

						for (l = 0; l < gramatica->estados[k]->nOpcoes; l++)
						{
							if (gramatica->estados[k]->opcoes[l]->producao[0] == gramatica->alfabeto[i]) {
								int achou = 0;


								for (m = 0; m < strlen(novaOpcao->producao); m++)
								{
									if (novaOpcao->producao[m] == gramatica->estados[k]->opcoes[l]->producao[1]) {
										achou = 1;
										break;
									}
								}

								if (!achou) {
									novaOpcao->producao[len++] = gramatica->estados[k]->opcoes[l]->producao[1];
								}
							}
						}
					}
				}
			}
		}

		if (strlen(novaOpcao->producao) > 0) {
			novoEstado->opcoes[novoEstado->nOpcoes++] = novaOpcao;
		}
	}


	return novoEstado;
}

void imprimeGramatica(Gramatica *gramatica) {

	int i,j;

	printf("+--------------------------------------------------+\n");
	printf("|" ANSI_COLOR_GREEN "             TABELA DE TRANSIÇÃO  " ANSI_COLOR_RESET "                |\n");
	printf("+--------------------------------------------------+\n");
	printf("|    %3s   ",gramatica->estados[0]->identificador );

	for (i = 0; i < strlen(gramatica->alfabeto); i++)
	{
		printf("|   %3c   ",gramatica->alfabeto[i]);
	}
	printf("|\n");
	printf("+--------------------------------------------------+\n");
	for (i = 0; i < gramatica->numEstados; i++)
	{
		if (i == 0 && gramatica->estados[i]->ehFinal == 0)
		{
			printf("|   ->%3s  ",gramatica->estados[i]->identificador );
		}
		else if (gramatica->estados[i]->ehFinal == 1 && i == 0)
		{
			printf("|  -> *%3s  ",gramatica->estados[i]->identificador );
		}
		else if (gramatica->estados[i]->ehFinal == 1)
		{
			printf("|   *%3s   ",gramatica->estados[i]->identificador );
		}
		else
		{
			printf("|   %3s    ",gramatica->estados[i]->identificador );
		}
		
		for (j = 0; j < strlen(gramatica->alfabeto); j++)
		{
			int achou = 0,k;

			for (k = 0; k < gramatica->estados[i]->nOpcoes; k++)
			{

				if (gramatica->estados[i]->opcoes[k]->producao[0] == gramatica->alfabeto[j])
				{
					printf("|  %3s    ",gramatica->estados[i]->opcoes[k]->producao+1);
					achou = 1;
					break;
				}
				
			}	

			if(achou == 0)
			{
				printf("|   --    ");
			}
			
		}
		printf("|\n");
	}
	printf("+--------------------------------------------------+\n");

}