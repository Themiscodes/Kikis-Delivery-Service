///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Deque μέσω ADT Map.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTDeque.h"
#include "ADTMap.h"


int* create_int(int value) { // σημαντικο για να μην εχω leaks οσα create_ints καλω να τα αποδεσμευω στο τελος χρησης τους
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

struct deque {
	Map dequemap; 
};


Deque deque_create(int size, DestroyFunc destroy_value) {
	Deque dequeito = malloc(sizeof(dequeito));
	dequeito->dequemap = map_create(compare_keys, destroy_key, destroy_value);
	return dequeito;
}

int deque_size(Deque dec) {
	return map_size(dec->dequemap);
}

Pointer deque_get_at(Deque dec, int pos) {          // κανω προσαρμογη στη θεση με την "relative" σχεση σε συγκριση με το κλειδι του πρωτου στοιχειου
    int* position = create_int(pos+(*(int*)map_node_key(dec->dequemap, map_first(dec->dequemap))));
    Pointer hold_this = map_find(dec->dequemap, position); // ξεκιναω να μετραω δηλαδη απο το πρωτο
    free(position);
    return hold_this;
}

void deque_set_at(Deque dec, int pos, Pointer value) { 
    if (deque_size(dec)==0) map_insert(dec->dequemap, create_int(0), value); // εδω το κανω κατευθιαν create_int χωρις free επειδη θα γινει αποδεσμευση οταν σβησει απο το deque με τηνε destroy_value για το key
    else {
        int* position = create_int(pos+(*(int*)map_node_key(dec->dequemap, map_first(dec->dequemap))));
        if (map_find_node(dec->dequemap, position)!=MAP_EOF){ // αν και στο header file λεει μη ορισμενη συμπεριφορα τσεκαρω πρωτα αν υπαρχει στο Deque σε αυτο το position 
		    map_remove(dec->dequemap, position);			  // αν ναι αφαιρω το στοιχειο που υπαρχει απο αυτη τη θεση (ταυτοχρονα διαγραφεται και τοπ προηγουμενο key poy ειχε)
		    map_insert(dec->dequemap, position, value);		  // και βαζω στην ιδια θεση το νεο value
	    }
        else 
            map_insert(dec->dequemap, position, value); // αν δεν υπαρχει απλα το βαζω στη θεση 
    }
}

void deque_insert_first(Deque dec, Pointer value){ // προσθετω ενα στοιχειο πριν το αρχικο, δεν με πειραζει που θα ειναι αρνητικα
    if (deque_size(dec)==0) map_insert(dec->dequemap, create_int(0), value); // ελεγχω αν ειναι αδειο τωρα
    else map_insert(dec->dequemap, create_int((*(int*)map_node_key(dec->dequemap, map_first(dec->dequemap)))-1), value);
}

void deque_insert_last(Deque dec, Pointer value) {
    if (deque_size(dec)==0) map_insert(dec->dequemap, create_int(0), value); // ειναι σημαντικο πταν ειναι αδειο για να μην γινεται segfault στο map_node_key απο κατω
	else map_insert(dec->dequemap, create_int(deque_size(dec)+(*(int*)map_node_key(dec->dequemap, map_first(dec->dequemap)))), value); // ετσι οταν μπαινει ενα στοιχειο θα μπαινει παντα στο τελος αφου θα ναι στη θεση size που ειναι +1 position απο το τελευταιο στοιχειο
}

void deque_remove_last(Deque dec) {
	if (deque_size(dec)>0) {
        int* position = create_int((*(int*)map_node_key(dec->dequemap, map_first(dec->dequemap)))+deque_size(dec)-1);
		map_remove(dec->dequemap, position);
        free(position);
	}
}

void deque_remove_first(Deque dec){
    if(deque_size(dec)>0) {
        int* position = create_int(*(int*)map_node_key(dec->dequemap, map_first(dec->dequemap)));
        map_remove(dec->dequemap, position);
        free(position);
    }
}

Pointer deque_find(Deque dec, Pointer value, CompareFunc compare) { 
	int i=0; // εδω δεν αρχιζω με τη σχετικη θεση (δηλ. +τον αριθμο του pos του πρωτου στοιχειου) επειδη χρησιμοποιω την get_at που το κανει εσωτερικα αυτο
    while (i<deque_size(dec)){
	    if (compare(deque_get_at(dec, i), value)==0) return deque_get_at(dec, i); // αν το βρω το επιστρεφω
		i++;
	}
	return NULL;
}

DestroyFunc deque_set_destroy_value(Deque dec, DestroyFunc destroy_value) {
	return map_set_destroy_value(dec->dequemap, destroy_value);
}

void deque_destroy(Deque dec) {
	map_destroy(dec->dequemap);
	free(dec);
}


// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

DequeNode deque_first(Deque dec) {
	return (DequeNode)map_first(dec->dequemap);
}

DequeNode deque_last(Deque dec) {
    if (deque_size(dec)==0) return DEQUE_EOF; // σημαντικο να ελεγχω αν ειναι αδεια
    int* position = create_int((*(int*)map_node_key(dec->dequemap, map_first(dec->dequemap)))+deque_size(dec)-1);
	DequeNode hold_this = (DequeNode)map_find_node(dec->dequemap, position);
    free(position);
    return hold_this;
}

DequeNode deque_next(Deque dec, DequeNode node) {
	// if (map_next(dec->dequemap, (MapNode)node)==MAP_EOF) return Deque_EOF;
	return (DequeNode)map_next(dec->dequemap, (MapNode)node);
}

DequeNode deque_previous(Deque dec, DequeNode node) {
	// βρισκω το key του node και το κανω cast σε int αφαιρω ενα να παω στο προηγουμενο position και το βρισκω με την map_find_node
	if (node==deque_first(dec)) return DEQUE_BOF;
    int* position = create_int((*(int*)(map_node_key(dec->dequemap, (MapNode)node)))-1);
	DequeNode hold_this =  (DequeNode)map_find_node(dec->dequemap, position);
    free(position);
    return hold_this;
}

Pointer deque_node_value(Deque dec, DequeNode node) {
	return map_node_value(dec->dequemap, (MapNode)(node));
}

DequeNode deque_find_node(Deque dec, Pointer value, CompareFunc compare) {
	int i=0; // και εδω διατρεχω τα στοιχεια μεχρι να το βρω το value
	while (i<deque_size(dec)){ 
		if (compare(deque_get_at(dec, i), value)==0) {
            int* position = create_int(i+(*(int*)map_node_key(dec->dequemap, map_first(dec->dequemap))));
            DequeNode hold_this = (DequeNode)map_find_node(dec->dequemap, position); // εδω χρειαζομαι την σχετικη θεση
            free(position);
            return hold_this;
        }
        i++;
	}
	return NULL;
}