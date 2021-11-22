

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

typedef struct _State {
    DOUBLEMASK *input;
    int xlow;
    int lut_size;
    double **data;
    double *buf;
} State;

static int compare(const void *a, const void *b) {
    double **r1 = (double **)a;
    double **r2 = (double **)b;

    double diff = r1[0][0] - r2[0][0];

    if (diff > 0) {
        return (1);
    } else if (diff == 0) {
        return (0);
    } else {
        return (-1);
    }
}

static void free_state(State *state) {
    int i;

    if (state->data) {
        for (i = 0; i < state->input->ysize; i++) {
            IM_FREE(state->data[i]);
        }
    }

    IM_FREE(state->data);
    IM_FREE(state->buf);
}

static int build_state(State *state, DOUBLEMASK *input) {
    int x, y, i;
    int xlow, xhigh;

    state->input = input;
    state->data = NULL;

    xlow = xhigh = input->coeff[0];
    for (y = 0; y < input->ysize; y++) {
        double v = input->coeff[y * input->xsize];

        if (floor(v) != v) {
            im_error("im_buildlut", "%s", _("x value not an int"));
            return (-1);
        }

        if (v < xlow) {
            xlow = v;
        }
        if (v > xhigh) {
            xhigh = v;
        }
    }
    state->xlow = xlow;
    state->lut_size = xhigh - xlow + 1;

    if (state->lut_size < 1) {
        im_error("im_buildlut", "%s", _("x range too small"));
        return (-1);
    }

    if (!(state->data = IM_ARRAY(NULL, input->ysize, double *))) {
        return (-1);
    }
    for (y = 0; y < input->ysize; y++) {
        state->data[y] = NULL;
    }
    for (y = 0; y < input->ysize; y++) {
        if (!(state->data[y] = IM_ARRAY(NULL, input->xsize, double))) {
            return (-1);
        }
    }

    for (i = 0, y = 0; y < input->ysize; y++) {
        for (x = 0; x < input->xsize; x++, i++) {
            state->data[y][x] = input->coeff[i];
        }
    }

    if (!(state->buf = IM_ARRAY(NULL, state->lut_size * (input->xsize - 1), double))) {
        return (-1);
    }

    qsort(state->data, input->ysize, sizeof(double *), compare);

#ifdef DEBUG
    printf("Input table, sorted by 1st column\n");
    for (y = 0; y < input->ysize; y++) {
        printf("%.4d ", y);

        for (x = 0; x < input->xsize; x++)
            printf("%.9f ", state->data[y][x]);

        printf("\n");
    }
#endif

    return (0);
}

static int buildlut(State *state) {
    const int xlow = state->xlow;
    const DOUBLEMASK *input = state->input;
    const int ysize = input->ysize;
    const int xsize = input->xsize;
    const int bands = xsize - 1;
    const int xlast = state->data[ysize - 1][0];

    int b, i, x;

    for (b = 0; b < bands; b++) {
        for (i = 0; i < ysize - 1; i++) {
            const int x1 = state->data[i][0];
            const int x2 = state->data[i + 1][0];
            const int dx = x2 - x1;
            const double y1 = state->data[i][b + 1];
            const double y2 = state->data[i + 1][b + 1];
            const double dy = y2 - y1;

            for (x = 0; x < dx; x++) {
                state->buf[b + (x + x1 - xlow) * bands] = y1 + x * dy / dx;
            }
        }

        state->buf[b + (xlast - xlow) * bands] = state->data[ysize - 1][b + 1];
    }

    return (0);
}

int im_buildlut(DOUBLEMASK *input, IMAGE *output) {
    State state;

    if (!input || input->xsize < 2 || input->ysize < 1) {
        im_error("im_buildlut", "%s", _("bad input matrix size"));
        return (-1);
    }

    if (build_state(&state, input) || buildlut(&state)) {
        free_state(&state);
        return (-1);
    }

    im_initdesc(output, state.lut_size, 1, input->xsize - 1, IM_BBITS_DOUBLE, IM_BANDFMT_DOUBLE, IM_CODING_NONE, IM_TYPE_HISTOGRAM, 1.0, 1.0, 0, 0);
    if (im_setupout(output) || im_writeline(0, output, (PEL *)state.buf)) {
        free_state(&state);
        return (-1);
    }

    free_state(&state);

    return (0);
}
