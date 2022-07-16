//////////////////////////////////////////////////////////////////
//
// Unit tests για τον ADT Deque.
// Οποιαδήποτε υλοποίηση οφείλει να περνάει όλα τα tests.
//
// Τα tests είναι ολόιδια με τα αντίστοιχα του ADTVector, με
// μοναδική διαφορά τα test_insert_first, test_remove_first.
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "ADTDeque.h"


void test_create(void) {
	Deque deque = deque_create(0, NULL);
	deque_set_destroy_value(deque, NULL);

	TEST_ASSERT(deque_size(deque) == 0);

	deque_destroy(deque);
}

void test_insert_last(void) {
	Deque deque = deque_create(0, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));		// Στο deque θα προσθέσουμε pointers προς τα στοιχεία αυτού του πίνακα

	// insert 1000 στοιχεία ώστε να συμβούν πολλαπλά resizes
	for (int i = 0; i < 1000; i++) {
		deque_insert_last(deque, &array[i]);
		TEST_ASSERT(deque_size(deque) == i+1);		// Το size ενημερώθηκε;
		TEST_ASSERT(deque_get_at(deque, i) == &array[i]);	// Μπορούμε να κάνουμε at το στοιχείο που μόλις βάλαμε;
	}

	// Δοκιμή ότι μετά τα resizes τα στοιχεία είναι ακόμα προσπελάσιμα
	for (int i = 0; i < 1000; i++)
		TEST_ASSERT(deque_get_at(deque, i) == &array[i]);

	deque_destroy(deque);
	free(array);
}

void test_insert_first(void) {
	Deque deque = deque_create(0, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));		// Στο deque θα προσθέσουμε pointers προς τα στοιχεία αυτού του πίνακα

	// insert 1000 στοιχεία ώστε να συμβούν πολλαπλά resizes οπως και πριν
	for (int i = 0; i < 1000; i++) {
		deque_insert_first(deque, &array[i]);	// απλα εδω θα μπαινουν αναποδα δλδ στην αρχη
		TEST_ASSERT(deque_size(deque) == i+1);		// Το size ενημερώθηκε
		TEST_ASSERT(deque_get_at(deque, 0) == &array[i]);	// ελεγχω αν ειναι το πρωτο στοιχειο αυτο που βαλαμε μολις
	}

	// Δοκιμή ότι μετά τα resizes τα στοιχεία είναι ακόμα προσπελάσιμα και στις σωστες θεσεις
	for (int i = 0; i < 1000; i++){
		TEST_ASSERT(deque_get_at(deque, 999-i) == &array[i]); //999-i μιας και εχουν μπει αναποδα
	}

	deque_destroy(deque);
	free(array);
}

void test_remove_last(void) {
	Deque deque = deque_create(1000, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));

	// replace για προσθήκη δεδομένων, χωρίς ελέγχους (έχουμε ξεχωριστό test για το replace)
	for (int i = 0; i < 1000; i++)
		deque_set_at(deque, i, &array[i]);

	// Διαδοχικά remove ώστε να συμβούν και resizes
	for (int i = 999; i >= 0; i--) {
		TEST_ASSERT(deque_get_at(deque, i) == &array[i]);
		deque_remove_last(deque);
		TEST_ASSERT(deque_size(deque) == i);
	}

	deque_destroy(deque);
	free(array);
}

void test_remove_first(void) {
	Deque deque = deque_create(1000, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));

	for (int i = 0; i < 1000; i++)
		deque_set_at(deque, i, &array[i]);

	// Διαδοχικά remove ώστε να συμβούν και resizes
	for (int i = 0; i < 1000; i++) {
		TEST_ASSERT(deque_get_at(deque, 0) == &array[i]); 
		deque_remove_first(deque);
		TEST_ASSERT(deque_size(deque) == 999-i); 
	}

	deque_destroy(deque);
	free(array);
}

void test_get_set_at(void) {
	Deque deque = deque_create(0, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));

	// insert πολλαπλά NULL, θα τα αλλάξουμε μετά με replace
	for (int i = 0; i < 1000; i++)
		deque_insert_last(deque, NULL);

	for (int i = 0; i < 1000; i++) {
		TEST_ASSERT(deque_get_at(deque, i) == NULL);
		deque_set_at(deque, i, &array[i]);
		TEST_ASSERT(deque_get_at(deque, i) == &array[i]);
	}

	deque_destroy(deque);
	free(array);
}

void test_iterate(void) {
	Deque deque = deque_create(0, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));		// Στο deque θα προσθέσουμε pointers προς τα στοιχεία αυτού του πίνακα

	// first/last σε κενό deque
	TEST_ASSERT(deque_first(deque) == DEQUE_BOF);
	TEST_ASSERT(deque_last(deque) == DEQUE_EOF);

	// εισαγωγή στοιχείων
	for (int i = 499; i > -1; i--)
		deque_insert_first(deque, &array[i]);
	// τα εισαγω ετσι σπαστα με δυο τροπους για να δοκιμασω
	// οτι ολα λειτουργουν σωστα οι previous και next μιας
	// και με αυτο το τροπο μπαινουνε ανακατεμενα στο πινακα
	// ειναι σημαντικο για την υποπεριπτωση που παμε previous
	// απο την αρχη του πινακα αν εχουν γινει αναδιαταξεις
	//και το αντιστοιχο για την next
	for (int i = 500; i < 1000; i++)
		deque_insert_last(deque, &array[i]);

	for (int i = 0; i < 1000; i++) {
		TEST_ASSERT(deque_get_at(deque, i) == &array[i]); 
	}

	int i = 0;
	for (DequeNode node = deque_first(deque); node != DEQUE_EOF; node = deque_next(deque, node))
		TEST_ASSERT(deque_node_value(deque, node) == &array[i++]);
	TEST_ASSERT(i == N);

	for (DequeNode node = deque_last(deque); node != DEQUE_BOF; node = deque_previous(deque, node))
		TEST_ASSERT(deque_node_value(deque, node) == &array[--i]);
	TEST_ASSERT(i == 0);

	deque_destroy(deque);
	free(array);
}


int compare_ints(Pointer a, Pointer b) {
	return *(int*)a - *(int*)b;
}

void test_find(void) {
	Deque deque = deque_create(1000, NULL);
	int N = 1000;
	int* array = malloc(N * sizeof(*array));

	// replace για προσθήκη δεδομένων
	for (int i = 0; i < 1000; i++) {
		array[i] = i;
		deque_set_at(deque, i, &array[i]);
	}

	for (int i = 0; i < 1000; i++) {
		int* p = deque_find(deque, &i, compare_ints);
		TEST_ASSERT(*p == i);

		DequeNode node = deque_find_node(deque, &i, compare_ints);
		TEST_ASSERT(*(int*)deque_node_value(deque, node) == i);
	}

	int not_exists = -12;
	TEST_ASSERT(deque_find(deque, &not_exists, compare_ints) == NULL);
	TEST_ASSERT(deque_find_node(deque, &not_exists, compare_ints) == DEQUE_EOF);

	deque_destroy(deque);
	free(array);
}

void test_destroy(void) {
	// Απλά εκτελούμε την destroy, για να ελέγξουμε αν όντως δουλεύει
	// σωστά τρέχουμε το test με valgrind.

	Deque deque = deque_create(1, free);

	deque_set_at(deque, 0, malloc(1));
	deque_insert_last(deque, malloc(1));
	deque_remove_last(deque);

	deque_destroy(deque);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "deque_create", test_create },
	{ "deque_insert_last", test_insert_last },
	{ "deque_insert_first", test_insert_first },
	{ "deque_remove_last", test_remove_last },
	{ "deque_remove_first", test_remove_first },
	{ "deque_get_set_at", test_get_set_at },
	{ "deque_iterate", test_iterate },
	{ "deque_find", test_find },
	{ "deque_destroy", test_destroy },
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};