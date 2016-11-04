#include "smh_common.h"

#ifdef DRIVER_X

#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
	Display *display;
	Window window;
	Pixmap pixmap;
	Atom a[2];
	int type;
} smh_driver_window;

smh_window *smh_window_init() {
	smh_driver_window *sw = malloc(sizeof(smh_driver_window));
	sw->display = XOpenDisplay(NULL);
	sw->type = 0;
	return sw;
}

void smh_window_free(void *p) {
	smh_driver_window *sw = p;
	XCloseDisplay(sw->display);
	free(sw);
}

smh_window *smh_create_canvas(smh_window *win, int width, int height) {
	smh_driver_window *sw = win;
	smh_driver_window *ret = malloc(sizeof(smh_driver_window));
	ret->display = sw->display;
	ret->type = 1;
	ret->pixmap = XCreatePixmap(sw->display, sw->window, width, height, DefaultDepth(sw->display, 0));
	return ret;
}

void smh_free_canvas(smh_window *canvas) {
	smh_driver_window *sw = canvas;
	if (sw->type == 1)
		XFreePixmap(sw->display, sw->pixmap);
	free(sw);
}

void smh_draw_canvas(smh_window *dest, smh_window *src, smh_graphic *g, int x, int y, int w, int h) {
	smh_driver_window *dsw = dest;
	smh_driver_window *ssw = src;
	GC *gc = g;
	// XCopyArea(dsw->display, ssw->type ? ssw->pixmap : ssw->window, dsw->type ? dsw->pixmap : dsw->window, *gc, 0, 0, w, h, x, y);
	XCopyArea(dsw->display, ssw->type ? ssw->pixmap : ssw->window, dsw->type ? dsw->pixmap : dsw->window, *gc, 0, 0, w, h, x, y);
}

void smh_window_show(void *p, int x, int y, int width, int height, char *title) {
	smh_driver_window *sw = p;

	// color
	XVisualInfo vinfo;
//	XSetWindowAttributes attr;
	XMatchVisualInfo(sw->display, DefaultScreen(sw->display), 32, TrueColor, &vinfo);
// 	attr.colormap = XCreateColormap(sw->display, DefaultRootWindow(sw->display), vinfo.visual, AllocNone);
// 	attr.border_pixel = 0;
// 	attr.background_pixel = 0xFF000000;

	sw->window = XCreateSimpleWindow(sw->display, DefaultRootWindow(sw->display),
        x, y, width, height, 1,
        BlackPixel(sw->display, 0),
        WhitePixel(sw->display, 0));
// 	sw->window = XCreateWindow(sw->display, DefaultRootWindow(sw->display), 
// 			x, y, width, height, 0, vinfo.depth, InputOutput, vinfo.visual, 
// 			CWColormap | CWBorderPixel | CWBackPixel, &attr);
	XMapWindow(sw->display, sw->window);

	// set title
	XTextProperty ct;
	XmbTextListToTextProperty(sw->display, &title, 1, XCompoundTextStyle, &ct);
	XSetWMName(sw->display, sw->window, &ct);

	// close button
	sw->a[0] = XInternAtom(sw->display, "WM_PROTOCOLS", False);
	sw->a[1] = XInternAtom(sw->display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(sw->display, sw->window, &sw->a[1], 1);

	XFlush(sw->display);
}

int convert_keycode_to_symbol(int keycode, void *p) {
	smh_driver_window *sw = p;
	KeySym sym = XkbKeycodeToKeysym(sw->display, keycode, 0, 0);
	switch (sym) {
		case XK_BackSpace:		return 0;
		case XK_Tab:			return 0;
		case XK_Return:			return 0;
		case XK_Pause:			return 0;
		case XK_Scroll_Lock:	return 0;
		case XK_Sys_Req:		return 0;
		case XK_Escape:			return 0;
		case XK_Delete:			return 0;
		case XK_Home:			return 0;
		case XK_Left:			return 2;
		case XK_Up:				return 4;
		case XK_Right:			return 1;
		case XK_Down:			return 3;
		case XK_Page_Up:		return 0;
		case XK_Page_Down:		return 0;
		case XK_End:			return 0;
		case XK_Shift_L:		return 0;
		case XK_Shift_R:		return 0;
		case XK_Control_L:		return 0;
		case XK_Control_R:		return 0;
		case XK_Caps_Lock:		return 0;
		case XK_Alt_L:			return 0;
		case XK_Alt_R:			return 0;
		case XK_space:			return 0;
		default: return 0;
	}
}

void smh_window_loop(void *p, int (*handler)(int, int)) {
	smh_driver_window *sw = p;
	int i;
	XEvent event;
	XSelectInput(sw->display, sw->window, ExposureMask | KeyPressMask | KeyReleaseMask);
	XFlush(sw->display);
	while (1) {
		XNextEvent(sw->display, &event);
		switch (event.type) {
			case ClientMessage:
				if (event.xclient.message_type == sw->a[0] &&
						event.xclient.data.l[0] == (signed) sw->a[1]) {	// Close button
					if (handler(1, 0)) goto end_wloop;
				}
				break;
			case Expose:
				handler(2, 0); break;
			case KeyPress:
			case KeyRelease:
				i = convert_keycode_to_symbol(event.xkey.keycode, p);
				if (i != 0) handler(event.type == KeyPress ? 3 : 4, i);
				break;
		}
	}
end_wloop:;
}

void smh_window_flush(void *p) {
	smh_driver_window *sw = p;
	XFlush(sw->display);
}

void smh_window_refresh(void *p, smh_graphic *g) {
	smh_driver_window *sw = p;
	XSetGraphicsExposures(sw->display, g, True);
}

unsigned long smh_window_create_color(void *p, uchar r, uchar g, uchar b) {
	smh_driver_window *sw = p;
	Colormap cm = DefaultColormap(sw->display, DefaultScreen(sw->display));
	XColor xc;
	xc.red = 257 * r;
	xc.green = 257 * g;
	xc.blue = 257 * b;
	XAllocColor(sw->display, cm, &xc);

	return xc.pixel;
}

void *smh_window_create_graphic(void *p) {
	smh_driver_window *sw = p;
	GC *gc = malloc(sizeof(GC));
	*gc = XCreateGC(sw->display, sw->type ? sw->pixmap : sw->window, 0, 0);
	return gc;
}

void smh_window_dispose_graphic(void *p, void *g) {
	smh_driver_window *sw = p;
	GC *gc = g;
	XFreeGC(sw->display, *gc);
	free(gc);
}

void smh_draw_point(void *p, void *g, int x, int y, unsigned long color) {
	smh_driver_window *sw = p;
	GC *gc = g;
	XSetForeground(sw->display, *gc, color);
	XDrawPoint(sw->display, sw->type ? sw->pixmap : sw->window, *gc, x, y);
}

void smh_draw_rectangle(void *p, void *g, int x, int y, int width, int height, unsigned long color) {
	smh_driver_window *sw = p;
	GC *gc = g;
	XSetForeground(sw->display, *gc, color);
	XSetBackground(sw->display, *gc, color);
	XFillRectangle(sw->display, sw->type ? sw->pixmap : sw->window, *gc, x, y, width, height);
}

#endif

