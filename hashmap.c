#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Define custom data types

int key_space = 0; //when it reaches the end should come back to the begining

/*
	Need to define 2 structures:
		1- For the hash table
		2- For the entry to the hash table
*/
typedef void *(*ResolveCollisionCallback)(void *old_data, void *new_data);
typedef void (*callback)(char *key, void *data);
typedef void (*DestroyDataCallback)(void *data);

typedef struct entry
{
	char *key;
	char *value;
	struct entry *next; //create linked list to avoid collision
} entry;

typedef struct HashMap
{
	entry **entries; //The hashtable is an array of pointers to entries
} HashMap;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
/*
	This function should allocate enough memoryto fit key_space(rows) buckets.
	The allocated memory should be zeroed?????
*/
HashMap *create_hashmap(size_t key_space){
	key_space = key_space;
	HashMap *table = malloc(sizeof(HashMap)); //Allocate space for the table
	assert(table != NULL);
	table -> entries = malloc(sizeof(entry *) * key_space);

	// Populate the table with NULL pointers
	// Later if its not NULL we already know there is an entry and collision should be handled
	for(int i = 0; i < key_space; i++){
		table -> entries[i] = NULL;
	}

	return table;
}


unsigned int hash(char *key){
	int i = 0;
	unsigned int hash_code = 0;
	while(key[i] != '\0'){
		hash_code += key[i];
		i += 1;
	}
	hash_code %= 10; // Apply modulo to the result so the value is in the 0 to key_space - 1 range
	return hash_code;
}

void insert_data(HashMap *hm, char *key, void *data, ResolveCollisionCallback resolve_collision){
	//apply the hash function on the key to calculate the entry index for the key
	unsigned int index = hash(key);
	entry *node	= (hm->entries[index]);
	if (node == NULL)
	{
		entry *pair = malloc(sizeof(entry));
		strcpy(pair->key, key);
		//strcpy(pair->value, data); //no need to copy just directly move the pointerof data to value
		pair->value = data;
		pair->next = NULL;
		return;
	}
	else{
		printf("Collision occured!\n");
		void *solved = resolve_collision(node->value, data);
		// what should be stored in the bucket?
		node->value = solved;
		return;
	}
	printf("should not reach this part!\n");
	assert(1);
	return;
}

void *get_data(HashMap *hm, char *key){
	//apply the hash function on the key to calculate the entry index for the key
	unsigned int index = hash(key);
	entry *node	= (hm->entries[index]);
	if (node == NULL)
		return NULL;
	else
		//walk through each node in that index to find a matching key
		while(node != NULL){
			if (strcmp(node->key,key) == 0)
				return node->value; //return the value if the key match
			else
				node = node->next; //else fo to the next entry
		}

	// There was entry but no key found so return NULL
	return NULL;
}

void iterator(HashMap *hm, callback callback){
	// check if the callbacks exists
	//iterate over th entire hashmap
	for (int i = 0; i < key_space; ++i){
		entry *node = (hm->entries[i]);
		if (node == NULL)
			continue;
		else{
			printf("In index %d\n", i);
			while(node->next != NULL){
				callback(node->key, node->value);
			}
		}
	}
}

void remove_data(HashMap *hm, char *key, DestroyDataCallback destroy_data){
	unsigned int index = hash(key);
	entry *node	= (hm->entries[index]);

	//go throught the whole list 
	if (destroy_data != NULL){
		void *get_removed = get_data(hm, key);
		destroy_data(get_removed);
		//free(node); //?
	}
	else
		printf("Call back is NULL\n");
}

void delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data){
	if (destroy_data != NULL){
		//call desttroy data for every element(with data pointer of the element)
		printf("Destroying all elements of the hashmap!\n");
		for (int i = 0; i < key_space; ++i){
			entry *node	= (hm->entries[i]);
			while(node != NULL){
				destroy_data(node->value);
				node = node->next;
			}
			//destroy_data(); is it necessary?
			// yes cause u cant just free the first element of the linked list
			free(node);
		}
	}
	else
		free(hm);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char const *argv[])
{
	char *key = "AC";
	unsigned int h =  0;
	h = hash(key);
	printf("the hash is %u\n", h);

	// ResolveCollisionCallback is a pointer to resolve_collision fun() 
    //void *(*ResolveCollisionCallback);
    //ResolveCollisionCallback = &resolve_collision; 

	HashMap *map = create_hashmap(10);
	delete_hashmap(map, NULL);
	return 0;
}