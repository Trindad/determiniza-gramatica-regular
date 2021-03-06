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

/**
 * Pertence ao alfabeto
 * Tem como dado de entrada uma produção e o alfabeto
 * Gera como dado de saída o número de caracteres que pertence ao alfabeto
 */
int pertenceAlfabeto(char *producao, char *alfabeto);

/**
 * Faz a leitura do alfabeto da linguagem
 * Tem como dado de entrada a gramatica
 */
void leAlfabeto(Gramatica *gramatica);

/**
 * Verifica se a grmatica é regular
 * Tem como dado de entrada uma gramatica
 * Verificando suas limitações
 * Se é regular retorna 1 (um)
 * Do contrario retorna 0 (zero)
 */
int ehRegular(Gramatica *gramatica);

/**
 * Verifica se há indeterminismo 
 * Tem como dado de entrada uma grámatica
 * Caso seja indeterministica retorna 1 (um)
 * Do contrario 0 (zero)
 */
int ehIndeterministica(Gramatica *gramatica);

/**
 * Agrupa as Opções por simbolo
 * Tem como dado de entrada um estado e um caractere
 * Pega todas as opções de um estado com o caratere terminal e monta o nome de um novo estado
 * E retorna o mesmo
 */
char *agrupaOpcoesPorSimbolo(Estado *estado,char caractere);

/**
 * Mescla estados
 * Tem como dado de entrada uma gramatica e uma produção 
 * Pegando como base a produção faz a concatenação gerando 
 * Gera um estado com novas produções
 */
Estado *mesclaEstados(Gramatica *gramatica, char *producao);

/**
 * Imprime a gramatica determinizada em uma tabela
 */
void imprimeGramatica(Gramatica *gramatica);

/**
 * Busco o estado identificador
 * tem como entrado uma gramatica e um estado não terminal
 * tem como saída o estado identificador
 */
Estado * buscaEstadoPorIdentificador(Gramatica *gramatica, char *estado);

/**
 * Funções para determinização da gramatica
 * Inicia pelo estado 'S' (inicial por exemplo)
 * varre cada estado da gramatica anterior
 * faz a concatenação gerando novos estados 
 * colocando este novo estado em uma fila de prioridade
 */
Gramatica *gramaticaDetermizada(Gramatica *gramatica);

/**
 * Elimina os estados que nunca acontecem, ou seja não aparecem na nova gramatica
 * Tem como entrada uma gramatica
 */
void eliminaInalcancaveis(Gramatica *gramatica);

/**
 * Limpa memória de gramatica
 * varre cada posição
 */
void freeGramatica(Gramatica *gramatica);


int main(int argc, char *argv[]) {

	Gramatica *gramatica = leGramatica();
	

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
	eliminaInalcancaveis(novaGramatica);
	imprimeGramatica(novaGramatica);

	freeGramatica(gramatica);
	freeGramatica(novaGramatica);

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
			producao = strcat(producao, &estado->opcoes[i]->producao[0]+1);
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

	Estado *filaDeInsercao[500];

	int t;
	for (t = 0; t < 500; t++)
	{
		filaDeInsercao[t] = NULL;
	}

	if (novaGramatica == NULL)
	{
		printf("Erro de Alocação\n");
		return NULL;
	}

	int i;
	int count = -1, quantosJaTem = 0;

	novaGramatica->numEstados = 0;
	filaDeInsercao[0] = gramatica->estados[0];
	quantosJaTem++;

	strcpy(novaGramatica->alfabeto,gramatica->alfabeto);

	while (filaDeInsercao[count+1] != NULL) {

		count++;

		Estado *estadoAtual = filaDeInsercao[count];

		Estado *novoEstado = (Estado*) malloc(sizeof(Estado));

		novoEstado->nOpcoes = 0;
		novoEstado->ehFinal = estadoAtual->ehFinal;
		strcpy(novoEstado->identificador, estadoAtual->identificador);

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
					proximoEstado = mesclaEstados(gramatica, novaProducao);
				}

				int r, achou = 0;

				for (r = 0; r < 500 && filaDeInsercao[r]; r++)
				{

					if (strcmp(novaProducao, filaDeInsercao[r]->identificador) == 0)
					{
						achou = 1;	
					}
				}

				if (!achou) {
					filaDeInsercao[quantosJaTem++] = proximoEstado;
				}
			}

			free(novaProducao);
		}

		if (strlen(estadoAtual->identificador) > 1) {
			novaGramatica->estados[novaGramatica->numEstados++] = estadoAtual;
		} else {
			novaGramatica->estados[novaGramatica->numEstados++] = novoEstado;
		}
	}

	return novaGramatica;
}

Estado *mesclaEstados(Gramatica *gramatica, char *producao) {

	Estado *novoEstado = (Estado*) malloc (sizeof(Estado));
	int i, j, k, l, m;

	if (novoEstado == NULL)
	{
		printf("Erro na Alocação de memória\n");
		return NULL;
	}
	novoEstado->nOpcoes = 0;
	strcpy(novoEstado->identificador, producao);

	// Loop que agrupa identificadores encontrados nas
	// produções do estado, pelo simbolo do alfabeto
	for (i = 0; i < strlen(gramatica->alfabeto); i++)
	{
		Opcao *novaOpcao = (Opcao*) malloc (sizeof(Opcao));

		if (novaOpcao == NULL)
		{
			printf("Erro na alocação de memória\n");
			exit(1);
		}

		int len = 0;

		// adiciona letra do alfabeto sendo buscada na produção da nova
		// opção do estado
		novaOpcao->producao[len++] = gramatica->alfabeto[i];

		for (j = 0; j < strlen(producao); j++)
		{
			for (k = 0; k < gramatica->numEstados; k++)
			{
				// Verifica se o identificador sendo procurado é o mesmo identificador da
				// gramatica sendo verificada
				if (producao[j] == gramatica->estados[k]->identificador[0]) {

					// Se o estado é final, o novo estado também é
					if (gramatica->estados[k]->ehFinal == 1) {
						novoEstado->ehFinal = 1;
					}

					// Varre as opções do estado
					for (l = 0; l < gramatica->estados[k]->nOpcoes; l++)
					{
						// Verifica se a produção sendo verificada possui simbolo do alfabeto procurado
						if (gramatica->estados[k]->opcoes[l]->producao[0] == gramatica->alfabeto[i]) {
							int achou = 0;

							// Procura se produção já está ou não no nova produção
							for (m = 0; m < strlen(novaOpcao->producao); m++)
							{
								if (novaOpcao->producao[m] == gramatica->estados[k]->opcoes[l]->producao[1]) {
									achou = 1;
									break;
								}
							}

							// concatena produção do estado no nova produção
							// se não encontrar o mesmo na produção atual
							if (!achou) {
								novaOpcao->producao[len++] = gramatica->estados[k]->opcoes[l]->producao[1];
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

void eliminaInalcancaveis(Gramatica *gramatica)
{
	int i,j, k, o;

	for (i = 1; i < gramatica->numEstados; i++)
	{
		Estado *estado = gramatica->estados[i];
		int achou = 0;

		for (j = 0; j < gramatica->numEstados; j++)
		{
			if (j == i) {
				continue;
			}

			for (k = 0; k < gramatica->estados[j]->nOpcoes; k++)
			{
				if (strcmp(estado->identificador, gramatica->estados[j]->opcoes[k]->producao+1) == 0) {
					achou = 1;
					break;
				}
			}
		}

		if (!achou)
		{
			int restantes = gramatica->numEstados - i + 1;

			for (o = 0; o < restantes; o++)
			{
				gramatica->estados[i + o] = gramatica->estados[i + o + 1];
			}

			gramatica->numEstados--;

			break;
		}
	}
}

void freeGramatica(Gramatica *gramatica)
{
	if (!gramatica) {
		return;
	}

	int i, j;

	for (i = 0; i < gramatica->numEstados; i++)
	{
		for (j = 0; j < gramatica->estados[i]->nOpcoes; j++)
		{
			free(gramatica->estados[i]->opcoes[j]->producao);
		}

		free(gramatica->estados[i]);
	}

	free(gramatica);
}