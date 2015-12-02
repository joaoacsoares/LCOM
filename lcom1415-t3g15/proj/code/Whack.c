#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include <stdio.h>
#include "i8042.h"
#include "Whack.h"
#include "Graphics.h"
#include "Mouse.h"
#include "Game.h"
#include "MainMenu.h"
#include "keyboard.h"
#include "Bitmap.h"
#include "OptionsMenu.h"

void isDone(Whack* whack) {
	switch (whack->currentState) {
	case GAME:
		if (((Game*) (whack->state))->done) {
			resetMouse();

			reset_counter(whack->timer);
			changeState(whack, MAINMENU);
		}
		break;
	case MAINMENU:
		if (((MainMenu*) (whack->state))->choice == 1) {
			resetMouse();
			reset_counter(whack->timer);
			changeState(whack, GAME);
		}
		else if (((MainMenu*) (whack->state))->choice == 2) {
			resetMouse();
			changeState(whack, SETTINGS);
		}
		else if (((MainMenu*) (whack->state))->choice == 3) {
			whack->done = 1;
		}
		break;
	case SETTINGS:
		if (((OptionsMenu*) (whack->state))->choiceExit) {
			resetMouse();
			whack->dif = whack->dif * ((OptionsMenu*) (whack->state))->difficulty;
			whack->time = ((OptionsMenu*) (whack->state))->gameTime;
			changeState(whack, MAINMENU);
		}
		break;
	default:
		break;
	}
}

void delCurrentState(Whack* whack) {
	switch (whack->currentState) {
	case MAINMENU:
		deleteMainMenu(whack->state);
		break;
	case GAME:
		deleteGame(whack->state);
		break;
	case SETTINGS:
		deleteOptionsMenu(whack->state);
		break;
	default:
		break;
	}
}

Whack* startWhack() {
	Whack* whack = (Whack*) malloc(sizeof(Whack));

	whack->irq_set_mouse = mouse_subscribe_int();
	whack->irq_set_timer = timer_subscribe_int();
	whack->irq_set_kbd = kbd_subscribe_int();

	whack->scancode = 0;
	whack->done = 0;
	whack->draw = 1;
	whack->dif = 120;

	whack->time=30;
	whack->currentState = MAINMENU;

	whack->state = (MainMenu*) createMainMenu();

	whack->timer = newTimer();

	return whack;
}

void updateWhack(Whack* whack) {

	int ipc_status, r = 0;
	message msg;

	if(get_counter(whack->timer) >= (15*60) && whack->currentState == MAINMENU){
		whack->scancode = ESC_CODE;
	}

	if (driver_receive(ANY, &msg, &ipc_status) != 0)
		return;

	if (is_ipc_notify(ipc_status)) {
		switch (_ENDPOINT_P(msg.m_source)) {
		case HARDWARE:

			if (msg.NOTIFY_ARG & BIT(whack->irq_set_timer)) {
				timer_int_handler(whack->timer);

				if (get_counter(whack->timer) % 3 == 0){
					switch (whack->currentState) {
					case MAINMENU:
						updateMainMenu(whack->state);
						break;
					case GAME:
						updateGame(whack->state, whack->timer->counter, whack->time);
						break;
					case SETTINGS:
						updateOptionsMenu(whack->state);
						break;
					default:
						break;
					}
				}
			}
			if (msg.NOTIFY_ARG & BIT(whack->irq_set_mouse))
				updateMouse();
			if (msg.NOTIFY_ARG & BIT(whack->irq_set_kbd))
				whack->scancode = kbd_read_buff();
			else
				whack->scancode = 0;
			break;
		default:
			break;
		}
	}


	if (get_counter(whack->timer) % (whack->dif) == 0 && whack->currentState == GAME){
		resetHoles(whack->state);
	}

	if(whack->scancode == ESC_CODE && whack->currentState == MAINMENU)
		whack->done = 1;
	if(whack->scancode == ESC_CODE && whack->currentState == GAME)
		((Game*) (whack->state))->done = 1;
	if(whack->scancode == ESC_CODE && whack->currentState == SETTINGS)
		((OptionsMenu*) (whack->state))->done = 1;

	isDone(whack);

}

void drawWhack(Whack* whack) {
	switch (whack->currentState) {
	case MAINMENU:
		drawMainMenu(whack->state);
		break;
	case GAME:
		drawGame(whack->state);
		break;
	case SETTINGS:
		drawOptionsMenu(whack->state);
		break;
	default:
		break;
	}
}

void changeState(Whack* whack, State state) {

	delCurrentState(whack);

	whack->currentState = state;

	switch (whack->currentState) {
	case MAINMENU:
		whack->state = (MainMenu*) createMainMenu();
		break;
	case GAME:
		whack->state = (Game*) createGame();
		break;
	case SETTINGS:
		whack->state = (OptionsMenu*) createOptionsMenu();
		break;
	}

	whack->draw = 1;
}

void stopWhack(Whack* whack) {
	delCurrentState(whack);
	deleteMouse();
	mouse_unsubscribe_int();
	timer_unsubscribe_int();
	kbd_unsubscribe_int();
	free(whack);
}
