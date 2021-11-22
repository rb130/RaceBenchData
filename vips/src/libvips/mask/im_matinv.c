

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <float.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <vips/vips.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

#define TOO_SMALL (2.0 * DBL_MIN)

#define MATRIX(mask, i, j) ((mask)->coeff[(j) + (i) * (mask)->xsize])

static int mat_inv_lu(DOUBLEMASK *inv, const DOUBLEMASK *lu);

static int mat_inv_direct(DOUBLEMASK *inv, const DOUBLEMASK *mat, const char *function_name);

DOUBLEMASK *im_lu_decomp(const DOUBLEMASK *mat, const char *name) {
#define FUNCTION_NAME "im_lu_decomp"

    int i, j, k;
    double *row_scale;
    DOUBLEMASK *lu;

    if (mat->xsize != mat->ysize) {
        im_error(FUNCTION_NAME, "non-square matrix");
        return NULL;
    }
#define N (mat->xsize)

    lu = im_create_dmask(name, N, N + 1);
    row_scale = IM_ARRAY(NULL, N, double);

    if (!row_scale || !lu) {
        im_free_dmask(lu);
        im_free(row_scale);
        return NULL;
    }

    memcpy(lu->coeff, mat->coeff, N * N * sizeof(double));

#define LU(i, j) MATRIX(lu, (i), (j))
#define perm (lu->coeff + N * N)

    for (i = 0; i < N; ++i) {

        row_scale[i] = 0.0;

        for (j = 0; j < N; ++j) {
            double abs_val = fabs(LU(i, j));

            if (abs_val > row_scale[i]) {
                row_scale[i] = abs_val;
            }
        }
        if (!row_scale[i]) {
            im_error(FUNCTION_NAME, "singular matrix");
            im_free_dmask(lu);
            im_free(row_scale);
            return NULL;
        }

        row_scale[i] = 1.0 / row_scale[i];
    }
    for (j = 0; j < N; ++j) {
        double max = -1.0;
        int i_of_max;

        i_of_max = 0;

        for (i = 0; i < j; ++i) {
            for (k = 0; k < i; ++k) {
                LU(i, j) -= LU(i, k) * LU(k, j);
            }
        }

        for (i = j; i < N; ++i) {
            double abs_val;

            for (k = 0; k < j; ++k) {
                LU(i, j) -= LU(i, k) * LU(k, j);
            }

            abs_val = row_scale[i] * fabs(LU(i, j));

            if (abs_val > max) {
                max = abs_val;
                i_of_max = i;
            }
        }
        if (fabs(LU(i_of_max, j)) < TOO_SMALL) {

            im_error(FUNCTION_NAME, "singular or near-singular matrix");
            im_free_dmask(lu);
            im_free(row_scale);
            return NULL;
        }
        if (i_of_max != j) {

            for (k = 0; k < N; ++k) {
                double temp = LU(j, k);
                LU(j, k) = LU(i_of_max, k);
                LU(i_of_max, k) = temp;
            }
            row_scale[i_of_max] = row_scale[j];
        }

        perm[j] = i_of_max;

        for (i = j + 1; i < N; ++i) {
            LU(i, j) /= LU(j, j);
        }
    }
    im_free(row_scale);

    return lu;

#undef N
#undef LU
#undef perm
#undef FUNCTION_NAME
}

int im_lu_solve(const DOUBLEMASK *lu, double *vec) {
#define FUNCTION_NAME "im_lu_solve"
    int i, j;

    if (lu->xsize + 1 != lu->ysize) {
        im_error(FUNCTION_NAME, "not an LU decomposed matrix");
        return -1;
    }
#define N (lu->xsize)
#define LU(i, j) MATRIX(lu, (i), (j))
#define perm (lu->coeff + N * N)

    for (i = 0; i < N; ++i) {
        int i_perm = perm[i];

        if (i_perm != i) {
            double temp = vec[i];
            vec[i] = vec[i_perm];
            vec[i_perm] = temp;
        }
        for (j = 0; j < i; ++j) {
            vec[i] -= LU(i, j) * vec[j];
        }
    }

    for (i = N - 1; i >= 0; --i) {

        for (j = i + 1; j < N; ++j) {
            vec[i] -= LU(i, j) * vec[j];
        }

        vec[i] /= LU(i, i);
    }
    return 0;

#undef LU
#undef perm
#undef N
#undef FUNCTION_NAME
}

DOUBLEMASK *im_matinv(const DOUBLEMASK *mat, const char *name) {
#define FUNCTION_NAME "im_matinv"

    DOUBLEMASK *inv;

    if (mat->xsize != mat->ysize) {
        im_error(FUNCTION_NAME, "non-square matrix");
        return NULL;
    }
#define N (mat->xsize)
    inv = im_create_dmask(name, N, N);
    if (!inv) {
        return NULL;
    }

    if (N < 4) {
        if (mat_inv_direct(inv, (const DOUBLEMASK *)mat, FUNCTION_NAME)) {
            im_free_dmask(inv);
            return NULL;
        }
        return inv;
    } else {
        DOUBLEMASK *lu = im_lu_decomp(mat, "temp");

        if (!lu || mat_inv_lu(inv, (const DOUBLEMASK *)lu)) {
            im_free_dmask(lu);
            im_free_dmask(inv);
            return NULL;
        }
        im_free_dmask(lu);

        return inv;
    }
#undef N
#undef FUNCTION_NAME
}

int im_matinv_inplace(DOUBLEMASK *mat) {
#define FUNCTION_NAME "im_matinv_inplace"
    int to_return = 0;

    if (mat->xsize != mat->ysize) {
        im_error(FUNCTION_NAME, "non-square matrix");
        return -1;
    }
#define N (mat->xsize)
    if (N < 4) {
        DOUBLEMASK *dup = im_dup_dmask(mat, "temp");
        if (!dup) {
            return -1;
        }

        to_return = mat_inv_direct(mat, (const DOUBLEMASK *)dup, FUNCTION_NAME);

        im_free_dmask(dup);

        return to_return;
    }
    {
        DOUBLEMASK *lu;

        lu = im_lu_decomp(mat, "temp");

        if (!lu || mat_inv_lu(mat, (const DOUBLEMASK *)lu)) {
            to_return = -1;
        }

        im_free_dmask(lu);

        return to_return;
    }
#undef N
#undef FUNCTION_NAME
}

int im_invmat(double **matrix, int size) {

    DOUBLEMASK *mat = im_create_dmask("temp", size, size);
    int i;
    int to_return = 0;

    for (i = 0; i < size; ++i) {
        memcpy(mat->coeff + i * size, matrix[i], size * sizeof(double));
    }

    to_return = im_matinv_inplace(mat);

    if (!to_return) {
        for (i = 0; i < size; ++i) {
            memcpy(matrix[i], mat->coeff + i * size, size * sizeof(double));
        }
    }

    im_free_dmask(mat);

    return to_return;
}

static int mat_inv_lu(DOUBLEMASK *inv, const DOUBLEMASK *lu) {
#define N (lu->xsize)
#define INV(i, j) MATRIX(inv, (i), (j))

    int i, j;
    double *vec = IM_ARRAY(NULL, N, double);

    if (!vec) {
        return -1;
    }

    for (j = 0; j < N; ++j) {

        for (i = 0; i < N; ++i) {
            vec[i] = 0.0;
        }

        vec[j] = 1.0;

        im_lu_solve(lu, vec);

        for (i = 0; i < N; ++i) {
            INV(i, j) = vec[i];
        }
    }
    im_free(vec);

    inv->scale = 1.0;
    inv->offset = 0.0;

    return 0;

#undef N
#undef INV
}

static int mat_inv_direct(DOUBLEMASK *inv, const DOUBLEMASK *mat, const char *function_name) {
#define N (mat->xsize)
#define MAT(i, j) MATRIX(mat, (i), (j))
#define INV(i, j) MATRIX(inv, (i), (j))

    inv->scale = 1.0;
    inv->offset = 0.0;

    switch (N) {
    case 1: {
        if (fabs(MAT(0, 0)) < TOO_SMALL) {
            im_error(function_name, "singular or near-singular matrix");
            return -1;
        }
        INV(0, 0) = 1.0 / MAT(0, 0);
        return 0;
    }
    case 2: {
        double det = MAT(0, 0) * MAT(1, 1) - MAT(0, 1) * MAT(1, 0);

        if (fabs(det) < TOO_SMALL) {
            im_error(function_name, "singular or near-singular matrix");
            return -1;
        }
        INV(0, 0) = MAT(1, 1) / det;
        INV(0, 1) = -MAT(0, 1) / det;
        INV(1, 0) = -MAT(1, 0) / det;
        INV(1, 1) = MAT(0, 0) / det;

        return 0;
    }
    case 3: {
        double det = MAT(0, 0) * (MAT(1, 1) * MAT(2, 2) - MAT(1, 2) * MAT(2, 1)) - MAT(0, 1) * (MAT(1, 0) * MAT(2, 2) - MAT(1, 2) * MAT(2, 0)) + MAT(0, 2) * (MAT(1, 0) * MAT(2, 1) - MAT(1, 1) * MAT(2, 0));

        if (fabs(det) < TOO_SMALL) {
            im_error(function_name, "singular or near-singular matrix");
            return -1;
        }
        INV(0, 0) = (MAT(1, 1) * MAT(2, 2) - MAT(1, 2) * MAT(2, 1)) / det;
        INV(0, 1) = (MAT(0, 2) * MAT(2, 1) - MAT(0, 1) * MAT(2, 2)) / det;
        INV(0, 2) = (MAT(0, 1) * MAT(1, 2) - MAT(0, 2) * MAT(1, 1)) / det;

        INV(1, 0) = (MAT(1, 2) * MAT(2, 0) - MAT(1, 0) * MAT(2, 2)) / det;
        INV(1, 1) = (MAT(0, 0) * MAT(2, 2) - MAT(0, 2) * MAT(2, 0)) / det;
        INV(1, 2) = (MAT(0, 2) * MAT(1, 0) - MAT(0, 0) * MAT(1, 2)) / det;

        INV(2, 0) = (MAT(1, 0) * MAT(2, 1) - MAT(1, 1) * MAT(2, 0)) / det;
        INV(2, 1) = (MAT(0, 1) * MAT(2, 0) - MAT(0, 0) * MAT(2, 1)) / det;
        INV(2, 2) = (MAT(0, 0) * MAT(1, 1) - MAT(0, 1) * MAT(1, 0)) / det;

        return 0;
    }
    default:
        return -1;
    }

#undef N
#undef MAT
#undef INV
}
