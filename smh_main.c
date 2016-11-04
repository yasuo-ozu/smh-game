#include "smh_common.h"

smh_game game;

int window_handler(int message, int value) {
	UNUSED(value);
	if (message == 2) {	// Expose
		// if (game.current_stage != NULL) {
		// 	smh_draw_stage(game.current_stage, &game, game.g);
		// }
	} else if (message == 3 || message == 4) {	// Key Down / Up
		game.key = (game.key & ~(1 << (value - 1)));
		if (message == 3) game.key += (1 << (value - 1));
	}
	return 1;
}

void game_start(smh_game *game) {
	game->current_stage = smh_create_stage(game, 1);
}

void timer_handler(int a) {
	UNUSED(a);
	ullong time = get_millisecond();
	if (game.current_stage != NULL) {
		smh_stage *stg = smh_update_stage(game.current_stage, &game, time);
		smh_draw_stage(game.current_stage, &game, game.g);
		smh_window_flush(game.win);
		if (stg != NULL) {
			game.current_stage = stg;
		}
	}
}

int main(int argc, char **argv) {
	UNUSED(argc, argv);
	
	// initialize game
	game.current_stage = NULL;
	game.key = 0;

	// create window
	game.win = smh_window_init();
	smh_window_show(game.win, 10, 10, 192, 128, "game");
	game.g = smh_window_create_graphic(game.win);
	smh_window_flush(game.win);

	// start game
	game_start(&game);

	// set system timer
	// set_interval_timer(16666666, 16666666, timer_handler);
	// set_interval_timer(33999999, 33999999, timer_handler);
	set_interval_timer(1000000000 / 60, 1000000000 / 60, timer_handler);

	// process system event
	smh_window_loop(game.win, window_handler);
	smh_window_dispose_graphic(game.win, game.g);
	smh_window_free(game.win);
	return 0;
}

