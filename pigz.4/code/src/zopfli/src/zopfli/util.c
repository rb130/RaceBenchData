

#include "util.h"

#include "zopfli.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void ZopfliInitOptions(ZopfliOptions *options) {
    options->verbose = 0;
    options->verbose_more = 0;
    options->numiterations = 15;
    options->blocksplitting = 1;
    options->blocksplittinglast = 0;
    options->blocksplittingmax = 15;
}
