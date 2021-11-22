

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

    double Lb, Lw;
    double Ps, Pm, Ph;
    double S, M, H;

    double Ls, Lm, Lh;
} ToneShape;

static double shad(ToneShape *ts, double x) {
    double x1 = (x - ts->Lb) / (ts->Ls - ts->Lb);
    double x2 = (x - ts->Ls) / (ts->Lm - ts->Ls);
    double out;

    if (x < ts->Lb) {
        out = 0;
    } else if (x < ts->Ls) {
        out = 3.0 * x1 * x1 - 2.0 * x1 * x1 * x1;
    } else if (x < ts->Lm) {
        out = 1.0 - 3.0 * x2 * x2 + 2.0 * x2 * x2 * x2;
    } else {
        out = 0;
    }

    return (out);
}

static double mid(ToneShape *ts, double x) {
    double x1 = (x - ts->Ls) / (ts->Lm - ts->Ls);
    double x2 = (x - ts->Lm) / (ts->Lh - ts->Lm);
    double out;

    if (x < ts->Ls) {
        out = 0;
    } else if (x < ts->Lm) {
        out = 3.0 * x1 * x1 - 2.0 * x1 * x1 * x1;
    } else if (x < ts->Lh) {
        out = 1.0 - 3.0 * x2 * x2 + 2.0 * x2 * x2 * x2;
    } else {
        out = 0;
    }

    return (out);
}

static double high(ToneShape *ts, double x) {
    double x1 = (x - ts->Lm) / (ts->Lh - ts->Lm);
    double x2 = (x - ts->Lh) / (ts->Lw - ts->Lh);
    double out;

    if (x < ts->Lm) {
        out = 0;
    } else if (x < ts->Lh) {
        out = 3.0 * x1 * x1 - 2.0 * x1 * x1 * x1;
    } else if (x < ts->Lw) {
        out = 1.0 - 3.0 * x2 * x2 + 2.0 * x2 * x2 * x2;
    } else {
        out = 0;
    }

    return (out);
}

static double tone_curve(ToneShape *ts, double x) {
    double out;

    out = x + ts->S * shad(ts, x) + ts->M * mid(ts, x) + ts->H * high(ts, x);

    return (out);
}

int im_tone_build_range(IMAGE *out, int in_max, int out_max, double Lb, double Lw, double Ps, double Pm, double Ph, double S, double M, double H) {
    ToneShape *ts;
    unsigned short lut[65536];
    int i;

    if (!(ts = IM_NEW(out, ToneShape)) || im_outcheck(out)) {
        return (-1);
    }
    if (in_max < 0 || in_max > 65535 || out_max < 0 || out_max > 65535) {
        im_error("im_tone_build", "%s", _("bad in_max, out_max parameters"));
        return (-1);
    }
    if (Lb < 0 || Lb > 100 || Lw < 0 || Lw > 100 || Lb > Lw) {
        im_error("im_tone_build", "%s", _("bad Lb, Lw parameters"));
        return (-1);
    }
    if (Ps < 0.0 || Ps > 1.0) {
        im_error("im_tone_build", "%s", _("Ps not in range [0.0,1.0]"));
        return (-1);
    }
    if (Pm < 0.0 || Pm > 1.0) {
        im_error("im_tone_build", "%s", _("Pm not in range [0.0,1.0]"));
        return (-1);
    }
    if (Ph < 0.0 || Ph > 1.0) {
        im_error("im_tone_build", "%s", _("Ph not in range [0.0,1.0]"));
        return (-1);
    }
    if (S < -30 || S > 30) {
        im_error("im_tone_build", "%s", _("S not in range [-30,+30]"));
        return (-1);
    }
    if (M < -30 || M > 30) {
        im_error("im_tone_build", "%s", _("M not in range [-30,+30]"));
        return (-1);
    }
    if (H < -30 || H > 30) {
        im_error("im_tone_build", "%s", _("H not in range [-30,+30]"));
        return (-1);
    }

    ts->Lb = Lb;
    ts->Lw = Lw;
    ts->Ps = Ps;
    ts->Pm = Pm;
    ts->Ph = Ph;
    ts->S = S;
    ts->M = M;
    ts->H = H;

    ts->Ls = Lb + Ps * (Lw - Lb);
    ts->Lm = Lb + Pm * (Lw - Lb);
    ts->Lh = Lb + Ph * (Lw - Lb);

    for (i = 0; i <= in_max; i++) {
        int v = (out_max / 100.0) * tone_curve(ts, 100.0 * i / in_max);

        if (v < 0) {
            v = 0;
        } else if (v > out_max) {
            v = out_max;
        }

        lut[i] = v;
    }

    im_initdesc(out, in_max + 1, 1, 1, IM_BBITS_SHORT, IM_BANDFMT_USHORT, IM_CODING_NONE, IM_TYPE_HISTOGRAM, 1.0, 1.0, 0, 0);
    if (im_setupout(out)) {
        return (-1);
    }

    if (im_writeline(0, out, (PEL *)lut)) {
        return (-1);
    }

    return (0);
}

int im_tone_build(IMAGE *out, double Lb, double Lw, double Ps, double Pm, double Ph, double S, double M, double H) {
    IMAGE *t1;

    if (!(t1 = im_open_local(out, "im_tone_build", "p")) || im_tone_build_range(t1, 32767, 32767, Lb, Lw, Ps, Pm, Ph, S, M, H) || im_clip2fmt(t1, out, IM_BANDFMT_SHORT)) {
        return (-1);
    }

    return (0);
}

int im_ismonotonic(IMAGE *lut, int *out) {
    IMAGE *t[2];
    INTMASK *mask;
    double m;

    if (im_check_hist("im_ismonotonic", lut) || im_open_local_array(lut, t, 2, "im_ismonotonic", "p")) {
        return (-1);
    }

    if (lut->Xsize == 1) {
        mask = im_create_imaskv("im_ismonotonic", 1, 2, -1, 1);
    } else {
        mask = im_create_imaskv("im_ismonotonic", 2, 1, -1, 1);
    }
    if (!(mask = im_local_imask(lut, mask))) {
        return (-1);
    }
    mask->offset = 128;

    if (im_conv(lut, t[0], mask) || im_moreeqconst(t[0], t[1], 128) || im_min(t[1], &m)) {
        return (-1);
    }

    *out = m;

    return (0);
}

int im_tone_map(IMAGE *in, IMAGE *out, IMAGE *lut) {
    IMAGE *t[8];

    if (im_check_hist("im_tone_map", lut) || im_open_local_array(out, t, 8, "im_tone_map", "p")) {
        return (-1);
    }

    if (in->Coding == IM_CODING_LABQ) {
        if (im_LabQ2LabS(in, t[0])) {
            return (-1);
        }
    } else {
        t[0] = in;
    }

    if (im_extract_band(t[0], t[1], 0)) {
        return (-1);
    }
    if (t[0]->Bands > 1) {
        if (im_extract_bands(t[0], t[2], 1, t[0]->Bands - 1)) {
            return (-1);
        }
    }

    if (im_maplut(t[1], t[3], lut)) {
        return (-1);
    }

    if (t[0]->Bands > 1) {
        if (im_bandjoin(t[3], t[2], t[4])) {
            return (-1);
        }
    } else {
        t[4] = t[3];
    }

    if (in->Coding == IM_CODING_LABQ) {
        if (im_LabS2LabQ(t[4], t[5])) {
            return (-1);
        }
    } else {
        t[5] = t[4];
    }

    return (im_copy(t[4], out));
}

int im_tone_analyse(IMAGE *in, IMAGE *out, double Ps, double Pm, double Ph, double S, double M, double H) {
    IMAGE *t[4];
    int low, high;
    double Lb, Lw;

    if (im_open_local_array(out, t, 4, "im_tone_map", "p")) {
        return (-1);
    }

    if (in->Coding == IM_CODING_LABQ) {
        if (im_LabQ2LabS(in, t[0])) {
            return (-1);
        }
    } else {
        t[0] = in;
    }

    if (im_check_uncoded("im_tone_analyse", t[0]) || im_check_bands("im_tone_analyse", t[0], 3) || im_check_format("im_tone_analyse", t[0], IM_BANDFMT_SHORT)) {
        return (-1);
    }

    if (im_extract_band(t[0], t[1], 0) || im_clip2fmt(t[1], t[2], IM_BANDFMT_USHORT) || im_histgr(t[2], t[3], -1)) {
        return (-1);
    }

    if (im_mpercent_hist(t[3], 0.1 / 100.0, &high) || im_mpercent_hist(t[3], 99.9 / 100.0, &low)) {
        return (-1);
    }

    Lb = 100 * low / 32768;
    Lw = 100 * high / 32768;

    im_diag("im_tone_analyse", "set Lb = %g, Lw = %g", Lb, Lw);

    return (im_tone_build(out, Lb, Lw, Ps, Pm, Ph, S, M, H));
}
