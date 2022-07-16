
#include <stdlib.h>
#include "time.h"
#include "set_utils.h"

#include "ADTSet.h"
#include "ADTList.h"
#include "kikistate.h"

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
	Level level;
	StateInfo info;
};

// To αντικειμενα του λεβελ
struct level{
	Set objects;
	Set flying_objects;
};

Level level_create(int character_level){

	Level level = malloc(sizeof(*level));
	// Δημιουργία των objects (πύλες / εμπόδια / εχθροί) και προσθήκη στο vector
	level->objects = set_create(compare_coordinates, destroy_objects);
	level->flying_objects = set_create(compare_coordinates, destroy_objects);
	for (int i = 0; i < (character_level*7+3); i++) {
		// Δημιουργία του Object και προσθήκη στο set
		Object obj = malloc(sizeof(*obj));
		if (i==0){ 								// portal στην αρχη του λεβελ
			obj->type = PORTAL;					// είναι τσεκποιντ να αφησει τα ψωμια
			obj->rect.width = 135;				//actual width is 500 though		
			obj->rect.height = 486;
			obj->rect.x = -93;
			obj->rect.y = - obj->rect.height;
			set_insert(level->objects, obj);
			Object invisible_barrier = malloc(sizeof(*invisible_barrier)); //αυτο για να κραταω τα ιπταμενα "μαζεμενα"
			invisible_barrier->type = BARRIER;
			invisible_barrier->rect.width = 45;					
			invisible_barrier->rect.height = 550;
			invisible_barrier->rect.x = (i+1) * SPACING;
			invisible_barrier->rect.y = - obj->rect.height;
			set_insert(level->flying_objects, invisible_barrier);
		}
		else if(i == (((character_level*7+3)*2)/3) ) {							// σκουπα για να πεταει
			obj->type = BROOM;						// είναι σκουπα
			obj->rect.width = 185;					
			obj->rect.height = 185;
			obj->rect.x = (i+1) * SPACING;
			obj->rect.y = - obj->rect.height;
			set_insert(level->objects, obj);
			Object invisible_barrier = malloc(sizeof(*invisible_barrier)); //αυτο για να κραταω τα ιπταμενα μαζεμενα
			invisible_barrier->type = BARRIER;
			invisible_barrier->rect.width = 45;					
			invisible_barrier->rect.height = 550;
			invisible_barrier->rect.x = (i+1) * SPACING;
			invisible_barrier->rect.y = - obj->rect.height;
			set_insert(level->flying_objects, invisible_barrier);
		} 
		else if (i == (character_level*7+3)-1){						//Tο τελευταιο
			obj->type = CHECKPOINT;									// είναι τσεκποιντ να αφησει τα ψωμια.
			obj->rect.width = 154;					
			obj->rect.height = 537;
			obj->rect.x = (i+1) * SPACING;
			obj->rect.y = - obj->rect.height;
			set_insert(level->objects, obj);
			Object invisible_barrier = malloc(sizeof(*invisible_barrier)); //αυτο για να κραταω τα ιπταμενα μαζεμενα στο τελος για να γινεται πιο δυσκολο
			invisible_barrier->type = BARRIER;
			invisible_barrier->rect.width = 45;					
			invisible_barrier->rect.height = 550;
			invisible_barrier->rect.x = (i+1) * SPACING;
			invisible_barrier->rect.y = - obj->rect.height;
			set_insert(level->flying_objects, invisible_barrier);
		}
		else if (i<(((character_level*7+3)*2)/3)&&((i%6==5))){ 	// καθε 5 ειναι ψωμια αλλα οχι οταν πεταει και παει σε τσεκποιντ 
			obj->type = BREAD;				
			obj->rect.width = 59;					
			obj->rect.height = 102;
			obj->rect.x = (i+1) * SPACING;
			obj->rect.y = - obj->rect.height;
			set_insert(level->objects, obj);
		}
        else if(rand() % 3 == 0) {								// Για τα υπόλοιπα, με πιθανότητα 33% επιλέγουμε αετους.
			obj->type = BIRD;					 
			obj->rect.width = 44; // μικροτερο εδω απο το μεγεθος της εικοννας για να ειναι playable
			obj->rect.height = 25;  // να μην ειναι τελειως δυσκολο
			obj->forward = false;
			obj->rect.x = (i+1) * SPACING;
			obj->rect.y = -(((rand()%4)*47)+150); // δηλαδη να ειναι με καποιο τροπο μαξιμουμ το height της κικης απτο εδαφος
			set_insert(level->flying_objects, obj); 		// να μην ερχεται κατα πανω της
		} 
        else {
			obj->type = GHOST;						// Και τα υπόλοιπα είναι λυκοι.
			obj->rect.width = 77; 		// δεν ειναι το actual width of img
			obj->rect.height = 45; 	//  δεν ειναι το actual height of img
			obj->forward = false;					// Οι εχθροί αρχικά κινούνται προς τα αριστερά.
			obj->rect.x = (i+1) * SPACING;
			obj->rect.y = - obj->rect.height;
			set_insert(level->objects, obj);
		}
	}
	return level;
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού
State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));
	StateInfo information = malloc(sizeof(*information));
	// Γενικές πληροφορίες
	information->lives=3; 						// Τρεις ζωες αρχικα
	information->level = 1; 
	information->baskets = 0;			
	information->score = 0;						// Δεν έχουμε ποντους ακόμα
	information->start = false; 				// δεν αρχισει αν δεν πατηθει enter
	information->playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	information->paused = false;				// Χωρίς να είναι paused.
	// Πληροφορίες για το χαρακτήρα.
	Object characterq = malloc(sizeof(*characterq));
	characterq->type = CHARACTER;
	characterq->flying= false; //δεν πεταει αρχικα
	characterq->forward = true;
	characterq->jumping = false;
	// Αλλαγες στο μεγεθος εδω επειδη εχω την Κικι
	characterq->rect.width = 120; 	//220 actual size τα χω αλλαξει να φαινεται πιο ακριβες με το τι φαινεται στην οθονη
	characterq->rect.height = 100; 	//261 actual size
	characterq->rect.x = 0;
	characterq->rect.y = - characterq->rect.height;
	information->character=characterq;
	state->info=information;
	// μεσα δημιουργουνται ολα τα objects του level
	state->level=level_create(information->level); 
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
	if (((Object)(set_node_value(state->level->objects, set_last(state->level->objects))))->rect.x<=x_from) return object_list;
	Object object_first = malloc(sizeof(*object_first)); //θα το κανω free μεσα στην set_find_eq_or_greater οποτε οχι free στο τελος
    object_first->rect.x=x_from;
	SetNode nodo=set_find_node(state->level->objects, set_find_eq_or_greater(state->level->objects, object_first));
	while((nodo!=SET_EOF)&&(((Object)set_node_value(state->level->objects, nodo))->rect.x<x_to)){
		if (((Object)set_node_value(state->level->objects, nodo))->rect.x>x_from){
			list_insert_next(object_list, list_last(object_list), set_node_value(state->level->objects, nodo));
		}
		nodo =set_next(state->level->objects, nodo);
	}
	return object_list;
}

// αντιστοιχη για τα ιπταμενα αντικειμενα
List state_flying_objects(State state, float x_from, float x_to) {
	List object_list = list_create(NULL);
	if (((Object)(set_node_value(state->level->flying_objects, set_last(state->level->flying_objects))))->rect.x<=x_from) return object_list;
	Object object_first = malloc(sizeof(*object_first)); //θα το κανω free μεσα στην set_find_eq_or_greater οποτε οχι free στο τελος
    object_first->rect.x=x_from;
	SetNode nodo=set_find_node(state->level->flying_objects, set_find_eq_or_greater(state->level->flying_objects, object_first));
	while((nodo!=SET_EOF)&&(((Object)set_node_value(state->level->flying_objects, nodo))->rect.x<x_to)){
		if (((Object)set_node_value(state->level->flying_objects, nodo))->rect.x>x_from){
			list_insert_next(object_list, list_last(object_list), set_node_value(state->level->flying_objects, nodo));
		}
		nodo =set_next(state->level->flying_objects, nodo);
	}
	return object_list;
}
// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
void state_update(State state, KeyState keys) {
	if ((state->info->start==true)&&(state->info->paused==false)&&(state->info->playing)) {
		if (keys->p) state->info->paused=true; //Αν πατηθεί P το παιχνίδι μπαίνει σε pause και δεν ενημερώνεται πλέον.
		else {
			//οριζοντια κινηση χαρακτηρα στον αξονα x
			if (keys->right){
				if (state->info->character->forward){
					state->info->character->rect.x += 14;
				}
				else{
					state->info->character->forward=true;
				}
			}
			else if (keys->left){
				if (!(state->info->character->forward)){
					state->info->character->rect.x -= 14;
				}
				else{
					state->info->character->forward=false;
				}
			}
			else {
				if (!(state->info->character->forward)){
					state->info->character->rect.x -= 8;
				}
				else{
					state->info->character->rect.x += 8;
				}
			}
			if (state->info->character->flying==false){
				//Κινηση στο καθετο αξονα των y
				if((state->info->character->jumping==true)&&(keys->up)){ //και αν πατιεται το up ωστε να γινει πιο διαδραστικο
																		 // πηδαει οσο ψηλα οσο πατιεται και το κουμπι παλι με ενα set maximum ομως
					if (state->info->character->rect.y>-365) {// το αλλαξα σε -365 απο -250
						state->info->character->rect.y-=35; // το αλλαξα σε 35 απο 15
						state->info->character->rect.x+=6; //ενα μικρο boost οταν πηδαει και μπροστα
					}
					else state->info->character->jumping=false;
				}
				else {
					if (state->info->character->rect.y==-44){ //δεν κοιταω αν ειναι πατημενο το up αν δεν ειναι στο εδαφος ηδη ωστε να μη μπορει να κανει hover ο χαρακτηρας σε μια θεση στο αερα
						if (keys->up) state->info->character->jumping=true;
					}
					else if (state->info->character->rect.y<-44){
						state->info->character->rect.y+=25; // πεφτει δηλαδη, απλα "αναποδα"
						state->info->character->rect.x+=8;
					}
					else state->info->character->rect.y=-44;
				}
			}
			else{ // για οταν πεταει
				if (state->info->character->rect.y>-150) state->info->character->rect.y=-150;
				if(keys->up){ 
					if (state->info->character->rect.y>-400) { //ποσο ψηλα πεταει
						state->info->character->rect.y-=15;
					}
				}
				else if (keys->down){
					if (state->info->character->rect.y<-150){
						state->info->character->rect.y+=15;
					}
				}
			}
			//Κινηση εχθρων
            for (SetNode nodobject = set_first(state->level->objects); nodobject!=SET_EOF; nodobject=set_next(state->level->objects,nodobject)){
                if (((Object)set_node_value(state->level->objects, nodobject))->type==GHOST){
					if (((Object)set_node_value(state->level->objects, nodobject))->forward){
						if ((set_next(state->level->objects, nodobject)==SET_EOF)||(CheckCollisionRecs(((Object)set_node_value(state->level->objects, nodobject))->rect, ((Object)set_node_value(state->level->objects,  set_next(state->level->objects, nodobject)))->rect)==false))
								 ((Object)set_node_value(state->level->objects, nodobject))->rect.x+=2; // λιγο πιο αργα πηγαινει μπροστα για να μπορει να το αποφυγει
					}
					else {
						if ((set_previous(state->level->objects, nodobject)==SET_BOF)||(CheckCollisionRecs(((Object)set_node_value(state->level->objects, nodobject))->rect, ((Object)set_node_value(state->level->objects,  set_previous(state->level->objects, nodobject)))->rect))==false)
							((Object)set_node_value(state->level->objects, nodobject))->rect.x-=4;
					}
                }
            }
			for (SetNode nodobject = set_first(state->level->flying_objects); nodobject!=SET_EOF; nodobject=set_next(state->level->flying_objects,nodobject)){
                if (((Object)set_node_value(state->level->flying_objects, nodobject))->type==BIRD){
					if (((Object)set_node_value(state->level->flying_objects, nodobject))->forward){
						if ((set_next(state->level->flying_objects, nodobject)==SET_EOF)||(CheckCollisionRecs(((Object)set_node_value(state->level->flying_objects, nodobject))->rect, ((Object)set_node_value(state->level->flying_objects,  set_next(state->level->flying_objects, nodobject)))->rect)==false)||((((Object)set_node_value(state->level->flying_objects, nodobject))->rect.x)+80>(((Object)set_node_value(state->level->flying_objects, set_next(state->level->flying_objects, nodobject)))->rect.x)))
								 ((Object)set_node_value(state->level->flying_objects, nodobject))->rect.x+=6; // ελεγχω πριν μετακινηθει μην εχει χτυπησει καπου
					}
					else {
						if ((set_previous(state->level->flying_objects, nodobject)==SET_BOF)||(CheckCollisionRecs(((Object)set_node_value(state->level->flying_objects, nodobject))->rect, ((Object)set_node_value(state->level->flying_objects,  set_previous(state->level->flying_objects, nodobject)))->rect)==false)||((((Object)set_node_value(state->level->flying_objects, nodobject))->rect.x)-80<(((Object)set_node_value(state->level->flying_objects, set_previous(state->level->flying_objects, nodobject)))->rect.x)))
							((Object)set_node_value(state->level->flying_objects, nodobject))->rect.x-=6;
					}
                }
            }
			//Συγκρουσεις χαρακτηρα οταν δεν πεταει
				List objects_around_me = state_objects(state, (state->info->character->rect.x-100), (state->info->character->rect.x+100));
				for (ListNode nodo = list_first(objects_around_me); nodo!=LIST_EOF; nodo=list_next(objects_around_me,nodo)){
					if (CheckCollisionRecs((state->info->character->rect), ((Object)list_node_value(objects_around_me, nodo))->rect)){
						if (((Object)list_node_value(objects_around_me, nodo))->type==GHOST&&(state->info->character->flying==false)){ // οταν δεν πεταει μονο
							state->info->lives--;
							if (state->info->lives==0){
								state->info->playing=false; //game over γιατι εχασε ολες τις ζωες
							}
							else{	//αλλιως αρχιζει απτην αρχη του λεβελ
								level_destroy(state->level);
								state->level=level_create(state->info->level);
								state->info->character->rect.x=0;
								state->info->baskets=0;
								state->info->character->forward=true;
								state->info->character->flying=false;
							}		
						}
						else if (((Object)list_node_value(objects_around_me, nodo))->type==PORTAL){
							state->info->character->rect.x=0;
							state->info->character->forward=true;
						}
						else if (((Object)list_node_value(objects_around_me, nodo))->type==BREAD){
							state->info->score+=45;
							state->info->baskets++;
							set_remove(state->level->objects, ((Object)list_node_value(objects_around_me, nodo))); 
						}
						else if (((Object)list_node_value(objects_around_me, nodo))->type==CHECKPOINT){ //παει επομενο λεβελ
							state->info->level++;
							if (state->info->lives<3) state->info->lives++; // κερδιζει μια ζωη αν εχει χασει
							state->info->score+=((state->info->baskets)*(state->info->level)*45);
							level_destroy(state->level);
							state->level=level_create(state->info->level);
							state->info->character->rect.x=0;
							state->info->baskets=0;
							state->info->character->flying=false;
							state->info->character->forward=true;					
						}
						else if (((Object)list_node_value(objects_around_me, nodo))->type==BROOM){
							state->info->character->flying=true;
							set_remove(state->level->objects, ((Object)list_node_value(objects_around_me, nodo))); //εξαφανιζει και τη σκουπα
						}
					}
				}
				list_destroy(objects_around_me);
			//Συγκρουσεις χαρακτηρα με ιπταμενα
			List flying_objects_around_me = state_flying_objects(state, (state->info->character->rect.x-155), (state->info->character->rect.x+155));
			for (ListNode flynodo = list_first(flying_objects_around_me); flynodo!=LIST_EOF; flynodo=list_next(flying_objects_around_me,flynodo)){
					if (CheckCollisionRecs((state->info->character->rect), ((Object)list_node_value(flying_objects_around_me, flynodo))->rect)){
						if (((Object)list_node_value(flying_objects_around_me, flynodo))->type==BIRD){
							state->info->lives--;
							if (state->info->lives==0){
								state->info->playing=false; //game over γιατι εχασε ολες τις ζωες
							}
							else{	//αλλιως αρχιζει απτην αρχη του λεβελ
								level_destroy(state->level);
								state->level=level_create(state->info->level);
								state->info->character->rect.x=0;
								state->info->baskets=0;
								state->info->character->forward=true;
								state->info->character->flying=false;
							}
						}
					}
			}
			list_destroy(flying_objects_around_me);
			
			//Ελεγχοι συγκρουσεις τερατων
			if(state->info->playing==true){ //αν δεν εχει συγκρουστει με κατι ο ηρωας συνεχιζω
				for (SetNode nodo = set_first(state->level->objects); nodo!=SET_EOF;nodo=set_next(state->level->objects,nodo)){
					if (((Object)set_node_value(state->level->objects, nodo))->type==GHOST){
						if (((Object)set_node_value(state->level->objects, nodo))->forward==true){
							if (set_next(state->level->objects, nodo)!=SET_EOF){
								SetNode nexto_nodo = set_next(state->level->objects, nodo);
								if (CheckCollisionRecs(((Object)set_node_value(state->level->objects, nodo))->rect, ((Object)set_node_value(state->level->objects, nexto_nodo))->rect)){
									if (((Object)set_node_value(state->level->objects, nexto_nodo))->type==GHOST){ //στις περιπτωσεις συγκρουσης με αλλο εχθρο ή εμποδιο δεν αλλοιωνεται η διαταξη του σετ
										((Object)set_node_value(state->level->objects, nodo))->forward=false;
										((Object)set_node_value(state->level->objects, nexto_nodo))->forward=true; //αντιστρεφει κατευθυνση και το αλλο τερας
									}
									else {
										((Object)set_node_value(state->level->objects, nodo))->forward=false;
									}
								}
							}
							else { //ελεγχω μη φυγει απτο οριο τελειως των floats και τον παω στν αρχη της πιστας
								if (((Object)set_node_value(state->level->objects, nodo))->rect.x>(__FLT_MAX__-555)) 
									((Object)set_node_value(state->level->objects, nodo))->rect.x=0;
							}
						}
						else {
							if (set_previous(state->level->objects, nodo)!=SET_BOF){ 	//αν δεν ειναι το πρωτο δηλαδη
								SetNode prevo_nodo = set_previous(state->level->objects, nodo); //κοιταει το προηγουμενο αντι για το επομενο αφου παει αντιθετα					 
								if (CheckCollisionRecs(((Object)set_node_value(state->level->objects, nodo))->rect, ((Object)set_node_value(state->level->objects, prevo_nodo))->rect)){
									if (((Object)set_node_value(state->level->objects, prevo_nodo))->type==GHOST){
										((Object)set_node_value(state->level->objects, nodo))->forward=true;
										((Object)set_node_value(state->level->objects, prevo_nodo))->forward=false;
									}
									else {
										((Object)set_node_value(state->level->objects, nodo))->forward=true; //απλα αλλαζει κατευθυνση
									}
								}
							}
							else { //ελεγχω μη φυγει απτο οριο τελειως των floats και τον παω στν αρχη της πιστας
								if (((Object)set_node_value(state->level->objects, nodo))->rect.x<(555-__FLT_MAX__)) 
									((Object)set_node_value(state->level->objects, nodo))->rect.x=0;
							}
						}
					}
				}
				// το ιδιο για τα ιπταμενα αντικειμενα, με τη διαφορα οτι κραταω καποια "συγκεντρωμενα" απο τη σκουπα μεχρι το checkpoint
				for (SetNode nodo = set_first(state->level->flying_objects); nodo!=SET_EOF;nodo=set_next(state->level->flying_objects,nodo)){
					if (((Object)set_node_value(state->level->flying_objects, nodo))->type==BIRD){
						if (((Object)set_node_value(state->level->flying_objects, nodo))->forward==true){
							if (set_next(state->level->flying_objects, nodo)!=SET_EOF){
								SetNode nexto_nodo = set_next(state->level->flying_objects, nodo);
								if ((CheckCollisionRecs(((Object)set_node_value(state->level->flying_objects, nodo))->rect, ((Object)set_node_value(state->level->flying_objects, nexto_nodo))->rect))||((((Object)set_node_value(state->level->flying_objects, nodo))->rect.x)+80>(((Object)set_node_value(state->level->flying_objects, nexto_nodo))->rect.x))){
									if (((Object)set_node_value(state->level->flying_objects, nexto_nodo))->type==BIRD){ //στις περιπτωσεις συγκρουσης με αλλο εχθρο ή εμποδιο δεν αλλοιωνεται η διαταξη του σετ
										((Object)set_node_value(state->level->flying_objects, nodo))->forward=false;
										((Object)set_node_value(state->level->flying_objects, nexto_nodo))->forward=true; //αντιστρεφει κατευθυνση και το αλλο τερας
									}
									else {
										((Object)set_node_value(state->level->flying_objects, nodo))->forward=false;
									}
								}
							}
							else { //ελεγχω μη φυγει απτο οριο τελειως των floats και τον παω στν αρχη της πιστας
								if (((Object)set_node_value(state->level->flying_objects, nodo))->rect.x>(__FLT_MAX__-555)) 
									((Object)set_node_value(state->level->flying_objects, nodo))->rect.x=0;
							}
						}
						else {
							if (((Object)set_node_value(state->level->flying_objects, nodo))->rect.x==-10){ //αν φτασει στο πορταλ
								((Object)set_node_value(state->level->flying_objects, nodo))->forward=true;
							}
							if (set_previous(state->level->flying_objects, nodo)!=SET_BOF){ 	//αν δεν ειναι το πρωτο δηλαδη
								SetNode prevo_nodo = set_previous(state->level->flying_objects, nodo); //κοιταει το προηγουμενο αντι για το επομενο αφου παει αντιθετα					 
								if ((CheckCollisionRecs(((Object)set_node_value(state->level->flying_objects, nodo))->rect, ((Object)set_node_value(state->level->flying_objects, prevo_nodo))->rect))||((((Object)set_node_value(state->level->flying_objects, nodo))->rect.x)-80<(((Object)set_node_value(state->level->flying_objects, prevo_nodo))->rect.x))){
									if (((Object)set_node_value(state->level->flying_objects, prevo_nodo))->type==BIRD){
										((Object)set_node_value(state->level->flying_objects, nodo))->forward=true;
										((Object)set_node_value(state->level->flying_objects, prevo_nodo))->forward=false;

									}
									else {
										((Object)set_node_value(state->level->flying_objects, nodo))->forward=true; //απλα αλλαζει κατευθυνση
									}
								}
							}
							else { //ελεγχω μη φυγει απτο οριο τελειως των floats και τον παω στν αρχη της πιστας
								if (((Object)set_node_value(state->level->flying_objects, nodo))->rect.x<(555-__FLT_MAX__)) 
									((Object)set_node_value(state->level->flying_objects, nodo))->rect.x=0;
							}
						}
					}
				}
			}

		}
	}
	else if (state->info->paused==true){
		if (keys->p) state->info->paused=false;
	}
}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.
void state_destroy(State state) {
    level_destroy(state->level);
	free(state->info->character);
	free(state->info);
	free(state);
}

// Καταστρέφει την κατάσταση level ελευθερώνοντας τη δεσμευμένη μνήμη.
void level_destroy(Level level) {
    set_destroy(level->flying_objects);
    set_destroy(level->objects);
	free(level);
}