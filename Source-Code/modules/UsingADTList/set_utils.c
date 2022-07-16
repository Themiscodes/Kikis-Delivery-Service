#include <stdlib.h>

#include "set_utils.h"

Pointer set_find_eq_or_greater(Set set, Pointer value){
    if (set_find(set, value)!=NULL) return set_find(set, value);
    set_insert(set, value);
    SetNode greater = set_next(set, set_find_node(set, value)); 
    set_remove(set, value);
    if (greater == SET_EOF) return NULL;
    return (set_node_value(set, greater)); 
}

Pointer set_find_eq_or_smaller(Set set, Pointer value){
    if (set_find(set, value)!=NULL) return set_find(set, value);
    set_insert(set, value);
    SetNode smaller = set_previous(set, set_find_node(set, value)); 
    set_remove(set, value);
    if (smaller == SET_BOF) return NULL;
    return (set_node_value(set, smaller));
}