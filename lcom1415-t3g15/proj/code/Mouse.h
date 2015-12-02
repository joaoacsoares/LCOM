#pragma once

#include "Bitmap.h"

typedef struct {

	unsigned char packet[3];
	unsigned short count;
	int x, y;
	int dx, dy;

	int draw;
	int update;

	int leftBtnPressed;
	int leftBtnReleased;

	int rightBtnPressed;
	int rightBtnReleased;

	int middleBtnPressed;
	int middleBtnReleased;

	Bitmap* cursor1;
	Bitmap* cursor2;

} Mouse;

int mouse_subscribe_int(void);
int mouse_unsubscribe_int();
Mouse* createMouse();
Mouse* getMouse();
void updateMouse();
void drawMouse();
void deleteMouse();
void print_packets();
unsigned int getindPackets();
void resetMouse();
int test_packet(unsigned short cnt);
int mouseInsideRect(int x, int y, int xf, int yf);
