//////////////////////////////////////////////////////////////////
//
// Unit tests για τον ADT Graph.
// Οποιαδήποτε υλοποίηση οφείλει να περνάει όλα τα tests.
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "ADTGraph.h"

// Μια int compare συνάρτηση
int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}

// Επιστρέφει έναν ακέραιο σε νέα μνήμη με τιμή value
int* create_integ(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}

// Βοηθητική συνάρτηση για το ανακάτεμα του πίνακα τιμών
void shuffle(int* array[], int n) {
	for (int i = 0; i < n; i++) {
		int j = i + rand() / (RAND_MAX / (n - i) + 1);
		int* t = array[j];
		array[j] = array[i];
		array[i] = t;
	}
}

void test_create(void) {

	// Δημιουργια κενου γραφου (χωρίς free)
	Graph graph = graph_create(compare_ints, NULL);
	graph_set_hash_function(graph, hash_pointer);

	// Ελέγχος οτι αρχικοποιείται με μέγεθος 0
	TEST_ASSERT(graph != NULL);
	TEST_ASSERT(graph_size(graph) == 0);

	graph_destroy(graph);
}


void test_insert_vertices(void) {

	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_pointer);

	int N = 100;
	int** array = malloc(N * sizeof(*array));

	for (int i = 0; i < N; i++) {
		array[i] = create_integ(i);
	}

	// Ανακατεύουμε το array ώστε να υπάρχει ομοιόμορφη εισαγωγή τιμών
	shuffle(array, N);

	// Δοκιμαζω την insert εισάγοντας κάθε φορά νέες κορυφες
	for (int i = 0; i < N; i++) {
		graph_insert_vertex(graph, array[i]);
		TEST_ASSERT(graph_size(graph) == (i + 1)); 
	}

	// Δοκιμαζω να εισαγω μια κορυφη η οποια υπαρχει ηδη στο γραφο
	graph_insert_vertex(graph, array[3]);
	TEST_ASSERT(graph_size(graph) == N);  // και ελεγχω οτι δεν εισαγεται παλι

	// Δοκιμη οτι και η list_of_vertices επιστρεφει ολες τις κορυφες (η ταξινομηση γινεται με βαση τους pointers γιαυτο δεν ελεγχω την σειρα)
	List list_of_vertices = graph_get_vertices(graph);
	TEST_ASSERT(list_size(list_of_vertices)==N);
	int i=0;
	for(ListNode nodo= list_first(list_of_vertices); nodo!=LIST_EOF; nodo=list_next(list_of_vertices, nodo)){
		i++;
	}
	TEST_ASSERT(i==N);
	list_destroy(list_of_vertices);

	graph_destroy(graph);
}

void test_insert_edges(void) {

	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_pointer);

	Pointer vertex1 = create_integ(1);
	Pointer vertex2 = create_integ(2);
	Pointer vertex3 = create_integ(3);
	Pointer vertex4 = create_integ(4);
	Pointer vertex5 = create_integ(5);
	Pointer vertex6 = create_integ(6);
	Pointer vertex7 = create_integ(7);
	Pointer vertex8 = create_integ(8);

	// Δοκιμη εισαγωγης ακμης
	graph_insert_edge(graph, vertex1, vertex2, 12);
	TEST_ASSERT(graph_size(graph)==2); // ελεγχος οτι μπηκαν και οι δτο κορυφες
	TEST_ASSERT(graph_get_weight(graph, vertex1, vertex2)==12); // οτι εχει το σωστο βαρος
	
	graph_insert_vertex(graph, vertex3); // εισαγω μια κορυφη χωρις καποια ακμη
	TEST_ASSERT(graph_get_weight(graph, vertex3, vertex2)==INT32_MAX); // ελεγχω οτι δεν ειναι συνδεδεμενη με τη 2
	List vertex3_adjacents = graph_get_adjacent(graph, vertex3);
	TEST_ASSERT(list_size(vertex3_adjacents)==0);
	list_destroy(vertex3_adjacents);
	TEST_ASSERT(graph_size(graph)==3); // ομως εχω τωρα τρεις κορυφες στο γραφο

	graph_insert_edge(graph, vertex3, vertex2, 5); //συνδεω με ακμη τη 3 με 2
	TEST_ASSERT(graph_get_weight(graph, vertex3, vertex2)==5); // ελεγχω οτι ενημερωθηκε
	TEST_ASSERT(graph_size(graph)==3); // αλλα οτι δεν αλλαξε το size

	graph_insert_edge(graph, vertex4, vertex1, 2);
	TEST_ASSERT(graph_get_weight(graph, vertex4, vertex1)==2); // ελεγχω οτι ενημερωθηκε
	graph_insert_edge(graph, vertex5, vertex6, 4);
	TEST_ASSERT(graph_get_weight(graph, vertex5, vertex6)==4); // ελεγχω οτι ενημερωθηκε
	graph_insert_edge(graph, vertex6, vertex1, 2);
	TEST_ASSERT(graph_get_weight(graph, vertex6, vertex1)==2); // ελεγχω οτι ενημερωθηκε
	graph_insert_edge(graph, vertex7, vertex8, 4);
	TEST_ASSERT(graph_get_weight(graph, vertex7, vertex8)==4); // ελεγχω οτι ενημερωθηκε
	TEST_ASSERT(graph_size(graph)==8); // εχουν εισαχθει ολες σωστα

	//Με τη χρηση της graph_get_adjacent ελεγχω και οτι οι προσπιπτουσες ακμες ειναι σωστες
	List vertex1_adjacents = graph_get_adjacent(graph, vertex1);
	TEST_ASSERT(list_size(vertex1_adjacents)==3);
	list_destroy(vertex1_adjacents);

	List vertex2_adjacents = graph_get_adjacent(graph, vertex2);
	TEST_ASSERT(list_size(vertex2_adjacents)==2);
	list_destroy(vertex2_adjacents);

	List vertex7_adjacents = graph_get_adjacent(graph, vertex7);
	TEST_ASSERT(list_size(vertex7_adjacents)==1);
	list_destroy(vertex7_adjacents);

	graph_destroy(graph);

}

void test_remove_vertices(void) {

	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_pointer);

	Pointer vertex1 = create_integ(1);
	Pointer vertex2 = create_integ(2);
	Pointer vertex3 = create_integ(3);
	Pointer vertex4 = create_integ(4);

	graph_insert_edge(graph, vertex1, vertex2, 33); // εισαγω ακμες (κ ταυτοχρονα δλδ τις κορυφες)
	graph_insert_edge(graph, vertex2, vertex3, 5);
	graph_insert_vertex(graph, vertex4); // εισαγω αυτη την κορυφη μονη της χωρις συνδεση
	TEST_ASSERT(graph_size(graph)==4);

	graph_remove_vertex(graph, vertex4); // ελεγχος αφαιρεσης κορυφης
	TEST_ASSERT(graph_size(graph)==3);

	List vertex2_adjacents = graph_get_adjacent(graph, vertex2);
	TEST_ASSERT(list_size(vertex2_adjacents)==2);
	list_destroy(vertex2_adjacents);
	graph_remove_vertex(graph, vertex2); // ελεγχος αφαιρεσης κορυφης που συνδεεται με αλλη
	TEST_ASSERT(graph_size(graph)==2); // και οτι ο γραφος ενημερωθηκε
	TEST_ASSERT(graph_get_weight(graph, vertex1, vertex2)==INT32_MAX); // οσο και η ακμη οτι δεν υπαρχει πια
	List vertex1_adjacents = graph_get_adjacent(graph, vertex1); //και ουτε στο μαπ της κορυφης που κραταει τα adjacents 
	TEST_ASSERT(list_size(vertex1_adjacents)==0); // δεν υπαρχει πια καποια συνδεση
	list_destroy(vertex1_adjacents);

	graph_destroy(graph);

}

void test_remove_edges(void) {

	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_pointer);

	Pointer vertex1 = create_integ(1);
	Pointer vertex2 = create_integ(2);
	Pointer vertex3 = create_integ(3);
	Pointer vertex4 = create_integ(4);
	Pointer vertex5 = create_integ(5);
	Pointer vertex6 = create_integ(6);

	graph_insert_edge(graph, vertex1, vertex2, 33); // εισαγω ακμες (κ ταυτοχρονα δλδ τις κορυφες)
	graph_insert_edge(graph, vertex2, vertex3, 5);
	graph_insert_vertex(graph, vertex4); // εισαγω αυτη την κορυφη μονη της χωρις συνδεση
	graph_insert_vertex(graph, vertex5); // εισαγω αυτες τις κορυφες
	graph_insert_vertex(graph, vertex6); 
	graph_insert_edge(graph, vertex5, vertex6, 1); // και μετα τις συνδεω μεταξυ τους
	graph_insert_edge(graph, vertex5, vertex3, 23); // και με τοους αλλους κομβους

	TEST_ASSERT(graph_size(graph)==6);

	List vertex2_adjacents = graph_get_adjacent(graph, vertex2);
	TEST_ASSERT(list_size(vertex2_adjacents)==2);
	list_destroy(vertex2_adjacents);
	graph_remove_edge(graph, vertex1, vertex2); // αφαιρω την ακμη που ενωνει 1 και 2
	List vertex1_adjacents = graph_get_adjacent(graph, vertex1); // κοιταω οτι ενημερωθηκε στην 1
	TEST_ASSERT(list_size(vertex1_adjacents)==0); // και δεν εχει πια συνδεσεις 
	list_destroy(vertex1_adjacents);
	TEST_ASSERT(graph_size(graph)==6); // αλλα οτι δεν εχει σβησει καποια κορυφη

	List vertex3_adjacents = graph_get_adjacent(graph, vertex3); // κοιταω οτι ενημερωθηκε στην 5
	TEST_ASSERT(list_size(vertex3_adjacents)==2); // και εχει μονο μια ακμη πλεον με την 6
	list_destroy(vertex3_adjacents);

	graph_remove_edge(graph, vertex3, vertex5); // αφαιρω την ακμη που ενωνει 3 και 5 δοκιμαζωντας κ με αλλη σειρα
	List vertex5_adjacents = graph_get_adjacent(graph, vertex5); // κοιταω οτι ενημερωθηκε στην 5
	TEST_ASSERT(list_size(vertex5_adjacents)==1); // και εχει μονο μια ακμη πλεον με την 6
	list_destroy(vertex5_adjacents);

	graph_destroy(graph);

}

void test_get_weight(void) {

	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_pointer);

	Pointer vertex1 = create_integ(1);
	Pointer vertex2 = create_integ(2);
	Pointer vertex3 = create_integ(3);

	graph_insert_edge(graph, vertex1, vertex2, 4); // εισαγω ακμες με βαρη 4
	graph_insert_edge(graph, vertex2, vertex3, 5); // και 5

	TEST_ASSERT(graph_get_weight(graph, vertex1, vertex2)==4); //ελεγχω για αυτες που συνδεονται οτι ειναι σωστα τα βαρη
	TEST_ASSERT(graph_get_weight(graph, vertex3, vertex2)==5);
	TEST_ASSERT(graph_get_weight(graph, vertex1, vertex3)==INT32_MAX); //ελεγχω για αυτες που συνδεονται οτι ειναι INT_MAX
	TEST_ASSERT(graph_get_weight(graph, vertex2, vertex2)==0); //και με τον εαυτο της μια κορυφη εχει βαρος 0

	graph_destroy(graph);	

}

//Τεστ για το

void test_graph_shortest_path_lengths(void) {

	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_pointer);

	// Οι κορυφες
	Pointer vertex1 = create_integ(1);
	Pointer vertex2 = create_integ(2);
	Pointer vertex3 = create_integ(3);
	Pointer vertex4 = create_integ(4);
	Pointer vertex5 = create_integ(5);
	Pointer vertex6 = create_integ(6);

	// Εισαγωγη των κορυφων με τις ακμες τους
	graph_insert_edge(graph, vertex1, vertex2, 1);  
	graph_insert_edge(graph, vertex1, vertex3, 5); 
	graph_insert_edge(graph, vertex1, vertex5, 4);  
	graph_insert_edge(graph, vertex2, vertex3, 12); 
	graph_insert_edge(graph, vertex2, vertex4, 2);  
	graph_insert_edge(graph, vertex3, vertex5, 7);  
	graph_insert_edge(graph, vertex4, vertex3, 3); 
	graph_insert_edge(graph, vertex5, vertex2, 7);  
	graph_insert_vertex(graph, vertex6);

	// Μερικα τεστ οτι ολα εγιναν σωστα
	TEST_ASSERT(graph_size(graph)==6);
	TEST_ASSERT(list_size(graph_get_vertices(graph))==6);
	TEST_ASSERT(list_size(graph_get_adjacent(graph,vertex3))==4);
	TEST_ASSERT(list_size(graph_get_adjacent(graph,vertex1))==3);

	Map shortest_path_floyd_warshall = graph_shortest_path_lengths(graph);

	// Αρχικα ελεγχω οτι εχει 5Χ4/2=10 ακμες το map αφου συνδεωνται ολες οι κορυφες
	// και οτι για την εκτη κορυφη που δεν συνδεεται με καποια οτι δεν εχει μπει αυτη η ακμη
	TEST_ASSERT(map_size(shortest_path_floyd_warshall)==10); 

	// Εξεταζω οτι ολα τα μηκη εχουν υπολογιστει σωστα δηλαδη για αυτα που το μικροτερο μονοπατι ηταν απευθειας οπως το 1 -> 2
	// και για αυτα που ειναι μεσω καποιων αλλων κορυφων οτι και αυτα εχουν υπολογιστει ολα σωστα
	// εξεταζωντας μερικες απο αυτες τις ακμες οπως την 2->3 που ενω αμεσα το weight ειναι 12 μεσω της 2->4->3 γινεται 5
	for(MapNode nodo = map_first(shortest_path_floyd_warshall); nodo!=MAP_EOF; nodo=map_next(shortest_path_floyd_warshall, nodo) ){
		if(((*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex2)==1)&&(*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex1)==2){
			TEST_ASSERT((*(int*)map_node_key(shortest_path_floyd_warshall, nodo))==1);
		}
		if(((*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex2)==2)&&(*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex1)==1){
			TEST_ASSERT((*(int*)map_node_key(shortest_path_floyd_warshall, nodo))==1);
		}
		if(((*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex2)==2)&&(*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex1)==3){
			TEST_ASSERT((*(int*)map_node_key(shortest_path_floyd_warshall, nodo))==5);
		}
		if(((*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex2)==3)&&(*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex1)==2){
			TEST_ASSERT((*(int*)map_node_key(shortest_path_floyd_warshall, nodo))==5);
		}
		if(((*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex2)==3)&&(*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex1)==4){
			TEST_ASSERT((*(int*)map_node_key(shortest_path_floyd_warshall, nodo))==3);
		}
		if(((*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex2)==4)&&(*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex1)==3){
			TEST_ASSERT((*(int*)map_node_key(shortest_path_floyd_warshall, nodo))==3);
		}
		if(((*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex2)==4)&&(*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex1)==5){
			TEST_ASSERT((*(int*)map_node_key(shortest_path_floyd_warshall, nodo))==7);
		}
		if(((*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex2)==5)&&(*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex1)==4){
			TEST_ASSERT((*(int*)map_node_key(shortest_path_floyd_warshall, nodo))==7);
		}
		if(((*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex2)==1)&&(*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex1)==5){
			TEST_ASSERT((*(int*)map_node_key(shortest_path_floyd_warshall, nodo))==4);
		}
		if(((*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex2)==5)&&(*(int*)((GraphVertexPair)map_node_value(shortest_path_floyd_warshall, nodo))->vertex1)==1){
			TEST_ASSERT((*(int*)map_node_key(shortest_path_floyd_warshall, nodo))==4);
		}
	}
	
	map_destroy(shortest_path_floyd_warshall);
	
	graph_destroy(graph);	

}

// Παρομοιο τεστ με πριν με τον ιδιο γραφο αλλα με ελεγχους τωρα για αν εχει τους σωστους κομβους 
// ως επομενους στο βελτιστο μονοπατι της καθε κορυφης

void test_graph_shortest_path_nexts(void) {

	Graph graph = graph_create(compare_ints, free);
	graph_set_hash_function(graph, hash_pointer);

	// Οι κορυφες
	Pointer vertex1 = create_integ(1);
	Pointer vertex2 = create_integ(2);
	Pointer vertex3 = create_integ(3);
	Pointer vertex4 = create_integ(4);
	Pointer vertex5 = create_integ(5);
	Pointer vertex6 = create_integ(6);

	// Εισαγωγη των κορυφων με τις ακμες τους
	graph_insert_edge(graph, vertex1, vertex2, 1);  
	graph_insert_edge(graph, vertex1, vertex3, 5); 
	graph_insert_edge(graph, vertex1, vertex5, 4);  
	graph_insert_edge(graph, vertex2, vertex3, 12); 
	graph_insert_edge(graph, vertex2, vertex4, 2);  
	graph_insert_edge(graph, vertex3, vertex5, 7);  
	graph_insert_edge(graph, vertex4, vertex3, 3); 
	graph_insert_edge(graph, vertex5, vertex2, 7);  
	graph_insert_vertex(graph, vertex6);

	// Μερικα τεστ οτι ολα εγιναν σωστα
	TEST_ASSERT(graph_size(graph)==6);
	TEST_ASSERT(list_size(graph_get_vertices(graph))==6);
	TEST_ASSERT(list_size(graph_get_adjacent(graph,vertex3))==4);
	TEST_ASSERT(list_size(graph_get_adjacent(graph,vertex1))==3);

	Map shortest_path_next = graph_shortest_path_nexts(graph);

	// Αρχικα ελεγχω οτι εχει 5Χ4=20 ακμες το map αφου εδω εχω ολα τα μονοπατια
	// και οτι για την εκτη κορυφη που δεν συνδεεται με καποια οτι δεν εχει μπει αυτη η ακμη
	TEST_ASSERT(map_size(shortest_path_next)==20); 

	// Εξεταζω οτι οι επομενοι κομβοι ειναι σωστοι
	for(MapNode nodo = map_first(shortest_path_next); nodo!=MAP_EOF; nodo=map_next(shortest_path_next, nodo) ){
		// Για να παω απο το 1 στο 2 επομενος κομβος ειναι το 2 δν υπαρχει αλλο συντομοτερο
		if(((*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex1)==1)&&(*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex2)==2){
			TEST_ASSERT((*(int*)map_node_value(shortest_path_next, nodo))==2);
		}
		// Για να παω απο το 2 στο 1 επομενος κομβος ειναι αντιστοιχα το 1
		if(((*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex1)==2)&&(*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex2)==1){
			TEST_ASSERT((*(int*)map_node_value(shortest_path_next, nodo))==1);
		}
		// Για να παω απο το 2 στο 3 επομενος κομβος στο συντομοτερο μονοπατι ειναι το 4
		if(((*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex1)==2)&&(*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex2)==3){
			TEST_ASSERT((*(int*)map_node_value(shortest_path_next, nodo))==4);
		}
		// Για να παω απο το 5 στο 2 επομενος κομβος στο συντομοτερο μονοπατι ειναι το 1
		if(((*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex1)==5)&&(*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex2)==2){
			TEST_ASSERT((*(int*)map_node_value(shortest_path_next, nodo))==1);
		}
		// Για να παω απο το 4 στο 3 επομενος κομβος στο συντομοτερο μονοπατι ειναι το 3
		if(((*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex1)==4)&&(*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex2)==3){
			TEST_ASSERT((*(int*)map_node_value(shortest_path_next, nodo))==3);
		}
		// Για να παω απο το 3 στο 2 ειναι μεσω του 4
		if(((*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex1)==3)&&(*(int*)((GraphVertexPair)map_node_key(shortest_path_next, nodo))->vertex2)==2){
			TEST_ASSERT((*(int*)map_node_value(shortest_path_next, nodo))==4);
		}

	}
	
	map_destroy(shortest_path_next);
	
	graph_destroy(graph);	

}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "graph_create", test_create },
	{ "graph_insert_vertices", test_insert_vertices },
	{ "graph_insert_edges", test_insert_edges },
	{ "graph_remove_vertices", test_remove_vertices },
	{ "graph_remove_edges", test_remove_edges },
	{ "graph_get_weight", 	test_get_weight },
	{ "graph_shortest_path_lengths", test_graph_shortest_path_lengths },
	{ "graph_shortest_path_nexts", test_graph_shortest_path_nexts },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
}; 