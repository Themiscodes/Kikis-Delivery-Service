#include <stdlib.h>
#include "raylib.h"

#include "kiki_interface.h"
#include "kikistate.h"

State state;
KeyState keys;

void update_and_draw(){

	keys->enter=IsKeyPressed(KEY_ENTER);
	keys->left=IsKeyDown(KEY_LEFT);
	keys->right=IsKeyDown(KEY_RIGHT);
	keys->up=IsKeyDown(KEY_UP);
	keys->p=IsKeyPressed(KEY_P);
	keys->down=IsKeyDown(KEY_DOWN);
	StateInfo information = state_info(state);
	if ((!information->start)&&(keys->enter)) information->start=true;
	else if ((information->playing==false)&&(keys->enter)){ 
		state_destroy(state);
		state = state_create();
	}
	state_update(state, keys);
	interface_draw_frame(state);
}

int main() {
	state = state_create();
	interface_init();
	keys = malloc(sizeof(KeyState));
	start_main_loop(update_and_draw);
	interface_close();
	state_destroy(state);
	free(keys);
	return 0;
}
