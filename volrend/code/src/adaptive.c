#ifdef __cplusplus
extern "C"
#endif
void racebench_bug3(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug8(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug8(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug11(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug12(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug15(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug15(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug17(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug17(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug23(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug23(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug27(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug27(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug30(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug30(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug43(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug46(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug46(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug52(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug52(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug55(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug59(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug59(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug62(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug62(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug63(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug63(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug65(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug67(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug67(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug69(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug74(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug76(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug80(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug80(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug82(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug88(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug92(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug92(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug94(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug97(int);
#ifdef __cplusplus
extern "C"
#endif
void racebench_bug97(int);

#include "incl.h"

float invjacobian[NM][NM];

float invinvjacobian[NM][NM];

int num_rays_traced;
int num_traced_rays_hit_volume;
int num_samples_trilirped;
int itest;

#define RAY_TRACED ((MAX_PIXEL + 1) / 2)
#define START_RAY 1
#define INTERPOLATED ((MAX_PIXEL + 1) / 32)

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "anl.h"

Ray_Trace(int my_node) {
    int outx, outy, outz;
    int i, j;
    unsigned long starttime, stoptime, exectime, exectime1;
    int pid;
    char cmd[FILENAME_STRING_SIZE];

    for (i = 0; i < NM; i++) {
        invjacobian[X][i] = uout_invvertex[0][0][1][i] - uout_invvertex[0][0][0][i]; racebench_bug8(0); racebench_bug23(0); racebench_bug30(0); racebench_bug63(1); racebench_bug80(1);
        invjacobian[Y][i] = uout_invvertex[0][1][0][i] - uout_invvertex[0][0][0][i]; racebench_bug8(1); racebench_bug23(1); racebench_bug76(0);
        invjacobian[Z][i] = uout_invvertex[1][0][0][i] - uout_invvertex[0][0][0][i]; racebench_bug30(1); racebench_bug63(0); racebench_bug80(0);
    }

    for (i = 0; i < NM; i++) {
        for (j = 0; j < NM; j++) {
            if (ABS(invjacobian[i][j]) > SMALL) {
                invinvjacobian[i][j] = 1.0 / invjacobian[i][j]; racebench_bug11(0);
            }
        }
    }

    num_rays_traced = 0; racebench_bug15(0); racebench_bug15(1);
    num_traced_rays_hit_volume = 0;
    num_samples_trilirped = 0;

    if (adaptive) {

        {
            unsigned long Error, Cycle;
            int Cancel, Temp;

            Error = pthread_mutex_lock(&(Global->TimeBarrier).mutex);
            if (Error != 0) {
                printf("Error while trying to get lock in barrier.\n");
                exit(-1);
            }

            Cycle = (Global->TimeBarrier).cycle;
            if (++(Global->TimeBarrier).counter != (num_nodes)) {
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                while (Cycle == (Global->TimeBarrier).cycle) {
                    Error = pthread_cond_wait(&(Global->TimeBarrier).cv, &(Global->TimeBarrier).mutex);
                    if (Error != 0) {
                        break;
                    }
                }
                pthread_setcancelstate(Cancel, &Temp);
            } else {
                (Global->TimeBarrier).cycle = !(Global->TimeBarrier).cycle;
                (Global->TimeBarrier).counter = 0;
                Error = pthread_cond_broadcast(&(Global->TimeBarrier).cv);
            }
            pthread_mutex_unlock(&(Global->TimeBarrier).mutex);
        };

        {
            struct timeval FullTime;

            gettimeofday(&FullTime, NULL);
            (starttime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
        };
        Pre_Shade(my_node);

        { pthread_mutex_lock(&(Global->CountLock)); };
        Global->Counter--;
        { pthread_mutex_unlock(&(Global->CountLock)); };
        while (Global->Counter) {
            ;
        }

        Ray_Trace_Adaptively(my_node);

        {
            struct timeval FullTime;

            gettimeofday(&FullTime, NULL);
            (stoptime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
        };

        {
            unsigned long Error, Cycle;
            int Cancel, Temp;

            Error = pthread_mutex_lock(&(Global->TimeBarrier).mutex);
            if (Error != 0) {
                printf("Error while trying to get lock in barrier.\n");
                exit(-1);
            }

            Cycle = (Global->TimeBarrier).cycle;
            if (++(Global->TimeBarrier).counter != (num_nodes)) {
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                while (Cycle == (Global->TimeBarrier).cycle) {
                    Error = pthread_cond_wait(&(Global->TimeBarrier).cv, &(Global->TimeBarrier).mutex);
                    if (Error != 0) {
                        break;
                    }
                }
                pthread_setcancelstate(Cancel, &Temp);
            } else {
                (Global->TimeBarrier).cycle = !(Global->TimeBarrier).cycle;
                (Global->TimeBarrier).counter = 0;
                Error = pthread_cond_broadcast(&(Global->TimeBarrier).cv);
            }
            pthread_mutex_unlock(&(Global->TimeBarrier).mutex);
        };

        mclock(stoptime, starttime, &exectime);

        if (highest_sampling_boxlen > 1) {

            {
                unsigned long Error, Cycle;
                int Cancel, Temp;

                Error = pthread_mutex_lock(&(Global->TimeBarrier).mutex);
                if (Error != 0) {
                    printf("Error while trying to get lock in barrier.\n");
                    exit(-1);
                }

                Cycle = (Global->TimeBarrier).cycle;
                if (++(Global->TimeBarrier).counter != (num_nodes)) {
                    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                    while (Cycle == (Global->TimeBarrier).cycle) {
                        Error = pthread_cond_wait(&(Global->TimeBarrier).cv, &(Global->TimeBarrier).mutex);
                        if (Error != 0) {
                            break;
                        }
                    }
                    pthread_setcancelstate(Cancel, &Temp);
                } else {
                    (Global->TimeBarrier).cycle = !(Global->TimeBarrier).cycle;
                    (Global->TimeBarrier).counter = 0;
                    Error = pthread_cond_broadcast(&(Global->TimeBarrier).cv);
                }
                pthread_mutex_unlock(&(Global->TimeBarrier).mutex);
            };

            {
                struct timeval FullTime;

                gettimeofday(&FullTime, NULL);
                (starttime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
            };
            Interpolate_Recursively(my_node);

            {
                struct timeval FullTime;

                gettimeofday(&FullTime, NULL);
                (stoptime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
            };

            {
                unsigned long Error, Cycle;
                int Cancel, Temp;

                Error = pthread_mutex_lock(&(Global->TimeBarrier).mutex);
                if (Error != 0) {
                    printf("Error while trying to get lock in barrier.\n");
                    exit(-1);
                }

                Cycle = (Global->TimeBarrier).cycle;
                if (++(Global->TimeBarrier).counter != (num_nodes)) {
                    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                    while (Cycle == (Global->TimeBarrier).cycle) {
                        Error = pthread_cond_wait(&(Global->TimeBarrier).cv, &(Global->TimeBarrier).mutex);
                        if (Error != 0) {
                            break;
                        }
                    }
                    pthread_setcancelstate(Cancel, &Temp);
                } else {
                    (Global->TimeBarrier).cycle = !(Global->TimeBarrier).cycle;
                    (Global->TimeBarrier).counter = 0;
                    Error = pthread_cond_broadcast(&(Global->TimeBarrier).cv);
                }
                pthread_mutex_unlock(&(Global->TimeBarrier).mutex);
            };

            mclock(stoptime, starttime, &exectime1);
        }
    } else {

        {
            unsigned long Error, Cycle;
            int Cancel, Temp;

            Error = pthread_mutex_lock(&(Global->TimeBarrier).mutex);
            if (Error != 0) {
                printf("Error while trying to get lock in barrier.\n");
                exit(-1);
            }

            Cycle = (Global->TimeBarrier).cycle; racebench_bug65(0);
            if (++(Global->TimeBarrier).counter != (num_nodes)) {
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                while (Cycle == (Global->TimeBarrier).cycle) {
                    Error = pthread_cond_wait(&(Global->TimeBarrier).cv, &(Global->TimeBarrier).mutex);
                    if (Error != 0) {
                        break;
                    }
                }
                pthread_setcancelstate(Cancel, &Temp);
            } else {
                (Global->TimeBarrier).cycle = !(Global->TimeBarrier).cycle;
                (Global->TimeBarrier).counter = 0;
                Error = pthread_cond_broadcast(&(Global->TimeBarrier).cv);
            }
            pthread_mutex_unlock(&(Global->TimeBarrier).mutex);
        };

        {
            struct timeval FullTime;

            gettimeofday(&FullTime, NULL);
            (starttime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000); racebench_bug46(0); racebench_bug46(1); racebench_bug59(1);
        };

        Pre_Shade(my_node); racebench_bug59(0);

        { pthread_mutex_lock(&(Global->CountLock)); };
        Global->Counter--; racebench_bug43(1);
        { pthread_mutex_unlock(&(Global->CountLock)); };
        while (Global->Counter) {
            ;
        }

        Ray_Trace_Non_Adaptively(my_node);

        {
            struct timeval FullTime;

            gettimeofday(&FullTime, NULL);
            (stoptime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
        };

        {
            unsigned long Error, Cycle;
            int Cancel, Temp;

            Error = pthread_mutex_lock(&(Global->TimeBarrier).mutex); racebench_bug97(0); racebench_bug97(1);
            if (Error != 0) {
                printf("Error while trying to get lock in barrier.\n");
                exit(-1);
            }

            Cycle = (Global->TimeBarrier).cycle;
            if (++(Global->TimeBarrier).counter != (num_nodes)) {
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                while (Cycle == (Global->TimeBarrier).cycle) {
                    Error = pthread_cond_wait(&(Global->TimeBarrier).cv, &(Global->TimeBarrier).mutex);
                    if (Error != 0) {
                        break;
                    }
                }
                pthread_setcancelstate(Cancel, &Temp);
            } else {
                (Global->TimeBarrier).cycle = !(Global->TimeBarrier).cycle;
                (Global->TimeBarrier).counter = 0;
                Error = pthread_cond_broadcast(&(Global->TimeBarrier).cv);
            }
            pthread_mutex_unlock(&(Global->TimeBarrier).mutex);
        };

        mclock(stoptime, starttime, &exectime);
        exectime1 = 0;
    }

    { pthread_mutex_lock(&(Global->CountLock)); };
    printf("%3d\t%3d\t%6u\t%6u\t%6d\t%6d\t%8d\n", my_node, frame, exectime, exectime1, num_rays_traced, num_traced_rays_hit_volume, num_samples_trilirped);

    { pthread_mutex_unlock(&(Global->CountLock)); };

    {
        unsigned long Error, Cycle;
        int Cancel, Temp;

        Error = pthread_mutex_lock(&(Global->TimeBarrier).mutex);
        if (Error != 0) {
            printf("Error while trying to get lock in barrier.\n");
            exit(-1);
        }

        Cycle = (Global->TimeBarrier).cycle;
        if (++(Global->TimeBarrier).counter != (num_nodes)) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
            while (Cycle == (Global->TimeBarrier).cycle) {
                Error = pthread_cond_wait(&(Global->TimeBarrier).cv, &(Global->TimeBarrier).mutex);
                if (Error != 0) {
                    break;
                }
            }
            pthread_setcancelstate(Cancel, &Temp);
        } else {
            (Global->TimeBarrier).cycle = !(Global->TimeBarrier).cycle;
            (Global->TimeBarrier).counter = 0;
            Error = pthread_cond_broadcast(&(Global->TimeBarrier).cv);
        }
        pthread_mutex_unlock(&(Global->TimeBarrier).mutex);
    };
}

Ray_Trace_Adaptively(int my_node) {
    int i, outx, outy, yindex, xindex;

    int num_xqueue, num_yqueue, num_queue, lnum_xblocks, lnum_yblocks, lnum_blocks;
    int xstart, xstop, ystart, ystop, local_node, work;

    itest = 0;

    num_xqueue = ROUNDUP((float)image_len[X] / (float)image_section[X]);
    num_yqueue = ROUNDUP((float)image_len[Y] / (float)image_section[Y]);
    num_queue = num_xqueue * num_yqueue;
    lnum_xblocks = ROUNDUP((float)num_xqueue / (float)block_xlen);
    lnum_yblocks = ROUNDUP((float)num_yqueue / (float)block_ylen);
    lnum_blocks = lnum_xblocks * lnum_yblocks;
    local_node = my_node;
    Global->Queue[local_node][0] = 0;
    while (Global->Queue[num_nodes][0] > 0) {
        xstart = (local_node % image_section[X]) * num_xqueue;
        xstart = ROUNDUP((float)xstart / (float)highest_sampling_boxlen);
        xstart = xstart * highest_sampling_boxlen;
        xstop = MIN(xstart + num_xqueue, image_len[X]);
        ystart = (local_node / image_section[X]) * num_yqueue;
        ystart = ROUNDUP((float)ystart / (float)highest_sampling_boxlen);
        ystart = ystart * highest_sampling_boxlen;
        ystop = MIN(ystart + num_yqueue, image_len[Y]);
        { pthread_mutex_lock(&Global->QLock[local_node]); };
        work = Global->Queue[local_node][0];
        Global->Queue[local_node][0] += 1;
        { pthread_mutex_unlock(&Global->QLock[local_node]); };
        while (work < lnum_blocks) {
            xindex = xstart + (work % lnum_xblocks) * block_xlen;
            yindex = ystart + (work / lnum_xblocks) * block_ylen;
            for (outy = yindex; outy < yindex + block_ylen && outy < ystop; outy += highest_sampling_boxlen) {
                for (outx = xindex; outx < xindex + block_xlen && outx < xstop; outx += highest_sampling_boxlen) {

                    Ray_Trace_Adaptive_Box(outx, outy, highest_sampling_boxlen);
                }
            }
            { pthread_mutex_lock(&Global->QLock[local_node]); };
            work = Global->Queue[local_node][0];
            Global->Queue[local_node][0] += 1;
            { pthread_mutex_unlock(&Global->QLock[local_node]); };
        }
        if (my_node == local_node) {
            { pthread_mutex_lock(&Global->QLock[num_nodes]); };
            Global->Queue[num_nodes][0]--;
            { pthread_mutex_unlock(&Global->QLock[num_nodes]); };
        }
        local_node = (local_node + 1) % num_nodes;
        while (Global->Queue[local_node][0] >= lnum_blocks && Global->Queue[num_nodes][0] > 0) {
            local_node = (local_node + 1) % num_nodes;
        }
    }
}

Ray_Trace_Adaptive_Box(outx, outy, boxlen) int outx, outy, boxlen;
{
    int i, j;
    int half_boxlen;
    int min_volume_color, max_volume_color;
    float foutx, fouty;
    volatile int imask;

    PIXEL *pixel_address;

    min_volume_color = MAX_PIXEL;
    max_volume_color = MIN_PIXEL;

    for (i = 0; i <= boxlen && outy + i < image_len[Y]; i += boxlen) {
        for (j = 0; j <= boxlen && outx + j < image_len[X]; j += boxlen) {

            if (MASK_IMAGE(outy + i, outx + j) == 0) {

                MASK_IMAGE(outy + i, outx + j) = START_RAY;

                foutx = (float)(outx + j);
                fouty = (float)(outy + i);
                pixel_address = IMAGE_ADDRESS(outy + i, outx + j);

                Trace_Ray(outx + j, outy + i, foutx, fouty, pixel_address);

                MASK_IMAGE(outy + i, outx + j) = RAY_TRACED;
            }
        }
    }
    for (i = 0; i <= boxlen && outy + i < image_len[Y]; i += boxlen) {
        for (j = 0; j <= boxlen && outx + j < image_len[X]; j += boxlen) {
            imask = MASK_IMAGE(outy + i, outx + j);

            while (imask == START_RAY) {

                imask = MASK_IMAGE(outy + i, outx + j);
            }
            min_volume_color = MIN(IMAGE(outy + i, outx + j), min_volume_color);
            max_volume_color = MAX(IMAGE(outy + i, outx + j), max_volume_color);
        }
    }

    if (boxlen > lowest_volume_boxlen && max_volume_color - min_volume_color >= volume_color_difference) {
        half_boxlen = boxlen >> 1;
        for (i = 0; i < boxlen && outy + i < image_len[Y]; i += half_boxlen) {
            for (j = 0; j < boxlen && outx + j < image_len[X]; j += half_boxlen) {
                Ray_Trace_Adaptive_Box(outx + j, outy + i, half_boxlen);
            }
        }
    }
}

Ray_Trace_Non_Adaptively(int my_node) {
    int i, outx, outy, xindex, yindex;
    float foutx, fouty;
    PIXEL *pixel_address;

    int num_xqueue, num_yqueue, num_queue, lnum_xblocks, lnum_yblocks, lnum_blocks;
    int xstart, xstop, ystart, ystop, local_node, work;

    num_xqueue = ROUNDUP((float)image_len[X] / (float)image_section[X]);
    num_yqueue = ROUNDUP((float)image_len[Y] / (float)image_section[Y]);
    num_queue = num_xqueue * num_yqueue;
    lnum_xblocks = ROUNDUP((float)num_xqueue / (float)block_xlen);
    lnum_yblocks = ROUNDUP((float)num_yqueue / (float)block_ylen);
    lnum_blocks = lnum_xblocks * lnum_yblocks;
    local_node = my_node;
    Global->Queue[local_node][0] = 0; racebench_bug52(0);
    while (Global->Queue[num_nodes][0] > 0) {
        xstart = (local_node % image_section[X]) * num_xqueue;
        xstop = MIN(xstart + num_xqueue, image_len[X]); racebench_bug52(1);
        ystart = (local_node / image_section[X]) * num_yqueue;
        ystop = MIN(ystart + num_yqueue, image_len[Y]);
        { pthread_mutex_lock(&Global->QLock[local_node]); };
        work = Global->Queue[local_node][0]++;
        { pthread_mutex_unlock(&Global->QLock[local_node]); };
        while (work < lnum_blocks) {
            xindex = xstart + (work % lnum_xblocks) * block_xlen;
            yindex = ystart + (work / lnum_xblocks) * block_ylen;
            for (outy = yindex; outy < yindex + block_ylen && outy < ystop; outy++) {
                for (outx = xindex; outx < xindex + block_xlen && outx < xstop; outx++) {

                    foutx = (float)(outx); racebench_bug3(0); racebench_bug67(1);
                    fouty = (float)(outy); racebench_bug12(1); racebench_bug17(1); racebench_bug27(0); racebench_bug55(0); racebench_bug62(1); racebench_bug74(1); racebench_bug94(1);
                    pixel_address = IMAGE_ADDRESS(outy, outx); racebench_bug17(0); racebench_bug27(1); racebench_bug82(1); racebench_bug92(0); racebench_bug92(1);
                    Trace_Ray(outx, outy, foutx, fouty, pixel_address); racebench_bug62(0); racebench_bug67(0); racebench_bug69(1); racebench_bug88(1);
                }
            }
            { pthread_mutex_lock(&Global->QLock[local_node]); };
            work = Global->Queue[local_node][0]++;
            { pthread_mutex_unlock(&Global->QLock[local_node]); };
        }
        if (my_node == local_node) {
            { pthread_mutex_lock(&Global->QLock[num_nodes]); };
            Global->Queue[num_nodes][0]--;
            { pthread_mutex_unlock(&Global->QLock[num_nodes]); };
        }
        local_node = (local_node + 1) % num_nodes;
        while (Global->Queue[local_node][0] >= lnum_blocks && Global->Queue[num_nodes][0] > 0) {
            local_node = (local_node + 1) % num_nodes;
        }
    }
}

Ray_Trace_Fast_Non_Adaptively(int my_node) {
    int i, outx, outy, xindex, yindex;
    float foutx, fouty;
    PIXEL *pixel_address;

    for (i = 0; i < num_blocks; i += num_nodes) {
        yindex = ((my_node + i) / num_xblocks) * block_ylen;
        xindex = ((my_node + i) % num_xblocks) * block_xlen;

        for (outy = yindex; outy < yindex + block_ylen && outy < image_len[Y]; outy += lowest_volume_boxlen) {
            for (outx = xindex; outx < xindex + block_xlen && outx < image_len[X]; outx += lowest_volume_boxlen) {

                MASK_IMAGE(outy, outx) += RAY_TRACED;
                foutx = (float)(outx);
                fouty = (float)(outy);
                pixel_address = IMAGE_ADDRESS(outy, outx);
                Trace_Ray(outx, outy, foutx, fouty, pixel_address);
                num_rays_traced++;
            }
        }
    }
}

Interpolate_Recursively(int my_node) {
    int i, outx, outy, xindex, yindex;

    for (i = 0; i < num_blocks; i += num_nodes) {
        yindex = ((my_node + i) / num_xblocks) * block_ylen;
        xindex = ((my_node + i) % num_xblocks) * block_xlen;

        for (outy = yindex; outy < yindex + block_ylen && outy < image_len[Y]; outy += highest_sampling_boxlen) {
            for (outx = xindex; outx < xindex + block_xlen && outx < image_len[X]; outx += highest_sampling_boxlen) {

                Interpolate_Recursive_Box(outx, outy, highest_sampling_boxlen);
            }
        }
    }
}

Interpolate_Recursive_Box(outx, outy, boxlen) int outx, outy, boxlen;
{
    int i, j;
    int half_boxlen;
    int corner_color[2][2], color;
    int outx_plus_boxlen, outy_plus_boxlen;

    float one_over_boxlen;
    float xalpha, yalpha;
    float one_minus_xalpha, one_minus_yalpha;

    half_boxlen = boxlen >> 1;
    one_over_boxlen = 1.0 / (float)boxlen;

    outx_plus_boxlen = outx + boxlen < image_len[X] ? outx + boxlen : outx;
    outy_plus_boxlen = outy + boxlen < image_len[Y] ? outy + boxlen : outy;

    corner_color[0][0] = IMAGE(outy, outx);
    corner_color[0][1] = IMAGE(outy, outx_plus_boxlen);
    corner_color[1][0] = IMAGE(outy_plus_boxlen, outx);
    corner_color[1][1] = IMAGE(outy_plus_boxlen, outx_plus_boxlen);
    for (i = 0; i <= boxlen && outy + i < image_len[Y]; i += half_boxlen) {
        yalpha = (float)i * one_over_boxlen;
        one_minus_yalpha = 1.0 - yalpha;
        for (j = 0; j <= boxlen && outx + j < image_len[X]; j += half_boxlen) {
            xalpha = (float)j * one_over_boxlen;
            one_minus_xalpha = 1.0 - xalpha;
            if (MASK_IMAGE(outy + i, outx + j) == 0) {
                color = corner_color[0][0] * one_minus_xalpha * one_minus_yalpha + corner_color[0][1] * xalpha * one_minus_yalpha + corner_color[1][0] * one_minus_xalpha * yalpha + corner_color[1][1] * xalpha * yalpha;
                IMAGE(outy + i, outx + j) = color;
                MASK_IMAGE(outy + i, outx + j) += INTERPOLATED;
            }
        }
    }

    if (half_boxlen > 1) {
        for (i = 0; i < boxlen && outy + i < image_len[Y]; i += half_boxlen) {
            for (j = 0; j < boxlen && outx + j < image_len[X]; j += half_boxlen) {
                Interpolate_Recursive_Box(outx + j, outy + i, half_boxlen);
            }
        }
    }
}