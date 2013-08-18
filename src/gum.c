#include "gum.h"

#define GUMMAP_LEN 100

struct mapgum{
	struct gumpck* gumpck;
	struct mapgum* next_mapgum;
};

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
	}
	return -1;
}

void gum_add(struct gumpck* gum){
	struct mapgum *mgum, *newmgum;

	if(first_gum == 0)
		first_gum = last_gum = gum;
	else{
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
}
