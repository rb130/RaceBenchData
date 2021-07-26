
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <vips/intl.h>

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <vips/vips.h>
#include <vips/internal.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static int made_ucs_tables = 0;

static float LI[1001];
static float CI[3001];
static float hI[101][361];

void im_col_ab2Ch(float a, float b, float *C, float *h) {
    float in[3], out[3];

    in[1] = a;
    in[2] = b;
    imb_Lab2LCh(in, out, 1);
    *C = out[1];
    *h = out[2];
}

void im_col_Ch2ab(float C, float h, float *a, float *b) {
    float in[3], out[3];

    in[1] = C;
    in[2] = h;
    imb_LCh2Lab(in, out, 1);
    *a = out[1];
    *b = out[2];
}

void im_col_XYZ2Lab(float X, float Y, float Z, float *L, float *a, float *b) {
    float in[3], out[3];
    im_colour_temperature temp;

    in[0] = X;
    in[1] = Y;
    in[2] = Z;
    temp.X0 = IM_D65_X0;
    temp.Y0 = IM_D65_Y0;
    temp.Z0 = IM_D65_Z0;
    imb_XYZ2Lab(in, out, 1, &temp);
    *L = out[0];
    *a = out[1];
    *b = out[2];
}

void im_col_Lab2XYZ(float L, float a, float b, float *X, float *Y, float *Z) {
    float in[3], out[3];
    im_colour_temperature temp;

    in[0] = L;
    in[1] = a;
    in[2] = b;
    temp.X0 = IM_D65_X0;
    temp.Y0 = IM_D65_Y0;
    temp.Z0 = IM_D65_Z0;
    imb_Lab2XYZ(in, out, 1, &temp);
    *X = out[0];
    *Y = out[1];
    *Z = out[2];
}

float im_col_pythagoras(float L1, float a1, float b1, float L2, float a2, float b2) {
    float dL = L1 - L2;
    float da = a1 - a2;
    float db = b1 - b2;

    return (sqrt(dL * dL + da * da + db * db));
}

#define c1 21.75
#define c2 0.3838
#define c3 38.54

float im_col_L2Lucs(float L) {
    float Lucs;

    if (L >= 16.0) {
        Lucs = (c1 * log(L) + c2 * L - c3);
    } else {
        Lucs = 1.744 * L;
    }

    return (Lucs);
}

static void make_LI(void) {
    int i, j = 0;
    float L, Ll[1001];

    for (i = 0; i < 1001; i++) {
        L = i / 10.0;
        if (L >= 16.0) {
            Ll[i] = (c1 * log(L) + c2 * L - c3);
        } else {
            Ll[i] = 1.744 * L;
        }
    }

    for (i = 0; i < 1001; i++) {
        while ((Ll[j] <= i / 10.0) && (j < 1001)) {
            j++;
        }
        LI[i] = (j - 1) / 10.0 + (i / 10.0 - Ll[j - 1]) / ((Ll[j] - Ll[j - 1]) * 10.0);
    }
}

float im_col_Lucs2L(float Lucs) {
    int known;

    known = floor(Lucs * 10.0);
    if (known < 0) {
        known = 0;
    }
    if (known > 1000) {
        known = 1000;
    }

    return (LI[known] + (LI[known + 1] - LI[known]) * (Lucs * 10.0 - known));
}

#define c4 0.162
#define c5 10.92
#define c6 0.638
#define c7 0.07216
#define c8 4.907

float im_col_C2Cucs(float C) {
    float Cucs;

    Cucs = (c4 * C + c5 * (log(c6 + c7 * C)) + c8);
    if (Cucs < 0) {
        Cucs = 0;
    }

    return (Cucs);
}

static void make_CI(void) {
    int i;
    float C;
    float Cl[3001];

    for (i = 0; i < 3001; i++) {
        C = i / 10.0;
        Cl[i] = (c4 * C + c5 * (log(c6 + c7 * C)) + c8);
    }

    for (i = 0; i < 3001; i++) {
        int j;

        for (j = 0; j < 3001 && Cl[j] <= i / 10.0; j++) {
            ;
        }
        CI[i] = (j - 1) / 10.0 + (i / 10.0 - Cl[j - 1]) / ((Cl[j] - Cl[j - 1]) * 10.0);
    }
}

float im_col_Cucs2C(float Cucs) {
    int known;

    known = floor(Cucs * 10.0);
    if (known < 0) {
        known = 0;
    }
    if (known > 3000) {
        known = 3000;
    }

    return (CI[known] + (CI[known + 1] - CI[known]) * (Cucs * 10.0 - known));
}

float im_col_Ch2hucs(float C, float h) {
    float P, D, f, g;
    float k4, k5, k6, k7, k8;
    float hucs;

    if (h < 49.1) {
        k4 = 133.87;
        k5 = -134.5;
        k6 = -.924;
        k7 = 1.727;
        k8 = 340.0;
    } else if (h < 110.1) {
        k4 = 11.78;
        k5 = -12.7;
        k6 = -.218;
        k7 = 2.12;
        k8 = 333.0;
    } else if (h < 269.6) {
        k4 = 13.87;
        k5 = 10.93;
        k6 = 0.14;
        k7 = 1.0;
        k8 = -83.0;
    } else {
        k4 = .14;
        k5 = 5.23;
        k6 = .17;
        k7 = 1.61;
        k8 = 233.0;
    }

    P = cos(IM_RAD(k8 + k7 * h));
    D = k4 + k5 * P * pow(fabs(P), k6);
    g = C * C * C * C;
    f = sqrt(g / (g + 1900.0));
    hucs = h + D * f;

    return (hucs);
}

static void make_hI(void) {
    int i, j, k;
    float P, D, C, f, hl[101][361];
    float k4, k5, k6, k7, k8;

    for (i = 0; i < 361; i++) {
        if (i < 49.1) {
            k4 = 133.87;
            k5 = -134.5;
            k6 = -.924;
            k7 = 1.727;
            k8 = 340.0;
        } else if (i < 110.1) {
            k4 = 11.78;
            k5 = -12.7;
            k6 = -.218;
            k7 = 2.12;
            k8 = 333.0;
        } else if (i < 269.6) {
            k4 = 13.87;
            k5 = 10.93;
            k6 = 0.14;
            k7 = 1.0;
            k8 = -83.0;
        } else {
            k4 = .14;
            k5 = 5.23;
            k6 = .17;
            k7 = 1.61;
            k8 = 233.0;
        }

        P = cos(IM_RAD(k8 + k7 * i));
        D = k4 + k5 * P * pow(fabs(P), k6);

        for (j = 0; j < 101; j++) {
            float g;

            C = j * 2.0;
            g = C * C * C * C;
            f = sqrt(g / (g + 1900.0));

            hl[j][i] = i + D * f;
        }
    }

    for (j = 0; j < 101; j++) {
        k = 0;
        for (i = 0; i < 361; i++) {
            while (k < 361 && hl[j][k] <= i) {
                k++;
            }
            hI[j][i] = k - 1 + (i - hl[j][k - 1]) / (hl[j][k] - hl[j][k - 1]);
        }
    }
}

float im_col_Chucs2h(float C, float hucs) {
    int r, known;

    r = (int)((C + 1.0) / 2.0);
    if (r < 0) {
        r = 0;
    }
    if (r > 100) {
        r = 100;
    }

    known = floor(hucs);
    if (known < 0) {
        known = 0;
    }
    if (known > 360) {
        known = 360;
    }

    return (hI[r][known] + (hI[r][(known + 1) % 360] - hI[r][known]) * (hucs - known));
}

void im_col_make_tables_UCS(void) {
    if (!made_ucs_tables) {
        make_LI();
        make_CI();
        make_hI();
        made_ucs_tables = -1;
    }
}

float im_col_dECMC(float L1, float a1, float b1, float L2, float a2, float b2) {
    float h1, C1;
    float h2, C2;
    float Lucs1, Cucs1, hucs1;
    float Lucs2, Cucs2, hucs2;
    float aucs1, bucs1;
    float aucs2, bucs2;

    im_col_ab2Ch(a1, b1, &C1, &h1);
    im_col_ab2Ch(a2, b2, &C2, &h2);

    Lucs1 = im_col_L2Lucs(L1);
    Cucs1 = im_col_C2Cucs(C1);
    hucs1 = im_col_Ch2hucs(C1, h1);

    Lucs2 = im_col_L2Lucs(L2);
    Cucs2 = im_col_C2Cucs(C2);
    hucs2 = im_col_Ch2hucs(C2, h2);

    im_col_Ch2ab(Cucs1, hucs1, &aucs1, &bucs1);
    im_col_Ch2ab(Cucs2, hucs2, &aucs2, &bucs2);

    return (im_col_pythagoras(Lucs1, aucs1, bucs1, Lucs2, aucs2, bucs2));
}

double im_col_ab2h(double a, double b) {
    double h;

    if (a == 0) {
        if (b < 0.0) {
            h = 270;
        } else if (b == 0.0) {
            h = 0;
        } else {
            h = 90;
        }
    } else {
        double t = atan(b / a);

        if (a > 0.0) {
            if (b < 0.0) {
                h = IM_DEG(t + IM_PI * 2.0);
            } else {
                h = IM_DEG(t);
            }
        } else {
            h = IM_DEG(t + IM_PI);
        }
    }

    return (h);
}

float im_col_dE00(float L1, float a1, float b1, float L2, float a2, float b2) {

    double C1 = sqrt(a1 * a1 + b1 * b1);
    double C2 = sqrt(a2 * a2 + b2 * b2);
    double Cb = (C1 + C2) / 2;

    double Cb7 = Cb * Cb * Cb * Cb * Cb * Cb * Cb;
    double G = 0.5 * (1 - sqrt(Cb7 / (Cb7 + pow(25, 7))));

    double L1d = L1;
    double a1d = (1 + G) * a1;
    double b1d = b1;
    double C1d = sqrt(a1d * a1d + b1d * b1d);
    double h1d = im_col_ab2h(a1d, b1d);

    double L2d = L2;
    double a2d = (1 + G) * a2;
    double b2d = b2;
    double C2d = sqrt(a2d * a2d + b2d * b2d);
    double h2d = im_col_ab2h(a2d, b2d);

    double Ldb = (L1d + L2d) / 2;
    double Cdb = (C1d + C2d) / 2;
    double hdb = fabs(h1d - h2d) < 180 ? (h1d + h2d) / 2 : fabs(h1d + h2d - 360) / 2;

    double hdbd = (hdb - 275) / 25;
    double dtheta = 30 * exp(-(hdbd * hdbd));
    double Cdb7 = Cdb * Cdb * Cdb * Cdb * Cdb * Cdb * Cdb;
    double RC = 2 * sqrt(Cdb7 / (Cdb7 + pow(25, 7)));

    double RT = -sin(IM_RAD(2 * dtheta)) * RC;
    double T = 1 - 0.17 * cos(IM_RAD(hdb - 30)) + 0.24 * cos(IM_RAD(2 * hdb)) + 0.32 * cos(IM_RAD(3 * hdb + 6)) - 0.20 * cos(IM_RAD(4 * hdb - 63));

    double Ldb50 = Ldb - 50;
    double SL = 1 + (0.015 * Ldb50 * Ldb50) / sqrt(20 + Ldb50 * Ldb50);
    double SC = 1 + 0.045 * Cdb;
    double SH = 1 + 0.015 * Cdb * T;

    double dhd = fabs(h1d - h2d) < 180 ? h1d - h2d : 360 - (h1d - h2d);

    double dLd = L1d - L2d;
    double dCd = C1d - C2d;
    double dHd = 2 * sqrt(C1d * C2d) * sin(IM_RAD(dhd / 2));

    const double kL = 1.0;
    const double kC = 1.0;
    const double kH = 1.0;

    double nL = dLd / (kL * SL);
    double nC = dCd / (kC * SC);
    double nH = dHd / (kH * SH);

    double dE00 = sqrt(nL * nL + nC * nC + nH * nH + RT * nC * nH);

    return (dE00);
}
