#pragma once

#include "Bitmap.h"

typedef struct {
	Bitmap* numbs[9];
	/*Bitmap* um;
	Bitmap* dois;
	Bitmap* tres;
	Bitmap* quatro;
	Bitmap* cinco;
	Bitmap* seis;
	Bitmap* sete;
	Bitmap* oito;
	Bitmap* nove;*/
} Numbers;

Numbers* createNumbers();
void drawNumber(int n, int x, int y);
void deleteNumbers();
