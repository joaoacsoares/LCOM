#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include <minix/com.h>

#include "test5.h"
#include "read_xpm.h"

#include "lab3/test3.c"
#include "lab3/test3.h"
//#include "lab3/timer.c"
//#include "lab3/timer.h"


void begin_xpm(unsigned short xi, unsigned short yi, unsigned long x){
	xpm_NUMBER = x;

	char **xpm;

	test_xpm( xi, yi, xpm);

}

void begin_move(unsigned short xi, unsigned short yi, unsigned short x, unsigned short hor, short delta, unsigned short time){
	xpm_NUMBER = x;

	char **xpm;

	test_move( xi, yi, xpm, hor, delta, time);
}

void *test_init(unsigned short mode, unsigned short delay) {
	char *video_mem;

	video_mem = (char *) vg_init(mode);

	vg_init(mode);

	sleep(delay);

	if(!vg_exit()){
		printf("\nVRAM mapped on physical adress: 0x%X\n",video_mem);
		return video_mem;
	}
	return ;
}


int test_square(unsigned short x, unsigned short y, unsigned short size, unsigned long color) {

	vg_init(0x105);

	vg_draw_line(x, y, x+size, y, color);
	vg_draw_line(x, y, x, y+size, color);
	vg_draw_line(x, y+size, x+size, y+size, color);
	vg_draw_line(x+size, y, x+size, y+size, color);

	kbd_wait_esc();

	if(!vg_exit()){
		printf("\nReturning to text mode\n");
		return 0;
	}
	return -1;

}

int test_line(unsigned short xi, unsigned short yi,
		unsigned short xf, unsigned short yf, unsigned long color) {

	vg_init(0x105);

	vg_draw_line(xi, yi, xf, yf, color);

	kbd_wait_esc();

	if(!vg_exit()){
		printf("\nReturning to text mode\n");
		return 0;
	}
	return -1;
}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	vg_init(0x105);
	char *map;

	int c = 0;
	int width, height;
	if(xpm_NUMBER == 1){
		map = read_xpm(pic1, &width, &height);}
	else if (xpm_NUMBER == 2){
		map = read_xpm(pic2, &width, &height);}
	else if (xpm_NUMBER == 3){
		map = read_xpm(cross, &width, &height);}
	else if (xpm_NUMBER == 4){
		map = read_xpm(pic3, &width, &height);}
	else if (xpm_NUMBER == 5){
		map = read_xpm(penguin, &width, &height);}

	int i = 0;
	int j = 0;

	for(; i < height; i++){
		for(; j < width; j++){
			vg_set_pixel(xi+j, yi+i, map[c]);
			c++;
		}
		j=0;
	}

	kbd_wait_esc();

	if(!vg_exit()){
		printf("\nReturning to text mode\n");
		return 0;
	}
	return -1;
}

int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time) {

	vg_init(0x105);
	char *map;
	short incx, incy;

	int c = 0;
	int width, height;
	if(xpm_NUMBER == 1){
		map = read_xpm(pic1, &width, &height);}
	else if (xpm_NUMBER == 2){
		map = read_xpm(pic2, &width, &height);}
	else if (xpm_NUMBER == 3){
		map = read_xpm(cross, &width, &height);}
	else if (xpm_NUMBER == 4){
		map = read_xpm(pic3, &width, &height);}
	else if (xpm_NUMBER == 5){
		map = read_xpm(penguin, &width, &height);}

	int i = 0;
	int j = 0;

	for(; i < height; i++){
		for(; j < width; j++){
			vg_set_pixel(xi+j, yi+i, map[c]);
			c++;
		}
		j=0;
	}

	if(hor != 0 && delta < 0){
		incx = -1;
		incy = 0;
	}
	else if(hor != 0 && delta > 0){
		incx = 1;
		incy = 0;
	}
	else if(hor == 0 && delta > 0){
		incx = 0;
		incy = 1;
	}
	else if(hor == 0 && delta < 0){
		incx = 0;
		incy = -1;
	}

	int step = abs(delta) / time;

	int r;
	unsigned long data = 0, irq_set_KBD, irq_set_TIMER;
	int ipc_status;
	message msg;
	unsigned long limit = 60 * time;

	irq_set_KBD = kbd_subscribe_int();
	irq_set_TIMER = timer_subscribe_int();

	while (data != ESC_CODE){
		if(get_counter() >= limit){
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
				}
				if (msg.NOTIFY_ARG &BIT(irq_set_TIMER)) {
					timer_int_handler();
					if(get_counter()%60 == 0){

						vg_clear();
						int i = 0;
						int j = 0;

						xi = xi + (step + incx);
						yi = yi + (step + incy);

						for(; i < height; i++){
							for(; j < width; j++){
								vg_set_pixel(xi+j, yi+i, map[c]);
								c++;
							}
							j=0;
						}
					}
				}break;
			default: break;
			}
		}
	}
	kbd_unsubscribe_int();
	timer_unsubscribe_int();

	if(!vg_exit()){
			printf("\nReturning to text mode\n");
			return 0;
		}
		return -1;

}

int test_controller() {

	/* To be completed */

}

