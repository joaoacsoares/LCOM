#include <minix/syslib.h>
#include <minix/drivers.h>

#include <minix/com.h>
#include "i8254.h"
#include "timer.h"

#define BITORDER 3

static int hookid = 0;
int counter = 0;

int timer_set_square(unsigned long timer, unsigned long freq) {
	if (timer > 2 || timer < 0 || freq <= 0)
		return 1;

	unsigned short res;
	unsigned char timer_sig=0, timer_code, lsb_timer, msb_timer;

	res = TIMER_FREQ / freq;
	lsb_timer = (char) res;
	msb_timer = (char) (res >> 8);

	if(timer == 0){
		timer_sig |= TIMER_SEL0;
		timer_code = TIMER_0;
	}
	else if(timer == 1){
		timer_sig |= TIMER_SEL1;
		timer_code = TIMER_1;
	}
	else if(timer == 2){
		timer_sig |= TIMER_SEL2;
		timer_code = TIMER_2;
	}
	else
		return 1;

	timer_sig |= TIMER_LSB_MSB | TIMER_SQR_WAVE | TIMER_BIN;

	if (sys_outb(TIMER_CTRL, timer_sig) != OK || sys_outb(timer_code, lsb_timer) != OK || sys_outb(timer_code, msb_timer) != OK)
		return -1;
	else
		return 0;
}

int timer_subscribe_int(void ) {
	hookid = BITORDER;
	if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE,&hookid) != OK ||
			sys_irqenable(&hookid) != OK ) {

		return -1;
	}
	return BITORDER;
}

int timer_unsubscribe_int() {
	if(sys_irqrmpolicy(&hookid) == OK && sys_irqdisable(&hookid) == OK)
	{
		return 0;
	}
	return 1;
}

void timer_int_handler() {
	counter++;
}

int timer_get_conf(unsigned long timer, unsigned char *st) {
	unsigned char ctrl = 0;

	if(timer < 0 || timer > 2)
		return 1;//invalid arguments

	ctrl = ctrl | TIMER_RB_SEL(timer) | TIMER_RB_CMD;

	if(sys_outb(TIMER_CTRL, ctrl) != OK)
		return 1;
	unsigned long temp;
	if(sys_inb(TIMER_0 + timer, &temp) != OK)
		return 1;

	else{
		*st =(unsigned char) temp;
		return 0;
	}
}

int timer_display_conf(unsigned char conf) {

	unsigned char stat;
	if(timer_get_conf(conf, &stat)==0){
		printf("\nTimer %d Status: ", conf);
		if (stat & TIMER_LSB_MSB)
			printf("\nType of acess: TIMER_LSB_MSB");
		else if (stat & TIMER_LSB)
			printf("\nType of acess: TIMER_LSB");
		else if (stat & TIMER_MSB)
			printf("\nType of acess: TIMER_MSB");
		if (stat & TIMER_SQR_WAVE)
			printf("\nProgrammed mode: TIMER_SQR_WAVE");
		else if (stat & TIMER_RATE_GEN)
			printf("\nProgrammed mode: TIMER_RATE_GEN");
		if(stat & TIMER_BCD)
			printf("\nCounting Mode: BCD",1);
		else
			printf("\nCounting Mode: Binary",0);
		if(stat & BIT(6))
			printf("\nNull Count: %d",1);
		else
			printf("\nNull Count: %d",0);
		if(stat & BIT(7))
			printf("\nOut Pin: %d",1);
		else
			printf("\nOut Pin: %d \n",0);
		return 0;}
	else{
		printf("\nError reading config");
		return 1;
	}
}

int timer_test_square(unsigned long freq) {
	if (timer_set_square(0,freq) == 0)
		return 0;
	return 1;
}

int timer_test_int(unsigned long time) {

	unsigned int r = 0;
	int ipc_status, irq_set;
	message msg;
	unsigned long limit = 60 * time;

	irq_set = timer_subscribe_int();
	while( counter < limit ) {
		r = driver_receive(ANY, &msg, &ipc_status);
		if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & BIT(irq_set)) { /* subscribed interrupt */
					timer_int_handler();
					if(counter%60 == 0)
						printf("\nInterrupt number: %d \n", counter/60);

				}
				break;
			default:
				break; /* no other notifications expected: do nothing */
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
		}
	}

	timer_unsubscribe_int();
	return 1;
}

int timer_test_config(unsigned long timer) {
	if(timer_display_conf(timer) == 0)
		return 0;
	return 1;
}
