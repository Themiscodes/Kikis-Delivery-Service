///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Deque μέσω Dynamic Array.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTDeque.h"


// Το αρχικό μέγεθος που δεσμεύουμε
#define DEQUE_MIN_CAPACITY 10

// Ένα DequeNode είναι pointer σε αυτό το struct.
struct deque_node {
	Pointer value;				// Η τιμή του κόμβου.
};

// Ενα Deque είναι pointer σε αυτό το struct
struct deque {
	DequeNode array;			// Τα δεδομένα, πίνακας από struct deque_node
    int head;                   // το πρωτο στοιχειο, το "κεφαλι"
    int tail;                   // το τελευταιο στοιχειο, η "ουρα"
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει (το μέγεθος του array). Πάντα capacity >= size
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του vector.
};


Deque deque_create(int size, DestroyFunc destroy_value) {
	// Δημιουργία του struct
	Deque dec = malloc(sizeof(*dec));

	dec->size = size;
	dec->destroy_value = destroy_value;

	// Δέσμευση μνήμης για τον πίνακα. 
	
    dec->capacity = size < DEQUE_MIN_CAPACITY ? DEQUE_MIN_CAPACITY : size;
	dec->array = calloc(dec->capacity, sizeof(*dec->array));		// αρχικοποίηση σε 0 (NULL)

    // Αρχικοποιω να μη δειχνουν ακομα σε καποια θεση αλλα σαν -1 για να ξερω οτι ο πινακας ειναι αδειος, αργοτερα θα δειχνουν στο ιδιο με τη πρωτη εισαγωγη
    if (size==0) dec->head = -1; //αν ειναι οτιδηποτε αλλο απο 0 θα 
    else dec->head=0;
    dec->tail = size-1;

	return dec;
}

int deque_size(Deque dec) {
	return dec->size;
}

Pointer deque_get_at(Deque dec, int pos) {
	assert(pos >= 0 && pos < dec->size);	// LCOV_EXCL_LINE (αγνοούμε το branch από τα coverage reports, είναι δύσκολο να τεστάρουμε το false γιατί θα κρασάρει το test)

    if (dec->head<=dec->tail) // αν ειναι αριστερα το head απτο tail
        return dec->array[dec->head+pos].value; // αρκει να επιστρεψω την σχετικη σχεση ως προς το head
    else if (dec->capacity-dec->head > pos) // αν ειναι δεξια αλλα ζηταω μια θεση πριν το τελος του πινακα
        return dec->array[dec->head+pos].value; // το ιδιο και εδω
    else   // αν ομως ειναι μετα τοτε πρεπει να παω στην αρχη του πινακα να το βρω σε σχεση με τη διαφορα του head απο το τελος
        return dec->array[pos-(dec->capacity-dec->head)].value; //ΠΡΟΣΟΧΗ capacity kai oxi size για να παω στο τελος του πινακα

}

void deque_set_at(Deque dec, int pos, Pointer value) {
	assert(pos >= 0 && pos < dec->size);	// LCOV_EXCL_LINE

    int relative_position=0; // βρισκω την σχετικη θεση απο αυτη που ψαχνω
    if (dec->head<=dec->tail) // αν ειναι αριστερα το head απτο tail
        relative_position = dec->head+pos;
    else if (dec->capacity-dec->head > pos) // αν ειναι δεξια αλλα ψαχνω μια θεση πριν το τελος του πινακα
        relative_position = dec->head+pos;
    else   // αν ομως ειναι μετα τοτε πρεπει να παω στην αρχη του πινακα να το βρω σε σχεση με τη διαφορα του head απο το τελος
        relative_position=pos-(dec->capacity-dec->head);

	// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αντικαθίσταται
	if (value != dec->array[relative_position].value && dec->destroy_value != NULL)
		dec->destroy_value(dec->array[relative_position].value);

	
    dec->array[relative_position].value = value;

}

void deque_insert_first(Deque dec, Pointer value) {
   
    // πρωτα κοιταω αν ειναι γεματος ο πινακας
    if (dec->capacity == dec->size) {
        dec->capacity *= 2;
		dec->array = realloc(dec->array, dec->capacity * sizeof(*dec->array));

        if (dec->head!=0){  // αν δηλαδη το πρωτο στοιχειο ειναι δεξια του tail (μιας κ εχει γεμισει)
            for (int i=(dec->capacity/2)-1; i>=dec->head ; i--){ // μετακινω ολα τα στοιχεια αυτα
                dec->array[i+(dec->capacity/2)].value=dec->array[i].value;       // στο τελος του πινακα
                dec->array[i].value= NULL;                       // δεν καλω τη destroy value για αυτο το στοιχειο που δειχνει ο pointer για να μη διαγραφει
            }
            dec->head+=dec->capacity/2; // αλλαζω και το δεικτη να δειχνει στη σωστη νεα θεση
        }
        // αν ειναι κανονικα στην αρχη δεν χρειαζεται να κανω κατι
    }

    if (dec->head==-1){ //αν πρωτα ξεκιναμε να βαζουμε απτην αρχη 
        dec->head+=1;                            // αυξανω το δεικτη κατα μια θεση να δειχνει στο 0
        dec->array[dec->head].value= value;      // και το τοποθετω εκει 
        dec->tail=0;                             //και ο δεικτης της ουρας να δειχνει στο 0
    }
    else if (dec->head==0) {                         // αν ειναι στην αρχικη θεση ο δεικτης της αρχης
        dec->head= dec->capacity-1;             // βαζω να δειχνει στην τελικη του πινακα (ΠΡΟΣΟΧΗ capacity και οχι size εδω)
        dec->array[dec->head].value= value;     // και το τοποθετω εκει
    }
    else {                                      // αν οχι
        dec->head-=1;                           // μειωνω το δεικτη κατα μια θεση
        dec->array[dec->head].value= value;     // και το τοποθετω εκει
    }

    dec->size++;    //και αυξανω το size

}

void deque_insert_last(Deque dec, Pointer value) {
	// Μεγαλώνουμε τον πίνακα (αν χρειαστεί), ώστε να χωράει τουλάχιστον size στοιχεία
	// Διπλασιάζουμε κάθε φορά το capacity (σημαντικό για την πολυπλοκότητα!)
	if (dec->capacity == dec->size) {
		// Προσοχή: δεν πρέπει να κάνουμε free τον παλιό pointer, το κάνει η realloc
		dec->capacity *= 2;
		dec->array = realloc(dec->array, dec->capacity * sizeof(*dec->array));

        if (dec->head!=0){  // αν δηλαδη το πρωτο στοιχειο ειναι δεξια του tail (μιας κ εχει γεμισει)
            for (int i=(dec->capacity/2)-1; i>=dec->head ; i--){ // μετακινω ολα τα στοιχεια αυτα
                dec->array[i+(dec->capacity/2)].value=dec->array[i].value;       // στο τελος του πινακα
                dec->array[i].value= NULL;                       // δεν καλω τη destroy value για αυτο το στοιχειο που δειχνει ο pointer για να μη διαγραφει
            }
            dec->head+=dec->capacity/2; // αλλαζω και το δεικτη να δειχνει στη σωστη νεα θεση
        }
        // αν ειναι κανονικα στην αρχη δεν χρειαζεται να κανω κατι
	}
    
    //και προσθέτουμε το στοιχείο
    if (dec->tail==-1) {                         // αν ειναι δηλαδη το πρωτο στοιχειο που εισαγεται
        dec->tail+=1;                           // αυξανω το δεικτη κατα μια θεση να δειχνει στο 0
        dec->array[dec->tail].value= value;     // και το τοποθετω εκει 
        dec->head=0;                             //και ο δεικτης του κεφαλιου να δειχνει στο 0
    }
    else {                                      // αν οχι
        dec->tail+=1;                           // αυξανω το δεικτη κατα μια θεση
        dec->array[dec->tail].value= value;     // και το τοποθετω εκει 
    }

    dec->size++;    //και αυξανω το size

}

void deque_remove_first(Deque dec) {
    assert(dec->size != 0);		// LCOV_EXCL_LINE

	// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αφαιρείται
	if (dec->destroy_value != NULL)
		dec->destroy_value(dec->array[dec->head].value);

    if (dec->head==0){ //αν ειναι στην πρωτη θεση
        if (dec->head==dec->tail){ //αν ειναι και το tail εκει δηλαδη ειναι μονο ενα στοιχειο
            dec->tail=-1;           //βαζω -1 για να δειξω οτι πλεον ειναι αδειος ο πινακας
            dec->head=-1;           //το ιδιο και εδω
        }
        else 
            dec->head+=1; //αλλιως βαζω να δειχνει στο επομενο στοιχειο του πινακα
    }
    else {       //αν ειναι καπου αλλου 
        if(dec->head==dec->capacity-1){ //αν ειναι στο τελος
            if (dec->tail==dec->head){
                dec->tail=-1;           //βαζω -1 για να δειξω οτι πλεον ειναι αδειος ο πινακας
                dec->head=-1;           //το ιδιο και εδω
            }
            else
                dec->head=0;    // το επομενο ειναι η αρχη δηλαδη το 0
        }
        else    //αλλιως απλα το αυξανω να δειχνει στο επομενο
            dec->head+=1;
    }

	// Αφαιρούμε στοιχείο οπότε ο πίνακας μικραίνει
	dec->size--;

	// Μικραίνουμε τον πίνακα αν χρειαστεί, ώστε να μην υπάρχει υπερβολική σπατάλη χώρου.
	// Για την πολυπλοκότητα είναι σημαντικό να μειώνουμε το μέγεθος στο μισό, και μόνο
	// αν το capacity είναι τετραπλάσιο του size (δηλαδή το 75% του πίνακα είναι άδειος).

	if (dec->capacity > dec->size * 4 && dec->capacity > 2*DEQUE_MIN_CAPACITY) {
        if (dec->head!=0) { //αν δεν αρχιζει δηλαδη απτο πρωτο στοιχειο
            if(dec->tail<dec->head){ // αν εχει παει δηλαδη μονο μερος του πινακα προς το τελος
                for (int i=dec->capacity-1; i>=dec->head ; i--){ // μετακινω ολα τα στοιχεια αυτα
                    dec->array[i-(dec->capacity/2)].value=dec->array[i].value;       // στο τελος του μισου πινακα
                    dec->array[i].value= NULL;                       // δεν καλω τη destroy value για αυτο το στοιχειο που δειχνει ο pointer για να μη διαγραφει
                }
                dec->head=dec->capacity/2-dec->head; // αλλαζω και το δεικτη να δειχνει στη σωστη νεα θεση
            }
            else { // αν ολα τα στοιχεια εχουν μετακινηθει
                int k=0;
                for (int i =dec->head;i<1+dec->tail;i++){
                    dec->array[k].value=dec->array[i].value;       // τα μετακινω στην αρχη
                    dec->array[i].value= NULL;                       // δεν καλω τη destroy value για αυτο το στοιχειο που δειχνει ο pointer για να μη διαγραφει
                    k++;
                }
                dec->head=0;
                dec->tail=dec->size-1;
            }
        }
        // αν αρχιζε απτο πρωτο στοιχειο δεν χρειαζοταν να κανω κατi
        dec->capacity /= 2; 
        dec->array = realloc(dec->array, dec->capacity * sizeof(*dec->array));
    }

}

void deque_remove_last(Deque dec) {
	assert(dec->size != 0);		// LCOV_EXCL_LINE

	// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αφαιρείται
	if (dec->destroy_value != NULL)
		dec->destroy_value(dec->array[dec->tail].value);

    if (dec->tail==0){ //αν ειναι στην πρωτη θεση
        if (dec->head==dec->tail){ //αν ειναι και το head εκει δηλαδη ειναι μονο ενα στοιχειο
            dec->tail=-1;           //βαζω -1 για να δειξω οτι πλεον ειναι αδειος ο πινακας
            dec->head=-1;           //το ιδιο και εδω
        }
        else 
            dec->tail=dec->capacity-1; //αλλιως βαζω να δειχνει στο τελευταιο στοιχειο του πινακα
    }
    else        //αν ειναι καπου αλλου απλα το μειωνω να δειχνει στο προηγουμενο
        dec->tail-=1;   

	// Αφαιρούμε στοιχείο οπότε ο πίνακας μικραίνει
	dec->size--;

	// Μικραίνουμε τον πίνακα αν χρειαστεί, ώστε να μην υπάρχει υπερβολική σπατάλη χώρου.
	// Για την πολυπλοκότητα είναι σημαντικό να μειώνουμε το μέγεθος στο μισό, και μόνο
	// αν το capacity είναι τετραπλάσιο του size (δηλαδή το 75% του πίνακα είναι άδειος).

	if (dec->capacity > dec->size * 4 && dec->capacity > 2*DEQUE_MIN_CAPACITY) {
        if (dec->head!=0) { //αν δεν αρχιζει δηλαδη απτο πρωτο στοιχειο
            if(dec->tail<dec->head){ // αν εχει παει δηλαδη μονο μερος του πινακα προς το τελος
                for (int i=dec->capacity-1; i>=dec->head ; i--){ // μετακινω ολα τα στοιχεια αυτα
                    dec->array[i-(dec->capacity/2)].value=dec->array[i].value;       // στο τελος του μισου πινακα
                    dec->array[i].value= NULL;                       // δεν καλω τη destroy value για αυτο το στοιχειο που δειχνει ο pointer για να μη διαγραφει
                }
                dec->head=dec->capacity/2-dec->head; // αλλαζω και το δεικτη να δειχνει στη σωστη νεα θεση
            }
            else { // αν τα στοιχεια εχουν μετακινηθει
                int k=0;
                for (int i =dec->head;i<dec->tail+1;i++){
                    dec->array[k].value=dec->array[i].value;       // τα μετακινω στην αρχη
                    dec->array[i].value= NULL;                       // δεν καλω τη destroy value για αυτο το στοιχειο που δειχνει ο pointer για να μη διαγραφει
                    k++;
                }
                dec->head=0;
                dec->tail=dec->size-1;
            }
        }
        // αν αρχιζε απτο πρωτο στοιχειο δεν χρειαζοταν να κανω κατi
        dec->capacity /= 2; 
        dec->array = realloc(dec->array, dec->capacity * sizeof(*dec->array));
    }
}

Pointer deque_find(Deque dec, Pointer value, CompareFunc compare) {
	// Διάσχιση του deque , απλα χρειαζεται η get_at για να πηγαινω στις σωστες θεσεις
	for (int i = 0; i < dec->size; i++)
		if (compare(deque_get_at(dec, i), value) == 0)
			return deque_get_at(dec, i);		// βρέθηκε

	return NULL;				// δεν υπάρχει
}

DestroyFunc deque_set_destroy_value(Deque dec, DestroyFunc destroy_value) {
	DestroyFunc old = dec->destroy_value;
	dec->destroy_value = destroy_value;
	return old;
}

void deque_destroy(Deque dec) {
	// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για όλα τα στοιχεία
	if (dec->destroy_value != NULL)
		for (int i = 0; i < dec->size; i++)
			dec->destroy_value(deque_get_at(dec, i));

	// Πρέπει να κάνουμε free τόσο τον πίνακα όσο και το struct!
	free(dec->array);
	free(dec);			// τελευταίο το dec!
}


// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

DequeNode deque_first(Deque dec) {
	if (dec->size == 0)
		return DEQUE_BOF;
	else	
		return &dec->array[dec->head];
}

DequeNode deque_last(Deque dec) {
	if (dec->size == 0)
		return DEQUE_EOF;
	else
		return &dec->array[dec->tail];
}

DequeNode deque_next(Deque dec, DequeNode node) {
	if (node == &dec->array[dec->tail])
		return DEQUE_EOF;
	else if (node==&dec->array[dec->capacity-1]) // αν δηλαδη ειναι στο τελος του πινακα αλλα δεν ειναι η ουρα (αφου εχει αποριφθει αυτη η περιπτωση απο το πρωτο if)
                    return &dec->array[0]; // επιστρεφω το πρωτο στοιχειο του πινακα (το "πραγματικο" πρωτο)
    // σε ολες τις αλλες περιπτωσεις επιστρεφω το επομενο του node 
    return node +1;
}

DequeNode deque_previous(Deque dec, DequeNode node) {
	if (node == &dec->array[dec->head])
		return DEQUE_EOF;
	else if (node==&dec->array[0]) // αν δηλαδη ειναι στην αρχη του πινακα
                    return &dec->array[dec->capacity-1]; // επιστρεφω το τελευταιο στοιχειο του πινακα
    // σε ολες τις αλλες περιπτωσεις επιστρεφω το προηγουμενο του node 
    return node -1;
}

Pointer deque_node_value(Deque dec, DequeNode node) {
	return node->value;
}

DequeNode deque_find_node(Deque dec, Pointer value, CompareFunc compare) {
	// Διάσχιση του deque
	for (int i = 0; i < dec->size; i++){
		if (compare(deque_get_at(dec, i), value) == 0){
            int relative_position=0; // βρισκω την σχετικη θεση απο αυτη που βρηκα
            if (dec->head<=dec->tail) // αν ειναι αριστερα το head απτο tail
                relative_position = dec->head+i;
            else if (dec->capacity-dec->head > i) // αν ειναι δεξια αλλα ψαχνω μια θεση πριν το τελος του πινακα
                relative_position = dec->head+i;
            else   // αν ομως ειναι μετα τοτε πρεπει να παω στην αρχη του πινακα να το βρω σε σχεση με τη διαφορα του head απο το τελος
                relative_position=i-(dec->capacity-dec->head);
			return &dec->array[relative_position];		// βρέθηκε και το επιστρεφω
        }
    }

	return DEQUE_EOF;				// δεν υπάρχει
}