#include "raylib.h"
#include "kikistate.h"
#include "kiki_interface.h"

//Kiki
Texture kiki_img;
Texture kiki_left_img;
Texture kiki_flying_img;
Texture kiki_flying_left_img;

//Enemies
Texture eagle_img;
Texture eagle_left_img;
Texture ghost_img;
Texture ghost_left_img;

//scenery & Items
Texture portal_img;
Texture flag_img;
Texture broom_img;
Texture lito_broom_img;
Texture bread_img;
Texture lito_bread_img;
Texture meadow_img;
Texture heart_img;
Texture keys_img;

//loading screens
Texture totoro_img;
Texture logo_img;
Texture kiki_pause_img;
Texture kiki_angry_img;
Texture kiki_broom_img;

//Music
Music intro_song;

float frameWidth;
int maxFrames; 
float timer;
int frame;

void interface_init() {
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kiki's Delivery Service");
	SetTargetFPS(60);
	InitAudioDevice();

	// Φόρτωση εικόνων και ήχων
	kiki_img = LoadTextureFromImage(LoadImage("assets/kikiall.png"));
	kiki_angry_img = LoadTextureFromImage(LoadImage("assets/angry.png"));
	kiki_pause_img = LoadTextureFromImage(LoadImage("assets/pause.png"));
	kiki_left_img = LoadTextureFromImage(LoadImage("assets/kikialleft.png"));
	kiki_flying_img = LoadTextureFromImage(LoadImage("assets/flying.png"));
	kiki_flying_left_img =LoadTextureFromImage(LoadImage("assets/flyingleft.png"));
	kiki_broom_img = LoadTextureFromImage(LoadImage("assets/kiki6.png"));
	meadow_img = LoadTextureFromImage(LoadImage("assets/mead.png"));
	portal_img = LoadTextureFromImage(LoadImage("assets/portal5.png"));
	bread_img = LoadTextureFromImage(LoadImage("assets/bread.png"));
	lito_bread_img = LoadTextureFromImage(LoadImage("assets/breadsmall.png"));
	broom_img = LoadTextureFromImage(LoadImage("assets/broom.png"));
	flag_img = LoadTextureFromImage(LoadImage("assets/flag.png"));
	heart_img = LoadTextureFromImage(LoadImage("assets/heart.png"));
	ghost_img = LoadTextureFromImage(LoadImage("assets/wolf.png"));
	eagle_img = LoadTextureFromImage(LoadImage("assets/eagsmall.png"));
	ghost_left_img = LoadTextureFromImage(LoadImage("assets/wolfleft.png"));
	eagle_left_img = LoadTextureFromImage(LoadImage("assets/eagsmallleft.png"));
	intro_song = LoadMusicStream("assets/intro.mp3");
	totoro_img = LoadTextureFromImage(LoadImage("assets/totoro.png"));
	logo_img =LoadTextureFromImage(LoadImage("assets/logo.png"));
	keys_img =LoadTextureFromImage(LoadImage("assets/keys4.png"));
	lito_broom_img =LoadTextureFromImage(LoadImage("assets/broomsmall.png"));

	// αυτο ειναι για να υπολογιζω αργοτερα ποιο frame απτο png θα παιρνω για τα animations
	// τελικα δεν το χρησιμοποιω ετσι γιατι δεν δουλευει για καποιο λογο στο web version
	// αλλα δεν το σβηνω γιατι το χρησιμοποιω για την desktop version
	frameWidth = (float)(kiki_img.width / 8);
	maxFrames = 8;
	timer = 0.0f;
	frame = 0;
}


// με οδηγιες LOADING SCREEN αριστερα κατω
void interface_loading_screen(){
	BeginDrawing();
	ClearBackground(DARKBLUE);

	DrawTexture(totoro_img, SCREEN_WIDTH-totoro_img.width, SCREEN_HEIGHT-totoro_img.height, WHITE);
	DrawTexture(logo_img, (SCREEN_WIDTH-logo_img.width)/2, 65, WHITE);
	DrawTexture(kiki_broom_img, 10, 115, WHITE);


	DrawText(TextFormat("AVOID"), 24, SCREEN_HEIGHT- 13-keys_img.height/2, 20, YELLOW);
	DrawText(TextFormat("ENEMIES"), 7, SCREEN_HEIGHT+6-keys_img.height/2, 20, YELLOW);
	DrawTexture(keys_img, keys_img.width+37, SCREEN_HEIGHT-34 -keys_img.height/2, WHITE);

	DrawText(TextFormat("COLLECT"), 185, SCREEN_HEIGHT-3-keys_img.height/2, 20, YELLOW);
	DrawTexture(lito_bread_img, 283, SCREEN_HEIGHT-38-lito_bread_img.height/2, WHITE);
	
	DrawText(TextFormat("DELIVER"), 340, SCREEN_HEIGHT- 3-keys_img.height/2, 20, YELLOW);
	DrawTexture(lito_broom_img, 423, SCREEN_HEIGHT- 29-lito_broom_img.height/2, WHITE);

	DrawText(
			"PRESS [ENTER] TO PLAY",
			 GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO PLAY", 20) / 2,
			 GetScreenHeight() - 305, 20, WHITE
		);
	DrawTexture(kiki_broom_img, 10, 115, WHITE);
	EndDrawing();
}

void interface_pause_screen(StateInfo info){
	BeginDrawing();
	ClearBackground(DARKBLUE);
	PauseMusicStream(intro_song);

	DrawTexture(totoro_img, SCREEN_WIDTH-totoro_img.width, SCREEN_HEIGHT-totoro_img.height, WHITE);
	DrawTexture(logo_img, (SCREEN_WIDTH-logo_img.width)/2, 58, WHITE);
	DrawTexture(kiki_pause_img, 120, 357, WHITE);
		DrawText(
			"GAME PAUSED",
			 43,
			 GetScreenHeight() - 91, 45, YELLOW
		);
		DrawText(
			"PRESS [p] TO RESUME",
			 90,
			 GetScreenHeight() - 40, 20, WHITE
		);


	DrawText(TextFormat("LEVEL: %d", info->level), 10, 0, 30, YELLOW);
	DrawText(TextFormat("SCORE: %d", info->score), 10, 30, 30, YELLOW);
	for (int i=0; i<info->lives;i++)
		DrawTexture(heart_img, SCREEN_WIDTH - 3-(2*i) - ((i+1)*heart_img.width), 2, WHITE);

	EndDrawing();
}

void interface_gameover_screen(int highscore){
	BeginDrawing();
	ClearBackground(DARKBLUE);

	DrawTexture(totoro_img, SCREEN_WIDTH-totoro_img.width, SCREEN_HEIGHT-totoro_img.height, WHITE);
	DrawTexture(logo_img, (SCREEN_WIDTH-logo_img.width)/2, 65, WHITE);
	DrawTexture(kiki_angry_img, 120, 357, WHITE);

	DrawText(TextFormat("HIGHSCORE: %d", highscore), 10, 0, 30, YELLOW);

	DrawText(
			"GAME OVER",
			 43,
			 GetScreenHeight() - 91, 60, WHITE
		);
		DrawText(
			"PRESS [ENTER] TO PLAY AGAIN",
			 60,
			 GetScreenHeight() - 40, 20, WHITE
		);
	
	EndDrawing();
}

void interface_close() {
	CloseWindow();
	UnloadMusicStream(intro_song);
    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)
}

// Draw game (one frame)
void interface_draw_frame(State state) {

	StateInfo info = state_info(state);

	if (!info->start){
		interface_loading_screen();
	}
	// Αν εχει πατηθει pause
	else if (info->paused) {
		interface_pause_screen(info);
	}
	// Αν το παιχνίδι έχει τελειώσει, σχεδιάζομαι το μήνυμα για να ξαναρχίσει
	else if (!(info->playing)) { 
		StopMusicStream(intro_song);
		interface_gameover_screen(info->score);
	}
	else {

		BeginDrawing();
		ClearBackground(WHITE);
		
		UpdateMusicStream(intro_song); //για την μουσικη
		PlayMusicStream(intro_song);

		timer += GetFrameTime();
		frame = frame % maxFrames; 

		if ((IsKeyDown(KEY_RIGHT))||(IsKeyDown(KEY_LEFT))) {
			if(timer>=0.05f){ // για να φαινεται οτι παει γρηγορα:)
				timer =0.0f;
				frame+=1;
			}
		}
		else {
			if(timer>=0.1f){
				timer =0.0f;
				frame+=1;
			}
		}

		if (info->character->flying==false){
			// Σχεδιάζουμε τον χαρακτήρα και το εδαφος. Κραταω τον χαρακτηρα σταθερο στον αξονα των χ, μονο στο y να αλλαζει (το -72 ειναι για το υψος του εδαφος)
			if (info->character->forward==true){
				Vector2 position = {230, (SCREEN_HEIGHT-182-meadow_img.height+info->character->rect.y)};
				Rectangle rect = {(220*(frame%8)), 0, 220, (float)kiki_img.height};
				DrawTextureRec(
					kiki_img, 
					rect,
					position,
					WHITE); 
			}
			else {
				Vector2 position = {230, (SCREEN_HEIGHT-182-meadow_img.height+info->character->rect.y)};
				Rectangle rect = {(float)(220*(7-(frame%8))), 0, 220, (float)kiki_left_img.height}; 
				DrawTextureRec(
					kiki_left_img, 
					rect,
					position,
					WHITE); 
			}
		}
		else{
			//Το ιδιο για αν πεταει
			if (info->character->forward==true){
				Vector2 position = {230, (SCREEN_HEIGHT-182-meadow_img.height+info->character->rect.y)};
				Rectangle rect = {(300*(frame%2)), 0, 300, (float)kiki_flying_img.height};
				DrawTextureRec(
					kiki_flying_img, 
					rect,
					position,
					WHITE); 
			}
			else {
				//added 2 instead of MAXFRAMES
				Vector2 position = {230, (SCREEN_HEIGHT-182-meadow_img.height+info->character->rect.y)};
				Rectangle rect = {(float)(300*(1-(frame%2))), 0, 300, (float)kiki_flying_left_img.height}; 
				DrawTextureRec(
					kiki_flying_left_img, 
					rect,
					position,
					WHITE); 
			}
		}

		Vector2 position2 = {0, (SCREEN_HEIGHT-meadow_img.height+10)};
		Rectangle rect2 = {(1200*(frame%3)), 0, 1200, (float)meadow_img.height};
		DrawTextureRec(
				meadow_img, 
				rect2,
				position2,
				WHITE); 

		//Σχεδιαζει τα αντικειμενα
		List onscreen_objects = state_objects(state, (info->character->rect.x-230), (info->character->rect.x+SCREEN_WIDTH-230)); //Σχεδιαζω το κοσμο γυρω του συναρτησει του χαρακτηρα
		if (list_size(onscreen_objects)!=0){
			for(ListNode nodo = list_first(onscreen_objects); nodo!=LIST_EOF; nodo=list_next(onscreen_objects,nodo)){
				if (((Object)list_node_value(onscreen_objects,nodo))->type==PORTAL){
					float fwidth = ((Object)list_node_value(onscreen_objects,nodo))->rect.width;
					Vector2 position3 = {(230-(info->character->rect.x)+((Object)list_node_value(onscreen_objects,nodo))->rect.x), (SCREEN_HEIGHT-meadow_img.height+10+((Object)list_node_value(onscreen_objects,nodo))->rect.y)};
					Rectangle rect3 = {(fwidth*(frame%11)), 0, fwidth, (float)portal_img.height};
					DrawTextureRec(
						portal_img, 
						rect3,
						position3,
						WHITE); 
				}
				else if (((Object)list_node_value(onscreen_objects,nodo))->type==BREAD){
					DrawTexture(bread_img, (230-(info->character->rect.x)+((Object)list_node_value(onscreen_objects,nodo))->rect.x), (SCREEN_HEIGHT-meadow_img.height+10+((Object)list_node_value(onscreen_objects,nodo))->rect.y), WHITE);	
				}
				else if (((Object)list_node_value(onscreen_objects,nodo))->type==GHOST){
					if (((Object)list_node_value(onscreen_objects,nodo))->forward==true){
						float fwidth = 104; //actual size of the image οχι αυτο που χεις μεσα στο state
						Vector2 position3 = {(230-(info->character->rect.x)+((Object)list_node_value(onscreen_objects,nodo))->rect.x), (SCREEN_HEIGHT-meadow_img.height-50)};
						Rectangle rect3 = {(fwidth*(maxFrames-(frame%11))), 0, fwidth, 62};
						DrawTextureRec(
							ghost_left_img, 
							rect3,
							position3,
							WHITE); 
					}
					else{
						float fwidth = 104;
						Vector2 position3 = {(230-(info->character->rect.x)+((Object)list_node_value(onscreen_objects,nodo))->rect.x), (SCREEN_HEIGHT-meadow_img.height-50)};
						Rectangle rect3 = {(fwidth*(frame%11)), 0, fwidth, 62};
						DrawTextureRec(
							ghost_img, 
							rect3,
							position3,
							WHITE); 	
					}
				}
				else if (((Object)list_node_value(onscreen_objects,nodo))->type==BROOM){
					DrawTexture(broom_img, (230-(info->character->rect.x)+((Object)list_node_value(onscreen_objects,nodo))->rect.x), (SCREEN_HEIGHT-meadow_img.height+10+((Object)list_node_value(onscreen_objects,nodo))->rect.y), WHITE);	
				}
				else if (((Object)list_node_value(onscreen_objects,nodo))->type==CHECKPOINT){
					DrawTexture(flag_img, (230-(info->character->rect.x)+((Object)list_node_value(onscreen_objects,nodo))->rect.x), (SCREEN_HEIGHT-meadow_img.height+12+((Object)list_node_value(onscreen_objects,nodo))->rect.y), WHITE);	
				}
			}
		}
		list_destroy(onscreen_objects);
		//Και για τα ιπταμενα το ιδιο
		List onscreen_flying_objects = state_flying_objects(state, (info->character->rect.x-230), (info->character->rect.x+SCREEN_WIDTH-230)); //Σχεδιαζω το κοσμο γυρω του συναρτησει του χαρακτηρα
		if (list_size(onscreen_flying_objects)!=0){
			for(ListNode nodo = list_first(onscreen_flying_objects); nodo!=LIST_EOF; nodo=list_next(onscreen_flying_objects,nodo)){
				if (((Object)list_node_value(onscreen_flying_objects,nodo))->type==BIRD){
					if (((Object)list_node_value(onscreen_objects,nodo))->forward==true){
						float fwidth = 157.5;
						Vector2 position3 = {(230-(info->character->rect.x)+((Object)list_node_value(onscreen_flying_objects,nodo))->rect.x), (SCREEN_HEIGHT-meadow_img.height-191+((Object)list_node_value(onscreen_flying_objects,nodo))->rect.y)};
						Rectangle rect3 = {(fwidth*(frame%4)), 0, fwidth, 88};
						DrawTextureRec(
							eagle_left_img,
							rect3,
							position3,
							WHITE); 
					}
					else{
						float fwidth = 157.5;
						Vector2 position3 = {(230-(info->character->rect.x)+((Object)list_node_value(onscreen_flying_objects,nodo))->rect.x), (SCREEN_HEIGHT-meadow_img.height-191+((Object)list_node_value(onscreen_flying_objects,nodo))->rect.y)};
						Rectangle rect3 = {(fwidth*(frame%4)), 0, fwidth, 88};
						DrawTextureRec(
							eagle_img, 
							rect3,
							position3,
							WHITE); 
					}
				}
			}
		}
		list_destroy(onscreen_flying_objects);
		DrawText(TextFormat("LEVEL: %d", info->level), 10, 0, 30, RED);
		DrawText(TextFormat("SCORE: %d", info->score), SCREEN_WIDTH/2-80, 0, 30, DARKBLUE);
		for (int i=0; i<info->lives;i++)
			DrawTexture(heart_img, SCREEN_WIDTH - 3-(2*i) - ((i+1)*heart_img.width), 2, WHITE);
		EndDrawing();
	}
}