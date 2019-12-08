#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#ifndef HASHMAP_H
#define HASHMAP_H

typedef void *(*ResolveCollisionCallback)(void *old_data, void *new_data);
typedef void (*callback)(const char *key, void *data);
typedef void (*DestroyDataCallback)(void *data);

//Define custom data types
/*
	Need to define 2 structures:
		1- For the hash table
		2- For the entry to the hash table
*/
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


HashMap *create_hashmap(size_t key_space);
unsigned int hash(const char *key);
void insert_data(HashMap *hm, const char *key, void *data, ResolveCollisionCallback resolve_collision);
void *get_data(HashMap *hm, const char *key);
void iterate(HashMap *hm, callback callback);
void remove_data(HashMap *hm, const char *key, DestroyDataCallback destroy_data);
void delete_hashmap(HashMap *hm, DestroyDataCallback destroy_data);

#endif // HASHMAP_H