///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Vector μέσω ADT Map.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTVector.h"
#include "ADTMap.h"


int* create_int(int value) {
	int* pointer = malloc(sizeof(int));
	*pointer = value;
	return pointer;
}

int compare_keys(Pointer a, Pointer b){
	return *(int*)a - *(int*)b;
}

void destroy_key(Pointer key){
	free((int*)key);
}

// Ενα Vector είναι pointer σε αυτό το struct
struct vector {
	Map vectormap; 
};


Vector vector_create(int size, DestroyFunc destroy_value) {
	Vector vectorito = malloc(sizeof(vectorito));
	vectorito->vectormap = map_create(compare_keys, destroy_key, destroy_value);
	return vectorito;
}

int vector_size(Vector vec) {
	return map_size(vec->vectormap);
}

Pointer vector_get_at(Vector vec, int pos) {
	int* position = create_int(pos);
	Pointer hold_this = map_find(vec->vectormap, position);
	free(position);
	return hold_this;
}

void vector_set_at(Vector vec, int pos, Pointer value) {
	int* position = create_int(pos);
	if (map_find_node(vec->vectormap, position)!=MAP_EOF){ // στο header file λεει μη ορισμενη συμπεριφορα αλλα τσεκαρω πρωτα αν υπαρχει στο vector σε αυτο το position αλλο στοιχειο
		map_remove(vec->vectormap, position);			   // και αν ειναι σωστη η θεση αφαιρω το στοιχειο απο αυτη τη θεση
		map_insert(vec->vectormap, position, value); 	   // και βαζω στην ιδια θεση με το νεο value
	}
	else {
		map_insert(vec->vectormap, create_int(vector_size(vec)), value); // αλλιως το βαζω στη τελευταια θεση για να μη χαλασει η διαταξη του map
		free(position);
	}
}

void vector_insert_last(Vector vec, Pointer value) {
	map_insert(vec->vectormap, create_int(vector_size(vec)), value); // ετσι οταν μπαινει ενα στοιχειο θα μπαινει παντα στο τελος αφου θα ναι στη θεση size που ειναι +1 position απο το τελευταιο στοιχειο
}

void vector_remove_last(Vector vec) {
	if (vector_size(vec)>0) {
		int* position = create_int(vector_size(vec)-1);
		map_remove(vec->vectormap, position);
		free(position);
	}
}

Pointer vector_find(Vector vec, Pointer value, CompareFunc compare) { 
	int i=0; // αναγκαστικα εδω διατρεχω τα στοιχεια μεχρι να το βρω βαση των διαθεσιμων συναρτησεων της map
	while (i<vector_size(vec)){
		if (compare(vector_get_at(vec, i), value)==0) return vector_get_at(vec, i); // αν το βρω το επιστρεφω
		i++;
	}
	return NULL;
}

DestroyFunc vector_set_destroy_value(Vector vec, DestroyFunc destroy_value) {
	return map_set_destroy_value(vec->vectormap, destroy_value);
}

void vector_destroy(Vector vec) {
	map_destroy(vec->vectormap);
	free(vec);
}


// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

VectorNode vector_first(Vector vec) {
	return (VectorNode)map_first(vec->vectormap);
}

VectorNode vector_last(Vector vec) {
	int* position = create_int(vector_size(vec)-1);
	VectorNode hold_this = (VectorNode)map_find_node(vec->vectormap, position);
	free(position);
	return hold_this;
}

VectorNode vector_next(Vector vec, VectorNode node) {
	return (VectorNode)map_next(vec->vectormap, (MapNode)node);
}

VectorNode vector_previous(Vector vec, VectorNode node) {
	// βρισκω το key του node και το κανω cast σε int αφαιρω ενα να παω στο προηγουμενο position και το βρισκω με την map_find_node
	int* position = create_int((*(int*)(map_node_key(vec->vectormap, (MapNode)node)))-1);
	VectorNode hold_this = (VectorNode)map_find_node(vec->vectormap, position);
	free(position);
	return hold_this;
}

Pointer vector_node_value(Vector vec, VectorNode node) {
	return map_node_value(vec->vectormap, (MapNode)(node));
}

VectorNode vector_find_node(Vector vec, Pointer value, CompareFunc compare) {
	int i=0; // και εδω διατρεχω τα στοιχεια μεχρι να το βρω το value
	while (i<vector_size(vec)){
		if (compare(vector_get_at(vec, i), value)==0) {
			int* position =  create_int(i);
			VectorNode hold_this = (VectorNode)map_find_node(vec->vectormap, position); // αν το βρω επιστρεφω το node
			free(position);
			return hold_this;
		}
		i++;
	}
	return NULL;
}