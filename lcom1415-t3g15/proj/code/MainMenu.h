#pragma once

#include "Bitmap.h"

typedef struct {

	int choice;
	int activeChoice;
	Bitmap* board;
	Bitmap* playBtn;
	Bitmap* optionsBtn;
	Bitmap* exitBtn;

} MainMenu;

MainMenu* createMainMenu();
int updateMainMenu(MainMenu* state);
void drawMainMenu(MainMenu* state);
void deleteMainMenu(MainMenu* state);
