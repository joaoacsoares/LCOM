#include "Game.h"
#include "Graphics.h"
#include "Mouse.h"
#include "timer.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "Numbers.h"

Game* createGame() {
	Game* g = (Game*) malloc(sizeof(Game));

	g->done = 0;
	g->endGame = 0;
	g->board = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/board.bmp");
	g->digglet = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/digglet.bmp");
	g->hole = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/hole.bmp");
	g->hit = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/hit.bmp");
	g->points = 0;

	int i;
	for (i = 0; i < 9; i++) {
		g->holes[i] = 0;
	}

	srand(time(NULL));
		int r = rand() % 9;
		int s = rand() % 9;

		while(s == r)
			s = rand() % 9;

		g->holes[r] = 1;
		g->holes[s] = 1;

	g->coord[0] = 100;
	g->coord[1] = 120;
	g->coord[2] = 300;
	g->coord[3] = 280;

	g->coord[4] = 300;
	g->coord[5] = 120;
	g->coord[6] = 500;
	g->coord[7] = 280;

	g->coord[8] = 500;
	g->coord[9] = 120;
	g->coord[10] = 700;
	g->coord[11] = 280;
	//
	g->coord[12] = 100;
	g->coord[13] = 280;
	g->coord[14] = 300;
	g->coord[15] = 440;

	g->coord[16] = 300;
	g->coord[17] = 280;
	g->coord[18] = 500;
	g->coord[19] = 440;

	g->coord[20] = 500;
	g->coord[21] = 280;
	g->coord[22] = 700;
	g->coord[23] = 440;
	//
	g->coord[24] = 100;
	g->coord[25] = 440;
	g->coord[26] = 300;
	g->coord[27] = 600;

	g->coord[28] = 300;
	g->coord[29] = 440;
	g->coord[30] = 500;
	g->coord[31] = 600;

	g->coord[32] = 500;
	g->coord[33] = 440;
	g->coord[34] = 700;
	g->coord[35] = 600;

	return g;
}

int updateGame(Game* state, int counter, int max) {

	if (isFinished(state, counter, max))
		state->done = 1;

	if (getMouse()->leftBtnReleased
			&& (!(getMouse()->x < 100 || getMouse()->y < 120)
					|| getMouse()->x > 700
					|| getMouse()->y > 600)) {
		int i = 0, c;
		for(c = 0; c < 9; c++){
			if (getMouse()->leftBtnReleased
					&& state->holes[c] == 1 && mouseInsideRect(state->coord[i],state->coord[i+1],state->coord[i+2],state->coord[i+3])) {
				state->holes[c] = 2;
			}
			i = i + 4;
		}
	}
}

int isFinished(Game* state, int counter, int max) {
	if(counter >= 60 * max){
		state->endGame = 1;
		return 1;
	}
	else
		return 0;
}

void deleteGame(Game* state) {
	deleteBitmap(state->board);
	deleteBitmap(state->digglet);
	deleteBitmap(state->hole);
	deleteBitmap(state->hit);
	free(state);
}

void resetHoles(Game* state){

	int i, c;
	c = 0;
	for(i = 0; i < 9; i++){
		if(state->holes[i] == 2){
			c++;
		}
		state->holes[i] = 0;
	}

	state->points = state->points + c;

	srand(time(NULL));
	int r = rand() % 9;
	int s = rand() % 9;

	while(s == r)
		s = rand() % 9;

	state->holes[r] = 1;
	state->holes[s] = 1;

}

void drawGame(Game* state) {

	drawBitmap(state->board, 0, 0, ALIGN_LEFT);

	drawNumber(state->points, 760, 450);

	int i = 0, c;
	for(c = 0; c < 9 ;c++){
		switch(state->holes[c]){
		case 0:
			drawBitmapWithoutBg(state->hole, state->coord[i], state->coord[i+1], ALIGN_LEFT);
			break;
		case 1:
			drawBitmapWithoutBg(state->digglet, state->coord[i], state->coord[i+1], ALIGN_LEFT);
			break;
		case 2:
			drawBitmapWithoutBg(state->hit, state->coord[i], state->coord[i+1], ALIGN_LEFT);
			break;
		default:
			break;
		}
		i = i + 4;

	}


}
