
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

static int array[6][6] = {{2, 3, 2, 3, 4, 5}, {3, 3, 3, 3, 5, 5}, {2, 3, 2, 3, 4, 5}, {3, 3, 3, 3, 5, 5}, {4, 5, 4, 5, 4, 5}, {5, 5, 5, 5, 5, 5}};

#define select_tmp2_for_out_int(OUT)                   \
    case IM_BANDFMT_UCHAR:                             \
        select_tmp1_for_out_int(unsigned char, OUT);   \
        break;                                         \
    case IM_BANDFMT_CHAR:                              \
        select_tmp1_for_out_int(signed char, OUT);     \
        break;                                         \
    case IM_BANDFMT_USHORT:                            \
        select_tmp1_for_out_int(unsigned short, OUT);  \
        break;                                         \
    case IM_BANDFMT_SHORT:                             \
        select_tmp1_for_out_int(signed short, OUT);    \
        break;                                         \
    case IM_BANDFMT_UINT:                              \
        select_tmp1_for_out_int(unsigned int, OUT);    \
        break;                                         \
    case IM_BANDFMT_INT:                               \
        select_tmp1_for_out_int(signed int, OUT);      \
        break;                                         \
                                                       \
    default:                                           \
        im_error("im_gaddim", "Wrong tmp2 format(1)"); \
        free(line);                                    \
        return (-1);

#define select_tmp1_for_out_int(IN2, OUT)              \
    switch (tmp1->BandFmt) {                           \
    case IM_BANDFMT_UINT:                              \
        loop(unsigned int, IN2, OUT);                  \
        break;                                         \
    case IM_BANDFMT_INT:                               \
        loop(int, IN2, OUT);                           \
        break;                                         \
    default:                                           \
        im_error("im_gaddim", "Wrong tmp2 format(2)"); \
        free(line);                                    \
        return (-1);                                   \
    }

#define select_tmp2_for_out_short(OUT)                  \
    case IM_BANDFMT_UCHAR:                              \
        select_tmp1_for_out_short(unsigned char, OUT);  \
        break;                                          \
    case IM_BANDFMT_CHAR:                               \
        select_tmp1_for_out_short(signed char, OUT);    \
        break;                                          \
    case IM_BANDFMT_USHORT:                             \
        select_tmp1_for_out_short(unsigned short, OUT); \
        break;                                          \
    case IM_BANDFMT_SHORT:                              \
        select_tmp1_for_out_short(signed short, OUT);   \
        break;                                          \
    default:                                            \
        g_assert(0);
#define select_tmp1_for_out_short(IN2, OUT)              \
    switch (tmp1->BandFmt) {                             \
    case IM_BANDFMT_UCHAR:                               \
        loop(unsigned char, IN2, OUT);                   \
        break;                                           \
    case IM_BANDFMT_CHAR:                                \
        loop(signed char, IN2, OUT);                     \
        break;                                           \
    case IM_BANDFMT_USHORT:                              \
        loop(unsigned short, IN2, OUT);                  \
        break;                                           \
    case IM_BANDFMT_SHORT:                               \
        loop(signed short, IN2, OUT);                    \
        break;                                           \
    default:                                             \
        im_error("im_gaddim", "Wrong image1 format(4)"); \
        free(line);                                      \
        return (-1);                                     \
    }

int im_gaddim(a, in1, b, in2, c, out) IMAGE *in1, *in2, *out;
double a, b, c;
{
    static int fmt[] = {IM_BANDFMT_UCHAR, IM_BANDFMT_CHAR, IM_BANDFMT_USHORT, IM_BANDFMT_SHORT, IM_BANDFMT_UINT, IM_BANDFMT_INT};
    int y, x;
    int first, second, result;
    IMAGE *tmp1, *tmp2;
    PEL *line;
    int os;

    if ((im_iocheck(in1, out) == -1) || (im_iocheck(in2, out) == -1)) {
        return (-1);
    }

    if ((in1->Xsize != in2->Xsize) || (in1->Ysize != in2->Ysize) || (in1->Bands != in2->Bands) || (in1->Coding != in2->Coding)) {
        im_error("im_gaddim", " Input images differ");
        return (-1);
    }
    if (in1->Coding != IM_CODING_NONE) {
        im_error("im_gaddim", " images must be uncoded");
        return (-1);
    }

    switch (in1->BandFmt) {
    case IM_BANDFMT_UCHAR:
        first = 0;
        break;
    case IM_BANDFMT_CHAR:
        first = 1;
        break;
    case IM_BANDFMT_USHORT:
        first = 2;
        break;
    case IM_BANDFMT_SHORT:
        first = 3;
        break;
    case IM_BANDFMT_UINT:
        first = 4;
        break;
    case IM_BANDFMT_INT:
        first = 5;
        break;
    default:
        im_error("im_gaddim", " Unable to accept image1");
        return (-1);
    }
    switch (in2->BandFmt) {
    case IM_BANDFMT_UCHAR:
        second = 0;
        break;
    case IM_BANDFMT_CHAR:
        second = 1;
        break;
    case IM_BANDFMT_USHORT:
        second = 2;
        break;
    case IM_BANDFMT_SHORT:
        second = 3;
        break;
    case IM_BANDFMT_UINT:
        second = 4;
        break;
    case IM_BANDFMT_INT:
        second = 5;
        break;
    default:
        im_error("im_gaddim", " Unable to accept image2");
        return (-1);
    }

    result = array[first][second];

    if (im_cp_desc(out, in1) == -1) {
        im_error("im_gaddim", " im_cp_desc failed");
        return (-1);
    }
    out->BandFmt = fmt[result];

    if (im_setupout(out) == -1) {
        im_error("im_gaddim", " im_setupout failed");
        return (-1);
    }

    if (first >= second) {
        tmp1 = in1;
        tmp2 = in2;
    } else {
        tmp1 = in2;
        tmp2 = in1;
    }

#define loop(IN1, IN2, OUT)                                                     \
    {                                                                           \
        IN1 *input1 = (IN1 *)tmp1->data;                                        \
        IN2 *input2 = (IN2 *)tmp2->data;                                        \
                                                                                \
        for (y = 0; y < out->Ysize; y++) {                                      \
            OUT *cpline = (OUT *)line;                                          \
            for (x = 0; x < os; x++)                                            \
                *cpline++ = (OUT)(a * (*input1++) + b * (*input2++) + c + 0.5); \
            if (im_writeline(y, out, line) == -1) {                             \
                free(line);                                                     \
                return (-1);                                                    \
            }                                                                   \
        }                                                                       \
    }

    os = out->Xsize * out->Bands;
    line = (PEL *)calloc((unsigned)os, sizeof(double));
    if (line == NULL) {
        im_error("im_gaddim", " Unable to calloc");
        return (-1);
    }

    switch (out->BandFmt) {
    case IM_BANDFMT_INT:
        switch (tmp2->BandFmt) { select_tmp2_for_out_int(int); }
        break;

    case IM_BANDFMT_UINT:
        switch (tmp2->BandFmt) { select_tmp2_for_out_int(unsigned int); }
        break;
    case IM_BANDFMT_SHORT:
        switch (tmp2->BandFmt) { select_tmp2_for_out_short(short); }
        break;
    case IM_BANDFMT_USHORT:
        switch (tmp2->BandFmt) { select_tmp2_for_out_short(unsigned short); }
        break;
    default:
        im_error("im_gaddim", " Impossible output state");
        free(line);
        return (-1);
    }

    free(line);

    return (0);
}
