
#include <stdlib.h>
#include "time.h"
#include "set_utils.h"

#include "ADTSet.h"
#include "ADTList.h"
#include "state.h"

// Compare συναρτηση για το set για τις περιπτωσεις που ειναι ισο το rect.x τις αντιμετοπιζω στα if statemenets
int compare_coordinates(Pointer a, Pointer b){
	return (((Object)a)->rect.x-((Object)b)->rect.x);
}
// Destroy συναρτηση για το set
void destroy_objects(Pointer obj){
   if ((Object)obj!=NULL) {
		free((Object)obj);
	}
}
// H κατασταση state
struct state {
	Set objects;			            // περιέχει στοιχεία Object (Εμπόδια / Εχθροί / Πύλες)
	int portal_pairs[PORTAL_NUM];		// τα ζευγάρια πυλών (είσοδος ο δεικτης +1 /έξοδος οτι εχει μεσα).
	StateInfo info;
};

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού
State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));
	StateInfo information = malloc(sizeof(*information));
	// Γενικές πληροφορίες
	information->current_portal = 0;			// Δεν έχουμε περάσει καμία πύλη
	information->wins = 0;						// Δεν έχουμε νίκες ακόμα
	information->playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	information->paused = false;				// Χωρίς να είναι paused.
	// Πληροφορίες για το χαρακτήρα.
	Object characterq = malloc(sizeof(*characterq));
	characterq->type = CHARACTER;
	characterq->forward = true;
	characterq->jumping = false;
	//Μικρες αλλαγες στο μεγεθος της εκφωνησης εδω επειδη εχω τον Μαριο
	characterq->rect.width = 34;
	characterq->rect.height = 44;
	characterq->rect.x = 0;
	characterq->rect.y = - characterq->rect.height;
	information->character=characterq;
	state->info=information;
	// Δημιουργία των objects (πύλες / εμπόδια / εχθροί) και προσθήκη στο vector
	state->objects = set_create(compare_coordinates, destroy_objects);
	for (int i = 0; i < 4*PORTAL_NUM; i++) {
		// Δημιουργία του Object και προσθήκη στο set
		Object obj = malloc(sizeof(*obj));
		if(i % 4 == 3) {							// Το 4ο, 8ο, 12ο κλπ αντικείμενο
			obj->type = PORTAL;						// είναι πύλη.
			obj->rect.width = 50;					//Αλλαγη μεγεθους γτι χρησιμοποιω αλλο portal
			obj->rect.height = 50;
		} 
        else if(rand() % 2 == 0) {				// Για τα υπόλοιπα, με πιθανότητα 50%
			obj->type = OBSTACLE;					// επιλέγουμε εμπόδιο.
			obj->rect.width = 10;
			obj->rect.height = 80;
		} 
        else {
			obj->type = ENEMY;						// Και τα υπόλοιπα είναι εχθροί.
			obj->rect.width = 30;
			obj->rect.height = 30;
			obj->forward = false;					// Οι εχθροί αρχικά κινούνται προς τα αριστερά.
		}
		obj->rect.x = (i+1) * SPACING;
		obj->rect.y = - obj->rect.height;
        set_insert(state->objects, obj);
    }
	//Αναθεση τυχαιου ζευγαριου για την πυλη
	for (int i=0;i<PORTAL_NUM;i++) state->portal_pairs[i]=0;
	srand(time(NULL));
	for(int i=0;i<PORTAL_NUM;i++){
		int pick = rand()%PORTAL_NUM;           
		while(state->portal_pairs[pick]!=0){
			pick = rand()%PORTAL_NUM;
		}
		state->portal_pairs[pick]=i; //Tου ανατιθεται το ζευγαρι της πυλης, τα μαθηματικα γ τν υπολογισμο του x της πυλης γινονται οταν τηλεμεταφερεται
	}
	return state;
}
// Επιστρεφει το state info
StateInfo state_info(State state){
	return state->info;
}
// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη x είναι ανάμεσα στο x_from και x_to.
List state_objects(State state, float x_from, float x_to) {
	List object_list = list_create(NULL);
	if (((Object)(set_node_value(state->objects, set_last(state->objects))))->rect.x<=x_from) return object_list;
	Object object_first = malloc(sizeof(*object_first)); //θα το κανω free μεσα στην set_find_eq_or_greater οποτε οχι free στο τελος
    object_first->rect.x=x_from;
	SetNode nodo=set_find_node(state->objects, set_find_eq_or_greater(state->objects, object_first));
	while((nodo!=SET_EOF)&&(((Object)set_node_value(state->objects, nodo))->rect.x<x_to)){
		if (((Object)set_node_value(state->objects, nodo))->rect.x>x_from){
			list_insert_next(object_list, list_last(object_list), set_node_value(state->objects, nodo));
		}
		nodo =set_next(state->objects, nodo);
	}
	return object_list;
}
// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
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
			if((state->info->character->jumping==true)&&(keys->up)){ //και αν πατιεται το up ωστε να γινει πιο διαδραστικο,  πηδαει οσο ψηλα οσο πατιεται και το κουμπι παλι με ενα set maximum ομως
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
            for (SetNode nodobject = set_first(state->objects); nodobject!=SET_EOF; nodobject=set_next(state->objects,nodobject)){
                if (((Object)set_node_value(state->objects, nodobject))->type==ENEMY){
					if (((Object)set_node_value(state->objects, nodobject))->forward){
						if ((set_next(state->objects, nodobject)==SET_EOF)||(CheckCollisionRecs(((Object)set_node_value(state->objects, nodobject))->rect, ((Object)set_node_value(state->objects,  set_next(state->objects, nodobject)))->rect)==false))
								 ((Object)set_node_value(state->objects, nodobject))->rect.x+=5; // ελεγχω πριν μετακινηθει μην εχει χτυπησει καπου
					}
					else {
						if ((set_previous(state->objects, nodobject)==SET_EOF)||(CheckCollisionRecs(((Object)set_node_value(state->objects, nodobject))->rect, ((Object)set_node_value(state->objects,  set_previous(state->objects, nodobject)))->rect))==false)
							((Object)set_node_value(state->objects, nodobject))->rect.x-=5;
					}
                }
            }
			//Συγκρουσεις χαρακτηρα
			List objects_around_me = state_objects(state, (state->info->character->rect.x-100), (state->info->character->rect.x+100));
			for (ListNode nodo = list_first(objects_around_me); nodo!=LIST_EOF; nodo=list_next(objects_around_me,nodo)){
				if (CheckCollisionRecs((state->info->character->rect), ((Object)list_node_value(objects_around_me, nodo))->rect)){
					if ((((Object)list_node_value(objects_around_me, nodo))->type==ENEMY)||(((Object)list_node_value(objects_around_me, nodo))->type==OBSTACLE)){
						state->info->playing=false; //game over γιατι συγκρουστηκε
					}
					//Συγκρουση και εισοδος σε πυλη
					else if (((Object)list_node_value(objects_around_me, nodo))->type==PORTAL){
						if (state->info->character->forward==true){ //Απτο χ μπορω ευκολα να υπολογισω ποια πυλη ειναι μιας και δεν κινουνται
							int k =(((int)(((Object)list_node_value(objects_around_me, nodo))->rect.x)/SPACING)-4)/4;
							if ((k>=PORTAL_NUM)||(k<0)) { // για τη περιπτωση τελευταιας πυλης
								state->info->current_portal = PORTAL_NUM;
							}
							else {
								state->info->character->rect.x=((state->portal_pairs[k])*4+4)*SPACING+55; 
								state->info->current_portal=state->portal_pairs[k]+1; 
							}
						}
						else{
							int finder=0; 					// για να βρω το πορταλ που πρεπει να παει
							while ((finder<PORTAL_NUM-1)&&(((((int)(((Object)list_node_value(objects_around_me, nodo))->rect.x)/SPACING)-4)/4)!=state->portal_pairs[finder])) finder++;
							state->info->character->rect.x=((finder*4+4)*SPACING-55);
							state->info->current_portal=finder+1; 
						}
					} 
				}
			}
			
			List teleported_monsters = list_create(NULL);
			//Ελεγχοι συγκρουσεις τερατων
			if(state->info->playing==true){ //αν δεν εχει συγκρουστει με κατι ο ηρωας συνεχιζω
				for (SetNode nodo = set_first(state->objects); nodo!=SET_EOF;nodo=set_next(state->objects,nodo)){
					if (((Object)set_node_value(state->objects, nodo))->type==ENEMY){
						if (((Object)set_node_value(state->objects, nodo))->forward==true){
							if (set_next(state->objects, nodo)!=SET_EOF){
								SetNode nexto_nodo = set_next(state->objects, nodo);
								if (CheckCollisionRecs(((Object)set_node_value(state->objects, nodo))->rect, ((Object)set_node_value(state->objects, nexto_nodo))->rect)){
									if(((Object)set_node_value(state->objects, nexto_nodo))->type==PORTAL){ //τηλεμεταφορα με το πορταλ
										Object teleported = malloc(sizeof(*teleported));
										teleported->forward=((Object)set_node_value(state->objects, nodo))->forward;
										teleported->jumping=((Object)set_node_value(state->objects, nodo))->jumping;
										teleported->rect=((Object)set_node_value(state->objects, nodo))->rect;
										teleported->type=((Object)set_node_value(state->objects, nodo))->type;
										int finder =(((int)(((Object)set_node_value(state->objects, nexto_nodo))->rect.x)/SPACING)-4)/4;
										if ((finder>=PORTAL_NUM)||(finder<0)) finder=0;
										teleported->rect.x=((state->portal_pairs[finder])*4+4)*SPACING+60;
										SetNode temp = nodo;											//να κρατησει τη θεση αυτου που θα βγει
										if (set_previous(state->objects, nodo)==SET_BOF) nodo =set_next(state->objects, nodo);
										else nodo = set_previous(state->objects, nodo);						//επειδη στο τελος θα γινει set_next να δειχνει στο επομενο, οποτε να μη κανει skip ενα
										list_insert_next(teleported_monsters, list_last(teleported_monsters), teleported);
										set_remove(state->objects, set_node_value(state->objects, temp)); //αυτο θα σβησει και το object απτη μνημη με την destroy func
									}
									else if (((Object)set_node_value(state->objects, nexto_nodo))->type==ENEMY){ //στις περιπτωσεις συγκρουσης με αλλο εχθρο ή εμποδιο δεν αλλοιωνεται η διαταξη του σετ
										((Object)set_node_value(state->objects, nodo))->forward=false;
										((Object)set_node_value(state->objects, nexto_nodo))->forward=true; //αντιστρεφει κατευθυνση και το αλλο τερας
									}
									else {
										((Object)set_node_value(state->objects, nodo))->forward=false;
									}
								}
							}
							else { //ελεγχω μη φυγει απτο οριο τελειως των floats και τον παω στν αρχη της πιστας
								if (((Object)set_node_value(state->objects, nodo))->rect.x>(__FLT_MAX__-555)) 
									((Object)set_node_value(state->objects, nodo))->rect.x=0;
							}
						}
						else {
							if (set_previous(state->objects, nodo)!=SET_BOF){ 	//αν δεν ειναι το πρωτο δηλαδη
								SetNode prevo_nodo = set_previous(state->objects, nodo); //κοιταει το προηγουμενο αντι για το επομενο αφου παει αντιθετα					 
								if (CheckCollisionRecs(((Object)set_node_value(state->objects, nodo))->rect, ((Object)set_node_value(state->objects, prevo_nodo))->rect)){
									if(((Object)set_node_value(state->objects, prevo_nodo))->type==PORTAL){ //τηλεμεταφορα με το πορταλ
										Object teleported = malloc(sizeof(*teleported));
										teleported->forward=((Object)set_node_value(state->objects, nodo))->forward;
										teleported->jumping=((Object)set_node_value(state->objects, nodo))->jumping;
										teleported->rect=((Object)set_node_value(state->objects, nodo))->rect;
										teleported->type=((Object)set_node_value(state->objects, nodo))->type;
										int finder=0; 					// για να βρω το πορταλ που πρεπει να παει
										while ((finder<PORTAL_NUM-1)&&(((((int)(((Object)set_node_value(state->objects, prevo_nodo))->rect.x)/SPACING)-4)/4)!=state->portal_pairs[finder])) finder++;
										teleported->rect.x=((finder*4+4)*SPACING-55);
										SetNode temp = nodo;											//να κρατησει τη θεση αυτου που θα βγει
										if (set_previous(state->objects, nodo)==SET_BOF) nodo = set_next(state->objects, nodo);
										else nodo = set_previous(state->objects, nodo);						//επειδη στο τελος θα γινει set_next να δειχνει στο επομενο, οποτε να μη κανει skip ενα
										list_insert_next(teleported_monsters, list_last(teleported_monsters), teleported);
										set_remove(state->objects, set_node_value(state->objects, temp)); //αυτο θα σβησει και το object απτη μνημη με την destroy func
									}
									else if (((Object)set_node_value(state->objects, prevo_nodo))->type==ENEMY){
										((Object)set_node_value(state->objects, nodo))->forward=true;
										((Object)set_node_value(state->objects, prevo_nodo))->forward=false;

									}
									else {
										((Object)set_node_value(state->objects, nodo))->forward=true; //αλλαζει κατευθυνση
									}
								}
							}
							else { //ελεγχω μη φυγει απτο οριο τελειως των floats και τον παω στν αρχη της πιστας
								if (((Object)set_node_value(state->objects, nodo))->rect.x<(555-__FLT_MAX__)) 
									((Object)set_node_value(state->objects, nodo))->rect.x=0;
							}
						}
					}
				}	
				if (list_size(teleported_monsters)>0){
					for (ListNode teleport_me = list_first(teleported_monsters); teleport_me!=LIST_EOF; teleport_me=list_next(teleported_monsters, teleport_me)){
						set_insert(state->objects, list_node_value(teleported_monsters, teleport_me));
					}
				}
			}
		}
	}
	else if (state->info->paused==true){
		if (keys->p) state->info->paused=false; 
		else if (keys->n) { 		   //Αν το παιχνίδι είναι σε pause και πατηθεί N τότε ενημερώνεται για μόνο 1 frame
			state->info->paused=false; //δηλαδη σαν να γινεται το paused false
			state_update(state, keys); //γινεται update 1 frame το state
			state->info->paused=true;  //και μετα γινεται παλι paused
		}
	}
	// Αν το παιχνίδι έχει τελειώσει και πατηθεί enter, τότε ξαναρχίζει από την αρχή.
	// Αυτο ελεγχεται στο game.c για να κανει destroy(state) και μετα state_create να δημιουργησει δλδ το παιχνιδι απτην αρχη.
	if (state->info->current_portal==PORTAL_NUM){//Αν ο χαρακτήρας περασει απο την τελευταία πύλη, τότε προστίθεται μία νίκη 
			state->info->wins++;
			state->info->character->rect.x=0;
			state->info->current_portal=0;
	}
	
}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.
void state_destroy(State state) {
    set_destroy(state->objects);
	free(state->info->character);
	free(state->info);
	free(state);
}