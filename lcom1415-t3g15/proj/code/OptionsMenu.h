#pragma once

#include "Bitmap.h"

typedef struct {
	
	float difficulty;
	int gameTime;
	int choice;
	int choiceExit;
	int done;
	int activeChoice;
	Bitmap* board;
	Bitmap* easyBtn;
	Bitmap* mediumBtn;
	Bitmap* hardBtn;
	Bitmap* backBtn;
	Bitmap* btn30;
	Bitmap* btn90;
	Bitmap* btn120;

} OptionsMenu;

OptionsMenu* createOptionsMenu();
int updateOptionsMenu(OptionsMenu* state);
void drawOptionsMenu(OptionsMenu* state);
void deleteOptionsMenu(OptionsMenu* state);
