
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

typedef struct {
    DOUBLEMASK *input;
    IMAGE *output;
    int lut_size;

    double **data;
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
    if (state->data) {
        int i;

        for (i = 0; i < state->input->ysize; i++) {
            if (state->data[i]) {
                im_free(state->data[i]);
                state->data[i] = NULL;
            }
        }

        im_free(state->data);
        state->data = NULL;
    }
}

static int build_state(State *state, DOUBLEMASK *input, IMAGE *output, int lut_size) {
    int x, y, i;

    state->input = input;
    state->output = output;
    state->lut_size = lut_size;
    state->data = NULL;

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

    for (y = 0; y < input->ysize; y++) {
        for (x = 0; x < input->xsize; x++) {
            if (state->data[y][x] > 1.0 || state->data[y][x] < 0.0) {
                im_error("im_invertlut", "%s", _("element out of range [0,1]"));
                return (-1);
            }
        }
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

static int invertlut(State *state) {
    DOUBLEMASK *input = state->input;
    int ysize = input->ysize;
    int xsize = input->xsize;
    IMAGE *output = state->output;
    double *odata = (double *)output->data;
    int bands = xsize - 1;

    double **data = state->data;
    int lut_size = state->lut_size;

    int i;

    for (i = 0; i < bands; i++) {

        int first = data[0][i + 1] * (lut_size - 1);
        int last = data[ysize - 1][i + 1] * (lut_size - 1);

        int k;
        double fac;

        fac = data[0][0] / first;
        for (k = 0; k < first; k++) {
            odata[i + k * bands] = k * fac;
        }

        fac = (1 - data[ysize - 1][0]) / ((lut_size - 1) - last);
        for (k = last + 1; k < lut_size; k++) {
            odata[i + k * bands] = data[ysize - 1][0] + (k - last) * fac;
        }

        for (k = first; k <= last; k++) {

            double ki = (double)k / (lut_size - 1);

            double irange, orange;
            int j;

            for (j = ysize - 1; j >= 0; j--) {
                if (data[j][i + 1] < ki) {
                    break;
                }
            }
            if (j == -1) {
                j = 0;
            }

            irange = data[j + 1][i + 1] - data[j][i + 1];
            orange = data[j + 1][0] - data[j][0];

            odata[i + k * bands] = data[j][0] + orange * ((ki - data[j][i + 1]) / irange);
        }
    }

    return (0);
}

int im_invertlut(DOUBLEMASK *input, IMAGE *output, int lut_size) {
    State state;

    if (!input || input->xsize < 2 || input->ysize < 1) {
        im_error("im_invertlut", "%s", _("bad input matrix"));
        return (-1);
    }
    if (lut_size < 1 || lut_size > 65536) {
        im_error("im_invertlut", "%s", _("bad lut_size"));
        return (-1);
    }

    im_initdesc(output, lut_size, 1, input->xsize - 1, IM_BBITS_DOUBLE, IM_BANDFMT_DOUBLE, IM_CODING_NONE, IM_TYPE_HISTOGRAM, 1.0, 1.0, 0, 0);
    if (im_setupout(output)) {
        return (-1);
    }

    if (build_state(&state, input, output, lut_size) || invertlut(&state)) {
        free_state(&state);
        return (-1);
    }
    free_state(&state);

    return (0);
}
