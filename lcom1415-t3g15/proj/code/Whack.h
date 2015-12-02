#pragma once
#include "timer.h"
#include "Bitmap.h"

#define BIT(n) (0x01<<(n))

typedef enum {
	MAINMENU, GAME, SETTINGS,
} State;

typedef struct {

	int irq_set_mouse;
	int irq_set_kbd;
	int irq_set_timer;
	unsigned long scancode;
	int done;
	int draw;
	Timer_t* timer;
	State currentState;
	void* state;
	int dif;
	int time;

}Whack;

Whack* startWhack();
void updateWhack(Whack* Whack);
void drawWhack(Whack* Whack);
void deleteWhack(Whack* Whack);
void changeState(Whack* Whack, State state);
void stopWhack(Whack* Whack);
void delCurrentState(Whack* Whack);
void isDone(Whack* Whack);
