/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω Hash Table με open Separate Chaining 
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTMap.h"
#include "ADTSet.h"


// Το μέγεθος του Hash Table ιδανικά θέλουμε να είναι πρώτος αριθμός σύμφωνα με την θεωρία.
// Η παρακάτω λίστα περιέχει πρώτους οι οποίοι έχουν αποδεδιγμένα καλή συμπεριφορά ως μεγέθη.
// Κάθε re-hash θα γίνεται βάσει αυτής της λίστας. Αν χρειάζονται παραπάνω απο 1610612741 στοχεία, τότε σε καθε rehash διπλασιάζουμε το μέγεθος.
int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};

// Αφου εχω separate chaining, οπότε σύμφωνα με την θεωρία, πρέπει πάντα να διατηρούμε
// τον load factor του  hash table μικρότερο ή ίσο του 0.9, για να έχουμε αποδoτικές πράξεις
#define MAX_LOAD_FACTOR 0.9


// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	Set *array;				// Ο πίνακας που θα χρησιμοποιήσουμε για το map
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει.
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	CompareFunc compare;		// Συνάρτηση για σύγκρηση δεικτών, που πρέπει να δίνεται απο τον χρήστη
	HashFunc hash_function;		// Συνάρτηση για να παίρνουμε το hash code του κάθε αντικειμένου.
	DestroyFunc destroy_key;	// Συναρτήσεις που καλούνται όταν διαγράφουμε έναν κόμβο απο το map.
	DestroyFunc destroy_value; 
};

Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	// Δεσμεύουμε κατάλληλα τον χώρο που χρειαζόμαστε για το hash table
	Map map = malloc(sizeof(*map));
	map->capacity = prime_sizes[0];
	map->array = malloc(map->capacity * sizeof(*(map->array)));
   
	// Αρχικοποιούμε τους κόμβους που έχουμε σαν διαθέσιμους.
	for (int i = 0; i < map->capacity; i++)
		map->array[i] = NULL;

	map->size = 0;
	map->compare = compare;
	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;

	return map;
}

// Επιστρέφει τον αριθμό των entries του map σε μία χρονική στιγμή.
int map_size(Map map) {
	return map->size;
}

// Συνάρτηση για την επέκταση του Hash Table σε περίπτωση που ο load factor μεγαλώσει πολύ.
static void rehash(Map map) {

	// Αποθήκευση των παλιών δεδομένων
	int old_capacity = map->capacity;
	Set* old_array = map->array;

	// Βρίσκουμε τη νέα χωρητικότητα, διασχίζοντας τη λίστα των πρώτων ώστε να βρούμε τον επόμενο. 
	int prime_no = sizeof(prime_sizes) / sizeof(int);	// το μέγεθος του πίνακα
	for (int i = 0; i < prime_no; i++) {					// LCOV_EXCL_LINE
		if (prime_sizes[i] > old_capacity) {
			map->capacity = prime_sizes[i]; 
			break;
		}
	}

	// Αν έχουμε εξαντλήσει όλους τους πρώτους, διπλασιάζουμε
	if (map->capacity == old_capacity)					// LCOV_EXCL_LINE
		map->capacity *= 2;								// LCOV_EXCL_LINE

	// Δημιουργούμε ένα μεγαλύτερο hash table
	map->array = malloc(map->capacity * sizeof(*(map->array)));
	for (int i = 0; i < map->capacity; i++)
		map->array[i] = NULL;

	// Τοποθετούμε ΜΟΝΟ τα entries που όντως περιέχουν ένα στοιχείο (το rehash είναι και μία ευκαιρία να ξεφορτωθούμε τα deleted nodes)
	map->size = 0;
	for (int i = 0; i < old_capacity; i++){
		if (old_array[i] != NULL){
			for (SetNode nodo = set_first(old_array[i]);
				nodo!=SET_EOF;
				nodo = set_next(old_array[i], nodo)
				){
				map_insert(map, set_node_key(old_array[i], nodo), set_node_value(old_array[i], nodo));
			}
		}
	}

	// Αποδεσμεύουμε το παλιό pointer στο set ώστε να μήν έχουμε leaks
	for (int i = 0; i < old_capacity; i++)
		if (old_array[i]!=NULL)
			free(old_array[i]);
	free(old_array);
}

// Εισαγωγή στο hash table του ζευγαριού (key, value)

void map_insert(Map map, Pointer key, Pointer value) {
	// Βρισκω τη θεση που κανει hash το key
	uint pos = map->hash_function(key) % map->capacity;	
	
	if (map->array[pos] == NULL) {
		map->array[pos] = set_create(map->compare, map->destroy_key, map->destroy_value);
		set_insert(map->array[pos], key, value);
		map->size++;
	}
	else {
		int current_set_size = set_size(map->array[pos]); // κραταω το μεγεθος του σετ σε αυτο το μπακερ
		set_insert(map->array[pos], key , value);		// ωστε αν εισαχθει σε που δεν υπαρχει κατι μεσα σε αυτο το κευ
		if (current_set_size != set_size(map->array[pos])) map->size++; //να αλλαξει και το size του map
	}

	// Αν με την νέα εισαγωγή ξεπερνάμε το μέγιστο load factor, πρέπει να κάνουμε rehash
	float load_factor = (float)map->size / map->capacity;
	if (load_factor > MAX_LOAD_FACTOR)
		rehash(map);
}

// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool map_remove(Map map, Pointer key) {
	uint pos = map->hash_function(key) % map->capacity;
	if(map->array[pos]== NULL) return false;
	if (set_remove(map->array[pos], key, set_find(map->array[pos], key))){
		map->size--;
		return true;
	}
	return false;
}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer map_find(Map map, Pointer key) {
	uint pos = map->hash_function(key) % map->capacity;
	if (map->array[pos]!=NULL) return set_find(map->array[pos], key);
	return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc old = map->destroy_key;
	map->destroy_key = destroy_key;
	return old;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	DestroyFunc old = map->destroy_value;
	map->destroy_value = destroy_value;
	return old;
}

// Απελευθέρωση μνήμης που δεσμεύει το map
void map_destroy(Map map) {
	for (int i = 0; i < map->capacity; i++)
		if (map->array[i]!=NULL)
			set_destroy(map->array[i]);

	free(map->array);
	free(map);
}

/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////

MapNode map_first(Map map) {
	//Ξεκινάμε την επανάληψή μας απο το 1ο στοιχείο, μέχρι να βρούμε κάτι όντως τοποθετημένο
	for (int i = 0; i < map->capacity; i++)
		if (map->array[i] != NULL&&set_size(map->array[i])!=0)
			return (MapNode)set_first(map->array[i]);
	return MAP_EOF;
}

MapNode map_next(Map map, MapNode node) {
	uint pos = map->hash_function(set_node_overload_key((SetNode)node)) % map->capacity;
	// Βρισκω το node αν υπαρχει σε ποιο μπακετ θα βρισκεται
	if (map->array[pos]!=NULL){
		if (set_next(map->array[pos], (SetNode)node)!=NULL){ // αν σε αυτο το set υπαρχει επομενο
						return (MapNode)set_next(map->array[pos], (SetNode)node); // το επιστρεφω
		}
		else { // αλλιως κοιταω αν υπαρχει στα επομενα buckets καποιο set με στοιχεια
			for (int j=pos+1;j<map->capacity;j++){
				if (map->array[j]!=NULL) // και αν ναι το node που ψαχνω θα ειναι
					return (MapNode)set_first(map->array[j]); // το πρωτο στοιχειο του set
			}
			return MAP_EOF;
		}
	}

	return MAP_EOF;
}

Pointer map_node_key(Map map, MapNode node) {
	return set_node_overload_key((SetNode)node);
}

Pointer map_node_value(Map map, MapNode node) {
	return set_node_overload_value((SetNode)node);
}

MapNode map_find_node(Map map, Pointer key) {
	uint pos = map->hash_function(key) % map->capacity;
	return (MapNode)set_find_node(map->array[pos], key);
}

// Αρχικοποίηση της συνάρτησης κατακερματισμού του συγκεκριμένου map.
void map_set_hash_function(Map map, HashFunc func) {
	map->hash_function = func;
}

uint hash_string(Pointer value) {
	// djb2 hash function, απλή, γρήγορη, και σε γενικές γραμμές αποδοτική
    uint hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}

uint hash_int(Pointer value) {
	return *(int*)value;
}

uint hash_pointer(Pointer value) {
	return (size_t)value;				// cast σε sizt_t, που έχει το ίδιο μήκος με έναν pointer
}