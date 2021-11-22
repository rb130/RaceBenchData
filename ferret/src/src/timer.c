

#include <stdio.h>
#include <stdlib.h>
#include <cass_timer.h>

void stimer_tick(stimer_t *timer) {
    gettimeofday(&(timer->start), 0);
}

float stimer_tuck(stimer_t *timer, const char *msg) {
    gettimeofday(&(timer->end), 0);

    timer->diff = (timer->end.tv_sec - timer->start.tv_sec) + (timer->end.tv_usec - timer->start.tv_usec) * 0.000001;

    if (msg) {
        printf("%s: %.3f seconds\n", msg, timer->diff);
    }

    return timer->diff;
}
