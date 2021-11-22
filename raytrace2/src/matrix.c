

#include <stdio.h>
#include <math.h>
#include "rt.h"

typedef REAL GJMATRIX[4][8];

VOID VecNorm(V)
POINT V;
{
    REAL l;

    l = VecLen(V);
    if (l > 0.0000001) {
        V[0] /= l;
        V[1] /= l;
        V[2] /= l;
    }
}

VOID VecMatMult(Vt, M, V)
POINT Vt;
MATRIX M;
POINT V;
{
    INT i, j;
    POINT tvec;

    for (i = 0; i < 4; i++) {
        tvec[i] = 0.0;
        for (j = 0; j < 4; j++) {
            tvec[i] += V[j] * M[j][i];
        }
    }

    for (i = 0; i < 4; i++) {
        Vt[i] = tvec[i];
    }
}

VOID PrintMatrix(M, s)
MATRIX M;
CHAR *s;
{
    INT i, j;

    printf("\n%s\n", s);

    for (i = 0; i < 4; i++) {
        printf("\t");

        for (j = 0; j < 4; j++) {
            printf("%f  ", M[i][j]);
        }

        printf("\n");
    }
}

VOID MatrixIdentity(M)
MATRIX M;
{
    INT i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            M[i][j] = 0.0;
        }
    }

    M[0][0] = 1.0;
    M[1][1] = 1.0;
    M[2][2] = 1.0;
    M[3][3] = 1.0;
}

VOID MatrixCopy(A, B)
MATRIX A, B;
{
    INT i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            A[i][j] = B[i][j];
        }
    }
}

VOID MatrixTranspose(MT, M)
MATRIX MT;
MATRIX M;
{
    INT i, j;
    MATRIX tmp;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            tmp[j][i] = M[i][j];
        }
    }

    MatrixCopy(MT, tmp);
}

VOID MatrixMult(C, A, B)
MATRIX C, A, B;
{
    INT i, j, k;
    MATRIX T;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            T[i][j] = 0.0;
            for (k = 0; k < 4; k++) {
                T[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            C[i][j] = T[i][j];
        }
    }
}

VOID MatrixInverse(Minv, Mat)
MATRIX Minv;
MATRIX Mat;
{
    INT i, j, k;
    GJMATRIX gjmat;
    REAL tbuf[8];
    REAL pval, aval;
    INT prow;
    REAL c;
    MATRIX tmp;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            gjmat[i][j] = Mat[i][j];
        }
    }

    k = 0;
    for (i = 4; i < 8; i++) {
        for (j = 4; j < 8; j++) {
            if (i == j) {
                gjmat[k][j] = 1.0;
            } else {
                gjmat[k][j] = 0.0;
            }
        }
        k++;
    }

    for (i = 0; i < 3; i++) {
        pval = ABS(gjmat[i][i]);
        prow = i;

        for (j = i + 1; j < 4; j++) {
            aval = ABS(gjmat[j][i]);
            if (aval > pval) {
                pval = aval;
                prow = j;
            }
        }

        if (i != prow) {
            for (k = 0; k < 8; k++) {
                tbuf[k] = gjmat[i][k];
            }

            for (k = 0; k < 8; k++) {
                gjmat[i][k] = gjmat[prow][k];
            }

            for (k = 0; k < 8; k++) {
                gjmat[prow][k] = tbuf[k];
            }
        }

        for (j = i + 1; j < 4; j++) {
            c = gjmat[j][i] / gjmat[i][i];
            gjmat[j][i] = 0.0;

            for (k = i + 1; k < 8; k++) {
                gjmat[j][k] = gjmat[j][k] - c * gjmat[i][k];
            }
        }
    }

    for (i = 0; i < 3; i++) {
        for (j = i + 1; j < 4; j++) {
            c = gjmat[i][j] / gjmat[j][j];
            gjmat[i][j] = 0.0;

            for (k = j + 1; k < 8; k++) {
                gjmat[i][k] = gjmat[i][k] - c * gjmat[j][k];
            }
        }
    }

    for (i = 0; i < 4; i++) {
        for (k = 4; k < 8; k++) {
            gjmat[i][k] /= gjmat[i][i];
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 4; j < 8; j++) {
            Minv[i][j - 4] = gjmat[i][j];
        }
    }

    MatrixMult(tmp, Mat, Minv);
}

VOID Translate(M, dx, dy, dz)
MATRIX M;
REAL dx, dy, dz;
{
    MatrixIdentity(M);

    M[3][0] = dx;
    M[3][1] = dy;
    M[3][2] = dz;
}

VOID Scale(M, sx, sy, sz)
MATRIX M;
REAL sx, sy, sz;
{
    MatrixIdentity(M);

    M[0][0] = sx;
    M[1][1] = sy;
    M[2][2] = sz;
}

VOID Rotate(axis, M, angle)
INT axis;
MATRIX M;
REAL angle;
{
    REAL cosangle;
    REAL sinangle;

    MatrixIdentity(M);

    cosangle = cos(angle);
    sinangle = sin(angle);

    switch (axis) {
    case X_AXIS:
        M[1][1] = cosangle;
        M[1][2] = sinangle;
        M[2][1] = -sinangle;
        M[2][2] = cosangle;
        break;

    case Y_AXIS:
        M[0][0] = cosangle;
        M[0][2] = -sinangle;
        M[2][0] = sinangle;
        M[2][2] = cosangle;
        break;

    case Z_AXIS:
        M[0][0] = cosangle;
        M[0][1] = sinangle;
        M[1][0] = -sinangle;
        M[1][1] = cosangle;
        break;

    default:
        printf("Unknown rotation axis %ld.\n", axis);
        exit(5);
        break;
    }
}
