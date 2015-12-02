#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <minix/com.h>

#include "i8042.h"

#define MAX_TRIES 3
#define BITORDER 3

unsigned long dataASM;
static int hook = 0;

int kbd_subscribe_int(void )
{
	hook = BITORDER;
	if( sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook) == OK && sys_irqenable(&hook) == OK)
		return BITORDER;
	return -1;
}

int kbd_unsubscribe_int()
{
	if(sys_irqrmpolicy(&hook) == OK && sys_irqdisable(&hook))
		return 0;
	return 1;
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

int kbd_test_scan(unsigned short ass){
	if(ass == 0)
		kbd_test_scan_C();
	else
		kbd_test_scan_assembly();
		return 0;
}

int kbd_test_scan_C(void) {
	int r;
	unsigned long data = 0, irq_set;
	int ipc_status;
	message msg;

	irq_set = kbd_subscribe_int();

	while (data != ESC_CODE){
		if ( r = driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG &BIT(irq_set)) {
					data = kbd_read_buff();
					if(BIT(7) & data)
						printf("Break code: 0x%2X", data);
					else
						printf("Make code: 0x%2X", data);
					printf("\n ----///---- \n");
				}break;
			default: break;
			}
		}
	}
	kbd_unsubscribe_int();
	return 0;
}

int kbd_test_scan_assembly(void) {

}

int sendCMDtoKBD(unsigned long command) {
	int n = 0;
	unsigned long stat;

	for(;n<MAX_TRIES; n++) {
		if( sys_inb(BUF_STAT, &stat) == OK && (stat & ERR_IN_FULL) == 0 && sys_outb(BUF_IN, command) == OK) {
			return 0;
		}
		tickdelay(micros_to_ticks(DELAY));}
	return -1;
}

int timer0_sleep(int n){

	int r;
	int irq_set_timer;
	int ipc_status;
	message msg;

	int time = 60 * n;

	irq_set_timer = timer_subscribe_int();
	while( /*get_counter() < time*/1 ) {
		r = driver_receive(ANY, &msg, &ipc_status);
		if ( driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG & BIT(irq_set_timer)) { /* subscribed interrupt */
					//timer_int_handler();
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
	//reset_counter();

	return 0;
}

int kbd_test_leds(unsigned short n, unsigned short *leds) {
	//proccess an element of toggle once per second
	int r;
	unsigned short controler = 0;
	unsigned short nr = 0;
	unsigned short i = 0;

	for (; i < n; i++){
		nr = leds[i];//LED que se deve alterar
		controler ^= BIT(nr);
		do{
			if (sendCMDtoKBD(LEDS) == 0){
				r = sendCMDtoKBD(controler);}
		}while(r != 0);

		if(timer0_sleep(1) == 0)
			continue;
		else{
			return -1;
		}
	}
	return 0;
}

int kbd_test_timed_scan(unsigned short n){
	int r;
	unsigned long data = 0, irq_set_KBD, irq_set_TIMER;
	int ipc_status;
	message msg;
	unsigned long limit = 60 * n;

	irq_set_KBD = kbd_subscribe_int();
	irq_set_TIMER = timer_subscribe_int();

	while (data != ESC_CODE){
		if(/*get_counter() >= limit*/1){
			printf("Your time ran out!!!\n\n");
			break;
		}
		if ( r = driver_receive(ANY, &msg, &ipc_status) != 0 ) {
			printf("driver_receive failed with: %d", r);
			continue;}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.NOTIFY_ARG &BIT(irq_set_KBD)) {
					data = kbd_read_buff();
					if(BIT(7) & data)
						printf("Break code: 0x%2X", data);
					else
						printf("Make code: 0x%2X", data);
					printf("\n ----///---- \n");
					//reset_counter();
				}
				if (msg.NOTIFY_ARG &BIT(irq_set_TIMER)) {
					//timer_int_handler();
				}break;
			default: break;
			}
		}
	}
	kbd_unsubscribe_int();
	timer_unsubscribe_int();
	return 0;
}

int get_kbd_status()
{
	unsigned long status;
	sys_inb(BUF_STAT, &status);
	return status;

}
