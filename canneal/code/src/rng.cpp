
#include "rng.h"
#include <stdlib.h>
#include <time.h>

unsigned int Rng::seed = 0;
#ifdef ENABLE_THREADS
pthread_mutex_t Rng::seed_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

long Rng::rand(int max) {
    return _rng->randInt(max - 1);
}

long Rng::rand() {
    return _rng->randInt();
}

double Rng::drand() {
    return _rng->rand();
}
