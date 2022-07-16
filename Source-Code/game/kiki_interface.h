#pragma once

#include "kikistate.h"

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 675

// Αρχικοποιεί το interface του παιχνιδιού
void interface_init();

// Κλείνει το interface του παιχνιδιού
void interface_close();

void interface_loading_screen();

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(State state);