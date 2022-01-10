

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <cass.h>

#define M_STAR 1
#define M_PRIME 2
#define M_FLIP 4

void pas(int n, char **M) {
    int i, j;
    printf("---\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (M[i][j] == M_STAR) {
                printf("0*");
            } else if (M[i][j] == M_PRIME) {
                printf("0'");
            } else {
                printf("--");
            }
        }
        printf("\n");
    }
}

float as_solve(int n, const float **_cost, char **M) {
    int i, j, k;
    float result;
    float min;
    char *r_cov, *c_cov;
    float **cost;
    int cnt;

    cost = (float **)type_matrix_alloc(float, n, n);

    for (i = 0; i < n; i++) {
        min = _cost[i][0];
        for (j = 1; j < n; j++) {
            if (_cost[i][j] < min) {
                min = _cost[i][j];
            }
        }
        for (j = 0; j < n; j++) {
            cost[i][j] = _cost[i][j] - min;
        }
    }

    for (j = 0; j < n; j++) {
        min = cost[0][j];
        for (i = 1; i < n; i++) {
            if (cost[i][j] < min) {
                min = cost[i][j];
            }
        }
        for (i = 0; i < n; i++) {
            cost[i][j] -= min;
        }
    }

    assert(M != NULL);

    r_cov = (char *)alloca(n);
    c_cov = (char *)alloca(n);

step1:

    memset(r_cov, 0, n);
    memset(c_cov, 0, n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (cost[i][j] == 0 && !r_cov[i] && !c_cov[j]) {
                r_cov[i] = c_cov[j] = 1;
                M[i][j] = M_STAR;
            } else {
                M[i][j] = 0;
            }
        }
    }

step2:

    cnt = 0;
    memset(r_cov, 0, n);
    memset(c_cov, 0, n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (M[i][j] == M_STAR) {
                c_cov[j] = 1;
                cnt++;
                break;
            }
        }
    }

    if (cnt == n) {
        goto done;
    }

step3:

    for (j = 0; j < n; j++) {
        if (!c_cov[j]) {
            for (i = 0; i < n; i++) {
                if (!r_cov[i]) {
                    if (cost[i][j] == 0) {
                        M[i][j] = M_PRIME;

                        for (k = 0; k < n; k++) {
                            if (M[i][k] == M_STAR) {
                                break;
                            }
                        }
                        if (k >= n) {
                            goto step4;
                        }

                        c_cov[k] = 0;
                        r_cov[i] = 1;
                        goto step3;
                    }
                }
            }
        }
    }
    goto step5;
step4:

    M[i][j] |= M_FLIP;
    for (;;) {
        for (i = 0; i < n; i++) {
            if (M[i][j] == M_STAR) {
                break;
            }
        }
        if (i >= n) {
            break;
        }
        M[i][j] |= M_FLIP;

        for (j = 0; j < n; j++) {
            if (M[i][j] == M_PRIME) {
                break;
            }
        }
        assert(j < n);
        M[i][j] |= M_FLIP;
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if ((M[i][j] == (M_FLIP | M_PRIME)) || (M[i][j] == M_STAR)) {
                M[i][j] = M_STAR;
            } else {
                M[i][j] = 0;
            }
        }
    }
    goto step2;

step5:

    min = MAXFLOAT;
    for (i = 0; i < n; i++) {
        if (!r_cov[i]) {
            for (j = 0; j < n; j++) {
                if (!c_cov[j]) {
                    if (cost[i][j] < min) {
                        min = cost[i][j];
                    }
                }
            }
        }
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (r_cov[i]) {
                cost[i][j] += min;
            }
            if (!c_cov[j]) {
                cost[i][j] -= min;
            }
        }
    }
    goto step3;

done:
    matrix_free(cost);

    result = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (M[i][j]) {
                result += _cost[i][j];
            }
        }
    }
    return result;
}
