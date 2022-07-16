///////////////////////////////////////////////////////////////////
//
// ADT Deque
//
// Abstract double-ended queue (deque). Παρέχει πρόσβαση σε
// οποιοδήποτε στοιχείο (μέσω της θέσης του), και προσθήκη/αφαίρεση
// στοιχείων στην αρχή ή στο τέλος του deque.
//
///////////////////////////////////////////////////////////////////

#pragma once // #include το πολύ μία φορά

#include "common_types.h"


// Ένα deque αναπαριστάται από τον τύπο Deque. Ο χρήστης δε χρειάζεται να γνωρίζει το περιεχόμενο
// του τύπου αυτού, απλά χρησιμοποιεί τις συναρτήσεις deque_<foo> που δέχονται και επιστρέφουν Deque.
//
// Ο τύπος Deque ορίζεται ως pointer στο "struct deque" του οποίου το περιεχόμενο είναι άγνωστο
// (incomplete struct), και εξαρτάται από την υλοποίηση του ADT Deque.
//
typedef struct deque* Deque;


// Δημιουργεί και επιστρέφει ένα νεό deque μεγέθους size, με στοιχεία αρχικοποιημένα σε NULL.
// Αν destroy_value != NULL, τότε καλείται destroy_value(value) κάθε φορά που αφαιρείται (ή αντικαθίσταται) ένα στοιχείο.

Deque deque_create(int size, DestroyFunc destroy_value);

// Επιστρέφει τον αριθμό στοιχείων που περιέχει το Deque deque.

int deque_size(Deque deque);

// Προσθέτει την τιμή value στην _αρχή_ του deque deque. Το μέγεθος του deque μεγαλώνει κατά 1.
// Τα περιεχόμενα του deque "μετακινούνται" κατά μία θέση, δηλαδή το στοιχείο που ήταν στην θέση
// pos πλέον βρίσκεται στη θέση pos+1.

void deque_insert_first(Deque deque, Pointer value);

// Προσθέτει την τιμή value στο _τέλος_ του deque. Το μέγεθος του deque μεγαλώνει κατά 1.

void deque_insert_last(Deque deque, Pointer value);

// Αφαιρεί το πρώτο στοιχείο του deque. Το μέγεθος του deque μικραίνει κατά 1.
// Τα περιεχόμενα του deque "μετακινούνται" κατά μία θέση, δηλαδή το στοιχείο που ήταν στην θέση
// pos πλέον βρίσκεται στη θέση pos-1. Αν το deque είναι κενό η συμπεριφορά είναι μη ορισμένη.

void deque_remove_first(Deque deque);

// Αφαιρεί το τελευταίο στοιχείο του deque. Το μέγεθος του deque μικραίνει κατά 1.
// Αν το deque είναι κενό η συμπεριφορά είναι μη ορισμένη.

void deque_remove_last(Deque deque);

// Επιστρέφει την τιμή στη θέση pos του deque deque (μη ορισμένο αποτέλεσμα αν pos < 0 ή pos >= size)

Pointer deque_get_at(Deque deque, int pos);

// Αλλάζει την τιμή στη θέση pos του Deque deque σε value.
// ΔΕΝ μεταβάλλει το μέγεθος του deque, αν pos >= size το αποτέλεσμα δεν είναι ορισμένο.

void deque_set_at(Deque deque, int pos, Pointer value);

// Βρίσκει και επιστρέφει το πρώτο στοιχείο στο deque που να είναι ίσο με value
// (με βάση τη συνάρτηση compare), ή NULL αν δεν βρεθεί κανένα στοιχείο.

Pointer deque_find(Deque deque, Pointer value, CompareFunc compare);

// Αλλάζει τη συνάρτηση που καλείται σε κάθε αφαίρεση/αντικατάσταση στοιχείου σε
// destroy_value. Επιστρέφει την προηγούμενη τιμή της συνάρτησης.

DestroyFunc deque_set_destroy_value(Deque deque, DestroyFunc destroy_value);

// Ελευθερώνει όλη τη μνήμη που δεσμεύει το deque deque.
// Οποιαδήποτε λειτουργία πάνω στο deque μετά το destroy είναι μη ορισμένη.

void deque_destroy(Deque deque);


// Διάσχιση του deque ////////////////////////////////////////////////////////////
//
// Οι παρακάτω συναρτήσεις επιτρέπουν τη διάσχιση του deque μέσω κόμβων.
// Δεν είναι τόσο συχνά χρησιμοποιούμενες όσο για άλλα ADTs, γιατί μπορούμε
// εύκολα να διασχίσουμε το deque και μέσω indexes. Παραμένουν πάντως χρήσιμες,
// τόσο για ομοιομορφία με τους άλλους ADTs, αλλά και γιατί για κάποιες υλοποιήσεις
// η διάσχιση μέσω κόμβων μπορεί να είναι πιο γρήγορη.

// Οι σταθερές αυτές συμβολίζουν εικονικούς κόμβους _πριν_ τον πρώτο και _μετά_ τον τελευταίο
#define DEQUE_BOF (DequeNode)0
#define DEQUE_EOF (DequeNode)0

typedef struct deque_node* DequeNode;

// Επιστρέφουν τον πρώτο και τον τελευταίο κομβο του deque, ή DEQUE_BOF / DEQUE_EOF αντίστοιχα αν το deque είναι κενό

DequeNode deque_first(Deque deque);
DequeNode deque_last(Deque deque);

// Επιστρέφουν τον επόμενο και τον προηγούμενο κομβο του node, ή DEQUE_EOF / DEQUE_BOF
// αντίστοιχα αν ο node δεν έχει επόμενο / προηγούμενο.

DequeNode deque_next(Deque deque, DequeNode node);
DequeNode deque_previous(Deque deque, DequeNode node);

// Επιστρέφει το περιεχόμενο του κόμβου node

Pointer deque_node_value(Deque deque, DequeNode node);

// Βρίσκει το πρώτο στοιχείο στο deque που να είναι ίσο με value (με βάση τη συνάρτηση compare).
// Επιστρέφει τον κόμβο του στοιχείου, ή DEQUE_EOF αν δεν βρεθεί.

DequeNode deque_find_node(Deque deque, Pointer value, CompareFunc compare);
