///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Graph μέσω Map γειτνίασης.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTMap.h"
#include "ADTSet.h"
#include "ADTGraph.h"


// Επιστρέφει έναν ακέραιο σε νέα μνήμη με τιμή value
int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}

struct vertex_node {            // Node για τις κορυφες του γραφου
	Pointer vertex;             // Η κορυφη
    CompareFunc compare;		// Συνάρτηση για σύγκρηση δεικτών για το map adjacent_vertexes
	Map adjacent_vertexes;      // Οι κορυφες στις οποιες συνδεεται (αυτες εχουν σαν value το weight της αποστασης)
                                // με αυτον τον τροπο ειναι αλληλενδετα τα vertex_nodes με τα vertex_pairs και μπορω απτο ενα να βρω το αλλο ευκολα
};

struct graph {
	Map vertex_nodes;           // Οι κομβοι
    Map vertex_pairs;           // Οι ακμες
    CompareFunc compare;		// Συνάρτηση για σύγκρηση δεικτών, που πρέπει να δίνεται απο τον χρήστη
    DestroyFunc destroy_vertex;
};

// Δημιουργεί και επιστρέφει ένα γράφο, στον οποίο τα στοιχεία (οι κορυφές)
// συγκρίνονται με βάση τη συνάρτηση compare. Αν destroy_vertex != NULL, τότε
// καλείται destroy_vertex(vertex) κάθε φορά που αφαιρείται μια κορυφή.
Graph graph_create(CompareFunc compare, DestroyFunc destroy_vertex){
    // Δεσμεύουμε κατάλληλα τον χώρο που χρειαζόμαστε για το γραφο
    Graph graph = malloc(sizeof(*graph));
    graph->compare=compare;
    graph->destroy_vertex = destroy_vertex;
    graph->vertex_nodes = map_create(compare, destroy_vertex, free);
    graph->vertex_pairs = map_create(compare, free, free); // key ειναι το βαρος αλλα ως pointer
    map_set_hash_function(graph->vertex_nodes, hash_pointer);
    map_set_hash_function(graph->vertex_pairs, hash_pointer);
	return graph;
}

// Επιστρέφει τον αριθμό στοιχείων (κορυφών) που περιέχει ο γράφος graph.
int graph_size(Graph graph){
    return map_size(graph->vertex_nodes);
}

// Προσθέτει μια κορυφή στο γράφο.
void graph_insert_vertex(Graph graph, Pointer vertex){
    if (map_find(graph->vertex_nodes, vertex)==NULL){           // ελεγχω αν αυτο το vertex υπαρχει ηδη
        VertexNode vertex_node = malloc(sizeof(*vertex_node));  // πρωτα φτιαχνω το node που θα μπει το vertex
        vertex_node->adjacent_vertexes=NULL;
        vertex_node->compare=graph->compare;
        vertex_node->vertex=vertex; 
        map_insert(graph->vertex_nodes, vertex, vertex_node);   // το vertex θα ειναι και το key
    }
}

// Επιστρέφει λίστα με όλες τις κορυφές του γράφου. Η λίστα δημιουργείται σε κάθε
// κληση και είναι ευθύνη του χρήστη να κάνει list_destroy.
List graph_get_vertices(Graph graph){
    if (graph_size(graph)==0) return NULL;
    List vertices_list = list_create(NULL);
    ListNode listnodo = LIST_BOF;
    for (MapNode node = map_first(graph->vertex_nodes); node != MAP_EOF; node = map_next(graph->vertex_nodes, node)) {
		list_insert_next(vertices_list, listnodo, (Pointer)map_node_key(graph->vertex_nodes, node));
        listnodo = list_last(vertices_list);
    } 
    return vertices_list;
}

// Διαγράφει μια κορυφή από τον γράφο (αν υπάρχουν ακμές διαγράφονται επίσης).
void graph_remove_vertex(Graph graph, Pointer vertex){

    if ((((VertexNode)map_find(graph->vertex_nodes, vertex))->adjacent_vertexes)!=NULL) {
        Pointer vertices[map_size(((VertexNode)map_find(graph->vertex_nodes, vertex))->adjacent_vertexes)]; // σημαντικο να τα αποθηκευω καπου προσωρινα για να μη διαγραφω απο το map οσο το διατρεχω
        int i=0; 
        for (MapNode nodo = map_first(((VertexNode)map_find(graph->vertex_nodes, vertex))->adjacent_vertexes); nodo != MAP_EOF; nodo = map_next(((VertexNode)map_find(graph->vertex_nodes, vertex))->adjacent_vertexes, nodo)){
            vertices[i]= map_node_key(((VertexNode)map_find(graph->vertex_nodes, vertex))->adjacent_vertexes, nodo);
            i++;
        }
        for (int j=0;j<i;j++){
            graph_remove_edge(graph, vertex, vertices[j]);
        }
    }
    map_remove(graph->vertex_nodes, vertex);
}

// Αυτη κανει update και στους κομβους των adjacent κομβων σε αυτους
static void add_the_edge(VertexNode vertex1, VertexNode vertex2, Pointer weight){
    if (vertex1->adjacent_vertexes==NULL){
        vertex1->adjacent_vertexes=map_create(vertex1->compare, NULL, NULL);  // το pointer προσοχη null γιατι ειναι κομβος δεν τον ελευθερωνουμε οταν διαφραφεται ακμη
        map_set_hash_function(vertex1->adjacent_vertexes, hash_pointer);
    }
    if (vertex2->adjacent_vertexes==NULL){
        vertex2->adjacent_vertexes=map_create(vertex2->compare, NULL, NULL); // το pointer προσοχη null γιατι ειναι κομβος δεν τον ελευθερωνουμε οταν διαφραφεται ακμη
        map_set_hash_function(vertex2->adjacent_vertexes, hash_pointer);
    }
    map_insert(vertex1->adjacent_vertexes, vertex2->vertex, weight); // αντιστροφα απ'οτι στις ακμες εδω σαν key χρησιμοποιω το vertex και σαν value το βαρος
    map_insert(vertex2->adjacent_vertexes, vertex1->vertex, weight); // επισης εδω δεν εισαγω το vertex_node αλλα το ιδιο το vertex

}

// Προσθέτει μια ακμή με βάρος weight στο γράφο.
void graph_insert_edge(Graph graph, Pointer vertex1, Pointer vertex2, int weight){
    
        Pointer edgeweight = create_int(weight);    

        // αν δεν υπαρχουν ηδη προσθετω τους κομβους
        if(map_find(graph->vertex_nodes, vertex1)==NULL){ 
            graph_insert_vertex(graph, vertex1);
        }
        if(map_find(graph->vertex_nodes, vertex2)==NULL){
            graph_insert_vertex(graph, vertex2);
        }

        // φτιαχνω το ζευγαρι και το προσθετω στον γραφο
        GraphVertexPair newpair = malloc(sizeof(*newpair));
        newpair->owner=graph;
        newpair->vertex1=vertex1;
        newpair->vertex2=vertex2;
        map_insert(graph->vertex_pairs, edgeweight, newpair);

        // εισαγω τις ακμες και στα αντιστοιχα nodes 
        add_the_edge(((VertexNode)map_find(graph->vertex_nodes, vertex1)), ((VertexNode)map_find(graph->vertex_nodes, vertex2)), edgeweight);
    

}

// Αφαιρεί μια ακμή από το γράφο.
void graph_remove_edge(Graph graph, Pointer vertex1, Pointer vertex2){

    if(((VertexNode)map_find(graph->vertex_nodes, vertex1))!=NULL&&((VertexNode)map_find(graph->vertex_nodes, vertex2))!=NULL){ // Αρχικα κοιταω αν υπαρχουν αυτοι οι κομβοι στο γραφο
        if (map_find((((VertexNode)map_find(graph->vertex_nodes, vertex1))->adjacent_vertexes), vertex2)!=NULL){ // και αν υπαρχουν οτι ενωνονται
            Pointer edgeweight = map_find((((VertexNode)map_find(graph->vertex_nodes, vertex1))->adjacent_vertexes), vertex2); // τοτε απο το βαρος που εχει η ακμη που του ενωνει
            map_remove(graph->vertex_pairs, edgeweight); // χρησιμοποιωντας το ως key βρισκω την ακμη που τους ενωνει και την διαγραφω

            map_remove((((VertexNode)map_find(graph->vertex_nodes, vertex1))->adjacent_vertexes), vertex2); // και τα αφαιρω και απο τα αντιστοιχα vertices
            map_remove((((VertexNode)map_find(graph->vertex_nodes, vertex2))->adjacent_vertexes), vertex1); 
            
        }
    }

}

// Επιστρέφει το βάρος της ακμής ανάμεσα στις δύο κορυφές, ή INT_MAX αν δεν είναι γειτονικές.
int graph_get_weight(Graph graph, Pointer vertex1, Pointer vertex2){
    int weight = INT32_MAX;
    if (vertex1==vertex2) return 0;
    else if(((VertexNode)map_find(graph->vertex_nodes, vertex1))!=NULL&&((VertexNode)map_find(graph->vertex_nodes, vertex2))!=NULL){ // Αρχικα κοιταω αν υπαρχουν αυτοι οι κομβοι στο γραφο
        if (((((VertexNode)map_find(graph->vertex_nodes, vertex1))->adjacent_vertexes)!=NULL)&&(map_find((((VertexNode)map_find(graph->vertex_nodes, vertex1))->adjacent_vertexes), vertex2)!=NULL)){ // και αν υπαρχουν οτι ενωνονται
            weight=*(int*)map_find((((VertexNode)map_find(graph->vertex_nodes, vertex1))->adjacent_vertexes), vertex2); // και παιρνω το βαρος
        }
    }
    return weight;
}

// Επιστρέφει λίστα με τους γείτονες μιας κορυφής. Η λίστα δημιουργείται σε κάθε
// κληση και είναι ευθύνη του χρήστη να κάνει list_destroy.
List graph_get_adjacent(Graph graph, Pointer vertex){
    List vertices_list = list_create(NULL);
    if (((((VertexNode)map_find(graph->vertex_nodes, vertex))->adjacent_vertexes)==NULL)||(map_size(((VertexNode)map_find(graph->vertex_nodes, vertex))->adjacent_vertexes)==0)) return vertices_list;

    ListNode listnodo = LIST_BOF;
    for (MapNode nodo = map_first(((VertexNode)map_find(graph->vertex_nodes, vertex))->adjacent_vertexes); nodo != MAP_EOF; nodo = map_next(((VertexNode)map_find(graph->vertex_nodes, vertex))->adjacent_vertexes, nodo)){
        list_insert_next(vertices_list, listnodo, map_node_key(((VertexNode)map_find(graph->vertex_nodes, vertex))->adjacent_vertexes, nodo));
        listnodo=list_last(vertices_list);    
    }

    return vertices_list;
}

// Επιστρέφει το μήκος του συντομότερου μονοπάτι ανάμεσα σε κάθε ζευγάρι κορυφών του
// γράφου. Το Map που επιστρέφεται αντιστοιχεί:
//    Ζευγάρι κορυφών (GraphVertexPair)  =>  μήκος συντoμότερου μονοπατιού (int)
// Αν δεν υπάρχει κανένα μονοπάτι το αντίστοιχο μήκος είναι INT_MAX.
// Το Map δημιουργείται σε κάθε κληση και είναι ευθύνη του χρήστη να κάνει map_destroy.

Map graph_shortest_path_lengths(Graph graph){

    if (graph_size(graph)==0) return NULL;

    // Πρωτα φτιαχνω τον πινακα που θα χρησιμοποιει ο αλγοριθμος Floyd-Warshall
    int dist[graph_size(graph)][graph_size(graph)];
    // Και τον αρχικοποιω
    MapNode nodo = map_first(graph->vertex_nodes);
    for(int i=0; i<graph_size(graph);i++){
        MapNode nodo2 = map_first(graph->vertex_nodes);
        for(int j=0; j<graph_size(graph);j++){
            if(i==j) // Οι διαγονιες μηδεν
                dist[i][j]=0;
            else // Το βαρος των ακμων
                dist[i][j]=graph_get_weight(graph, map_node_key(graph->vertex_nodes, nodo), map_node_key(graph->vertex_nodes, nodo2));
            nodo2=map_next(graph->vertex_nodes, nodo2);   
        }
        nodo=map_next(graph->vertex_nodes, nodo);
    }

    // στη συνεχεια ο αλγοριθμος υπολογιζει τα βελτιστα μονοπατια
    for(int k=0; k<graph_size(graph);k++){
        for(int i=0; i<graph_size(graph);i++){
            for(int j=0; j<graph_size(graph);j++){
                if(i==j) dist[i][j]=0;
                else {
                    if ((dist[i][k]!=INT32_MAX&&dist[k][j]!=INT32_MAX)&&(dist[i][j]>dist[i][k]+dist[k][j]))
                        dist[i][j]=dist[i][k]+dist[k][j];
                }
            }
        }
    }

    // δημιουργω το map που θα τα αποθηκευσω
    Map shortest_paths = map_create(graph->compare, free, free);
    map_set_hash_function(shortest_paths, hash_pointer);

    // Απο τον πινακα τωρα εισαγω στο map τις ακμες που ενωνονται. Οσες δεν ενωνονται δεν τις περναω
    // Ενω δεν χρειαζεται να διασχισω ολο το πινακα αφου ειναι συμμετρικος, μιας και ειναι μη κατευθυνομενος γραφος
    MapNode nodo3 = map_first(graph->vertex_nodes);
    for(int i=1; i<graph_size(graph);i++){
        nodo3=map_next(graph->vertex_nodes, nodo3);  // επειδη αρχιζω απο τη δευτερη γραμμη του πινακα
        MapNode nodo4 = map_first(graph->vertex_nodes);
        for(int j=0; j<i;j++){
            if(dist[i][j]!=INT32_MAX){
                GraphVertexPair edge = malloc(sizeof(*edge));
                edge->owner=graph;
                edge->vertex1= map_node_key(graph->vertex_nodes, nodo3);
                edge->vertex2= map_node_key(graph->vertex_nodes, nodo4);
                Pointer pathweight = create_int(dist[i][j]);
                map_insert(shortest_paths, pathweight, edge);
            }
            nodo4=map_next(graph->vertex_nodes, nodo4);   
        }
    }

    return shortest_paths;
}

// Επιστρέφει τον επομενο κομβο στο συντομότερο μονοπάτι ανάμεσα σε κάθε ζευγάρι κορυφών του
// γράφου. Το Map που επιστρέφεται αντιστοιχεί:
// Ζευγάρι κορυφών (GraphVertexPair)  =>  επομενη κορυφη στο μονοπατι (Pointer)

Map graph_shortest_path_nexts(Graph graph){

    if (graph_size(graph)==0) return NULL;

    // Πρωτα φτιαχνω τον πινακα που θα χρησιμοποιει ο αλγοριθμος Floyd-Warshall
    int dist[graph_size(graph)][graph_size(graph)];

    // Και τον πινακα που θα κραταει τον αμεσως επομενο κομβο του βελτιστου μονοπατιου
    Pointer neXt[graph_size(graph)][graph_size(graph)];

    // Και τους αρχικοποιω
    MapNode nodo = map_first(graph->vertex_nodes);
    for(int i=0; i<graph_size(graph);i++){
        MapNode nodo2 = map_first(graph->vertex_nodes);
        for(int j=0; j<graph_size(graph);j++){
            if(i==j){ // Oi διαγονιες μηδεν
                dist[i][j]=0;
                neXt[i][j]=NULL;
            }
            else {// Το βαρος των ακμων
                dist[i][j]=graph_get_weight(graph, map_node_key(graph->vertex_nodes, nodo), map_node_key(graph->vertex_nodes, nodo2));
                if (dist[i][j]==INT32_MAX) neXt[i][j]=NULL;
                else neXt[i][j]=map_node_key(graph->vertex_nodes, nodo2); // ως βελτιστο μονοπατι ειναι προφανως ο επομενος κομβος στην ακμη οποτε και αυτος ο κομβος
            }
            nodo2=map_next(graph->vertex_nodes, nodo2);   
        }
        nodo=map_next(graph->vertex_nodes, nodo);         
    }

    // Παρομοια οπως και πριν βρισκω τα βελτιστα μονοπατια
    MapNode nodo7 = map_first(graph->vertex_nodes);
    for(int k=0; k<graph_size(graph);k++){
        for(int i=0; i<graph_size(graph);i++){
            for(int j=0; j<graph_size(graph);j++){
                if(i==j) dist[i][j]=0;
                else {
                    if ((dist[i][k]!=INT32_MAX&&dist[k][j]!=INT32_MAX)&&(dist[i][j]>dist[i][k]+dist[k][j])){
                        dist[i][j]=dist[i][k]+dist[k][j];
                        neXt[i][j]= map_node_key(graph->vertex_nodes, nodo7); // ο επομενος κομβος για αυτο το βελτιστο μονοπατι θα ειναι μεσω αυτου που ειναι τωρα ενδιαμεσος
                    }
                }
            }
        }
         nodo7=map_next(graph->vertex_nodes, nodo7);  
    }
    
    // το map που θα χρησιμοποιησω
    Map shortest_paths_nexts = map_create(graph->compare, NULL, NULL);
    map_set_hash_function(shortest_paths_nexts, hash_pointer);

    // Απο τον πινακα τωρα εισαγω στο map τις ακμες που ενωνονται. 
    // Εδω το κανω για ολες τις θεσεις αφου θελουμε τον επομενο κομβο για να παει 
    // απο ενα κομβο στο αλλο, δηλαδη και απο την αντιστροφη διαδρομη
    // μας ενδιαφερει δηλαδη και το μονοπατι

    MapNode nodo3 = map_first(graph->vertex_nodes);
    for(int i=0; i<graph_size(graph);i++){
        MapNode nodo4 = map_first(graph->vertex_nodes);
        for(int j=0; j<graph_size(graph);j++){
            // if (i!=j&&neXt!=placeholder){
            if(i!=j&&dist[i][j]!=INT32_MAX){
                // Εδω αντιθετα με πριν ως key εχω το ζευγαρι και τον κομβο που ειναι επομενος στο μονοπατι το value
                GraphVertexPair edge = malloc(sizeof(*edge));
                edge->owner=graph;
                edge->vertex1= map_node_key(graph->vertex_nodes, nodo3);
                edge->vertex2= map_node_key(graph->vertex_nodes, nodo4);
                map_insert(shortest_paths_nexts, edge, neXt[i][j]);
            }
            nodo4=map_next(graph->vertex_nodes, nodo4);   
        }
        nodo3=map_next(graph->vertex_nodes, nodo3);  
    }

    return shortest_paths_nexts;

}

// Ελευθερώνει όλη τη μνήμη που δεσμεύει ο γράφος.
// Οποιαδήποτε λειτουργία πάνω στο γράφο μετά το destroy είναι μη ορισμένη.

void graph_destroy(Graph graph){
    map_destroy(graph->vertex_pairs);
    map_destroy(graph->vertex_nodes);
    free(graph);
}

//// Για την περίπτωση που ο γράφος χρησιμοποιεί πίνακα κατακερματισμού
// Ορίζει τη συνάρτηση κατακερματισμού hash_func για το συγκεκριμένο γράφο.
// Πρέπει να κληθεί μετά την graph_create και πριν από οποιαδήποτε άλλη συνάρτηση.
// Για τη συγκεκριμενη υλοποιηση θα χρησιμοποιω το hash_pointer

void graph_set_hash_function(Graph graph, HashFunc hash_func){
    map_set_hash_function(graph->vertex_nodes, hash_func);
    map_set_hash_function(graph->vertex_pairs, hash_func);
}

