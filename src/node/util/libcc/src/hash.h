/* ifndef guard */
#ifndef _HASH_H_
#define _HASH_H_

/* ESTRUTURAS DE DADOS */
typedef struct entry_s entry_t;
typedef struct hashtable_s hashtable_t;

/* FUNÇÕES */
hashtable_t* ht_create(int size);					            // construtor
void ht_set(hashtable_t *hashtable, char *key, void *value);	// seta uma entr
void* ht_get(hashtable_t* hashtable, char* key);                // get celula
void* ht_get_random(hashtable_t* hashtable);	                // get rand cel
entry_t* ht_get_kth(hashtable_t* hashtable, int k);	            // get kth entry
entry_t* ht_get_next(hashtable_t* hashtable, entry_t* pair);	// get next entr
void* ht_get_value(hashtable_t* hashtable, entry_t* pair);		// get pair->val
char* ht_get_key(hashtable_t* hashtable, entry_t* pair);	    // get pair->key
int ht_get_size(hashtable_t* hashtable);			            // get ht->size

#endif /* _HASH_H_ */
