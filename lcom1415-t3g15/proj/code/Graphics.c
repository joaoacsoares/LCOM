#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "Graphics.h"
#include "VBE.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

/* Private global variables */

static char *video_mem; /* Process address to which VRAM is mapped */
static char *buffer; /*back buffer*/
static char *mouseBuffer; /*mouse buffer*/

static unsigned h_res; /* Horizontal screen resolution in pixels */
static unsigned v_res; /* Vertical screen resolution in pixels */
static unsigned bytes_per_pixel; /* Number of VRAM bits per pixel */
static unsigned memSize;

void *vg_init(unsigned short mode) {
	struct reg86u r;
	struct mem_range memory;
	vbe_mode_info_t vm;

	r.u.b.intno = 0x10;
	r.u.w.ax = 0x4F02;
	r.u.w.bx = 1 << 14 | mode;

	if (sys_int86(&r) != OK) {
		return NULL;
	}

	if (vbe_get_mode_info(mode, &vm) != 0) {
		return NULL;
	}

	h_res = vm.XResolution;
	v_res = vm.YResolution;

	short pixel_size;
	if ((vm.BitsPerPixel / BITS_PER_PIXEL) > 0)
		pixel_size = vm.BitsPerPixel / BITS_PER_PIXEL;
	else
		pixel_size = 1;

	bytes_per_pixel = pixel_size;

	long memory_size = h_res * v_res * vm.BitsPerPixel;

	memSize = memory_size;

	memory.mr_base = vm.PhysBasePtr;
	memory.mr_limit = memory.mr_base + memory_size;

	if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &memory)) {
		return NULL;
	}

	video_mem = vm_map_phys(SELF, (void*) memory.mr_base, memory_size);

	buffer = (char*) malloc(memSize);
	mouseBuffer = (char*) malloc(memSize);

	return video_mem;
}

unsigned long getMemSize() {
	return memSize;
}

unsigned long rgb(unsigned char r, unsigned char g, unsigned char b) {

	return ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
}

unsigned int getHorRes() {
	return h_res;
}

unsigned int getVerRes() {
	return v_res;
}

char* getVideoMem() {
	return buffer;
}

char* getVideoMemMouse() {
	return mouseBuffer;
}


void saveBuffer(){
	memcpy(buffer, video_mem, memSize);
}

int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color) {
	int a = color / 1000;
	int b = color % 1000;
	*(buffer + x * bytes_per_pixel + y * bytes_per_pixel * h_res) = a;
	*(buffer + x * bytes_per_pixel + y * bytes_per_pixel * h_res + 1) = b;
	return 0;
}

long vg_get_pixel(unsigned long x, unsigned long y) {
	return *(video_mem + x * bytes_per_pixel + y * bytes_per_pixel * h_res) * 1000 +
	*(video_mem + x * bytes_per_pixel + y * bytes_per_pixel * h_res + 1);
}

int vg_draw_line(unsigned long xi, unsigned long yi, unsigned long xf,
		unsigned long yf, unsigned long color) {
	long dx, dy, sx, sy, err, e2;
	dx = abs(xf - xi);
	dy = abs(yf - yi);
	if (xi < xf)
		sx = 1;
	else
		sx = -1;
	if (yi < yf)
		sy = 1;
	else
		sy = -1;
	err = dx - dy;

	while (1) {
		vg_set_pixel(xi, yi, color);
		if (xi == xf && yi == yf)
			return 0;

		e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			xi += sx;
		}

		if (xi == xf && yi == yf) {
			vg_set_pixel(xi, yi, color);
			return 0;
		}
		if (e2 < dx) {
			err += dx;
			yi += sy;
		}
	}
	return 0;
}

int drawSquare(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	int a = color / 1000;
	int b = color % 1000;
	int i, j;
	for (i = x; i < x + size; i++) {
		for(j = y; j < y + size; j++) {
			*(mouseBuffer + i * bytes_per_pixel + j * bytes_per_pixel * h_res) = a;
			*(mouseBuffer + i * bytes_per_pixel + j * bytes_per_pixel * h_res + 1) = b;
		}
	}
	return 0;
}

int vg_fill(unsigned long color) {
	int i = 0;
	int j = 0;
	for (; i < h_res; i++) {
		for (; j < v_res; j++) {
			vg_set_pixel(i, j, color);
		}
		j = 0;
	}
	return 0;
}

void flipMouse() {
	memcpy(mouseBuffer, buffer, memSize);
}

void flipDisplay() {
	memcpy(video_mem, mouseBuffer, memSize);
}


int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10; /* BIOS video services */

	reg86.u.w.ax = 0x0003; /* Set Video Mode function */

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

void vg_get_info() {

}

/*#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "Graphics.h"
#include "VBE.h"

#define VRAM_PHYS_ADDR	0xF0000000
#define H_RES             1024
#define V_RES		  768
#define BITS_PER_PIXEL	  8

static char *video_mem;
static char *buffer;
static char *mouseBuffer;

static unsigned h_res;
static unsigned v_res;
static unsigned bytes_per_pixel;
static unsigned memory_size;

void *vg_init(unsigned short mode) {
	struct reg86u r;
	struct mem_range memory;
	vbe_mode_info_t vm;

	r.u.b.intno = 0x10;
	r.u.w.ax = 0x4F02;
	r.u.w.bx = 1 << 14 | mode;

	if (sys_int86(&r) != OK) {
		return NULL;
	}

	if (vbe_get_mode_info(mode, &vm) != 0) {
		return NULL;
	}

	h_res = vm.XResolution;
	v_res = vm.YResolution;

	short pixel_size;
	if ((vm.BitsPerPixel / BITS_PER_PIXEL) > 0)
		pixel_size = vm.BitsPerPixel / BITS_PER_PIXEL;
	else
		pixel_size = 1;

	bytes_per_pixel = pixel_size;

	long memory_size = h_res * v_res * vm.BitsPerPixel;

	memory_size = memory_size;

	memory.mr_base = vm.PhysBasePtr;
	memory.mr_limit = memory.mr_base + memory_size;

	if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &memory)) {
		return NULL;
	}

	video_mem = vm_map_phys(SELF, (void*) memory.mr_base, memory_size);

	buffer = (char*) malloc(memory_size);
	mouseBuffer = (char*) malloc(memory_size);

	return video_mem;
}

unsigned long getmemory_size() {
	return memory_size;
}

unsigned long rgb(unsigned char r, unsigned char g, unsigned char b) {

	return ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
}

unsigned int getHorRes() {
	return h_res;
}

unsigned int getVerRes() {
	return v_res;
}

char* getVideoMem() {
	return video_mem;
}

int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color) {
	int a = color / 1000;
	int b = color % 1000;
	*(buffer + x * bytes_per_pixel + y * bytes_per_pixel * h_res) = a;
	*(buffer + x * bytes_per_pixel + y * bytes_per_pixel * h_res + 1) = b;
	return 0;
}

long vg_get_pixel(unsigned long x, unsigned long y) {
	return *(video_mem + x * bytes_per_pixel + y * bytes_per_pixel * h_res) * 1000 +
	*(video_mem + x * bytes_per_pixel + y * bytes_per_pixel * h_res + 1);
}

int vg_draw_line(unsigned long xi, unsigned long yi, unsigned long xf,
		unsigned long yf, unsigned long color) {
	long dx, dy, sx, sy, err, e2;
	dx = abs(xf - xi);
	dy = abs(yf - yi);
	if (xi < xf)
		sx = 1;
	else
		sx = -1;
	if (yi < yf)
		sy = 1;
	else
		sy = -1;
	err = dx - dy;

	while (1) {
		vg_set_pixel(xi, yi, color);
		if (xi == xf && yi == yf)
			return 0;

		e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			xi += sx;
		}

		if (xi == xf && yi == yf) {
			vg_set_pixel(xi, yi, color);
			return 0;
		}
		if (e2 < dx) {
			err += dx;
			yi += sy;
		}
	}
	return 0;
}

int drawSquare(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	int a = color / 1000;
	int b = color % 1000;
	int i, j;
	for (i = x; i < x + size; i++) {
		for(j = y; j < y + size; j++) {
			*(mouseBuffer + i * bytes_per_pixel + j * bytes_per_pixel * h_res) = a;
			*(mouseBuffer + i * bytes_per_pixel + j * bytes_per_pixel * h_res + 1) = b;
		}
	}
	return 0;
}

int vg_fill(unsigned long color) {
	int i = 0;
	int j = 0;
	for (; i < h_res; i++) {
		for (; j < v_res; j++) {
			vg_set_pixel(i, j, color);
		}
		j = 0;
	}
	return 0;
}

void flipMouse() {
	memcpy(mouseBuffer, buffer, memory_size);
}

void flipDisplay() {
	memcpy(video_mem, mouseBuffer, memory_size);
}

int vg_exit() {
	struct reg86u reg86;

	reg86.u.b.intno = 0x10;

	reg86.u.w.ax = 0x0003;

	if (sys_int86(&reg86) != OK) {
		printf("\tvg_exit(): sys_int86() failed \n");
		return 1;
	} else
		return 0;
}

void vg_get_info() {

}*/
