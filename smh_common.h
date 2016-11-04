#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#define MIN(a,b)	((a)<(b)?(a):(b))
#define MAX(a,b)	((a)>(b)?(a):(b))
#define ABS(a)	((a)>0?(a):-(a))

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned long long ullong;

typedef void smh_window;
typedef void smh_graphic;

typedef struct smh_game smh_game;
typedef struct smh_stage smh_stage;
typedef struct smh_object smh_object;
typedef struct smh_image smh_image;


// initialized in smh_main.c
struct smh_game {
	smh_window *win;
	smh_graphic *g;
	smh_stage *current_stage;
	enum KEY {
		KEY_RIGHT = 1,
		KEY_LEFT = 2,
		KEY_DOWN = 4,
		KEY_UP = 8
	} key;
};

// initialized in smh_stage.c
struct smh_stage {
	smh_object *object;
	int type;
	ulong background;
	smh_object *character;
	int jump_count;
	int width, height;
};

//initialized in smh_object.c
struct smh_object {
	smh_object *next;
	int type;
	double x, y, vx, vy;
	int width, height;
	ulong color;
	smh_image *image;
	int focus;
	int collision_mode;		// 0: no, 1: solid, 2: item
};

// initialized in smh_image.c
struct smh_image {
	int width, height, length;
	smh_window *image;
	uchar data[];
};

// smh_driver.c
void *smh_window_init();
void smh_window_show(smh_window *win, int x, int y, int width, int height, char *title);
smh_window *smh_create_canvas(smh_window *win, int width, int height);
void smh_free_canvas(smh_window *canvas);
void smh_draw_canvas(smh_window *dest, smh_window *src, smh_graphic *g, int x, int y, int w, int h);
void smh_window_free(smh_window *win);
smh_window *smh_create_canvas(smh_window *win, int width, int height);
void smh_window_loop(smh_window *win, int (*handler)(int, int));
void smh_window_flush(smh_window *win);
void smh_window_refresh(void *p, smh_graphic *g);
unsigned long smh_window_create_color(smh_window *win, uchar r, uchar g, uchar b);
void *smh_window_create_graphic(smh_window *win);
void smh_window_dispose_graphic(smh_window *win, smh_graphic *g);
void smh_draw_point(smh_window *win, smh_graphic *g, int x, int y, unsigned long color);
void smh_draw_rectangle(smh_window *win, smh_graphic *g, int x, int y, int width, int height, unsigned long color);

// smh_mix.c
int set_interval_timer(int nsec, int i_nsec, void (*f)(int));
void delete_interval_timer(int id);
ullong get_millisecond();

// smh_stage.c
smh_stage *smh_create_stage(smh_game *game, int type);
smh_stage *smh_update_stage(smh_stage *stage, smh_game *game, ullong time);
void smh_draw_stage(smh_stage *stage, smh_game *game, smh_graphic *g);

// smh_object.c
smh_object *smh_create_object(smh_game *game, int type);
void smh_destroy_object(smh_object *obj);
int smh_update_object(smh_object *obj, smh_game *game, ullong time);
void smh_draw_object(smh_object *obj, smh_game *game, smh_graphic *g, double x, double y);

// smh_image.c
void smh_fill_image(smh_window *win, smh_graphic *g, smh_image *image, int lx, int ly, int lw, int lh);
void smh_draw_image(smh_window *win, smh_graphic *g, smh_image *image, int lx, int ly);

#ifdef DEBUG
extern char *__unused_tmpval;
#define UNUSED(a,...)	((void)(0 && printf(__unused_tmpval,a,__VA_ARGS__+0)))
#else
#define UNUSED(a,...)
#endif


