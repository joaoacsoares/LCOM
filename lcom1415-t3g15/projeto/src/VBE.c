#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "VBE.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
	struct reg86u reg86;
	mmap_t mmap;

	if (lm_init())
		return -1;

	lm_alloc(sizeof(vbe_mode_info_t), &mmap);

	reg86.u.b.intno = 0x10;
	reg86.u.w.ax = 0x4F01;

	reg86.u.w.es = PB2BASE(mmap.phys);
	reg86.u.w.di = PB2OFF(mmap.phys);

	reg86.u.w.cx = 1 << 14 | mode;

	if (sys_int86(&reg86) != OK) {
		printf("\n vbe: sys_int86() failed\n");
		return -1;
	}

	*vmi_p = *(vbe_mode_info_t*) mmap.virtual;

	lm_free(&mmap);

	return 0;
}

int vbe_get_info(vbe_mode_info_t *vmi_p) {
	struct reg86u reg86;
	mmap_t mmap;

	if (lm_init())
		return -1;

	lm_alloc(sizeof(vbe_mode_info_t), &mmap);

	reg86.u.b.intno = 0x10;
	reg86.u.w.ax = 0x4F00;

	if (sys_int86(&reg86) != OK) {
		printf("\n vbe: sys_int86() failed\n");
		return -1;
	}

	*vmi_p = *(vbe_mode_info_t*) mmap.virtual;

	lm_free(&mmap);

	return 0;
}

