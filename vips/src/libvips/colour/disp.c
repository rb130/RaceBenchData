

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

static GHashTable *im__col_display_tables = NULL;

static struct im_col_display srgb_profile = {"sRGB", DISP_DUMB, {{3.2410, -1.5374, -0.4986}, {-0.9692, 1.8760, 0.0416}, {0.0556, -0.2040, 1.0570}},

    80.0, .3127, .3291, 100, 100, 100, 255, 255, 255, 1, 1, 1, 2.4, 2.4, 2.4, 100, 100};

static struct im_col_display ultra2 = {"ultra2-20/2/98", DISP_DUMB, {{.704, -0.302, -.103}, {-.708, 1.317, .032}, {.005, -.015, .071}},

    64.0, .2137, .3291, 14.4, 44.0, 5.4, 255, 255, 255, 0.03, 0.03, 0.03, 2.5, 2.5, 2.4, 100, 100};

static struct im_col_display im_col_screen_white = {"Screen", DISP_DUMB, {{.660, -0.276, -.10}, {-.663, 1.293, .0265}, {.003, -.017, .0734}},

    58.7, .284, .273, 14.2, 38.4, 6.1, 255, 255, 255, 0.0, 0.0, 0.0, 2.8, 2.9, 2.9, 100, 100};

static struct im_col_display im_col_SPARC_white = {"SPARC", DISP_DUMB, {{.660, -0.276, -.10}, {-.663, 1.293, .0265}, {.003, -.017, .0734}},

    58.7, .284, .273, 14.2, 38.4, 4, 255, 255, 255, 0.0, 0.0, 0.0, 2.0, 2.0, 2.0, 100, 100};

static struct im_col_display im_col_D65_white = {"D65", DISP_DUMB, {{.660, -0.276, -.10}, {-.663, 1.293, .0265}, {.003, -.017, .0734}},

    49.9, .3127, .3290, 11.6, 35.0, 3.3, 241, 255, 177, 0.1, 0.1, 0.1, 2.8, 2.9, 2.7, 100, 100};

static struct im_col_display im_col_barco_white = {"Barco", DISP_DUMB, {{.749, -0.322, -.123}, {-.755, 1.341, .033}, {.007, -.019, .0898}},

    80.0, .3128, .3292, 20.45, 52.73, 6.81, 255, 255, 255, 0.02, 0.053, 0.007, 2.23, 2.13, 2.12, 100, 100};

static struct im_col_display im_col_mitsubishi = {"Mitsubishi_3_colour", DISP_DUMB, {{1.1997, -0.6296, -0.2755}, {-1.1529, 1.7383, -0.1074}, {-0.047, -0.109, 0.3829}},

    95, .3152, .3316, 25.33, 42.57, 15.85, 255, 255, 255, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 100, 100};

static struct im_col_display im_col_relative = {"relative", DISP_DUMB, {{.660, -0.276, -.10}, {-.663, 1.293, .0265}, {.003, -.017, .0734}},

    100.0, .284, .273, 24.23, 69.20, 6.57, 255, 255, 255, 0.0, 0.0, 0.0, 2.3, 2.3, 2.3, 100, 100};

struct im_col_display *im_col_displays(int n) {
    static struct im_col_display *displays[] = {&im_col_screen_white, &im_col_SPARC_white, &im_col_D65_white, &im_col_barco_white, &im_col_mitsubishi, &im_col_relative, &ultra2, &srgb_profile, NULL};

    if (n < 0 || n > IM_NUMBER(displays)) {
        return (NULL);
    }

    return (displays[n]);
}

struct im_col_display *im_col_display_name(const char *name) {
    int i;
    struct im_col_display *d;

    for (i = 0; (d = im_col_displays(i)); i++) {
        if (g_ascii_strcasecmp(d->d_name, name) == 0) {
            return (d);
        }
    }

    return (NULL);
}

static void calcul_tables(struct im_col_display *d, struct im_col_tab_disp *table) {
    int i;
    float a, ga_i, ga, c, f, yo, p;
    float maxr, maxg, maxb;

    c = (d->d_B - 100.0) / 500.0;

    yo = d->d_Y0R;
    a = d->d_YCR - yo;
    ga = d->d_gammaR;
    ga_i = 1.0 / ga;
    p = d->d_P / 100.0;
    f = d->d_Vrwr / p;

    maxr = (float)d->d_Vrwr;
    table->ristep = maxr / 1500.0;
    table->rstep = a / 1500.0;

    for (i = 0; i < 1501; i++) {
        table->t_Yr2r[i] = f * (pow(i * table->rstep / a, ga_i) - c);
    }

    for (i = 0; i < 1501; i++) {
        table->t_r2Yr[i] = yo + a * pow(i * table->ristep / f + c, ga);
    }

    yo = d->d_Y0G;
    a = d->d_YCG - yo;
    ga = d->d_gammaG;
    ga_i = 1.0 / ga;
    p = d->d_P / 100.0;
    f = d->d_Vrwg / p;

    maxg = (float)d->d_Vrwg;
    table->gistep = maxg / 1500.0;
    table->gstep = a / 1500.0;

    for (i = 0; i < 1501; i++) {
        table->t_Yg2g[i] = f * (pow(i * table->gstep / a, ga_i) - c);
    }

    for (i = 0; i < 1501; i++) {
        table->t_g2Yg[i] = yo + a * pow(i * table->gistep / f + c, ga);
    }

    yo = d->d_Y0B;
    a = d->d_YCB - yo;
    ga = d->d_gammaB;
    ga_i = 1.0 / ga;
    p = d->d_P / 100.0;
    f = d->d_Vrwb / p;

    maxb = (float)d->d_Vrwb;
    table->bistep = maxb / 1500.0;
    table->bstep = a / 1500.0;

    for (i = 0; i < 1501; i++) {
        table->t_Yb2b[i] = f * (pow(i * table->bstep / a, ga_i) - c);
    }

    for (i = 0; i < 1501; i++) {
        table->t_b2Yb[i] = yo + a * pow(i * table->bistep / f + c, ga);
    }
}

struct im_col_tab_disp *im_col_make_tables_RGB(IMAGE *im, struct im_col_display *d) {
    struct im_col_tab_disp *table;
    double **temp;
    int i, j;

    if (!(table = IM_NEW(im, struct im_col_tab_disp))) {
        return (NULL);
    }

    if (d->d_type == DISP_DUMB) {
        calcul_tables(d, table);
    }

    if (!(temp = im_dmat_alloc(0, 2, 0, 2))) {
        return (NULL);
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            table->mat_XYZ2lum[i][j] = d->d_mat[i][j];
            temp[i][j] = d->d_mat[i][j];
        }
    }

    if (im_invmat(temp, 3)) {
        im_free_dmat(temp, 0, 2, 0, 2);
        return (NULL);
    }

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            table->mat_lum2XYZ[i][j] = temp[i][j];
        }
    }

    im_free_dmat(temp, 0, 2, 0, 2);

    return (table);
}

struct im_col_tab_disp *im_col_display_get_table(struct im_col_display *d) {
    struct im_col_tab_disp *table;

    if (!im__col_display_tables) {
        im__col_display_tables = g_hash_table_new(g_str_hash, g_str_equal);
    }

    if (!(table = g_hash_table_lookup(im__col_display_tables, d->d_name))) {
        table = im_col_make_tables_RGB(NULL, d);
        g_hash_table_insert(im__col_display_tables, d->d_name, table);
    }

    return (table);
}

int im_col_rgb2XYZ(struct im_col_display *d, int r, int g, int b, float *X, float *Y, float *Z) {
    struct im_col_tab_disp *table = im_col_display_get_table(d);
    float *mat = &table->mat_lum2XYZ[0][0];

    float Yr, Yg, Yb;
    int i;

    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
        im_error("im_col_rgb2XYZ", "%s", _("out of range [0,255]"));
        return (-1);
    }

    switch (d->d_type) {
    case DISP_DUMB:

        i = r / table->ristep;
        Yr = table->t_r2Yr[i];

        i = g / table->gistep;
        Yg = table->t_g2Yg[i];

        i = b / table->bistep;
        Yb = table->t_b2Yb[i];

        break;

    case DISP_BARCO:
        Yr = d->d_Y0R + r * (d->d_YCR - d->d_Y0R) / 255.0;
        Yg = d->d_Y0G + g * (d->d_YCG - d->d_Y0G) / 255.0;
        Yb = d->d_Y0B + b * (d->d_YCB - d->d_Y0B) / 255.0;
        break;

    default:
        im_error("im_col_rgb2XYZ", "%s", _("bad display type"));
        return (-1);
    }

    *X = mat[0] * Yr + mat[1] * Yg + mat[2] * Yb;
    *Y = mat[3] * Yr + mat[4] * Yg + mat[5] * Yb;
    *Z = mat[6] * Yr + mat[7] * Yg + mat[8] * Yb;

    return (0);
}

int im_col_XYZ2rgb(struct im_col_display *d, float X, float Y, float Z, int *r_ret, int *g_ret, int *b_ret, int *or_ret) {
    struct im_col_tab_disp *table = im_col_display_get_table(d);
    float *mat = &table->mat_XYZ2lum[0][0];

    int or = 0;

    float Yr, Yg, Yb;
    int Yint;
    int r, g, b;

    Yr = mat[0] * X + mat[1] * Y + mat[2] * Z;
    Yg = mat[3] * X + mat[4] * Y + mat[5] * Z;
    Yb = mat[6] * X + mat[7] * Y + mat[8] * Z;

    if (Yr < d->d_Y0R) {
        or = 1;
        Yr = d->d_Y0R;
    }
    if (Yg < d->d_Y0G) {
        or = 1;
        Yg = d->d_Y0G;
    }
    if (Yb < d->d_Y0B) {
        or = 1;
        Yb = d->d_Y0B;
    }

    switch (d->d_type) {
    case DISP_DUMB:
        Yint = (Yr - d->d_Y0R) / table->rstep;
        if (Yint > 1500) {
            or = 1;
            Yint = 1500;
        }
        r = IM_RINT(table->t_Yr2r[Yint]);

        Yint = (Yg - d->d_Y0G) / table->gstep;
        if (Yint > 1500) {
            or = 1;
            Yint = 1500;
        }
        g = IM_RINT(table->t_Yg2g[Yint]);

        Yint = (Yb - d->d_Y0B) / table->bstep;
        if (Yint > 1500) {
            or = 1;
            Yint = 1500;
        }
        b = IM_RINT(table->t_Yb2b[Yint]);

        break;

    case DISP_BARCO:
        r = IM_RINT(((Yr - d->d_Y0R) / (d->d_YCR - d->d_Y0R)) * 255);
        g = IM_RINT(((Yg - d->d_Y0G) / (d->d_YCG - d->d_Y0G)) * 255);
        b = IM_RINT(((Yb - d->d_Y0B) / (d->d_YCB - d->d_Y0B)) * 255);

        if (r > d->d_Vrwr) {
            or = 1;
            r = d->d_Vrwr;
        }
        if (g > d->d_Vrwg) {
            or = 1;
            g = d->d_Vrwg;
        }
        if (b > d->d_Vrwb) {
            or = 1;
            b = d->d_Vrwb;
        }
        if (r < 0) {
            or = 1;
            r = 0;
        }
        if (g < 0) {
            or = 1;
            g = 0;
        }
        if (b < 0) {
            or = 1;
            b = 0;
        }

        break;

    default:
        im_error("XYZ2rgb", "%s", _("display unknown"));
        return (-1);
    }

    *r_ret = r;
    *g_ret = g;
    *b_ret = b;

    *or_ret = or ;

    return (0);
}

int im_Lab2disp(IMAGE *in, IMAGE *out, struct im_col_display *disp) {
    IMAGE *t[1];

    if (im_open_local_array(out, t, 1, "im_Lab2disp:1", "p") || im_Lab2XYZ(in, t[0]) || im_XYZ2disp(t[0], out, disp)) {
        return (-1);
    }

    return (0);
}

int im_dECMC_fromdisp(IMAGE *im1, IMAGE *im2, IMAGE *out, struct im_col_display *d) {
    IMAGE *t[4];

    if (im_open_local_array(out, t, 4, "im_dECMC_fromdisp:1", "p") || im_disp2XYZ(im1, t[0], d) || im_XYZ2Lab(t[0], t[1]) || im_disp2XYZ(im2, t[2], d) || im_XYZ2Lab(t[2], t[3]) || im_dECMC_fromLab(t[1], t[3], out)) {
        return (-1);
    }

    return (0);
}

int im_dE_fromdisp(IMAGE *im1, IMAGE *im2, IMAGE *out, struct im_col_display *d) {
    IMAGE *t[2];

    if (im_open_local_array(out, t, 2, "im_dE_fromdisp:1", "p") || im_disp2XYZ(im1, t[0], d) || im_disp2XYZ(im2, t[1], d) || im_dE_fromXYZ(t[0], t[1], out)) {
        return (-1);
    }

    return (0);
}

int im_disp2Lab(IMAGE *in, IMAGE *out, struct im_col_display *d) {
    IMAGE *t[1];

    if (im_open_local_array(out, t, 1, "im_disp2Lab:1", "p") || im_disp2XYZ(in, t[0], d) || im_XYZ2Lab(t[0], out)) {
        return (-1);
    }

    return (0);
}
