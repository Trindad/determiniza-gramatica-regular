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


/* estruturas */

typedef struct __opcao
{
	char producao[20];
	short int ehEpsilon;
} Opcao;

typedef struct __estado
{
	char identificador[80]; //caractere
	int nOpcoes;
	Opcao *opcoes[100];
	short int ehFinal;
} Estado;

typedef struct __gramatica
{
	char alfabeto[255];
	int numEstados;
	Estado *estados[100];
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

	printf("%s\n",gramatica->alfabeto);
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
	novaGramatica->numEstados += 0;
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
	gramatica->estados[gramatica->numEstados] = (Estado *) malloc(sizeof(Estado));
	gramatica->estados[gramatica->numEstados]->nOpcoes = 0;

	char temporario[256];

	while(fgets(temporario,255,stdin) != NULL) {

		if (strcmp(temporario,"$$") == 0)
		{
			break;
		}

		/**
		 * identifica linha digitada pelo usuário
		 */
		char *token;
		token = strtok(temporario,":|");

		int count = 0;
		

		while(token != NULL) {

			int tamanho = 0;

			tamanho = strlen(token);

			if (token[tamanho-1] == '\n')
			{
				token[tamanho-1] = '\0';
				
			}
			if (count == 0)
			{
				printf("%s\n", token);
				strcpy(gramatica->estados[gramatica->numEstados]->identificador,token);
			}
			else
			{

				Opcao *opcao = (Opcao *) malloc(sizeof(Opcao));

				printf("%s\n", token);


				if (strcmp(token,"epsilon") == 0)
				{
					gramatica->estados[gramatica->numEstados]->ehFinal = 1;
					opcao->ehEpsilon = 1;
				}
				else
				{
					gramatica->estados[gramatica->numEstados]->ehFinal = 0;
					opcao->ehEpsilon = 0;

					if (strlen(token) == 1 && pertenceAlfabeto(token, gramatica->alfabeto))
					{
						strcat(token,"X");
					}
					strcpy(opcao->producao,token);
				}

				gramatica->estados[gramatica->numEstados]->opcoes[gramatica->estados[gramatica->numEstados]->nOpcoes] = opcao;
				gramatica->estados[gramatica->numEstados]->nOpcoes++;
			}
			token = strtok(NULL,":|");
			printf("%d\n", gramatica->estados[gramatica->numEstados]->nOpcoes);
			count++;
		}
		gramatica->numEstados++;
		gramatica->estados[gramatica->numEstados] = (Estado *) malloc(sizeof(Estado));
		gramatica->estados[gramatica->numEstados]->nOpcoes = 0;
	}
	return gramatica;
}

void leAlfabeto(Gramatica *gramatica) {

	char temporario[255];

	if(fgets(temporario,255,stdin) != NULL) {

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
		if (strlen(gramatica->estados[i]->identificador) >= 2 || numpertence >= 1)
		{
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
				return 0; // gramatica não é regular
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
					printf("uiouoiui %d  --- %s\n", numpertence,gramatica->estados[i]->opcoes[j]->producao);
					return 0;
				}

			}
		}
		
	}
	return 1;

}

/**
 * 
 */
int pertenceAlfabeto(char *producao, char *alfabeto) {

	int i,j,count = 0;

	for (i = 0; i < strlen(producao); i++)
	{
		for (j = 0; j < strlen(alfabeto); j++)
		{
			if (alfabeto[j] == producao[i])
			{
				count++;
			}
		}
	}

	return count;
}
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
 * 
 */

char *agrupaOpcoesPorSimbolo(Estado *estado, char caractere) {

	int i;
	char *producao = (char*) malloc (sizeof(char)*80);

	if (producao == NULL)
	{
		printf("Erro na Alocação de memória\n");
		exit(1);
	}
	for (i = 0; i < estado->nOpcoes; i++)
	{
		if (estado->opcoes[i]->producao[0] == caractere)
		{
			producao = strncat(producao,&estado->opcoes[i]->producao[0],1);
		}
	}

	return producao;
}

Gramatica *gramaticaDetermizada(Gramatica *gramatica) {

	Gramatica *novaGramatica = (Gramatica*) malloc (sizeof(Gramatica));

	Estado *ordenado[500];

	if (novaGramatica == NULL)
	{
		printf("Erro de Alocação\n");
		return NULL;
	}

	int i;
	int count = -1;

	novaGramatica->numEstados = 0;
	ordenado[0] = gramatica->estados[0];

	while (ordenado[count+1]) {

		count++;

		Estado *estadoAtual = ordenado[count];

		Estado *novoEstado = (Estado*) malloc(sizeof(Estado));

		novoEstado->nOpcoes = 0;

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

				strcpy(novaOpcao->producao,novaProducao);
				novoEstado->opcoes[novoEstado->nOpcoes] = novaOpcao;
				novoEstado->nOpcoes++;
			}

			free(novaProducao);
		}

		novaGramatica->estados[ novaGramatica->numEstados++] = novoEstado;
	}

	return NULL;
}