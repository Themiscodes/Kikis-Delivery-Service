
#include <stdlib.h>
#include "time.h"
#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"

//η state που κραταει τη κατασταση του παιχνιδιου
struct state {
	Vector objects;			// περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες)
	List portal_pairs;		// περιέχει PortaPair (ζευγάρια πυλών, είσοδος/έξοδος)
	StateInfo info;
};
// Ζευγάρια πυλών
typedef struct portal_pair {
	Object entrance;		// η πύλη entrance
	Object exit;			// οδηγεί στην exit
}* PortalPair;
//destroyfunc για το vector objects
void destroy_objects(Pointer object){ 
	if ((Object)object!=NULL) free((Object)object);
}
//destroyfunc για τη λιστα portal_pairs
void destroy_portal_pair(Pointer portal_pair){
	if(((PortalPair)portal_pair)!=NULL) free((PortalPair)portal_pair);
}
// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού
State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));
	StateInfo information = malloc(sizeof(*information));
	// Γενικές πληροφορίες
	information->current_portal = 0;			// Δεν έχουμε περάσει καμία πύλη
	information->wins = 0;					// Δεν έχουμε νίκες ακόμα
	information->playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	information->paused = false;				// Χωρίς να είναι paused.
	// Πληροφορίες για το χαρακτήρα.
	Object characterq = malloc(sizeof(*characterq));
	characterq->type = CHARACTER;
	characterq->forward = true;
	characterq->jumping = false;
	// Μικρες αλλαγες στο μεγεθος εδω επειδη εχω τον mario στο interface
	characterq->rect.width = 34;
	characterq->rect.height = 44;
	characterq->rect.x = 0;
	characterq->rect.y = - characterq->rect.height;
	information->character=characterq;
	state->info=information;
	// Δημιουργία των objects (πύλες / εμπόδια / εχθροί)
	state->objects = vector_create(0, destroy_objects);		// Δημιουργία του vector
	for (int i = 0; i < 4*PORTAL_NUM; i++) {
		// Δημιουργία του Object
		Object obj = malloc(sizeof(*obj));
		// Κάθε 4 αντικείμενα υπάρχει μια πύλη.
		if(i % 4 == 3) {							// Το 4ο, 8ο, 12ο κλπ αντικείμενο
			obj->type = PORTAL;						// είναι πύλη.
			obj->rect.width = 50;
			obj->rect.height = 50;
		} 
		else if(rand() % 2 == 0) {				// Για τα υπόλοιπα, με πιθανότητα 50%
			obj->type = OBSTACLE;					// επιλέγουμε εμπόδιο.
			obj->rect.width = 10;
			obj->rect.height = 80;
		} 
		else{
			obj->type = ENEMY;						// Και τα υπόλοιπα είναι εχθροί.
			obj->rect.width = 30;
			obj->rect.height = 30;
			obj->forward = false;					// Οι εχθροί αρχικά κινούνται προς τα αριστερά.
		}
		// Τα αντικείμενα είναι ομοιόμορφα τοποθετημένα σε απόσταση SPACING
		obj->rect.x = (i+1) * SPACING;
		obj->rect.y = - obj->rect.height;
		// Εισαγωγη στο vector
		vector_insert_last(state->objects, obj);
	}
	//Αναθεση τυχαιου ζευγαριου για την πυλη
	int draft_lottery[PORTAL_NUM]= {0};
	srand(time(NULL));
	for(int i=0;i<PORTAL_NUM;i++){
		int pick = rand()%PORTAL_NUM;           
		while(draft_lottery[pick]!=0){ // Αν δεν ειναι μηδεν εχει βρει ηδη καποια πυλη για ζευγαρι
			pick = rand()%PORTAL_NUM;
		}
		draft_lottery[pick]=i*4+3; //Tου ανατιθεται το ζευγαρι της πυλης
	}
	//Aρχικοποίηση της λίστας obj->portal_pairs με αυτα τα τυχαια ζευγαρια
	state->portal_pairs = list_create(destroy_portal_pair);
	ListNode nodo = list_first(state->portal_pairs);
	for (int i=0; i<PORTAL_NUM;i++){
		PortalPair pair = malloc(sizeof(*pair));
		pair->entrance=((Object)(vector_get_at(state->objects, (i*4+3))));
		pair->exit=((Object)(vector_get_at(state->objects, (draft_lottery[i]))));
		list_insert_next(state->portal_pairs, nodo, pair);
		nodo = list_last(state->portal_pairs);
	}
	return state;
}
// H state_info που επιστρεφει το info
StateInfo state_info(State state){
	return state->info;
}
// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.
List state_objects(State state, float x_from, float x_to) {
	List object_list = list_create(NULL); // οχι free για να μη σβηστουν και απο το vector, που σβηνονται στο τελος
	for (int i=0; i<vector_size(state->objects);i++){
		if (((((Object)(vector_get_at((state->objects),i)))->rect.x) > x_from)&&((((Object)(vector_get_at((state->objects),i)))->rect.x) < x_to)){
			list_insert_next(object_list, list_last(object_list), (vector_get_at((state->objects),i)));
		}
	}
	return object_list;
}
// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.
void state_update(State state, KeyState keys) {
	if ((state->info->paused==false)&&(state->info->playing)) {
		if (keys->p) state->info->paused=true; //Αν πατηθεί P το παιχνίδι μπαίνει σε pause και δεν ενημερώνεται πλέον.
		else {
			//οριζοντια κινηση χαρακτηρα στον αξονα x
			if (keys->right){
				if (state->info->character->forward){
					state->info->character->rect.x += 12;
				}
				else{
					state->info->character->forward=true;
				}
			}
			else if (keys->left){
				if (!(state->info->character->forward)){
					state->info->character->rect.x -= 12;
				}
				else{
					state->info->character->forward=false;
				}
			}
			else {
				if (!(state->info->character->forward)){
					state->info->character->rect.x -= 7;
				}
				else{
					state->info->character->rect.x += 7;
				}
			}
			//Κινηση στο καθετο αξονα των y
			if((state->info->character->jumping==true)&&(keys->up)){ //και αν πατιεται το up ωστε να γινει πιο διαδραστικο, ωστε να πηδαει οσο ψηλα οσο πατιεται και το κουμπι παλι με ενα set maximum ομως
				if (state->info->character->rect.y>-250)
					state->info->character->rect.y-=15;
				else state->info->character->jumping=false;
			}
			else {
				if (state->info->character->rect.y==-44){ //δεν κοιταω αν ειναι πατημενο το up αν δεν ειναι στο εδαφος ηδη ωστε να μη μπορει να κανει hover ο χαρακτηρας σε μια θεση στο αερα
					if (keys->up) state->info->character->jumping=true;
				}
				else if (state->info->character->rect.y<-44){
					state->info->character->rect.y+=15; // πεφτει δηλαδη, απλα "αναποδα"
				}
				else state->info->character->rect.y=-44;
			}
			//Κινηση εχθρων
			for (int i=0; i<vector_size(state->objects);i++){
				if (((Object)vector_get_at(state->objects, i))->type==ENEMY){
					if (((Object)vector_get_at(state->objects, i))->forward)
						((Object)vector_get_at(state->objects, i))->rect.x+=5;
					else ((Object)vector_get_at(state->objects, i))->rect.x-=5;
				}
			}
			// Συγκρουσεις χαρακτηρα με εχθρο ή εμποδιo
			List objects_around_me = state_objects(state, (state->info->character->rect.x-100), (state->info->character->rect.x+100));
			for (ListNode nodo = list_first(objects_around_me); nodo!=LIST_EOF; nodo=list_next(objects_around_me,nodo)){
				if (CheckCollisionRecs((state->info->character->rect), ((Object)list_node_value(objects_around_me, nodo))->rect)){
					if ((((Object)list_node_value(objects_around_me, nodo))->type==ENEMY)||(((Object)list_node_value(objects_around_me, nodo))->type==OBSTACLE)){
						state->info->playing=false; //game over γιατι συγκρουστηκε
					}
					//Συγκρουση και εισοδος σε πυλη
					else if (((Object)list_node_value(objects_around_me, nodo))->type==PORTAL){
						if (state->info->character->forward==true){
							ListNode pair_nodo =list_first(state->portal_pairs);
							while ((((PortalPair)list_node_value(state->portal_pairs,pair_nodo))->entrance)!=((Object)list_node_value(objects_around_me, nodo)))
								pair_nodo=list_next(state->portal_pairs,pair_nodo); //να βρει δηλαδη την εισοδο στα ζευγαρια
							state->info->character->rect.x=((PortalPair)(list_node_value(state->portal_pairs, pair_nodo)))->exit->rect.x+55;
							state->info->current_portal=(((PortalPair)(list_node_value(state->portal_pairs, pair_nodo)))->exit->rect.x)/(SPACING*4); //επειδη απτο χ μπορω ευκολα να υπολογισω ποια πυλη ειναι μιας και δεν κινουνται
						}
						else{
							ListNode pair_nodo =list_first(state->portal_pairs);
							while ((((PortalPair)list_node_value(state->portal_pairs,pair_nodo))->exit)!=((Object)list_node_value(objects_around_me, nodo))) //ελεγχω τις exit εδω για οταν μπαινουν αντιθετα
								pair_nodo=list_next(state->portal_pairs,pair_nodo); //να βρει δηλαδη την εισοδο στα ζευγαρια
							state->info->character->rect.x=((PortalPair)(list_node_value(state->portal_pairs, pair_nodo)))->entrance->rect.x-55;
							state->info->current_portal=(((PortalPair)(list_node_value(state->portal_pairs, pair_nodo)))->entrance->rect.x)/(SPACING*4); //επειδη απτο χ μπορω ευκολα να υπολογισω ποια πυλη ειναι μιας και δεν κινουνται
						}
					} 
				}
			}
			list_destroy(objects_around_me);
			// Συγκρουσεις εχθρου με εμποδια και πυλες, η περιπτωση χαρακτηρα εχει καληφθει απο πανω
			for (int i=0; i<vector_size(state->objects);i++){
				if (((Object)vector_get_at(state->objects, i))->type==ENEMY){
					List objects_around_me = state_objects(state, (((Object)vector_get_at(state->objects, i))->rect.x-100), (((Object)vector_get_at(state->objects, i))->rect.x+100));
					for (ListNode nodo = list_first(objects_around_me); nodo!=LIST_EOF; nodo=list_next(objects_around_me,nodo)){ 
						if (((Object)list_node_value(objects_around_me, nodo))!=((Object)vector_get_at(state->objects, i))){//εδω θα πρεπει επιπλεον να τσεκαρω οτι δεν ειναι το ιδιο object με αυτο που εχω
							if (CheckCollisionRecs((((Object)list_node_value(objects_around_me, nodo))->rect), ((Object)vector_get_at(state->objects, i))->rect)){
								if ((((Object)list_node_value(objects_around_me, nodo))->type==OBSTACLE)||(((Object)list_node_value(objects_around_me, nodo))->type==ENEMY)){ 
									if (((Object)vector_get_at(state->objects, i))->forward==false)
										((Object)vector_get_at(state->objects, i))->forward=true; //αλλαζει κατευθυνση γιατι συγκρουστηκε με εμποδιο ή αλλον εχθρο
									else ((Object)vector_get_at(state->objects, i))->forward=false;
								}
								else { //Αν ειναι δηλαδη portal κανω το ιδιο με το αν ειχα και χαρακτηρα βρισκωντας το ζευγαρι απτο portal pairs
									if (((Object)vector_get_at(state->objects, i))->forward==true){
										ListNode pair_nodo =list_first(state->portal_pairs);
										while ((((PortalPair)list_node_value(state->portal_pairs, pair_nodo))->entrance)!=((Object)list_node_value(objects_around_me, nodo)))
											pair_nodo=list_next(state->portal_pairs,pair_nodo); //να βρει δηλαδη την εισοδο στα ζευγαρια
										((Object)vector_get_at(state->objects, i))->rect.x=((PortalPair)(list_node_value(state->portal_pairs, pair_nodo)))->exit->rect.x+55;
									}
									else{
										ListNode pair_nodo =list_first(state->portal_pairs);
										while ((((PortalPair)list_node_value(state->portal_pairs,pair_nodo))->exit)!=((Object)list_node_value(objects_around_me, nodo))) //ελεγχω τις exit εδω για οταν μπαινουν αντιθετα
											pair_nodo=list_next(state->portal_pairs,pair_nodo); //να βρει δηλαδη την εισοδο στα ζευγαρια
										((Object)vector_get_at(state->objects, i))->rect.x=((PortalPair)(list_node_value(state->portal_pairs, pair_nodo)))->entrance->rect.x-55;
									}
								}
							}
						}
					}
					list_destroy(objects_around_me);
				}
			}
		}
	}
	else if (state->info->paused==true){
		if (keys->p) state->info->paused=false; 
		else if (keys->n) { //Αν το παιχνίδι είναι σε pause και πατηθεί N τότε ενημερώνεται για μόνο 1 frame
			state->info->paused=false; //δηλαδη σαν να γινεται το paused false
			state_update(state, keys); //γινεται update 1 frame το state
			state->info->paused=true; //και μετα γινεται παλι paused
		}
	}
	//Αν ο χαρακτήρας περασει απο την τελευταία πύλη, τότε προστίθεται μία νίκη 
	//και μετακινείται στην αρχή της πίστας (χωρίς να αλλάξει κάτι άλλο).
	if (state->info->current_portal==PORTAL_NUM){
		state->info->wins++; //προστιθεται νικη
		state->info->character->rect.x=0; //παει στην αρχη της πιστας
		state->info->current_portal=0;
	}
}
// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.
void state_destroy(State state) {
	vector_destroy(state->objects);
	list_destroy(state->portal_pairs);
	free(state->info->character);
	free(state->info);
	free(state);
}