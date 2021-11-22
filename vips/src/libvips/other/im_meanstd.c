

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

int im__mean_std_double_buffer(double *buffer, int size, double *pmean, double *pstd) {
    double mean, std;
    register int i;
    double sumf;
    double temp;
    double *pbuffer;
    double sumf2;
    double correction;
    double variance;

    if (size <= 0) {
        im_error("im_mean_std_double_buffer", "%s", _("wrong args"));
        return (-1);
    }
    mean = 0.0;
    std = 0.0;
    sumf = 0.0;
    sumf2 = 0.0;
    pbuffer = buffer;
    for (i = 0; i < size; i++) {
        temp = *pbuffer++;
        sumf += temp;
        sumf2 += (temp * temp);
    }

    correction = (sumf * sumf) / ((double)size);
    mean = sumf / ((double)size);
    variance = (sumf2 - correction) / ((double)size);
    std = sqrt(variance);
    *pmean = mean;
    *pstd = std;

    return (0);
}

int im__mean_std_int_buffer(int *buffer, int size, double *pmean, double *pstd) {
    double mean, std;
    register int i;
    int sumf;
    int temp;
    int *pbuffer;
    int sumf2;
    double correction;
    double variance;

    if (size <= 0) {
        im_error("im_mean_std_int_buffer", "%s", _("wrong args"));
        return (-1);
    }

    mean = 0.0;
    std = 0.0;
    sumf = 0;
    sumf2 = 0;
    pbuffer = buffer;
    for (i = 0; i < size; i++) {
        temp = *pbuffer++;
        sumf += temp;
        sumf2 += (temp * temp);
    }

    correction = ((double)(sumf * sumf)) / ((double)size);
    mean = ((double)sumf) / ((double)size);
    variance = (sumf2 - correction) / ((double)size);
    std = sqrt(variance);
    *pmean = mean;
    *pstd = std;

    return (0);
}
