//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "state.h"

void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	// Ελεγχοι για την state->info
	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);
	TEST_ASSERT(info->current_portal == 0);
	TEST_ASSERT(info->wins == 0);
	TEST_ASSERT(info->playing);
	TEST_ASSERT(!info->paused);
	TEST_ASSERT((info->character->rect.x==0));
	TEST_ASSERT(info->character->type==CHARACTER);
	TEST_ASSERT(info->character->forward==true);
	TEST_ASSERT(info->character->jumping==false);
	TEST_ASSERT((info->character->rect.x==0));

	// Ελεγχος της state_objects και ταυτοχρονα των αντικειμενων στην state->objects
	List objecti = state_objects(state, 0, 10000); 
	TEST_ASSERT(objecti!=NULL);
	int i =0;
	for(ListNode nodo = list_first(objecti); nodo!=LIST_EOF; nodo=list_next(objecti,nodo)){
		TEST_ASSERT((((Object)list_node_value(objecti,nodo))->rect.x<=10000)&&(((Object)list_node_value(objecti,nodo))->rect.x>=0)); //coordinates check
		if (i%4==3) {
			TEST_ASSERT(((Object)list_node_value(objecti,nodo))->type==1); // Ελεγχος οτι καθε τεταρτο αντικειμενο ειναι πυλη
		}
		i++;
	}

	// Αποδεσμευση μνημης
	list_destroy(objecti);
	state_destroy(state);
}

void test_state_update() {
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, ο χαρακτήρας μετακινείται 7 pixels μπροστά
	Rectangle old_rect = state_info(state)->character->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->character->rect;
	TEST_ASSERT( new_rect.x == old_rect.x + 7 && new_rect.y == old_rect.y );

	// Με πατημένο το δεξί βέλος, ο χαρακτήρας μετακινείται 12 pixels μπροστά
	keys.right = true;
	old_rect = state_info(state)->character->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;
	TEST_ASSERT( new_rect.x == old_rect.x + 12 && new_rect.y == old_rect.y );

	//Eλεγχος αλματος και πτωσης
	old_rect=state_info(state)->character->rect;
	keys.up=true;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;
	TEST_ASSERT(new_rect.y == old_rect.y && new_rect.x==old_rect.x+12 );
	state_update(state, &keys); // Αλλο ενα update γιατι πρωτα γινεται το jumping true κ στο επομενο update ξεκινα το αλμα
	new_rect = state_info(state)->character->rect;
	TEST_ASSERT(new_rect.y == old_rect.y-15 && new_rect.x==old_rect.x+24 ); // +24 γιατι συγκρινω με το old rect δυο update πριν
	keys.up=false; keys.right=false; // αρχιζει και πεφτει μολις αφεθει το up
	old_rect=state_info(state)->character->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;
	TEST_ASSERT(new_rect.y == old_rect.y+15 && new_rect.x==old_rect.x+7);

	//Ελεγχος αλλαγης κατευθυνσης
	keys.left=true;
	old_rect=state_info(state)->character->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;
	TEST_ASSERT(new_rect.y == old_rect.y && new_rect.x==old_rect.x);
	old_rect=state_info(state)->character->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;
	TEST_ASSERT(new_rect.y == old_rect.y && new_rect.x==old_rect.x-12);
	
	// Σταματαει να πατιεται το left, ελεγχος αν συνεχιζει αριστερα
	keys.left=false; 
	old_rect=state_info(state)->character->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->character->rect;
	TEST_ASSERT(new_rect.y == old_rect.y && new_rect.x==old_rect.x-7);

	state_destroy(state);
}

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },

	{NULL, NULL } // τερματίζουμε τη λίστα με NULL
};