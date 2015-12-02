#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <minix/com.h>
#include <stdio.h>
#include "Mouse.h"
#include "i8042.h"
#include "i8254.h"
#include "Graphics.h"
#include "Utilities.h"

static int HOOK_ID;
unsigned int indPackets = 0;
Mouse* mouse = NULL;

int mouse_subscribe_int(void) {
	HOOK_ID = HOOK_MOUSE;
	if (sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE, &HOOK_ID)
			== OK && sys_irqenable(&HOOK_ID) == OK)
		return HOOK_MOUSE;
	return -1;
}

int mouse_unsubscribe_int() {
	if (sys_irqrmpolicy(&HOOK_ID) == OK && sys_irqdisable(&HOOK_ID))
		return 0;
	return 1;
}

unsigned int getindPackets() {
	return indPackets;
}

int send_command_mouse(unsigned int command) {
	unsigned long status;
	unsigned long state;
	while (1) {
		sys_inb(BUF_STAT, &status);
		if (status & ERR_IN_FULL) {
			tickdelay(micros_to_ticks(DELAY));
			continue;
		}
		sys_outb(BUF_STAT, WRITE_BYTE_MOUSE);

		while (1) {
			if (sys_outb(BUF_IN, command) == OK) {
				sys_inb(BUF_OUT, &state);
				if (state == ACK) {
					return 0;
				} else if (state == ERROR) {
					printf("Error, NACK sent\n");
				}
			}
		}
	}
}

Mouse* createMouse() {
	Mouse* mouse = (Mouse*) malloc(sizeof(Mouse));

	mouse->x = 200;
	mouse->y = 300;
	mouse->dx = 0;
	mouse->dy = 0;

	mouse->draw = 1;
	mouse->update = 0;

	mouse->leftBtnPressed = 0;
	mouse->leftBtnReleased = 0;

	mouse->rightBtnPressed = 0;
	mouse->rightBtnReleased = 0;

	mouse->middleBtnPressed = 0;
	mouse->middleBtnReleased = 0;

	mouse->packet[0] = 0;
	mouse->packet[1] = 0;
	mouse->packet[2] = 0;

	mouse->cursor1 = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/mallet-up.bmp");
	mouse->cursor2 = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/mallet-down.bmp");

	return mouse;
}

void resetMouse() {

	mouse->x = 300;
	mouse->y = 300;
	mouse->dx = 0;
	mouse->dy = 0;

	mouse->draw = 1;
	mouse->update = 0;

	mouse->leftBtnPressed = 0;
	mouse->leftBtnReleased = 0;

	mouse->rightBtnPressed = 0;
	mouse->rightBtnReleased = 0;

	mouse->middleBtnPressed = 0;
	mouse->middleBtnReleased = 0;

	mouse->packet[0] = 0;
	mouse->packet[1] = 0;
	mouse->packet[2] = 0;
}

Mouse* getMouse() {
	if (!mouse) {
		send_command_mouse(ENABLE_SEND_DATA);
		mouse = createMouse();
	}
	return mouse;
}

void drawMouse() {
	//drawSquare(getMouse()->x, getMouse()->y, 15, BLACK);
	if(mouse->leftBtnPressed){
		drawBitmapMouse(getMouse()->cursor2, getMouse()->x, getMouse()->y, ALIGN_LEFT);
	}
	else{
		drawBitmapMouse(getMouse()->cursor1, getMouse()->x, getMouse()->y, ALIGN_LEFT);
	}
	getMouse()->draw = 0;
}

void updateMouse() {
	unsigned long data;
	if (sys_inb(BUF_OUT, &data) != OK) {
		printf("Error filling packets");
		return;
	}

	if (indPackets == 0 && !(data & BIT(3)))
		return;
	getMouse()->packet[indPackets++] = data;

	if (indPackets > 2) {
		indPackets = 0;

		getMouse()->dx = getMouse()->packet[1];
		getMouse()->dy = getMouse()->packet[2];

		if (getMouse()->packet[0] & X_SIGN) {
			getMouse()->dx |= (-1 << 8);
		}
		if (getMouse()->packet[0] & Y_SIGN) {
			getMouse()->dy |= (-1 << 8);
		}

		if (!(getMouse()->packet[0] & BIT(6))
				&& !(getMouse()->packet[0] & BIT(7))) {
			if (abs(getMouse()->dx) < 255 || abs(getMouse()->dy) < 255) {
				getMouse()->x += getMouse()->dx;
				getMouse()->y -= getMouse()->dy;
			}
		} else
			return;

		//left btn
		if (!(getMouse()->packet[0] & LEFT_BTN)) {
			getMouse()->leftBtnReleased = 0;
		}

		if (getMouse()->leftBtnPressed && !(getMouse()->packet[0] & LEFT_BTN)) {
			getMouse()->leftBtnReleased = 1;
			getMouse()->leftBtnPressed = 0;
			//mouse->cursor = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/mallet-up.bmp");
		}
		if (getMouse()->packet[0] & LEFT_BTN) {
			getMouse()->leftBtnReleased = 0;
			getMouse()->leftBtnPressed = 1;
			//mouse->cursor = loadBitmap("home/lcom/lcom1415-t3g15/projeto/res/mallet-Down.bmp");
		}
		//right btn
		if (!(getMouse()->packet[0] & RIGHT_BTN)) {
			getMouse()->rightBtnReleased = 0;
		}

		if (getMouse()->rightBtnPressed && !(getMouse()->packet[0] & RIGHT_BTN)) {
			getMouse()->rightBtnReleased = 1;
			getMouse()->rightBtnPressed = 0;
		}
		if (getMouse()->packet[0] & RIGHT_BTN) {
			getMouse()->rightBtnReleased = 0;
			getMouse()->rightBtnPressed = 1;
		}

		getMouse()->update = 0;
		getMouse()->draw = 1;

		if (getMouse()->x < 0)
			getMouse()->x = 0;
		if (getMouse()->y < 0)
			getMouse()->y = 0;
		if (getMouse()->x > getHorRes() - 15)
			getMouse()->x = getHorRes() - 15;
		if (getMouse()->y > getVerRes() - 15)
			getMouse()->y = getVerRes() - 15;

	}

}

int test_packet(unsigned short cnt) {
	int r, i = 0;
	unsigned long irq_set;
	int ipc_status;
	message msg;
	irq_set = mouse_subscribe_int();

	send_command_mouse(ENABLE_SEND_DATA);

	while (!getMouse()->leftBtnReleased) {
		if (r = driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & BIT(irq_set)) {
					updateMouse();
				}
				break;
			default:
				break;
			}
		}

	}
	send_command_mouse(MOUSE_DISABLE);
	mouse_unsubscribe_int();
}

void deleteMouse() {
	//deleteBitmap(getMouse()->cursor);
	send_command_mouse(MOUSE_DISABLE);
	if (mouse)
		free(getMouse());
}

int mouseInsideRect(int x, int y, int xf, int yf) {
	return (getMouse()->x > x && getMouse()->y > y && getMouse()->x < xf
			&& getMouse()->y < yf);
}
