#include "MainMenu.h"
#include "Graphics.h"
#include "Mouse.h"
#include "timer.h"
#include "Bitmap.h"

MainMenu* createMainMenu() {
	MainMenu* state = (MainMenu*) malloc(sizeof(MainMenu));

	state->choice = 0;
	state->activeChoice = 0;
	state->board = loadBitmap("/home/lcom/lcom1415-t3g15/projeto/res/board.bmp");
	state->playBtn = loadBitmap("/home/lcom/lcom1415-t3g15/projeto/res/play-button.bmp");
	state->optionsBtn = loadBitmap("/home/lcom/lcom1415-t3g15/projeto/res/options-button.bmp");
	state->exitBtn = loadBitmap("/home/lcom/lcom1415-t3g15/projeto/res/exit-button.bmp");

	return state;
}

int updateMainMenu(MainMenu* state) {

	int draw = 1;
	if (mouseInsideRect(200, 140, 600, 230)) {
		state->activeChoice = 1;
		if (getMouse()->leftBtnReleased)
			state->choice = 1;
	}
	else if (mouseInsideRect(200, 240, 600, 330)) {
		state->activeChoice = 2;
		if (getMouse()->leftBtnReleased)
			state->choice = 2;
	}
	else if (mouseInsideRect(200, 340, 600, 430)) {
		state->activeChoice = 3;
		if (getMouse()->leftBtnReleased)
			state->choice = 3;
	}
	else{
		state->activeChoice = 0;
		draw = 0;
	}

	return draw;
}

void drawMainMenu(MainMenu* state) {
	drawBitmap(state->board, 0, 0, ALIGN_LEFT);
	drawBitmapWithoutBg(state->playBtn, 200, 140, ALIGN_LEFT);
	drawBitmapWithoutBg(state->optionsBtn, 200, 240, ALIGN_LEFT);
	drawBitmapWithoutBg(state->exitBtn, 200, 340, ALIGN_LEFT);
}

void deleteMainMenu(MainMenu* state) {
	deleteBitmap(state->board);
	deleteBitmap(state->playBtn);
	deleteBitmap(state->optionsBtn);
	deleteBitmap(state->exitBtn);
	free(state);
}
