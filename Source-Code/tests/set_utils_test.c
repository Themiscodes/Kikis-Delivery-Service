//////////////////////////////////////////////////////////////////
//
// Test για το set_utils.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "ADTSet.h"
#include "set_utils.h"


// Βοηθητικες συναρτησεις για το test
int compare_ints(Pointer b, Pointer a){
    return *(int*)b - *(int*)a;
}

int compare_floats(Pointer b, Pointer a){
    return *(float*)b - *(float*)a;
}

int* create_int(int value) {
	int* pointer = malloc(sizeof(int));		
	*pointer = value;						
	return pointer;
}

// Test για set_utils 
void test_set_utils(){
	Set ultimateset = set_create(compare_ints, free);
	set_insert(ultimateset, create_int(67));
	set_insert(ultimateset, create_int(75));
	set_insert(ultimateset, create_int(53));
	set_insert(ultimateset, create_int(22));
	set_insert(ultimateset, create_int(33));
	
	//equal TEST
	TEST_ASSERT(set_find_eq_or_greater(ultimateset, create_int(22))==set_node_value(ultimateset, set_first(ultimateset)));

	//greater TEST
	TEST_ASSERT(set_find_eq_or_greater(ultimateset, create_int(73))==set_node_value(ultimateset, set_last(ultimateset))); 

	//smaller TEST
	TEST_ASSERT(set_find_eq_or_smaller(ultimateset, create_int(23))==set_node_value(ultimateset, set_first(ultimateset))); 

	set_destroy(ultimateset);

}

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_set_utils", test_set_utils },

	{NULL, NULL } // τερματίζουμε τη λίστα με NULL
};