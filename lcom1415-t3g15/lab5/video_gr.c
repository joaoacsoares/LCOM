#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"

/* Constants for VBE 0x105 mode */

#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.b.ah = 0x00;    /* Set Video Mode function */
	reg86.u.b.al = 0x03;    /* 80x25 text mode*/

	if( sys_int86(&reg86) != OK ) {
		printf("\ntvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

void *vg_init(unsigned short mode) {
	struct reg86u r;
	struct mem_range memory;
	vbe_mode_info_t vm;

	h_res = H_RES;
	v_res = V_RES;
	bits_per_pixel = BITS_PER_PIXEL;

	r.u.b.intno = 0x10;
	r.u.w.ax = 0x4F02;
	r.u.w.bx = 1<<14|0x105;

	if( sys_int86(&r) != OK ) {
		printf("\nset_vbe_mode: sys_int86() failed \n");
		return ;
	}

	if (vbe_get_mode_info(mode, &vm) != 0) {
		printf("\nvg_init(): vbe_get_mode_info() failed");
		return NULL;
	}

	h_res = vm.XResolution;
	v_res = vm.YResolution;
	bits_per_pixel = vm.BitsPerPixel;

	short size_pixel;
	if((bits_per_pixel / BITS_PER_PIXEL) > 0)
		size_pixel = bits_per_pixel / BITS_PER_PIXEL;
	else
		size_pixel = 1;

	long memory_size = h_res*v_res*size_pixel;

	memory.mr_base = vm.PhysBasePtr;
	memory.mr_limit = memory.mr_base + memory_size;

	if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &memory)){
		printf("\nvg_init(): sys_privctl() failed");
		return NULL;
	}

	video_mem = vm_map_phys(SELF, (void*)memory.mr_base, memory_size);

	return video_mem;
}

int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color)
{
	*(video_mem + x + y*h_res) = color;
	return 0;
}

int vg_clear(){
	int i = 0;
	int j = 0;
	for(;i < h_res; i++){
		for(;j < v_res; j++){
		vg_set_pixel(i, j, 0);
		}
		j = 0;
	}
	return 0;
}

int vg_draw_line(unsigned long xi, unsigned long yi,
		unsigned long xf, unsigned long yf, unsigned long color)
{
	//Bresenham's line algorithm -> (http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)
	long dx, dy, sx, sy, err, e2;
	dx = abs(xf-xi);
	dy = abs(yf-yi);
	if (xi < xf) sx = 1; 		//sx & sy sao incrementadores
	else sx = -1;
	if (yi < yf) sy = 1;
	else sy = -1;
	err = dx-dy;			//compensacao  (em vez do uso do slope)

	while(1)
	{
		vg_set_pixel(xi,yi, color);
		if (xi == xf && yi == yf) return 0;

		e2 = 2*err;
		if (e2 > -dy)
		{
			err -= dy;
			xi += sx;
		}

		if (xi == xf && yi == yf)
		{
			vg_set_pixel(xi,yi, color);
			return 0;
		}
		if (e2 < dx)
		{
			err += dx;
			yi += sy;
		}
	}
	return 0;
}

