#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <minix/com.h>

#include "i8042.h"
#include "timer.h"
#include "timer.c"

#define MAX_TRIES 3
#define BITORDER 12

unsigned short c = 0;//usado para guardar a ordem nos bits em read packets
unsigned char packet[3];
static int hook = 0;

int mouse_subscribe_int(void) {
	hook = BITORDER;
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook)
			== OK && sys_irqenable(&hook) == OK)
		return BITORDER;
	return -1;
}

int mouse_unsubscribe_int() {
	if (sys_irqrmpolicy(&hook) == OK && sys_irqdisable(&hook))
		return 0;
	return 1;
}

void in_buffer_handler() {
	unsigned long in;

	in = get_kbd_status();
	if (in & ERR_IN_FULL && (in &(ERR_PAR | ERR_TOUT)) == 0 ) {

		tickdelay(micros_to_ticks(DELAY));
		in_buffer_handler();
	}
}

int send_command_mouse_aux(unsigned int com){
	unsigned long stat;

	while(1) {
		in_buffer_handler();
		if (sys_outb(BUF_IN, com) == OK) {
			sys_inb(BUF_OUT, &stat);
			if (stat == ACK){
				return 0;
			}
			else if (stat == ERROR)
				return 1;
		}
	}
}

int send_command_mouse(unsigned int com) {
	do{
		in_buffer_handler();
		if(sys_outb(BUF_CMD, WRITE_BYTE_M) != OK) {
			printf("\nError Sending Command\n");
			return -1;
		}
	}
	while((send_command_mouse_aux(com) != 0));
}

int kbd_read_buff()
{
	unsigned long status;
	unsigned long data;

	while(1) {

		status = get_kbd_status();//read KBD status
		//Looping while outBuffer empty
		if( status & ERR_OUT_FULL ) {
			sys_inb(BUF_OUT, &data); /* assuming it returns OK */
			if ( (status &(ERR_PAR | ERR_TOUT)) == 0 )
				return data;
			else
				return -1;
		}
		tickdelay(micros_to_ticks(DELAY));
	}
}

int get_kbd_status()
{
	unsigned long status;
	sys_inb(BUF_STAT, &status);
	return status;

}

void read_packets() {
	unsigned long bit = 0;

	unsigned long merdadocaralhoquetafoda  = 1;
	bit = kbd_read_buff();

	switch (c) {
	case 0:
		if (bit & BIT(3)) {
			packet[0] = bit;
			c++;
		}
		read_packets();
		break;
	case 1:
		packet[1] = bit;
		c++;
		read_packets();
		break;
	case 2:
		packet[2] = bit;
		c = 0;
		break;
	}
}

void print_test_packets() {
	printf(
			"B1= 0x%02x  B2= 0x%02x  B3= 0x%02x  LB= %d  MB= %d  RB= %d  X0V= %d  YOV= %d  X= %d  Y=%d  \n",
			packet[0], packet[1], packet[2], (packet[0] & 1), ((packet[0] >> 2) & 1), ((packet[0] >> 1) & 1),
			((packet[0] >> 6) & 1), ((packet[0] >> 7) & 1), packet[1], packet[2]);
}

int test_packet(unsigned short cnt){
	int r, i = 0;
	unsigned long irq_set_MOUSE;
	int ipc_status;
	message msg;

	send_command_mouse(ENABLE_STREAM_M);

	irq_set_MOUSE = mouse_subscribe_int();

	while (i < cnt) {
		if (r = driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & BIT(irq_set_MOUSE)) {
					read_packets();
					print_test_packets();
					i++;
				}
				break;
			default:
				break;
			}
		}

	}

	mouse_unsubscribe_int();
	return 0;
}

int test_gesture(short length, unsigned short tolerance){

}

int test_config(void){
	unsigned long config;

	while (send_command_mouse(STATUS_M) != 0) {
	};

	if (sys_inb(BUF_OUT, &config) == OK) {
		if (config & BIT(3) & BIT(0)) {
			printf("1:1 - Linear mode\n");
		} else {
			printf("2:1 - Acceleration mode\n");
		}
		if (config & BIT(5)) {
			printf("Info Report: ON\n ");
		} else {
			printf("Info Report: OFF\n ");
		}
		if (config & BIT(6)) {
			printf("Remote: ON\n");
		} else {
			printf("Remote: OFF\n");
		}
		sys_inb(BUF_OUT, &config);
		printf("X Delta: 0x%x\n", config);

		if (sys_inb(BUF_OUT, &config) == OK) {
			printf("Y Delta: 0x%x\n", config);
		}
	}

	return 0;
}

int test_async(unsigned short idle_time){
	int r;
	int irq_set_MOUSE, irq_set_TIMER;
	int ipc_status;
	message msg;
	unsigned long limit = 60 * idle_time;

	send_command_mouse(ENABLE_STREAM_M);

	irq_set_MOUSE = mouse_subscribe_int();
	irq_set_TIMER = timer_subscribe_int();

	reset_counter();

	while (get_counter() < limit) {
		if (r = driver_receive(ANY, &msg, &ipc_status) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & BIT(irq_set_MOUSE)) {
					read_packets();
					print_test_packets();
					reset_counter();
				}
				if (msg.NOTIFY_ARG & BIT(irq_set_TIMER)) {
					timer_int_handler();
				}
				break;
			default:
				break;
			}
		}

	}

	printf("Your time ran out!!!\n\n");

	timer_unsubscribe_int();
	mouse_unsubscribe_int();
	return 0;
}
