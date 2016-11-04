#include "smh_common.h"

unsigned int colors[] = {
	0x00000000,		// 0 -> transparent
	0xFF0000CD,		// 1 -> mediumblue
	0xFFFFFAF0,		// 2 -> floralwhite
	0xFF228B22,		// 3 -> forestgreen
	0xFFFF8C00,		// 4 -> darkorange
	0xFFADFF2F,		// 5 -> greenyellow
	0xFFBA55D3,		// 6 -> mediumorchid
	0xFFA0522D,		// 7 -> sienna
	0xFFDC143C,		// 8 -> crimson
	0xFF808000		// 9 -> olive
};

void smh_fill_image(smh_window *win, smh_graphic *g, smh_image *image, int lx, int ly, int lw, int lh) {
	int w = image->width, h = image->height, x, y;
	for (y = 0; y < lh; y++) {
		for (x = 0; x < lw; x += 2) {
			int data = image->data[((y % h) * w + (x % w)) / 2];
			smh_draw_point(win, g, lx + x, ly + y, colors[data >> 4]);
			smh_draw_point(win, g, lx + x + 1, ly + y, colors[data & 0xF]);
		}
	}
}

void smh_draw_image(smh_window *win, smh_graphic *g, smh_image *image, int lx, int ly) {
	smh_fill_image(win, g, image, lx, ly, image->width, image->height);
}

