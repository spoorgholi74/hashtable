#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////

//Define custom data types
/*
	Need to define 2 structures:
		1- For the hash table
		2- For the entry to the hash table
*/
typedef void *(*ResolveCollisionCallback)(void *old_data, void *new_data);
typedef void (*callback)(const char *key, void *data);
typedef void (*DestroyDataCallback)(void *data);

typedef struct entry
{
	char *key;
	char *value;
	struct entry *next; //create linked list to avoid collision
} entry;

typedef struct HashMap
{
	entry **entries; //The hashtable is an array of pointers to first element of a linked list
	size_t h_size;
} HashMap;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Helper functions
/*
	This function should allocate enough memoryto fit key_space(rows) buckets.
	The allocated memory should be NULLED
*/
HashMap *create_hashmap(size_t key_space){
	assert(key_space > 0);
	HashMap *table = malloc(sizeof(HashMap)); //Allocate space for the table
	assert(table != NULL);
	table -> h_size = key_space;
	table -> entries = malloc(sizeof(entry *) * table -> h_size);

	// Populate the table with NULL pointers
	// Later if its not NULL we already know there is an entry and collision should be handled
	for(size_t i = 0; i < table -> h_size; i++){
		table -> entries[i] = NULL;
		//printf("the entries are: %s\n", table->entries[i]);
	}
	return table;
}


unsigned int hash(const char *key){
	int i = 0;
	unsigned int hash_code = 0;
	while(key[i] != '\0'){
		hash_code += key[i];
		i += 1;
	}
	return hash_code;
}


void insert_data(HashMap *hm, const char *key, void *data, ResolveCollisionCallback resolve_collision){
	assert(key != NULL);
	//apply the hash function on the key to calculate the entry index for the key
	unsigned int index = hash(key);
	index = index % hm->h_size;

	entry *entry_pointer = (hm->entries[index]);
	if (entry_pointer == NULL)
	{
		// Allocate memory to the node of a linked list
		entry *node = malloc(sizeof(entry));
		node->key = malloc(strlen(key) + 1);
		strcpy(node->key, key);
		// No need to copy just directly move the pointerof data to value
		node->value = data;
		node->next = NULL;
		// Save the node(linked list) to the array of pointers
		hm->entries[index] = node;
		return;
	}
	else{
		// The bucket has entries
		//printf("resolving consllision!\n");
		while(entry_pointer->next != NULL){
			if (strcmp(entry_pointer->key, key) == 0){
				entry_pointer->value = resolve_collision(entry_pointer->value, data);
				//hm->entries[index] = entry_pointer;
				return;
			}
			else
				entry_pointer = entry_pointer->next;
		}
		// Reaches here if it reaches the last node in the linked list
		// if the key matches update otherwise create a new node
		if (strcmp(entry_pointer->key, key) == 0){
				entry_pointer->value = resolve_collision(entry_pointer->value, data);
				return;
			}
		else{
			//create a new entry
			entry *new_node = malloc(sizeof(entry));
			new_node->key = malloc(strlen(key) + 1);
			strcpy(new_node->key, key);
			// No need to copy just directly move the pointerof data to value
			new_node->value = data;
			entry_pointer->next = new_node;
			new_node->next = NULL;
			// Save the new_node(linked list) to the array of pointers
			//hm->entries[index] = new_node;
			return;
		}	
		return;
	}
}


void *get_data(HashMap *hm, const char *key){
	//apply the hash function on the key to calculate the entry index for the key
	unsigned int index = hash(key);
	index = index % hm->h_size;
	entry *entry_pointer	= (hm->entries[index]);
	if (entry_pointer == NULL)
		return NULL;
	else{
		//walk through each entry_pointer in that index to find a matching key
		if (entry_pointer->key == NULL)
			return NULL;
		while(entry_pointer != NULL){
			if (strcmp(entry_pointer->key,key) == 0)
				return entry_pointer->value; //return the value if the key match
			else
				entry_pointer = entry_pointer->next; //else fo to the next entry
		}
	}

	// There was entry but no key found so return NULL
	return NULL;
}


void iterate(HashMap *hm, callback callback){
	for (size_t i = 0; i < (hm->h_size); ++i){
		entry *entry_pointer = (hm->entries[i]);
		if (entry_pointer == NULL)
			continue;
		else{
			//printf("In index %ld\n", i);
			do{
				callback(entry_pointer->key, entry_pointer->value);
				entry_pointer = entry_pointer->next;
			} while(entry_pointer != NULL);			
		}
	}
}


void remove_data(HashMap *hm, const char *key, DestroyDataCallback destroy_data){
	unsigned int index = hash(key);
	index = index % hm->h_size;
	entry *entry_pointer = (hm->entries[index]);
	entry *prev = entry_pointer;
	int x = 0;

	if (entry_pointer != NULL){
		do{
			if (strcmp(entry_pointer->key, key) == 0){
				prev->next = entry_pointer->next;
				if (x == 0)
					hm->entries[index] = entry_pointer->next;	
				else
					hm->entries[index] = prev;
				if (destroy_data != NULL)
					destroy_data(entry_pointer->value);
				free(entry_pointer->key);
				free(entry_pointer);
				return;
			}
			else{
				prev = entry_pointer;
				entry_pointer = entry_pointer->next;
				x += 1;
			}

		} while(entry_pointer != NULL);

	}

	else
		return;
}


void delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data){
	assert(hm != NULL);
	//printf("Deallocating all elements of the hashmap!\n");
	
	for (size_t i = 0; i < (hm->h_size); ++i){
		entry *entry_pointer = (hm->entries[i]);

		if (entry_pointer != NULL){
			entry *temp = entry_pointer->next;
			while(entry_pointer != NULL){
				printf("Found stuff at index %ld with key = %s and value = %s\n", \
					i, entry_pointer->key, entry_pointer->value);
				temp = entry_pointer->next;
				if (destroy_data != NULL)
					destroy_data(entry_pointer->value);
				free(entry_pointer->key);
				free(entry_pointer);
				entry_pointer = temp;
			}
		}
		else
			continue;
		
		//destroy_data(); is it necessary?
		// yes cause u cant just free the first element of the linked list
	}
	free(hm->entries);
	free(hm);
}