///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Vector μέσω Real-Time Dynamic Array.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTVector.h"


// Το αρχικό μέγεθος που δεσμεύουμε
#define VECTOR_MIN_CAPACITY 10

// Ένα VectorNode είναι pointer σε αυτό το struct. (το struct περιέχει μόνο ένα στοιχείο, οπότε θα μπροούσαμε και να το αποφύγουμε, αλλά κάνει τον κώδικα απλούστερο)
struct vector_node {
	Pointer value;				// Η τιμή του κόμβου.
};

// Ενα Vector είναι pointer σε αυτό το struct
struct vector {
	VectorNode array;			// Τα δεδομένα, πίνακας από struct vector_node
	VectorNode array_X;			// O πινακας που θα κραταει τον παλιο πινακα μεχρι να αντιγραφουν ολα τα στοιχεια στον καινουριο με καθε insert
	int position_X;				// Μεχρι ποιο στοιχειο εχουμε περασει στον καινουριο πινακα
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει (το μέγεθος του array). Πάντα capacity >= size, αλλά μπορεί να έχουμε
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του vector.
};


Vector vector_create(int size, DestroyFunc destroy_value) {
	// Δημιουργία του struct
	Vector vec = malloc(sizeof(*vec));

	vec->size = size;
	vec->destroy_value = destroy_value;

	// Δέσμευση μνήμης για τον πίνακα. Αρχικά το vector περιέχει size
	// μη-αρχικοποιημένα στοιχεία, αλλά εμείς δεσμεύουμε xώρο για τουλάχιστον
	// VECTOR_MIN_CAPACITY για να αποφύγουμε τα πολλαπλά resizes.
	
	vec->capacity = size < VECTOR_MIN_CAPACITY ? VECTOR_MIN_CAPACITY : size;
	vec->array = calloc(vec->capacity, sizeof(*vec->array));		// αρχικοποίηση σε 0 (NULL)
	vec->array_X = NULL;
	vec->position_X=size; 

	return vec;
}

int vector_size(Vector vec) {
	return vec->size;
}

Pointer vector_get_at(Vector vec, int pos) {
	assert(pos >= 0 && pos < vec->size);	// LCOV_EXCL_LINE (αγνοούμε το branch από τα coverage reports, είναι δύσκολο να τεστάρουμε το false γιατί θα κρασάρει το test)

	if ((vec->size<VECTOR_MIN_CAPACITY)||((vec->size>VECTOR_MIN_CAPACITY-1)&&((pos>vec->capacity/2-1)||(pos<vec->position_X+1)))){ // αν ειναι μετα την μεση κοιταω στον καινουριο
		// ή και αν ειναι μικροτερο απο τον αριθμο των στοιχειων που εχουν αντιγραφει ηδη
		// αν αυτο το στοιχειο εχει ηδη αντιγραφει στον καινουριο πινακα κοιταω σε αυτον για να εξασφαλισω αν αλλαξει οτι θα εχω τη σωστη του τιμη
		return vec->array[pos].value;
	} 
	else // αλλιως κοιταω στον παλιο για αυτο το στοιχειο
		return vec->array_X[pos].value;
}

void vector_set_at(Vector vec, int pos, Pointer value) {
	assert(pos >= 0 && pos < vec->size);	// LCOV_EXCL_LINE

	if ((vec->size<VECTOR_MIN_CAPACITY)||((vec->size>VECTOR_MIN_CAPACITY-1)&&((pos>vec->capacity/2-1)||(pos<vec->position_X+1)))){ // αν ειναι μετα την μεση κοιταω στον καινουριο
		// ή και αν ειναι μικροτερο απο τον αριθμο των στοιχειων που εχουν αντιγραφει ηδη
		// αν αυτο το στοιχειο εχει ηδη αντιγραφει στον καινουριο πινακα κοιταω σε αυτον για να εξασφαλισω αν αλλαξει οτι θα εχω τη σωστη του τιμη

		// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αντικαθίσταται
		// Κανω την αλλαγη στον πινακα
		if (value != vec->array[pos].value && vec->destroy_value != NULL)
			vec->destroy_value(vec->array[pos].value);

		// και βαζουμε το νεο στοιχειο σε αυτη τη θεση και στους δυο πινακες
		vec->array[pos].value = value;

	}
	else { // αλλιως αν δεν εχει αντιγραφει ηδη στον καινουριο κοιταω στον παλιο για αυτο το στοιχειο

		// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αντικαθίσταται
		if (value != vec->array_X[pos].value && vec->destroy_value != NULL)
			vec->destroy_value(vec->array_X[pos].value);

		// και βαζουμε το νεο στοιχειο σε αυτη τη θεση
		vec->array_X[pos].value = value;
	}
}

void vector_insert_last(Vector vec, Pointer value) {
	// Μεγαλώνουμε τον πίνακα (αν χρειαστεί), ώστε να χωράει τουλάχιστον size στοιχεία
	// Διπλασιάζουμε κάθε φορά το capacity (σημαντικό για την πολυπλοκότητα!)

	if (vec->capacity == vec->size) {
		// Προσοχή: δεν πρέπει να κάνουμε free τον παλιό pointer, το κάνει η realloc
		vec->capacity *= 2;
		// ελευθερωνω τον παλιο αφου τα εχω τα στοιχεια του τωρα πλεον στον array 
		free(vec->array_X);
		 // βαζω τον array_X να δειχνει στον παλιο
		vec->array_X=vec->array;
		// και φτιαχνω εναν νεο κενο πινακα
		vec->array = calloc(vec->capacity, sizeof(*vec->array)); // o νεος διπλασιος πινακας
	}

	//προσθέτουμε το στοιχείο, θα δειχνει στη σωστη θεση με το size ακομα και αν εχει αυξηθει και δημιουργηθει ο καινουριος πινακας
	vec->array[vec->size].value = value;

	// προσθετω και το αντιστοιχο στοιχειο στα κενα μερη του πινακα απτον παλιο πινακα αν εχει γινει resize
	if ((vec->size>VECTOR_MIN_CAPACITY-1)&&(vec->size>(vec->capacity/2-1))) {
		vec->array[vec->size-vec->capacity/2] = vec->array_X[vec->size-vec->capacity/2];
		vec->position_X = vec->size-vec->capacity/2; // η θεση μεχρι την οποια εχω αντιγραψει στοιχεια
	}

	// αυξανουμε και τον counter
	vec->size++;
}

void vector_remove_last(Vector vec) {
	assert(vec->size != 0);		// LCOV_EXCL_LINE

	if ((vec->size<VECTOR_MIN_CAPACITY)||((vec->size-1>vec->capacity/2)||(vec->size-1<vec->position_X+1))){ // αν ειναι στον καινουριο

		// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αφαιρείται
		if (vec->destroy_value != NULL)
			vec->destroy_value(vec->array[vec->size - 1].value);

	}
	else { // αλλιως αν εχουμε σβησει ολα απο τις θεσεις του καινουριου σβηνουμε το τελευταιο στον παλιο που δεν εχει μεταφερθει ακομα

		// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αφαιρείται
		if (vec->destroy_value != NULL)
			vec->destroy_value(vec->array_X[vec->size - 1].value);
	}

	// Αφαιρούμε στοιχείο οπότε ο πίνακας μικραίνει
	vec->size--;

	// Μικραίνουμε τον πίνακα αν χρειαστεί, ώστε να μην υπάρχει υπερβολική σπατάλη χώρου.
	// Για την πολυπλοκότητα είναι σημαντικό να μειώνουμε το μέγεθος στο μισό, και μόνο
	// αν το capacity είναι τετραπλάσιο του size (δηλαδή το 75% του πίνακα είναι άδειος).

	if (vec->capacity > vec->size * 4 && vec->capacity > 2*VECTOR_MIN_CAPACITY) {
		// δεν χρειαζεται να κανω κατι εδω παρα να σβησω το μισο πινακα 		
		vec->capacity /= 2;
		vec->array = realloc(vec->array, vec->capacity * sizeof(*vec->array)); 
		vec->array_X = realloc(vec->array_X, (vec->capacity/2) * sizeof(*vec->array_X));
	}
}

Pointer vector_find(Vector vec, Pointer value, CompareFunc compare) {
	// Διάσχιση του vector
	for (int i = 0; i < vec->size; i++){
		if ((vec->size<VECTOR_MIN_CAPACITY)||((vec->size>VECTOR_MIN_CAPACITY)&&((i>vec->capacity/2)||(i<vec->position_X+1)))){
			if (compare(vec->array[i].value, value) == 0)
				return vec->array[i].value;		// βρέθηκε
		}
		else // αν δεν εχει ακομα αντιγραφει κοιταω στον παλιο πινακα
			if (compare(vec->array_X[i].value, value) == 0)
				return vec->array_X[i].value;		// βρέθηκε
	}
	return NULL;				// δεν υπάρχει
}

DestroyFunc vector_set_destroy_value(Vector vec, DestroyFunc destroy_value) {
	DestroyFunc old = vec->destroy_value;
	vec->destroy_value = destroy_value;
	return old;
}

void vector_destroy(Vector vec) {
	// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για όλα τα στοιχεία
	if (vec->destroy_value != NULL) {
		for (int i = 0; i < vec->size; i++){ 
			if ((vec->size<VECTOR_MIN_CAPACITY)||((vec->size>VECTOR_MIN_CAPACITY)&&((i>vec->capacity/2)||(i<vec->position_X+1)))){
				vec->destroy_value(vec->array[i].value);
			}
			else vec->destroy_value(vec->array_X[i].value);
		}
	}

	// Πρέπει να κάνουμε free τόσο τον πίνακα όσο και το struct!
	free(vec->array);
	free(vec->array_X);
	free(vec);			// τελευταίο το vec!
}


// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

VectorNode vector_first(Vector vec) {
	if (vec->size == 0)
		return VECTOR_BOF;
	else
		return &vec->array[0];
}

VectorNode vector_last(Vector vec) {
	if (vec->size == 0)
		return VECTOR_EOF;
	else {
		if ((vec->size<VECTOR_MIN_CAPACITY)||((vec->size>VECTOR_MIN_CAPACITY)&&((vec->size>vec->capacity/2)||(vec->size-1<vec->position_X+1))))
			return &vec->array[vec->size-1];
		else	
			return &vec->array_X[vec->size-1];
	}
}

VectorNode vector_next(Vector vec, VectorNode node) {
	if (node == &vec->array[vec->size-1])
		return VECTOR_EOF;
	else if (vec->size>VECTOR_MIN_CAPACITY-1){ 				// οι περιπτωσεις που με ενδιαφερουν ειναι 
			if (node==&vec->array[vec->position_X]) 		// η θεση μεχρι την οποια εχει αντιγραφει 
				return &vec->array_X[vec->position_X+1];	// και τοτε παω στον προηγουμενο πινακα για να βρω το επομενο
			if (node==&vec->array_X[vec->capacity/2-1])		// το τελος του παλιου πινακα
				return &vec->array[vec->capacity/2];		// και τοτε παω στον καινουριο πινακα για να βρω το επομενο
	}
	return node +1;			// σε ολες τις αλλες περιπτωσεις αρκει να επιστρεψω το επομενο στον πινακα
}

VectorNode vector_previous(Vector vec, VectorNode node) {
	if (node == &vec->array[0])
		return VECTOR_EOF;
	else if (vec->size>VECTOR_MIN_CAPACITY-1){ 				// οι περιπτωσεις που με ενδιαφερουν ειναι 
			if (node==&vec->array_X[vec->position_X+1]) 	// η θεση μετα απο την τελευταια που εχει αντιγραφει 
				return &vec->array[vec->position_X];		// και τοτε παω στον καινουριο πινακα για να βρω το προηγουμενο
			if (node==&vec->array[vec->capacity/2])			// η μεση του καινουριου πινακα
				return &vec->array_X[vec->capacity/2-1];	// και τοτε παω στον παλιο πινακα για να βρω το προηγουμενο
	}
	return node-1;			// σε ολες τις αλλες περιπτωσεις αρκει να επιστρεψω το προηγουμενο του πινακα
}

Pointer vector_node_value(Vector vec, VectorNode node) {
	return node->value;
}

VectorNode vector_find_node(Vector vec, Pointer value, CompareFunc compare) {
	// Διάσχιση του vector
	for (int i = 0; i < vec->size; i++){
		if ((vec->size<VECTOR_MIN_CAPACITY)||((vec->size>VECTOR_MIN_CAPACITY)&&((i>vec->capacity/2)||(i<vec->position_X+1)))){
			if (compare(vec->array[i].value, value) == 0)
				return &vec->array[i];		// βρέθηκε
		}
		else // αν δεν εχει ακομα αντιγραφει κοιταω στον παλιο πινακα
			if (compare(vec->array_X[i].value, value) == 0)
				return &vec->array_X[i];		// βρέθηκε
	}

	return VECTOR_EOF;				// δεν υπάρχει
}