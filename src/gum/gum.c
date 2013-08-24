#include "gum.h"
#include <stdio.h>
#define GUMMAP_LEN 100

struct mapgum{
	struct gumpck* gumpck;
	struct mapgum* next_mapgum;
};

void init_gum_management();

struct gumpck* first_gum;
struct gumpck* last_gum;
struct mapgum* gum_map[GUMMAP_LEN];

int hash(uint32_t val){
	return val % GUMMAP_LEN;
}

struct gumpck* gum_list(){
	return first_gum;
}

int gum_lookup(uint32_t ip, struct gumpck* gum){
	struct mapgum* mgum;
	mgum = gum_map[hash(ip)];
	while(mgum != 0){
		if(mgum->gumpck->ip == ip){
			memcpy(gum, mgum->gumpck, sizeof(struct gumpck));
			return 0;
		}
		mgum = mgum->next_mapgum;
	}
	return -1;
}

void gum_add(struct gumpck* gum){
	struct mapgum *mgum, *newmgum;

	if(first_gum == 0){
		first_gum = gum;
		last_gum = gum;
	}else{
		last_gum->next = gum;
		last_gum = gum;
	}
	gum->next = 0;
	newmgum = (struct mapgum*)malloc(sizeof(struct mapgum));
	newmgum->gumpck = gum;
	newmgum->next_mapgum = 0;

	mgum = gum_map[hash(gum->ip)];
	if(mgum == 0)
		gum_map[hash(gum->ip)] = newmgum;
	else
		mgum->next_mapgum = newmgum;
	
}

void gum_remove(struct gumpck* gum){
	// TODO
}

void init_gums(){
	int i;
	for (i = 0; i < GUMMAP_LEN; i++){
		gum_map[i] = 0;
	}
	first_gum = 0;
	last_gum = 0;

	init_gum_management();
	init_gum_dhcp();
}


/* Gum Management */

#define GUM_HASH_SIZE 100000

struct gumhash{
	struct gumpck* gum;
	time_t access_time;
};

struct gumhash gumtable[GUM_HASH_SIZE];
struct gumpck* next;	

void init_gum_management(){
	memset(gumtable, 0, GUM_HASH_SIZE * sizeof(struct gumhash));
	next = 0;
}

/*
* Assign a gum to the given hash
*/
struct gumpck* set_assigned_gum(uint32_t hash){
	/* Round Robin gum allocation */
	if(next == 0)
		next = gum_list();
	gumtable[hash].gum = next;
	next = next->next;
	return gumtable[hash].gum;
}

/*
*	Return a gum assined with the given hash
*/
struct gumpck* get_assigned_gum(uint32_t hash){
	hash = hash % GUM_HASH_SIZE;
	if(gumtable[hash].gum == 0)
		set_assigned_gum(hash);
	gumtable[hash].access_time = time(0);
	return gumtable[hash].gum;
}
