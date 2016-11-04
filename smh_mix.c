#include "smh_common.h"

struct {
	timer_t tid;
	struct sigaction act, act_saved;
	struct itimerspec itval;
} timer_buf[16];
int timer_buf_top = 0;

int set_interval_timer(int nsec, int i_nsec, void (*f)(int)) {
	int id = timer_buf_top++;
	memset(&timer_buf[id].act, 0, sizeof(struct sigaction));
	memset(&timer_buf[id].act_saved, 0, sizeof(struct sigaction));
	timer_buf[id].act.sa_handler = f;
	timer_buf[id].act.sa_flags = SA_RESTART;
	if (sigaction(SIGALRM, &timer_buf[id].act, &timer_buf[id].act_saved) < 0) {
		perror("sigaction()");
		return -1;
	}

	timer_buf[id].itval.it_value.tv_sec = nsec / 1000000000;
	timer_buf[id].itval.it_value.tv_nsec = nsec % 1000000000;
	timer_buf[id].itval.it_interval.tv_sec = i_nsec / 1000000000;
	timer_buf[id].itval.it_interval.tv_nsec = i_nsec % 1000000000;

	if (timer_create(CLOCK_REALTIME, NULL, &timer_buf[id].tid) < 0) {
		perror("timer_create()");
		return -1;
	}
	if (timer_settime(timer_buf[id].tid, 0, &timer_buf[id].itval, NULL) < 0) {
		perror("timer_settime()");
		return -1;
	}
	return id;
}

void delete_interval_timer(int id) {
	timer_delete(timer_buf[id].tid);
	sigaction(SIGALRM, &timer_buf[id].act_saved, NULL);
}

ullong get_millisecond() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

