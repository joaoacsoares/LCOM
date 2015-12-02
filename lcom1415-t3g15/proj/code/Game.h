#pragma once

#include "Bitmap.h"

typedef struct {
	int done;
	Bitmap* board;
	Bitmap* digglet;
	Bitmap* hole;
	Bitmap* hit;
	Bitmap* mouse;
	Bitmap* mouseClick;
	int endGame;
	int holes[9];
	int coord[36];
	int points;

} Game;

Game* createGame();
int updateGame(Game* state, int counter, int max);
void drawGame(Game* state);
void deleteGame(Game* state);
int mouseInsideHoleNum();
void updateHoles(Game* state, int num);
void resetHoles(Game* state);
int isFinished(Game* state, int counter, int max);
