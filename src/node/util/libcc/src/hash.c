/*******************************************************************************
INCLUDES E MACROS
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "hash.h"

#define _XOPEN_SOURCE 500 	 // enable strdup on linux

/*******************************************************************************
ESTRUTURAS DE DADOS
******************************************************************************/

struct entry_s {
	char *key;									// chave == coordenadas
	void *value;								// célula da coordenada
	struct entry_s *next;				        // link pra lista ligada
};

struct hashtable_s {
	int size;									// tamanho da hashtable
	struct entry_s **table;		                // tabela
};

/*******************************************************************************
FUNÇÕES
******************************************************************************/

/* Inicialização da tabela */
hashtable_t *ht_create(int size) {

	if (size < 1)
	return NULL;
	hashtable_t *hashtable = NULL;

	/* Tentativa de alocação de memória para a estrutura da tabela */
	if ((hashtable = malloc(sizeof(hashtable_t))) == NULL)
	return NULL;

	/* Tentativa de alocação da tabela em si */
	if ((hashtable->table = malloc(sizeof(entry_t*) * size)) == NULL)
	return NULL;

	/* Inicialização dos elementos da tabela */
	for (int i = 0; i < size; i++) hashtable->table[i] = NULL;

	/* Setando o tamanho da tabela */
	hashtable->size = size;

	return hashtable;
}

/* Função de hashing */
int ht_hash(hashtable_t *hashtable, char *key) {

	unsigned long int hashval = 0;
	int i = 0;

	int len = strlen(key);

	/* Hashing pelo método da divisão */
	while (hashval < ULONG_MAX && i < len) {
		hashval = (hashval * 256 + key[i]) % hashtable->size;
		i++;
	}

	return hashval;
}

/* Cria um par chave-valor */
entry_t *ht_newpair(char *key, void *value) {

	entry_t *newpair;

	if ((newpair = malloc(sizeof(entry_t))) == NULL)
	return NULL;

	newpair->key = malloc((strlen(key) + 1) * sizeof(char));
	if (newpair->key == NULL) {
		free (newpair);
		return NULL;
	}
	strcpy(newpair->key, key);

	if ((newpair->value = value) == NULL)
	return NULL;

	newpair->next = NULL;

	return newpair;
}

/* Insere um par chave-valor na hashtable */
void ht_set(hashtable_t *hashtable, char *key, void *value) {

	int hashval = ht_hash(hashtable, key);
	entry_t *newpair = NULL;
	entry_t *next = hashtable->table[hashval];
	entry_t *last = NULL;

	while (next != NULL && next->key != NULL && strcmp(key, next->key) > 0) {
		last = next;
		next = next->next;
	}

	/* Achamos o par. Vamos substitui-lo. */
	if (next != NULL && next->key != NULL && strcmp(key, next->key) == 0) {
		free(next->value);
		next->value = value;
	}

	/* Não achamos o par. Vamos criá-lo. */
	else {
		newpair = ht_newpair(key, value);
		/* Estamos no início da lista ligada desse hashval */
		if (next == hashtable->table[hashval]) {
			newpair->next = next;
			hashtable->table[hashval] = newpair;
		}
		/* Estamos no fim da lista ligada desse hashval. */
		else if (next == NULL)
		last->next = newpair;
		/* Estamos no meio da lista */
		else  {
			newpair->next = next;
			last->next = newpair;
		}
	}
}

/* Recupera uma tupla chave-valor da hashtable */
void *ht_get(hashtable_t *hashtable, char* key) {

	unsigned int hashval = ht_hash(hashtable, key);
	entry_t *pair;

	/* Procuramos pelo valor e retornamos quando o encontramos */
	for (pair = hashtable->table[hashval]; pair != NULL; pair = pair->next)
	if (strcmp(key, pair->key) == 0)
	return pair->value;

	/* Se não encontramos, retornamos null */
	return NULL;
}

/* Recupera uma tupla chave-valor aleatória da hashtable */
void *ht_get_random(hashtable_t *hashtable) {

	/* variáveis temporárias pra receber os valores */
	unsigned int hashval;
	entry_t *pair;
	void *temp;

	/* enquanto a variável temp apontar pra um NULL */
	for (;;) {
		/* sorteio de um número entre 0 e o tamanho da arena */
		hashval = rand() % hashtable->size;
		/* pair aponta pra entrada sorteada sse a entrada não for NULL */
		if (hashtable->table[hashval]) {
			pair = hashtable->table[hashval];
			/* temp aponta pra célula dentro do pair sorteado */
			temp = pair->value;
			/* evita retornar um ponteiro NULL */
			if (temp)
			return temp;
		}
	}
}

/* Recupera o k-ésimo par de uma hashtable */
entry_t *ht_get_kth(hashtable_t *hashtable, int k) {
	return hashtable->table[k];
}

/* Recupera o próximo par na lista ligada */
entry_t *ht_get_next(hashtable_t *hashtable, entry_t *pair) {
	return pair->next;
}

/* Recupera valor do par */
void *ht_get_value(hashtable_t *hashtable, entry_t *pair) {
	return pair->value;
}

/* Recupera chave do par */
char *ht_get_key(hashtable_t *hashtable, entry_t *pair) {
	return pair->key;
}

/* Recupera tamanho da hashtable */
int ht_get_size(hashtable_t *hashtable) {
	return hashtable->size;
}
