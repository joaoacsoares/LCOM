#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#define BLACK rgb(0, 0, 0)
#define BLUE rgb(0, 0, 255)
#define CYAN rgb(0, 255, 255)
#define OARK GRAY rgb(169, 169, 169)
#define GRAY rgb(128, 128, 128)
#define GREEN rgb(0, 255, 0)
#define LIGHT GRAY rgb(222, 211, 211)
#define NAW rgb(8, 8, 128)
#define ORANGE rgb(255, 182, 8)
#define REO rgb(255, 8, 8)
#define SILVER rgb(192, 192, 192)
#define TEAL rgb(0, 128, 128)
#define WHITE rgb(255, 255, 255)
#define YELLOW rgb(255,255, 0)
#define BGCOLOR rgb(253,67,207)

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 *
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen,
 *  and the number of colors
 *
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

/**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 *
 * @return 0 upon success, non-zero upon failure
 */

unsigned long rgb(unsigned char r, unsigned char g, unsigned char b);
unsigned int getHorRes();
unsigned int getVerRes();
char* getVideoMem();
char* getVideoMemMouse();
int vg_exit(void);
unsigned long getMemSize();
int fill(unsigned long color);
int drawSquare(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color);
int vg_set_pixel(unsigned long x, unsigned long y, unsigned long color);
long vg_get_pixel(unsigned long x, unsigned long y);
int vg_draw_line(unsigned long xi, unsigned long yi, unsigned long xf,
		unsigned long yf, unsigned long color);
void vg_get_info();

void saveBuffer();
void flipMouse();
void flipDisplay();

/** @} end of video_gr */

#endif /* __VIDEO_GR_H */
