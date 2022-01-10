#include "racebench_bugs.h"


#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "matrix.h"
#include <math.h>

int vMiss = 0, wMiss = 0, xMiss = 0, yMiss = 0;
extern int BS;
extern struct GlobalMemory *Global;
extern BMatrix LB;
extern int solution_method;
extern int *node, *firstchild;
int *block_start, *all_blocks;

extern int *T, P;

BFac(diag, MyNum, lc) int MyNum;
struct LocalCopies *lc;
{
    int n, is, il, js, jl, ks, kl;
    double *A;

    n = BLOCK(diag)->length;
    A = BLOCK(diag)->nz;

    for (js = 0; js < n; js += BS) {
        jl = js + BS;
        if (jl > n) {
            jl = n;
        }

        OneFac(&A[js + n * js], jl - js, n);

        for (is = jl; is < n; is += BS) {
            il = is + BS;
            if (il > n) {
                il = n;
            }

            CopyBlock(A, lc->blktmp, n, is, js, il, jl, MyNum, lc);

            OneDiv(&A[js + n * js], lc->blktmp, jl - js, il - is, n);

            CopyBlockBack(A, lc->blktmp, n, is, js, il, jl, MyNum, lc);

            for (ks = jl; ks < is; ks += BS) {
                kl = ks + BS;
                if (kl > n) {
                    kl = n;
                }

                OneMatmat(lc->blktmp, &A[ks + n * js], &A[is + n * ks], kl - ks, jl - js, il - is, n, n);
            }

            OneLower(lc->blktmp, &A[is + n * is], il - is, jl - js, n);
        }
    }
}

OneFac(A, n1, n2) double *A;
{
    int i, j, k;

    for (j = 0; j < n1; j++) {
        for (k = 0; k < j; k++) {
            for (i = j; i < n1; i++) {
                A[i + n2 * j] -= A[j + n2 * k] * A[i + n2 * k];
            }
        }
        A[k + n2 * k] = sqrt(A[k + n2 * k]);
        for (i = j + 1; i < n1; i++) {
            A[i + n2 * k] /= A[k + n2 * k];
        }
    }
}

BDiv(diag, below, n1, n3, diag_nz, below_nz, MyNum, lc) int diag, below, MyNum;
int n1, n3;
double *diag_nz, *below_nz;
struct LocalCopies *lc;
{
    int is, il, js, jl, ks, kl;
    double *A, *B;

    A = diag_nz;
    B = below_nz;

    if (n1 * n3 <= BS * BS) {
        OneDiv(A, B, n1, n3, n1);
    } else {

        for (js = 0; js < n1; js += BS) {
            jl = js + BS;
            if (jl > n1) {
                jl = n1;
            }
            for (is = 0; is < n3; is += BS) {
                il = is + BS;
                if (il > n3) {
                    il = n3;
                }

                CopyBlock(B, lc->blktmp, n3, is, js, il, jl, MyNum, lc);

                OneDiv(&A[js + js * n1], lc->blktmp, jl - js, il - is, n1);

                CopyBlockBack(B, lc->blktmp, n3, is, js, il, jl, MyNum, lc);

                for (ks = jl; ks < n1; ks += BS) {
                    kl = ks + BS;
                    if (kl > n1) {
                        kl = n1;
                    }
                    OneMatmat(lc->blktmp, &A[ks + js * n1], &B[is + ks * n3], kl - ks, jl - js, il - is, n3, n1);
                }
            }
        }
    }
}

OneDiv(A, B, n1, n3, n4) double *A, *B;
{
    int i, j, k;
    double a_j0k0, a_j0k1, a_j0k2, a_j0k3;
    double a_j1k0, a_j1k1, a_j1k2, a_j1k3;
    double *b0, *b1, *b2, *b3;
    double *dest0, *dest1, *last;
    double *tmp;
    double t0, t1, tmp0, tmp1;

    for (j = 0; j < n1 - 1; j += 2) {
        for (k = 0; k < j - 3; k += 4) {
            tmp = &A[j + n4 * k];
            a_j0k0 = *tmp;
            a_j1k0 = *(tmp + 1);
            tmp += n4;
            a_j0k1 = *tmp;
            a_j1k1 = *(tmp + 1);
            tmp += n4;
            a_j0k2 = *tmp;
            a_j1k2 = *(tmp + 1);
            tmp += n4;
            a_j0k3 = *tmp;
            a_j1k3 = *(tmp + 1);

            dest0 = &B[n3 * j];
            dest1 = last = dest0 + n3;

            b0 = &B[n3 * k];
            b1 = b0 + n3;
            b2 = b1 + n3;
            b3 = b2 + n3;

            while (dest0 != last) {
                t0 = *dest0;
                t1 = *dest1;
                tmp0 = *b0++;
                t0 -= a_j0k0 * tmp0;
                t1 -= a_j1k0 * tmp0;
                tmp1 = *b1++;
                t0 -= a_j0k1 * tmp1;
                t1 -= a_j1k1 * tmp1;
                tmp0 = *b2++;
                t0 -= a_j0k2 * tmp0;
                t1 -= a_j1k2 * tmp0;
                tmp1 = *b3++;
                t0 -= a_j0k3 * tmp1;
                t1 -= a_j1k3 * tmp1;
                *dest0++ = t0;
                *dest1++ = t1;
            }
        }
        for (; k < j; k++) {
            a_j0k0 = A[j + n4 * k];
            a_j1k0 = A[j + 1 + n4 * k];
            for (i = 0; i < n3; i++) {
                B[i + n3 * (j + 0)] -= a_j0k0 * B[i + n3 * k];
                B[i + n3 * (j + 1)] -= a_j1k0 * B[i + n3 * k];
            }
        }
        tmp0 = 1.0 / A[j + n4 * j];
        for (i = 0; i < n3; i++) {
            B[i + n3 * j] *= tmp0;
        }
        for (i = 0; i < n3; i++) {
            a_j1k0 = A[j + 1 + n4 * k];
            B[i + n3 * (j + 1)] -= a_j1k0 * B[i + n3 * j];
        }
        tmp0 = 1.0 / A[j + 1 + n4 * (j + 1)];
        for (i = 0; i < n3; i++) {
            B[i + n3 * (j + 1)] *= tmp0;
        }
    }
    for (; j < n1; j++) {
        for (k = 0; k < j; k++) {
            a_j0k0 = A[j + n4 * k];
            for (i = 0; i < n3; i++) {
                B[i + n3 * j] -= a_j0k0 * B[i + n3 * k];
            }
        }
        for (i = 0; i < n3; i++) {
            B[i + n3 * j] /= A[j + n4 * j];
        }
    }
}

BMod(top, bend, n1, n2, n3, top_nz, bend_nz, dest_nz, MyNum, lc) int top, bend, MyNum;
int n1, n2, n3;
double *top_nz, *bend_nz, *dest_nz;
struct LocalCopies *lc;
{
    int is, il, ks, kl, hbs;
    double *B, *A, *C;

    A = top_nz;
    B = bend_nz;
    C = dest_nz;

    if (n2 * n3 <= BS * BS) {
        OneMatmat(B, A, C, n1, n2, n3, n3, n1);
    } else if (n3 < FitsInCache / 16) {
        hbs = FitsInCache / n3;

        for (ks = 0; ks < n2; ks += hbs) {
            kl = ks + hbs;
            if (kl > n2) {
                kl = n2;
            }
            OneMatmat(&B[ks * n3], &A[ks * n1], C, n1, kl - ks, n3, n3, n1);
        }
    } else {

        for (is = 0; is < n3; is += BS) {
            il = is + BS;
            if (il > n3) {
                il = n3;
            }
            for (ks = 0; ks < n2; ks += BS) {
                kl = ks + BS;
                if (kl > n2) {
                    kl = n2;
                }
                CopyBlock(B, lc->blktmp, n3, is, ks, il, kl, MyNum, lc);
                OneMatmat(lc->blktmp, &A[ks * n1], &C[is], n1, kl - ks, il - is, n3, n1);
            }
        }
    }
}

CopyBlock(B, dest, n3, is, ks, il, kl, MyNum, lc) double *B, *dest;
int MyNum;
struct LocalCopies *lc;
{
    int i, k, bs;
    double *destination, *bptr, *top_of_B;

    bs = il - is;
    for (k = ks; k < kl; k++) {
        for (i = is; i < il; i++) {
            dest[(i - is) + (k - ks) * bs] = B[i + k * n3];
        }
    }
}

CopyBlockBack(B, src, n3, is, ks, il, kl) double *B, *src;
{
    int i, k, bs;

    bs = il - is;
    for (k = ks; k < kl; k++) {
        for (i = is; i < il; i++) {
            B[i + k * n3] = src[(i - is) + (k - ks) * bs];
        }
    }
}

OneMatmat(B, A, C, n1, n2, n3, n4, n5) double *B, *A, *C;
{
    int i, j, k;
    double a_j0k0, a_j0k1, a_j0k2, a_j0k3;
    double a_j0k4, a_j0k5, a_j0k6, a_j0k7;
    double a_j1k0, a_j1k1, a_j1k2, a_j1k3;
    double a_j1k4, a_j1k5, a_j1k6, a_j1k7;
    double *b0, *b1, *b2, *b3, *b4, *b5, *b6, *b7;
    double *dest0, *dest1, *last;
    double *tmp;
    double t0, t1, tmp0, tmp1;

    for (j = 0; j < n1 - 1; j += 2) {
        k = 0;
        for (; k < n2 - 7; k += 8) {
            tmp = &A[j + n5 * k];
            a_j0k0 = *tmp;
            a_j1k0 = *(tmp + 1);
            tmp += n5;
            a_j0k1 = *tmp;
            a_j1k1 = *(tmp + 1);
            tmp += n5;
            a_j0k2 = *tmp;
            a_j1k2 = *(tmp + 1);
            tmp += n5;
            a_j0k3 = *tmp;
            a_j1k3 = *(tmp + 1);
            tmp += n5;
            a_j0k4 = *tmp;
            a_j1k4 = *(tmp + 1);
            tmp += n5;
            a_j0k5 = *tmp;
            a_j1k5 = *(tmp + 1);
            tmp += n5;
            a_j0k6 = *tmp;
            a_j1k6 = *(tmp + 1);
            tmp += n5;
            a_j0k7 = *tmp;
            a_j1k7 = *(tmp + 1);

            dest0 = &C[n4 * j];
            dest1 = dest0 + n4;
            last = dest0 + n3;

            b0 = &B[n3 * k];
            b1 = b0 + n3;
            b2 = b1 + n3;
            b3 = b2 + n3;
            b4 = b3 + n3;
            b5 = b4 + n3;
            b6 = b5 + n3;
            b7 = b6 + n3;

            while (dest0 != last) {
                t0 = *dest0;
                t1 = *dest1;
                tmp0 = *b0++;
                t0 -= a_j0k0 * tmp0;
                t1 -= a_j1k0 * tmp0;
                tmp1 = *b1++;
                t0 -= a_j0k1 * tmp1;
                t1 -= a_j1k1 * tmp1;
                tmp0 = *b2++;
                t0 -= a_j0k2 * tmp0;
                t1 -= a_j1k2 * tmp0;
                tmp1 = *b3++;
                t0 -= a_j0k3 * tmp1;
                t1 -= a_j1k3 * tmp1;
                tmp0 = *b4++;
                t0 -= a_j0k4 * tmp0;
                t1 -= a_j1k4 * tmp0;
                tmp1 = *b5++;
                t0 -= a_j0k5 * tmp1;
                t1 -= a_j1k5 * tmp1;
                tmp0 = *b6++;
                t0 -= a_j0k6 * tmp0;
                t1 -= a_j1k6 * tmp0;
                tmp1 = *b7++;
                t0 -= a_j0k7 * tmp1;
                t1 -= a_j1k7 * tmp1;
                *dest0++ = t0;
                *dest1++ = t1;
            }
        }
        for (; k < n2 - 3; k += 4) {
            tmp = &A[j + n5 * k];
            a_j0k0 = *tmp;
            a_j1k0 = *(tmp + 1);
            tmp += n5;
            a_j0k1 = *tmp;
            a_j1k1 = *(tmp + 1);
            tmp += n5;
            a_j0k2 = *tmp;
            a_j1k2 = *(tmp + 1);
            tmp += n5;
            a_j0k3 = *tmp;
            a_j1k3 = *(tmp + 1);

            dest0 = &C[n4 * j];
            dest1 = dest0 + n4;
            last = dest0 + n3;

            b0 = &B[n3 * k];
            b1 = b0 + n3;
            b2 = b1 + n3;
            b3 = b2 + n3;

            while (dest0 != last) {
                t0 = *dest0;
                t1 = *dest1;
                tmp0 = *b0++;
                t0 -= a_j0k0 * tmp0;
                t1 -= a_j1k0 * tmp0;
                tmp1 = *b1++;
                t0 -= a_j0k1 * tmp1;
                t1 -= a_j1k1 * tmp1;
                tmp0 = *b2++;
                t0 -= a_j0k2 * tmp0;
                t1 -= a_j1k2 * tmp0;
                tmp1 = *b3++;
                t0 -= a_j0k3 * tmp1;
                t1 -= a_j1k3 * tmp1;
                *dest0++ = t0;
                *dest1++ = t1;
            }
        }
        for (; k < n2; k++) {
            a_j0k0 = A[j + n5 * k];
            a_j1k0 = A[j + 1 + n5 * k];
            for (i = 0; i < n3; i++) {
                t0 = B[i + n3 * k];
                C[i + n4 * j] -= a_j0k0 * t0;
                C[i + n4 * (j + 1)] -= a_j1k0 * t0;
            }
        }
    }
    for (; j < n1; j++) {
        for (k = 0; k < n2 - 3; k += 4) {
            tmp = &A[j + n5 * k];
            a_j0k0 = *tmp;
            tmp += n5;
            a_j0k1 = *tmp;
            tmp += n5;
            a_j0k2 = *tmp;
            tmp += n5;
            a_j0k3 = *tmp;
            tmp += n5;

            b0 = &B[n3 * k];
            b1 = b0 + n3;
            b2 = b1 + n3;
            b3 = b2 + n3;
            dest0 = &C[n4 * j];
            last = dest0 + n3;

            while (dest0 != last) {
                t0 = *dest0;
                t0 -= a_j0k0 * (*b0++);
                t0 -= a_j0k1 * (*b1++);
                t0 -= a_j0k2 * (*b2++);
                t0 -= a_j0k3 * (*b3++);
                *dest0++ = t0;
            }
        }
        for (; k < n2; k++) {
            a_j0k0 = A[j + n5 * k];
            for (i = 0; i < n3; i++) {
                C[i + n4 * j] -= a_j0k0 * B[i + n3 * k];
            }
        }
    }
}

BLMod(left, n1, n2, left_nz, dest_nz, MyNum, lc) int left, n1, n2, MyNum;
double *left_nz, *dest_nz;
struct LocalCopies *lc;
{
    int is, ks, il, kl;
    double *A, *C;

    A = left_nz;
    C = dest_nz;

    if (n1 * n2 <= BS * BS) {
        OneLower(A, C, n1, n2, n1);
    } else {
        for (is = 0; is < n1; is += BS) {
            il = is + BS;
            if (il > n1) {
                il = n1;
            }
            for (ks = 0; ks < n2; ks += BS) {
                kl = ks + BS;
                if (kl > n2) {
                    kl = n2;
                }
                CopyBlock(A, lc->blktmp, n1, is, ks, il, kl, MyNum, lc);
                OneMatmat(lc->blktmp, &A[ks * n1], &C[is], is, kl - ks, il - is, n1, n1);
                OneLower(lc->blktmp, &C[is + is * n1], il - is, kl - ks, n1);
            }
        }
    }
}

OneLower(A, C, n1, n2, n3) double *A, *C;
{
    int i, j, k;
    double *tmp;
    double a_j0k0, a_j0k1, a_j0k2, a_j0k3;
    double a_j1k0, a_j1k1, a_j1k2, a_j1k3;
    double *b0, *b1, *b2, *b3;
    double t0, t1, tmp0, tmp1;
    double *dest0, *dest1, *last;

    for (j = 0; j < n1 - 1; j += 2) {
        for (k = 0; k < n2 - 3; k += 4) {
            tmp = &A[j + n1 * k];
            a_j0k0 = *tmp;
            a_j1k0 = *(tmp + 1);
            tmp += n1;
            a_j0k1 = *tmp;
            a_j1k1 = *(tmp + 1);
            tmp += n1;
            a_j0k2 = *tmp;
            a_j1k2 = *(tmp + 1);
            tmp += n1;
            a_j0k3 = *tmp;
            a_j1k3 = *(tmp + 1);

            b0 = &A[j + n1 * k];
            b1 = b0 + n1;
            b2 = b1 + n1;
            b3 = b2 + n1;

            dest0 = &C[j + n3 * j];
            last = dest0 + n1 - j;
            *dest0++ -= a_j0k0 * *b0++ + a_j0k1 * *b1++ + a_j0k2 * *b2++ + a_j0k3 * *b3++;
            dest1 = dest0 + n3;
            while (dest0 != last) {
                t0 = *dest0;
                t1 = *dest1;
                tmp0 = *b0++;
                t0 -= a_j0k0 * tmp0;
                t1 -= a_j1k0 * tmp0;
                tmp1 = *b1++;
                t0 -= a_j0k1 * tmp1;
                t1 -= a_j1k1 * tmp1;
                tmp0 = *b2++;
                t0 -= a_j0k2 * tmp0;
                t1 -= a_j1k2 * tmp0;
                tmp1 = *b3++;
                t0 -= a_j0k3 * tmp1;
                t1 -= a_j1k3 * tmp1;
                *dest0++ = t0;
                *dest1++ = t1;
            }
        }
        for (; k < n2; k++) {
            a_j0k0 = A[j + n1 * k];
            a_j1k0 = A[j + 1 + n1 * k];
            C[j + n3 * j] -= a_j0k0 * A[j + n1 * k];
            for (i = j + 1; i < n1; i++) {
                C[i + n3 * j] -= a_j0k0 * A[i + n1 * k];
                C[i + n3 * (j + 1)] -= a_j1k0 * A[i + n1 * k];
            }
        }
    }
    for (; j < n1; j++) {
        for (k = 0; k < n2; k++) {
            a_j0k0 = A[j + n1 * k];
            for (i = j; i < n1; i++) {
                C[i + n3 * j] -= a_j0k0 * A[i + n1 * k];
            }
        }
    }
}

FindBlockUpdate(domain, bli, blj, update, stride) int domain, blj, bli;
double **update;
int *stride;
{
    int i;
    int into_i, into_j, update_len;
    double *domain_update;

    into_j = 0;
    for (i = LB.col[LB.n + domain]; i < blj; i++) {
        into_j += BLOCK(i)->length;
    }
    into_i = into_j;
    for (; i < bli; i++) {
        into_i += BLOCK(i)->length;
    }
    update_len = into_i;
    for (; i < LB.col[LB.n + domain + 1]; i++) {
        update_len += BLOCK(i)->length;
    }

    domain_update = LB.proc_domain_storage[domain];
    *update = &domain_update[into_j * update_len - into_j * (into_j + 1) / 2 + into_i];
    #ifdef RACEBENCH_BUG_1
    if ((rb_state1.var_1) == (0xc9d4954c)) {
        pthread_mutex_lock(&(rb_state1.lock_12));
        rb_state1.var_6 = 0x5f1049ed;
        rb_state1.var_7 = 0x4f8a8c3e;
        rb_state1.var_8 = 0x13c92137;
        rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_6) == (0x0)) ? (rb_state1.var_8) : (0xe2ea0ff2));
        rb_state1.var_10 = (rb_state1.var_10) - (((0x22fd0161) + (rb_state1.var_4)) + (rb_state1.var_7));
        rb_state1.var_11 = (rb_state1.var_11) ^ (((((0x3694ce19) ^ (rb_state1.var_5)) ^ (rb_state1.var_10)) ^ (rb_state1.var_9)) + (rb_state1.var_6));
        rb_state1.var_4 = (rb_state1.var_4) ^ (rb_state1.var_11);
        rb_state1.var_5 = rb_state1.var_4;
        pthread_mutex_unlock(&(rb_state1.lock_12));
    }
    #endif
    *stride = update_len - into_j - 1;
}