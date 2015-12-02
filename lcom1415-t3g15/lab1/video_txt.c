#include <minix/drivers.h>
#include <sys/video.h>
#include <sys/mman.h>

#include <assert.h>

#include "vt_info.h"

#include "video_txt.h"

/* Private global variables */

static char *video_mem;		/* Address to which VRAM is mapped */

static unsigned scr_width;	/* Width of screen in columns */
static unsigned scr_lines;	/* Height of screen in lines */

void vt_fill(char ch, char attr) {

	char *ptr;
	ptr = video_mem;

	unsigned int i;
	for(i = 0; i< scr_width * scr_lines; i++, ptr++)
	{
		*ptr = ch;
		ptr++;
		*ptr = attr;
	}

}

void vt_blank() {

	//0x20 caracter espaco
	vt_fill(0x20,0x00);

}

int vt_print_char(char ch, char attr, int r, int c) {

	//2 bytes por cell * 80 cells por linha
	char *ptr = video_mem + 2*c + r*80*2;

	*ptr = ch;
	ptr++;
	*ptr = attr;

	return 0;

}

int vt_print_string(char *str, char attr, int r, int c) {

	char *ptr = video_mem + 2*c + r*80*2;

	unsigned int i;
	for (i = 0; str[i] != 0; ++i)
	{
		*ptr = str[i];
		ptr++;
		*ptr = attr;
		ptr++;
	}

}

int tamanho(int num)
{
	int i = 0;
	if(num < 0)
		num = -1 * num;
	while (num > 0)
	{
		++i;
		num /= 10;
	}
	return i;
}

int vt_print_int(int num, char attr, int r, int c) {

	int i = 0, num2;
	int max=tamanho(num);
	if(num < 0){
		max++;
		num = num * -1;
		vt_print_char('-', attr, r, c );
	}

	max--;
	c = c +max;

	for (i = 0; i<=max; i++)
	{
		vt_print_char(0x30+(num%10), attr, r, c );
		num = num/10;
		c--;
	}

}


int vt_draw_frame(int width, int height, char attr, int r, int c) {

	char *ptr = video_mem + 2*c + 160*r;
	int i;

	vt_print_char(0xDA, attr, r, c );
	vt_print_char(0xBF, attr, r, c + width - 1);
	vt_print_char(0xD9, attr, r + height -1, c + width - 1);
	vt_print_char(0xC0, attr, r + height -1, c );

	int countC;
	for(countC = c + 1; countC <= (c + width - 2); countC++){
		vt_print_char(0xC4, attr, r, countC );
		vt_print_char(0xC4, attr, r + height -1, countC );
	}

	int countR;
	for(countR = r + 1; countR <= (r + height - 2); countR++){
		vt_print_char(0xB3, attr, countR, c );
		vt_print_char(0xB3, attr, countR, c + width - 1 );
	}

	return 0;
}

/*
 * THIS FUNCTION IS FINALIZED, do NOT touch it
 */

char *vt_init(vt_info_t *vi_p) {

	int r;
	struct mem_range mr;

	/* Allow memory mapping */

	mr.mr_base = (phys_bytes)(vi_p->vram_base);
	mr.mr_limit = mr.mr_base + vi_p->vram_size;

	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("video_txt: sys_privctl (ADD_MEM) failed: %d\n", r);

	/* Map memory */

	video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vi_p->vram_size);

	if(video_mem == MAP_FAILED)
		panic("video_txt couldn't map video memory");

	/* Save text mode resolution */

	scr_lines = vi_p->scr_lines;
	scr_width = vi_p->scr_width;

	return video_mem;
}
