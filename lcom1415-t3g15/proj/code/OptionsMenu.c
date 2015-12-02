#include "OptionsMenu.h"
#include "Graphics.h"
#include "Mouse.h"
#include "timer.h"
#include "Bitmap.h"

OptionsMenu* createOptionsMenu() {
	OptionsMenu* state = (OptionsMenu*) malloc(sizeof(OptionsMenu));
	state->choice = 0;
	state->activeChoice = 0;
	state->difficulty = 2;
	state->gameTime = 60;
	state->choiceExit = 0;
	state->done = 0;
	state->board = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/board.bmp");
	state->easyBtn = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/easy-button.bmp");
	state->mediumBtn = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/medium-button.bmp");
	state->hardBtn = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/hard-button.bmp");
	state->btn30 = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/30-button.bmp");
	state->btn90 = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/90-button.bmp");
	state->btn120 = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/120-button.bmp");
	state->backBtn = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/back-button.bmp");
	return state;

}

int updateOptionsMenu(OptionsMenu* state) {

	if (mouseInsideRect(250, 200, 390, 280)) { //easy
		state->activeChoice = 1;
		if (getMouse()->leftBtnReleased) {
			state->choice = 1;
			state->difficulty = 2.0;
		}
	}
	else if (mouseInsideRect(250, 300, 390, 380)) { //medium
		state->activeChoice = 2;
		if (getMouse()->leftBtnReleased) {
			state->choice = 2;
			state->difficulty = 1.0;
		}
	}
	else if (mouseInsideRect(250, 400, 390, 480)) { //hard
		state->activeChoice = 3;
		if (getMouse()->leftBtnReleased){
			state->choice = 3;
			state->difficulty = 0.5;
		}
	}
	if (mouseInsideRect(400, 200, 540, 280)) { //30
		state->activeChoice = 4;
		if (getMouse()->leftBtnReleased) {
			state->gameTime = 30;
		}
	}
	else if (mouseInsideRect(400, 300, 540, 380)) { //90
		state->activeChoice = 5;
		if (getMouse()->leftBtnReleased) {
			state->gameTime = 90;
		}
	}
	else if (mouseInsideRect(400, 400, 540, 480)) { //120
		state->activeChoice = 6;
		if (getMouse()->leftBtnReleased){
			state->gameTime = 120;
		}
	}
	else if (mouseInsideRect(200, 500, 600, 590)) { //back to main menu
		state->activeChoice = 7;
		if (getMouse()->leftBtnReleased)
			state->choiceExit = 1;
	}
	else
		state->activeChoice = 0;
}


void drawOptionsMenu(OptionsMenu* state) {

	drawBitmap(state->board, 0, 0, ALIGN_LEFT);
	drawBitmapWithoutBg(state->easyBtn, 250, 200, ALIGN_LEFT);
	drawBitmapWithoutBg(state->mediumBtn, 250, 300, ALIGN_LEFT);
	drawBitmapWithoutBg(state->hardBtn, 250, 400, ALIGN_LEFT);
	drawBitmapWithoutBg(state->btn30, 400, 200, ALIGN_LEFT);
	drawBitmapWithoutBg(state->btn90, 400, 300, ALIGN_LEFT);
	drawBitmapWithoutBg(state->btn120, 400, 400, ALIGN_LEFT);
	drawBitmapWithoutBg(state->backBtn, 20, 500, ALIGN_LEFT);


}

void deleteOptionsMenu(OptionsMenu* state) {

	deleteBitmap(state->board);
	deleteBitmap(state->easyBtn);
	deleteBitmap(state->mediumBtn);
	deleteBitmap(state->hardBtn);
	deleteBitmap(state->btn30);
	deleteBitmap(state->btn90);
	deleteBitmap(state->btn120);
	deleteBitmap(state->backBtn);
	free(state);

}
