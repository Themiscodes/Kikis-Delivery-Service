////////////////////////////////////////////////////////////////////////
//
// ADT Graph
//
// Abstract μη κατευθυνόμενος γράφος με βάρη.
//
////////////////////////////////////////////////////////////////////////

#pragma once // #include το πολύ μία φορά

#include "common_types.h"
#include "ADTList.h"			// Ορισμένες συναρτήσεις επιστρέφουν λίστες
#include "ADTMap.h"				// Ορισμένες συναρτήσεις επιστρέφουν Map


// Ενα γράφος αναπαριστάται από τον τύπο Graph

typedef struct graph* Graph;

// Ενας κομβος με τη κορυφη καθως και τις προσπιπτουσες σε αυτο κορυφες

typedef struct vertex_node* VertexNode;

// Ενα ζεύγος κορυφών αναπαριστάται από τον τύπο GraphVertexPair

typedef struct graph_vertex_pair {
	Pointer vertex1;
	Pointer vertex2;
	Graph owner;			// ο γράφος στον οποίον "ανήκουν" οι κορυφές
} * GraphVertexPair;

// Δημιουργεί και επιστρέφει ένα γράφο, στον οποίο τα στοιχεία (οι κορυφές)
// συγκρίνονται με βάση τη συνάρτηση compare. Αν destroy_vertex != NULL, τότε
// καλείται destroy_vertex(vertex) κάθε φορά που αφαιρείται μια κορυφή.

Graph graph_create(CompareFunc compare, DestroyFunc destroy_vertex);

// Επιστρέφει τον αριθμό στοιχείων (κορυφών) που περιέχει ο γράφος graph.

int graph_size(Graph graph);

// Προσθέτει μια κορυφή στο γράφο.

void graph_insert_vertex(Graph graph, Pointer vertex);

// Επιστρέφει λίστα με όλες τις κορυφές του γράφου. Η λίστα δημιουργείται σε κάθε
// κληση και είναι ευθύνη του χρήστη να κάνει list_destroy.

List graph_get_vertices(Graph graph);

// Διαγράφει μια κορυφή από τον γράφο (αν υπάρχουν ακμές διαγράφονται επίσης).

void graph_remove_vertex(Graph graph, Pointer vertex);

// Προσθέτει μια ακμή με βάρος weight στο γράφο.

void graph_insert_edge(Graph graph, Pointer vertex1, Pointer vertex2, int weight);

// Αφαιρεί μια ακμή από το γράφο.

void graph_remove_edge(Graph graph, Pointer vertex1, Pointer vertex2);

// Επιστρέφει το βάρος της ακμής ανάμεσα στις δύο κορυφές, ή INT_MAX αν δεν είναι γειτονικές.

int graph_get_weight(Graph graph, Pointer vertex1, Pointer vertex2);

// Επιστρέφει λίστα με τους γείτονες μιας κορυφής. Η λίστα δημιουργείται σε κάθε
// κληση και είναι ευθύνη του χρήστη να κάνει list_destroy.

List graph_get_adjacent(Graph graph, Pointer vertex);

// Επιστρέφει το μήκος του συντομότερου μονοπάτι ανάμεσα σε κάθε ζευγάρι κορυφών του
// γράφου. Το Map που επιστρέφεται αντιστοιχεί:
//    Ζευγάρι κορυφών (GraphVertexPair)  =>  μήκος συντoμότερου μονοπατιού (int)
//
// Αν δεν υπάρχει κανένα μονοπάτι το αντίστοιχο μήκος είναι INT_MAX.
// Το Map δημιουργείται σε κάθε κληση και είναι ευθύνη του χρήστη να κάνει map_destroy.

Map graph_shortest_path_lengths(Graph graph);

// Επιστρέφει τον επομενο κομβο στο συντομότερο μονοπάτι ανάμεσα σε κάθε ζευγάρι κορυφών του
// γράφου. Το Map που επιστρέφεται αντιστοιχεί:
// Ζευγάρι κορυφών (GraphVertexPair)  =>  επομενη κορυφη στο μονοπατι (Pointer)

Map graph_shortest_path_nexts(Graph graph);

// Ελευθερώνει όλη τη μνήμη που δεσμεύει ο γράφος.
// Οποιαδήποτε λειτουργία πάνω στο γράφο μετά το destroy είναι μη ορισμένη.

void graph_destroy(Graph graph);


//// Για την περίπτωση που ο γράφος χρησιμοποιεί πίνακα κατακερματισμού

// Ορίζει τη συνάρτηση κατακερματισμού hash_func για το συγκεκριμένο γράφο.
// Πρέπει να κληθεί μετά την graph_create και πριν από οποιαδήποτε άλλη συνάρτηση.

void graph_set_hash_function(Graph graph, HashFunc hash_func);