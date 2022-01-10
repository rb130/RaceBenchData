#include "racebench_bugs.h"


#include "incl.h"
#include <sys/time.h>
#include <sys/resource.h>
#include "tiffio.h"

#define SH_MEM_AMT 60000000

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_THREADS 1024
pthread_t PThreadTable[MAX_THREADS];

#include "anl.h"

struct GlobalMemory *Global;

int image_section[NI];
int voxel_section[NM];

int num_nodes, frame;
int num_blocks, num_xblocks, num_yblocks;
PIXEL *image_address;
MPIXEL *mask_image_address;
PIXEL *image_block, *mask_image_block;
PIXEL *shd_address;
BOOLEAN *sbit_address;
long shd_length;
short image_len[NI], mask_image_len[NI];
uint32_t image_length, mask_image_length;
char *filename[FILENAME_STRING_SIZE];
void Render_Loop();

mclock(stoptime, starttime, exectime) unsigned long stoptime, starttime, *exectime;
{
    if (stoptime < starttime) {
        *exectime = ((MAX_INT - starttime) + stoptime) / 1000;
    } else {
        *exectime = (stoptime - starttime) / 1000;
    }
}

const char *input_file;

main(argc, argv) int argc;
char *argv[];
{
    if ((argc < 4) || (strncmp(argv[1], "-h", strlen("-h")) == 0) || (strncmp(argv[1], "-h", strlen("-H")) == 0)) {
        printf("usage:  VOLREND num_processes input_file <RaceBench Input>\n");
        exit(-1);
    }

    { ; };

    num_nodes = atoi(argv[1]);

    strcpy(filename, argv[2]);

    input_file = argv[3];

    Frame();

    if (num_nodes > 1) {
        unsigned long i, Error;
        for (i = 0; i < (num_nodes)-1; i++) {
            Error = pthread_join(PThreadTable[i], NULL);
            if (Error != 0) {
                printf("Error in pthread_join().\n");
                exit(-1);
            }
        }
    };
    { exit(0); };
}

Frame() {
    unsigned long starttime, stoptime, exectime, i;

    Init_Options();

    printf("*****Entering init_decomposition with num_nodes = %d\n", num_nodes);
    fflush(stdout);

    Init_Decomposition();

    printf("*****Exited init_decomposition with num_nodes = %d\n", num_nodes);
    fflush(stdout);

    Global = (struct GlobalMemory *)malloc(sizeof(struct GlobalMemory));
    ;
    {
        unsigned long Error;

        Error = pthread_mutex_init(&(Global->SlaveBarrier).mutex, NULL);
        if (Error != 0) {
            printf("Error while initializing barrier.\n");
            exit(-1);
        }

        Error = pthread_cond_init(&(Global->SlaveBarrier).cv, NULL);
        if (Error != 0) {
            printf("Error while initializing barrier.\n");
            pthread_mutex_destroy(&(Global->SlaveBarrier).mutex);
            exit(-1);
        }

        (Global->SlaveBarrier).counter = 0;
        (Global->SlaveBarrier).cycle = 0;
    };
    {
        unsigned long Error;

        Error = pthread_mutex_init(&(Global->TimeBarrier).mutex, NULL);
        if (Error != 0) {
            printf("Error while initializing barrier.\n");
            exit(-1);
        }

        Error = pthread_cond_init(&(Global->TimeBarrier).cv, NULL);
        if (Error != 0) {
            printf("Error while initializing barrier.\n");
            pthread_mutex_destroy(&(Global->TimeBarrier).mutex);
            exit(-1);
        }

        (Global->TimeBarrier).counter = 0;
        (Global->TimeBarrier).cycle = 0;
    };
    { pthread_mutex_init(&(Global->IndexLock), NULL); };
    { pthread_mutex_init(&(Global->CountLock), NULL); };
    {
        unsigned long i, Error;

        for (i = 0; i < MAX_NUMPROC + 1; i++) {
            Error = pthread_mutex_init(&Global->QLock[i], NULL);
            if (Error != 0) {
                printf("Error while initializing array of locks.\n");
                exit(-1);
            }
        }
    };

#ifndef RENDER_ONLY
    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (starttime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };
    Load_Map(filename);
    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (stoptime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };
    mclock(stoptime, starttime, &exectime);
    printf("wall clock execution time to load map:  %u ms\n", exectime);
#endif

    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (starttime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };
#ifndef RENDER_ONLY
    Compute_Normal();
#ifdef PREPROCESS
    Store_Normal(filename);
#endif
#else
    Load_Normal(filename);
#endif
    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (stoptime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };
    mclock(stoptime, starttime, &exectime);
    printf("wall clock execution time to compute normal:  %u ms\n", exectime);

    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (starttime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };
#ifndef RENDER_ONLY
    Compute_Opacity();
#ifdef PREPROCESS
    Store_Opacity(filename);
#endif
#else
    Load_Opacity(filename);
#endif
    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (stoptime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };
    mclock(stoptime, starttime, &exectime);
    printf("wall clock execution time to compute opacity:  %u ms\n", exectime);

    Compute_Pre_View();
    shd_length = LOOKUP_SIZE;
    Allocate_Shading_Table(&shd_address, &sbit_address, shd_length);

    image_len[X] = frust_len;
    image_len[Y] = frust_len;
    image_length = image_len[X] * image_len[Y];
    Allocate_Image(&image_address, image_length);

    if (num_nodes == 1) {
        block_xlen = image_len[X];
        block_ylen = image_len[Y];
        num_blocks = 1;
        num_xblocks = 1;
        num_yblocks = 1;
        image_block = image_address;
    } else {
        num_xblocks = ROUNDUP((float)image_len[X] / (float)block_xlen);
        num_yblocks = ROUNDUP((float)image_len[Y] / (float)block_ylen);
        num_blocks = num_xblocks * num_yblocks;
        Lallocate_Image(&image_block, block_xlen * block_ylen);
    }

    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (starttime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };
#ifndef RENDER_ONLY
    Compute_Octree();
#ifdef PREPROCESS
    Store_Octree(filename);
#endif
#else
    Load_Octree(filename);
#endif
    {
        struct timeval FullTime;

        gettimeofday(&FullTime, NULL);
        (stoptime) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
    };
    mclock(stoptime, starttime, &exectime);
    printf("wall clock execution time to compute octree:  %u ms\n", exectime);

#ifdef PREPROCESS
    return;
#endif

    if (adaptive) {
        for (i = 0; i < NI; i++) {
            mask_image_len[i] = image_len[i];
        }
        mask_image_length = image_length;
        Allocate_MImage(&mask_image_address, mask_image_length);
        if (num_nodes == 1) {
            mask_image_block = (PIXEL *)mask_image_address;
        } else {
            Lallocate_Image(&mask_image_block, block_xlen * block_ylen);
        }
    }

#ifndef RENDER_ONLY
    Deallocate_Map(&map_address);
#endif

    Global->Index = NODE0;

    printf("\nRendering...\n");
    printf("node\tframe\ttime\titime\trays\thrays\tsamples trilirped\n");

    {
        long i, Error;

        for (i = 0; i < (num_nodes)-1; i++) {
            #ifdef RACEBENCH_BUG_11
            rb_state11.var_1 = (rb_state11.var_1) ^ (5652 < rb_input_size ? (uint32_t)rb_input[5652] : 0xcd5fdd62);
            rb_state11.var_0 = (rb_state11.var_0) - (656 < rb_input_size ? (uint32_t)rb_input[656] : 0x230f12a8);
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Render_Loop), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_11
        rb_state11.var_1 = (rb_state11.var_1) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_2) : (0xfe186317));
        rb_state11.var_0 = (rb_state11.var_0) + ((0x5f7f7e48) ^ (0x320d74fc));
        if ((rb_state11.var_2) == (0x7f)) {
            pthread_mutex_lock(&(rb_state11.lock_16));
            rb_state11.var_5 = 0xfb3ecbf1;
            rb_state11.var_6 = 0x983dae5e;
            rb_state11.var_7 = (rb_state11.var_7) + ((0x257b4d0f) ^ (rb_state11.var_5));
            rb_state11.var_8 = (rb_state11.var_8) ^ (((((0x71423daa) + (rb_state11.var_6)) - (rb_state11.var_6)) + (0x1859abb6)) + (rb_state11.var_4));
            rb_state11.var_9 = (rb_state11.var_9) - (((0xfa2e42af) + (rb_state11.var_7)) ^ (rb_state11.var_5));
            rb_state11.var_10 = (rb_state11.var_10) - (((0x82ab32ef) + (rb_state11.var_8)) ^ (rb_state11.var_7));
            rb_state11.var_11 = (rb_state11.var_11) ^ (((rb_state11.var_9) == (0xdcb2fda4)) ? (rb_state11.var_8) : (0xc5b3015e));
            rb_state11.var_12 = (rb_state11.var_12) + (rb_state11.var_9);
            rb_state11.var_13 = (rb_state11.var_13) + (rb_state11.var_10);
            rb_state11.var_14 = (rb_state11.var_14) - (((((0xa62cea2d) - (rb_state11.var_12)) - (rb_state11.var_11)) + (rb_state11.var_11)) + (rb_state11.var_10));
            rb_state11.var_15 = (rb_state11.var_15) ^ (((rb_state11.var_13) == (0x2dfd654f)) ? (rb_state11.var_14) : (0xf7c2c38f));
            rb_state11.var_3 = (rb_state11.var_3) - (rb_state11.var_15);
            pthread_mutex_unlock(&(rb_state11.lock_16));
        }
        #endif
        Render_Loop();
    }
}

void Render_Loop() {
    int step, i, dim, pid;
    PIXEL *local_image_address;
    MPIXEL *local_mask_image_address;
    PIXEL *local_shd_address;
    char outfile[FILENAME_STRING_SIZE], cmd[FILENAME_STRING_SIZE];
    int image_partition, mask_image_partition, shd_table_partition;
    float inv_num_nodes;
    int my_node;
    int zz;
    int zzz;
    PIXEL *p;

    { pthread_mutex_lock(&(Global->IndexLock)); };
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_0 = (rb_state11.var_0) + ((0xcbae1c8b) - (0xd480df30));
    #endif
    my_node = Global->Index++;
    { pthread_mutex_unlock(&(Global->IndexLock)); };
    #ifdef RACEBENCH_BUG_11
    rb_state11.var_2 = (rb_state11.var_2) ^ (2337 < rb_input_size ? (uint32_t)rb_input[2337] : 0x9d578141);
    rb_state11.var_3 = (rb_state11.var_3) + (8832 < rb_input_size ? (uint32_t)rb_input[8832] : 0x34f81cca);
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) - (0x2f51801c);
    #endif
    my_node = my_node % num_nodes;

    #ifdef RACEBENCH_BUG_11
    rb_state11.var_1 = (rb_state11.var_1) ^ (0x415c6ac);
    if ((rb_state11.var_2) == (0x7f)) {
        rb_state11.var_4 = rb_state11.var_3;
    }
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_1 = (rb_state15.var_1) + (rb_state15.var_0);
    #endif
    #ifdef RACEBENCH_BUG_17
    rb_state17.var_0 = (rb_state17.var_0) - (0x5ef99c4b);
    #endif
    inv_num_nodes = 1.0 / (float)num_nodes;
    #ifdef RACEBENCH_BUG_11
    if ((rb_state11.var_2) == (0x7f)) {
        if (!((rb_state11.var_3) == (rb_state11.var_4))) {
            racebench_trigger(11);
        }
    }
    #endif
    image_partition = ROUNDUP(image_length * inv_num_nodes);
    mask_image_partition = ROUNDUP(mask_image_length * inv_num_nodes);

#ifdef DIM
    for (dim = 0; dim < NM; dim++) {
#endif

        for (step = 0; step < ROTATE_STEPS; step++) {

            #ifdef RACEBENCH_BUG_3
            rb_state3.var_0 = (rb_state3.var_0) + (0x98d0fcee);
            #endif
            #ifdef RACEBENCH_BUG_17
            rb_state17.var_1 = (rb_state17.var_1) - (12867 < rb_input_size ? (uint32_t)rb_input[12867] : 0xa659f5bf);
            #endif
            frame = step;

            #ifdef RACEBENCH_BUG_12
            rb_state12.var_1 = (rb_state12.var_1) - (11151 < rb_input_size ? (uint32_t)rb_input[11151] : 0x9dcbd618);
            #endif
            local_image_address = image_address + image_partition * my_node;
            #ifdef RACEBENCH_BUG_12
            rb_state12.var_0 = (rb_state12.var_0) + (rb_state12.var_0);
            #endif
            local_mask_image_address = mask_image_address + mask_image_partition * my_node;

            {
                unsigned long Error, Cycle;
                int Cancel, Temp;

                #ifdef RACEBENCH_BUG_15
                rb_state15.var_2 = (rb_state15.var_2) ^ (522 < rb_input_size ? (uint32_t)rb_input[522] : 0x76c80fc2);
                rb_state15.var_2 = (rb_state15.var_2) + (0xbadebc41);
                #endif
                Error = pthread_mutex_lock(&(Global->SlaveBarrier).mutex);
                if (Error != 0) {
                    printf("Error while trying to get lock in barrier.\n");
                    exit(-1);
                }

                Cycle = (Global->SlaveBarrier).cycle;
                if (++(Global->SlaveBarrier).counter != (num_nodes)) {
                    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                    while (Cycle == (Global->SlaveBarrier).cycle) {
                        #ifdef RACEBENCH_BUG_15
                        rb_state15.var_1 = (rb_state15.var_1) - ((0xa3e29c10) ^ (rb_state15.var_0));
                        rb_state15.var_1 = (rb_state15.var_1) ^ (8736 < rb_input_size ? (uint32_t)rb_input[8736] : 0x1658a57c);
                        #endif
                        Error = pthread_cond_wait(&(Global->SlaveBarrier).cv, &(Global->SlaveBarrier).mutex);
                        if (Error != 0) {
                            break;
                        }
                    }
                    pthread_setcancelstate(Cancel, &Temp);
                } else {
                    (Global->SlaveBarrier).cycle = !(Global->SlaveBarrier).cycle;
                    (Global->SlaveBarrier).counter = 0;
                    Error = pthread_cond_broadcast(&(Global->SlaveBarrier).cv);
                }
                #ifdef RACEBENCH_BUG_6
                rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_2);
                #endif
                pthread_mutex_unlock(&(Global->SlaveBarrier).mutex);
            };

            if (my_node == num_nodes - 1) {
                for (i = image_partition * my_node; i < image_length; i++) {
                    #ifdef RACEBENCH_BUG_1
                    rb_state1.var_2 = (rb_state1.var_2) - (5751 < rb_input_size ? (uint32_t)rb_input[5751] : 0x985c1b70);
                    rb_state1.var_1 = (rb_state1.var_1) + (0xb857a45b);
                    if ((rb_state1.var_3) == (0x27f3ab5b)) {
                        if ((rb_state1.var_4) != (0x0)) {
                            if (!((rb_state1.var_4) == (rb_state1.var_24))) {
                                racebench_trigger(1);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_2
                    rb_state2.var_2 = (rb_state2.var_2) - ((0xeac0d482) ^ (0xbec108f9));
                    if ((rb_state2.var_1) == (0x28de1d60)) {
                        rb_state2.var_19 = 0xa3a77a78;
                        rb_state2.var_20 = 0x71c3dd14;
                        rb_state2.var_21 = (rb_state2.var_21) - (0xe11352cf);
                        rb_state2.var_22 = (rb_state2.var_22) + (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_20) : (0xff6c0a62));
                        rb_state2.var_23 = (rb_state2.var_23) - (((0xc97ab8fe) + (0xe8e3856c)) ^ (rb_state2.var_10));
                        rb_state2.var_24 = (rb_state2.var_24) - (((rb_state2.var_19) == (0x0)) ? (rb_state2.var_21) : (0x2acb92bb));
                        rb_state2.var_25 = (rb_state2.var_25) - (((((0x723b9ebe) + (rb_state2.var_23)) ^ (rb_state2.var_12)) - (rb_state2.var_22)) + (rb_state2.var_13));
                        rb_state2.var_26 = (rb_state2.var_26) - (rb_state2.var_24);
                        rb_state2.var_27 = (rb_state2.var_27) - (((rb_state2.var_25) == (0x0)) ? (rb_state2.var_26) : (0xf744fa50));
                        rb_state2.var_18 = (rb_state2.var_18) - (rb_state2.var_27);
                        rb_state2.var_4 = rb_state2.var_18;
                    }
                    if ((rb_state2.var_1) == (0x6aaafba4)) {
                        pthread_mutex_lock(&(rb_state2.lock_36));
                        rb_state2.var_29 = 0x1cc653a4;
                        rb_state2.var_30 = 0x1293b62;
                        rb_state2.var_31 = (rb_state2.var_31) - (((rb_state2.var_15) == (0x0)) ? (rb_state2.var_30) : (0xf0e49a94));
                        rb_state2.var_32 = (rb_state2.var_32) + (((((0xabefc970) - (rb_state2.var_14)) ^ (rb_state2.var_29)) + (rb_state2.var_16)) - (0x21efb003));
                        rb_state2.var_33 = (rb_state2.var_33) - (rb_state2.var_31);
                        rb_state2.var_34 = (rb_state2.var_34) + (rb_state2.var_32);
                        rb_state2.var_35 = (rb_state2.var_35) ^ (((((0xaf9becc9) + (rb_state2.var_17)) + (rb_state2.var_33)) ^ (rb_state2.var_18)) - (rb_state2.var_34));
                        rb_state2.var_28 = (rb_state2.var_28) - (rb_state2.var_35);
                        rb_state2.var_4 = rb_state2.var_28;
                        pthread_mutex_unlock(&(rb_state2.lock_36));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_3
                    rb_state3.var_1 = (rb_state3.var_1) + ((0x5d62e489) ^ (0x4837a519));
                    rb_state3.var_0 = (rb_state3.var_0) - (0xd21c31b5);
                    rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_0);
                    rb_state3.var_2 = (rb_state3.var_2) - ((0x24b67240) ^ (rb_state3.var_2));
                    rb_state3.var_1 = (rb_state3.var_1) - ((0xbad6ca5f) + (0xb8fe4f07));
                    #endif
                    #ifdef RACEBENCH_BUG_4
                    if ((rb_state4.var_1) == (0xa5c62108)) {
                        pthread_mutex_lock(&(rb_state4.lock_30));
                        rb_state4.var_6 = 0xdaba4aeb;
                        rb_state4.var_7 = (rb_state4.var_7) + (rb_state4.var_6);
                        rb_state4.var_8 = (rb_state4.var_8) ^ (12936 < rb_input_size ? (uint32_t)rb_input[12936] : 0x1ae997a7);
                        rb_state4.var_9 = (rb_state4.var_9) + (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_5) : (0x7e9abcde));
                        rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_6);
                        rb_state4.var_11 = (rb_state4.var_11) + (((((0xeac8b937) ^ (rb_state4.var_8)) - (0x8b2977b0)) + (rb_state4.var_7)) + (rb_state4.var_7));
                        rb_state4.var_12 = (rb_state4.var_12) ^ (((0x3c6f959f) ^ (rb_state4.var_9)) - (0xabd5c7d3));
                        rb_state4.var_13 = (rb_state4.var_13) - (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_10) : (0xe15545af));
                        rb_state4.var_14 = (rb_state4.var_14) - (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_12) : (0xe6c6c7fe));
                        rb_state4.var_15 = (rb_state4.var_15) ^ (((0x8ac5d850) - (rb_state4.var_9)) - (rb_state4.var_13));
                        rb_state4.var_16 = (rb_state4.var_16) + (rb_state4.var_14);
                        rb_state4.var_17 = (rb_state4.var_17) + (rb_state4.var_15);
                        rb_state4.var_18 = (rb_state4.var_18) ^ (rb_state4.var_16);
                        rb_state4.var_19 = (rb_state4.var_19) + (((0x11bd1e75) - (rb_state4.var_17)) - (rb_state4.var_10));
                        rb_state4.var_20 = (rb_state4.var_20) ^ (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_18) : (0xb03a91e8));
                        rb_state4.var_21 = (rb_state4.var_21) + (rb_state4.var_19);
                        rb_state4.var_22 = (rb_state4.var_22) - (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_21) : (0x44820660));
                        rb_state4.var_5 = (rb_state4.var_5) ^ (rb_state4.var_22);
                        rb_state4.var_4 = rb_state4.var_5;
                        pthread_mutex_unlock(&(rb_state4.lock_30));
                    }
                    if ((rb_state4.var_1) == (0xdba700f2)) {
                        if (!((rb_state4.var_4) == (rb_state4.var_5))) {
                            racebench_trigger(4);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_5
                    rb_state5.var_0 = (rb_state5.var_0) ^ (0x9fad949);
                    rb_state5.var_0 = (rb_state5.var_0) ^ (rb_state5.var_0);
                    rb_state5.var_1 = (rb_state5.var_1) - (rb_state5.var_1);
                    rb_state5.var_3 = (rb_state5.var_3) + (rb_state5.var_2);
                    rb_state5.var_3 = (rb_state5.var_3) + (((rb_state5.var_3) == (0xa5c1705)) ? (rb_state5.var_2) : (0x206eeea2));
                    if ((rb_state5.var_3) == (0x17fd099)) {
                        pthread_mutex_lock(&(rb_state5.lock_15));
                        rb_state5.var_5 = 0xa3e07bdf;
                        rb_state5.var_6 = 0xbeaaafa7;
                        rb_state5.var_7 = (rb_state5.var_7) - (rb_state5.var_5);
                        rb_state5.var_8 = (rb_state5.var_8) ^ (((0x55fa17c4) + (rb_state5.var_6)) + (rb_state5.var_4));
                        rb_state5.var_9 = (rb_state5.var_9) - (rb_state5.var_5);
                        rb_state5.var_10 = (rb_state5.var_10) + (((((0xe63dc6e0) - (0x6f741522)) ^ (rb_state5.var_6)) + (rb_state5.var_7)) ^ (0x808e1f48));
                        rb_state5.var_11 = (rb_state5.var_11) + (((((0xd4cd3d3f) - (0xa755147d)) + (rb_state5.var_7)) ^ (rb_state5.var_9)) + (rb_state5.var_8));
                        rb_state5.var_12 = (rb_state5.var_12) + (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_10) : (0x4cd84218));
                        rb_state5.var_13 = (rb_state5.var_13) ^ (rb_state5.var_11);
                        rb_state5.var_14 = (rb_state5.var_14) ^ (((rb_state5.var_12) == (0x0)) ? (rb_state5.var_13) : (0x68bc5e96));
                        rb_state5.var_0 = (rb_state5.var_0) + (rb_state5.var_14);
                        pthread_mutex_unlock(&(rb_state5.lock_15));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_7
                    rb_state7.var_0 = (rb_state7.var_0) ^ (11067 < rb_input_size ? (uint32_t)rb_input[11067] : 0x6d99ba41);
                    rb_state7.var_1 = (rb_state7.var_1) ^ (9319 < rb_input_size ? (uint32_t)rb_input[9319] : 0x672d990d);
                    rb_state7.var_1 = (rb_state7.var_1) + ((0x57747c8f) ^ (0xa1317d97));
                    rb_state7.var_2 = (rb_state7.var_2) - (6074 < rb_input_size ? (uint32_t)rb_input[6074] : 0x7b9212f7);
                    rb_state7.var_3 = (rb_state7.var_3) ^ (0x2feb94b9);
                    if ((rb_state7.var_3) == (0xffffa927)) {
                        rb_state7.var_5 = 0x3469035c;
                        rb_state7.var_6 = 0x4987df11;
                        rb_state7.var_7 = 0xd36711b4;
                        rb_state7.var_8 = (rb_state7.var_8) - (4565 < rb_input_size ? (uint32_t)rb_input[4565] : 0xd82b7d25);
                        rb_state7.var_9 = (rb_state7.var_9) + (((rb_state7.var_4) == (0x0)) ? (rb_state7.var_5) : (0x5bf75ab5));
                        rb_state7.var_10 = (rb_state7.var_10) + (rb_state7.var_7);
                        rb_state7.var_11 = (rb_state7.var_11) ^ (((0x834d1ea7) + (rb_state7.var_6)) - (rb_state7.var_5));
                        rb_state7.var_12 = (rb_state7.var_12) ^ (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_9) : (0xfed6c073));
                        rb_state7.var_13 = (rb_state7.var_13) - (((rb_state7.var_6) == (0x0)) ? (rb_state7.var_10) : (0xb52ddc6f));
                        rb_state7.var_14 = (rb_state7.var_14) - (((rb_state7.var_7) == (0x0)) ? (rb_state7.var_11) : (0x784a2d47));
                        rb_state7.var_15 = (rb_state7.var_15) ^ (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_12) : (0x23341106));
                        rb_state7.var_16 = (rb_state7.var_16) ^ (((rb_state7.var_9) == (0x0)) ? (rb_state7.var_13) : (0xcbf088e2));
                        rb_state7.var_17 = (rb_state7.var_17) - (((((0x9d52a488) + (rb_state7.var_11)) ^ (rb_state7.var_14)) ^ (rb_state7.var_15)) ^ (rb_state7.var_10));
                        rb_state7.var_18 = (rb_state7.var_18) - (rb_state7.var_16);
                        rb_state7.var_19 = (rb_state7.var_19) ^ (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_17) : (0x12be34ee));
                        rb_state7.var_20 = (rb_state7.var_20) + (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_18) : (0xd1ca0c21));
                        rb_state7.var_21 = (rb_state7.var_21) - (rb_state7.var_19);
                        rb_state7.var_22 = (rb_state7.var_22) + (rb_state7.var_20);
                        rb_state7.var_23 = (rb_state7.var_23) + (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_21) : (0x5a3d2bc5));
                        rb_state7.var_24 = (rb_state7.var_24) ^ (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_22) : (0x9b0833c3));
                        rb_state7.var_25 = (rb_state7.var_25) - (((((0xc221f3a7) - (rb_state7.var_24)) ^ (rb_state7.var_16)) + (0xdc70a927)) ^ (rb_state7.var_23));
                        rb_state7.var_0 = (rb_state7.var_0) ^ (rb_state7.var_25);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_0) : (0xe49d5948));
                    rb_state8.var_1 = (rb_state8.var_1) ^ (((rb_state8.var_1) == (0x1e0)) ? (rb_state8.var_1) : (0xca299480));
                    rb_state8.var_0 = (rb_state8.var_0) + (0x841204a7);
                    rb_state8.var_2 = (rb_state8.var_2) + (0x6b2bcdf2);
                    if ((rb_state8.var_1) == (0x6ae42a04)) {
                        pthread_mutex_lock(&(rb_state8.lock_17));
                        rb_state8.var_5 = 0xaf91eb41;
                        rb_state8.var_6 = (rb_state8.var_6) ^ (((((0xd7152659) ^ (0x5376b1b8)) + (rb_state8.var_4)) + (rb_state8.var_3)) ^ (rb_state8.var_5));
                        rb_state8.var_7 = (rb_state8.var_7) ^ (((((0xab122f7e) ^ (0xdec2bbf0)) - (rb_state8.var_6)) + (rb_state8.var_5)) - (0xce50a1ca));
                        rb_state8.var_4 = (rb_state8.var_4) + (rb_state8.var_7);
                        rb_state8.var_3 = rb_state8.var_4;
                        pthread_mutex_unlock(&(rb_state8.lock_17));
                    }
                    if ((rb_state8.var_1) == (0x81df0dd6)) {
                        if (!((rb_state8.var_3) == (rb_state8.var_4))) {
                            racebench_trigger(8);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_2 = (rb_state9.var_2) - (0x7c1f229d);
                    if ((rb_state9.var_1) == (0x3733454c)) {
                        rb_state9.var_3 = rb_state9.var_2;
                    }
                    if ((rb_state9.var_1) == (0x2be4194b)) {
                        if (!((rb_state9.var_2) == (rb_state9.var_3))) {
                            racebench_trigger(9);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_2 = (rb_state10.var_2) + (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_1) : (0xb719ba27));
                    rb_state10.var_3 = (rb_state10.var_3) + (((rb_state10.var_1) == (0xde4399a8)) ? (rb_state10.var_3) : (0x74424d34));
                    if ((rb_state10.var_1) == (0xf0c847e8)) {
                        rb_state10.var_20 = 0x2c38022d;
                        rb_state10.var_21 = 0xdcf55036;
                        rb_state10.var_22 = (rb_state10.var_22) - (rb_state10.var_20);
                        rb_state10.var_23 = (rb_state10.var_23) + (((rb_state10.var_21) == (0x0)) ? (rb_state10.var_11) : (0x255a72a3));
                        rb_state10.var_24 = (rb_state10.var_24) - (((((0xd1805cba) - (rb_state10.var_22)) ^ (0x7d6a4e2)) + (rb_state10.var_12)) + (rb_state10.var_23));
                        rb_state10.var_19 = (rb_state10.var_19) + (rb_state10.var_24);
                        rb_state10.var_4 = rb_state10.var_19;
                    }
                    if ((rb_state10.var_1) == (0xe971b330)) {
                        pthread_mutex_lock(&(rb_state10.lock_35));
                        rb_state10.var_26 = 0x695f7e0;
                        rb_state10.var_27 = 0x63261db8;
                        rb_state10.var_28 = (rb_state10.var_28) - (((rb_state10.var_13) == (0x0)) ? (rb_state10.var_27) : (0xa136f01a));
                        rb_state10.var_29 = (rb_state10.var_29) - (rb_state10.var_26);
                        rb_state10.var_30 = (rb_state10.var_30) - (((rb_state10.var_14) == (0x0)) ? (rb_state10.var_28) : (0xe239296e));
                        rb_state10.var_31 = (rb_state10.var_31) - (((0x19be7d45) ^ (0x495565be)) + (rb_state10.var_29));
                        rb_state10.var_32 = (rb_state10.var_32) ^ (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_30) : (0x2baf90c9));
                        rb_state10.var_33 = (rb_state10.var_33) + (((0x773777fd) - (0x1ce60a3d)) ^ (rb_state10.var_31));
                        rb_state10.var_34 = (rb_state10.var_34) - (((rb_state10.var_32) == (0x0)) ? (rb_state10.var_33) : (0x111532ee));
                        rb_state10.var_25 = (rb_state10.var_25) ^ (rb_state10.var_34);
                        rb_state10.var_4 = rb_state10.var_25;
                        pthread_mutex_unlock(&(rb_state10.lock_35));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_1 = (rb_state12.var_1) - ((0x4c6b5b5a) + (0x782372dc));
                    rb_state12.var_2 = (rb_state12.var_2) - (0x4736b597);
                    rb_state12.var_2 = (rb_state12.var_2) - (9406 < rb_input_size ? (uint32_t)rb_input[9406] : 0x69b7a93);
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_1 = (rb_state13.var_1) + ((0xe5a5641a) + (rb_state13.var_2));
                    if ((rb_state13.var_1) == (0xc954fc2f)) {
                        rb_state13.var_5 = (rb_state13.var_5) - (rb_state13.var_4);
                        rb_state13.var_6 = (rb_state13.var_6) - (rb_state13.var_3);
                        rb_state13.var_7 = (rb_state13.var_7) + ((0x390fd152) + (0x5543bc04));
                        rb_state13.var_8 = (rb_state13.var_8) + (rb_state13.var_5);
                        rb_state13.var_9 = (rb_state13.var_9) + (rb_state13.var_6);
                        rb_state13.var_10 = (rb_state13.var_10) ^ (((rb_state13.var_5) == (0x0)) ? (rb_state13.var_7) : (0xf3767a4));
                        rb_state13.var_11 = (rb_state13.var_11) ^ (((((0xad6a3d7b) ^ (rb_state13.var_6)) - (rb_state13.var_9)) ^ (rb_state13.var_7)) ^ (rb_state13.var_8));
                        rb_state13.var_12 = (rb_state13.var_12) - (((rb_state13.var_8) == (0x0)) ? (rb_state13.var_10) : (0x56eccc68));
                        rb_state13.var_13 = (rb_state13.var_13) + (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_11) : (0x5f3384a3));
                        rb_state13.var_14 = (rb_state13.var_14) - (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_13) : (0x1a103b79));
                        rb_state13.var_2 = (rb_state13.var_2) ^ (rb_state13.var_14);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_14
                    if ((rb_state14.var_4) == (0x9935d22b)) {
                        pthread_mutex_lock(&(rb_state14.lock_42));
                        if ((rb_state14.var_9) != (0x0)) {
                            if (!((rb_state14.var_9) == (rb_state14.var_26))) {
                                racebench_trigger(14);
                            }
                        }
                        pthread_mutex_unlock(&(rb_state14.lock_42));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    if ((rb_state15.var_2) == (0xc32ebc4)) {
                        rb_state15.var_12 = 0xce67e602;
                        rb_state15.var_13 = 0x1c6fe10d;
                        rb_state15.var_14 = (rb_state15.var_14) + (((((0x7190e1f) + (0xd4196dd7)) - (rb_state15.var_7)) ^ (rb_state15.var_8)) - (rb_state15.var_12));
                        rb_state15.var_15 = (rb_state15.var_15) - (((rb_state15.var_13) == (0x0)) ? (rb_state15.var_14) : (0xc6ea34a0));
                        rb_state15.var_4 = (rb_state15.var_4) + (rb_state15.var_15);
                    }
                    rb_state15.var_14 = (rb_state15.var_14) - (0xa3ebcbf);
                    if ((rb_state15.var_15) == (0xe4572d80)) {
                        pthread_mutex_lock(&(rb_state15.lock_40));
                        rb_state15.var_20 = 0xd2b8364e;
                        rb_state15.var_21 = 0x48817996;
                        rb_state15.var_22 = 0x81b5c189;
                        rb_state15.var_23 = (rb_state15.var_23) ^ (0x8876758f);
                        rb_state15.var_24 = (rb_state15.var_24) ^ (rb_state15.var_20);
                        rb_state15.var_25 = (rb_state15.var_25) ^ (((((0xff3a53dc) ^ (rb_state15.var_17)) - (0xdc988f25)) + (rb_state15.var_22)) + (rb_state15.var_21));
                        rb_state15.var_26 = (rb_state15.var_26) ^ (((0xe2484b70) + (0xffd84b5f)) - (rb_state15.var_23));
                        rb_state15.var_27 = (rb_state15.var_27) - (((0x68c786aa) ^ (rb_state15.var_18)) - (rb_state15.var_24));
                        rb_state15.var_28 = (rb_state15.var_28) ^ (((rb_state15.var_25) == (0x0)) ? (rb_state15.var_26) : (0x8396b99b));
                        rb_state15.var_29 = (rb_state15.var_29) ^ (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_27) : (0xcb069e9f));
                        rb_state15.var_30 = (rb_state15.var_30) + (((((0x408fcd73) ^ (rb_state15.var_28)) + (rb_state15.var_21)) + (rb_state15.var_29)) ^ (rb_state15.var_20));
                        rb_state15.var_19 = (rb_state15.var_19) + (rb_state15.var_30);
                        rb_state15.var_18 = rb_state15.var_19;
                        pthread_mutex_unlock(&(rb_state15.lock_40));
                    }
                    if ((rb_state15.var_15) == (0xe4572d80)) {
                        if (!((rb_state15.var_18) == (rb_state15.var_19))) {
                            racebench_trigger(15);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_17
                    if ((rb_state17.var_3) == (0x7461ddbe)) {
                        rb_state17.var_4 = rb_state17.var_0;
                    }
                    if ((rb_state17.var_3) == (0x2874ab1)) {
                        pthread_mutex_lock(&(rb_state17.lock_13));
                        if (!((rb_state17.var_0) == (rb_state17.var_4))) {
                            racebench_trigger(17);
                        }
                        pthread_mutex_unlock(&(rb_state17.lock_13));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_1 = (rb_state18.var_1) - (0x58fdfa25);
                    rb_state18.var_3 = (rb_state18.var_3) ^ (((rb_state18.var_1) == (0xa70205db)) ? (rb_state18.var_3) : (0x359b56ea));
                    rb_state18.var_3 = (rb_state18.var_3) ^ ((0xb143fb24) - (0x2fcb7e51));
                    if ((rb_state18.var_1) == (0xe945f576)) {
                        pthread_mutex_lock(&(rb_state18.lock_13));
                        rb_state18.var_6 = (rb_state18.var_6) - (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_5) : (0x708a6e12));
                        rb_state18.var_5 = (rb_state18.var_5) + (rb_state18.var_6);
                        rb_state18.var_4 = rb_state18.var_5;
                        pthread_mutex_unlock(&(rb_state18.lock_13));
                    }
                    if ((rb_state18.var_1) == (0x388b3c1c)) {
                        pthread_mutex_lock(&(rb_state18.lock_13));
                        if (!((rb_state18.var_4) == (rb_state18.var_5))) {
                            racebench_trigger(18);
                        }
                        pthread_mutex_unlock(&(rb_state18.lock_13));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    rb_state19.var_2 = (rb_state19.var_2) - (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_1) : (0x8270b5f0));
                    if ((rb_state19.var_1) == (0x9de5eca7)) {
                        if ((rb_state19.var_4) != (0x0)) {
                            if (!((rb_state19.var_4) == (rb_state19.var_17))) {
                                racebench_trigger(19);
                            }
                        }
                    }
                    #endif
                    *local_image_address++ = background;
                }
                if (adaptive) {
                    for (i = mask_image_partition * my_node; i < mask_image_length; i++) {
                        *local_mask_image_address++ = NULL_PIXEL;
                    }
                }
            } else {
                for (i = 0; i < image_partition; i++) {
                    #ifdef RACEBENCH_BUG_0
                    rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0x7fddb29a));
                    rb_state0.var_1 = (rb_state0.var_1) + (0xafa56e35);
                    rb_state0.var_2 = (rb_state0.var_2) + (0x4e74dea6);
                    rb_state0.var_3 = (rb_state0.var_3) ^ ((0x97df7255) + (rb_state0.var_1));
                    rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_3);
                    rb_state0.var_1 = (rb_state0.var_1) + (rb_state0.var_2);
                    rb_state0.var_3 = (rb_state0.var_3) + (rb_state0.var_0);
                    rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_2);
                    if ((rb_state0.var_3) == (0xd3250f3)) {
                        pthread_mutex_lock(&(rb_state0.lock_35));
                        rb_state0.var_16 = 0x5b4506a4;
                        rb_state0.var_17 = 0x1a4d3d44;
                        rb_state0.var_18 = 0x19c04014;
                        rb_state0.var_19 = (rb_state0.var_19) ^ (((0xb180e41a) ^ (rb_state0.var_8)) ^ (rb_state0.var_18));
                        rb_state0.var_20 = (rb_state0.var_20) ^ (rb_state0.var_16);
                        rb_state0.var_21 = (rb_state0.var_21) - (((0xb7eed746) - (0xa33cd986)) ^ (rb_state0.var_17));
                        rb_state0.var_22 = (rb_state0.var_22) + (rb_state0.var_19);
                        rb_state0.var_23 = (rb_state0.var_23) - (((rb_state0.var_20) == (0x0)) ? (rb_state0.var_21) : (0xdcd4ac1e));
                        rb_state0.var_24 = (rb_state0.var_24) + (((0x59779a02) + (rb_state0.var_22)) ^ (rb_state0.var_9));
                        rb_state0.var_25 = (rb_state0.var_25) - (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_24) : (0xd90ad9d4));
                        rb_state0.var_15 = (rb_state0.var_15) - (rb_state0.var_25);
                        rb_state0.var_4 = rb_state0.var_15;
                        pthread_mutex_unlock(&(rb_state0.lock_35));
                    }
                    if ((rb_state0.var_3) == (0x7bc7c85d)) {
                        if ((rb_state0.var_4) != (0x0)) {
                            if (!((rb_state0.var_4) == (rb_state0.var_26))) {
                                racebench_trigger(0);
                            }
                        }
                    }
                    if ((rb_state0.var_3) == (0x7bc7c85d)) {
                        pthread_mutex_lock(&(rb_state0.lock_35));
                        rb_state0.var_27 = 0xd488011a;
                        rb_state0.var_28 = 0x117eb1d9;
                        rb_state0.var_29 = (rb_state0.var_29) + (rb_state0.var_10);
                        rb_state0.var_30 = (rb_state0.var_30) + (((rb_state0.var_11) == (0x0)) ? (rb_state0.var_27) : (0x86eb41bb));
                        rb_state0.var_31 = (rb_state0.var_31) - (rb_state0.var_28);
                        rb_state0.var_32 = (rb_state0.var_32) - (((rb_state0.var_29) == (0x0)) ? (rb_state0.var_30) : (0xc938eb29));
                        rb_state0.var_33 = (rb_state0.var_33) - (((0xe7e6107f) - (0xd4272fb1)) ^ (rb_state0.var_31));
                        rb_state0.var_34 = (rb_state0.var_34) ^ (((((0x1e8ac55) - (rb_state0.var_32)) ^ (rb_state0.var_12)) + (rb_state0.var_33)) - (0xbf9cf9ee));
                        rb_state0.var_26 = (rb_state0.var_26) ^ (rb_state0.var_34);
                        rb_state0.var_4 = rb_state0.var_26;
                        pthread_mutex_unlock(&(rb_state0.lock_35));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_1
                    rb_state1.var_0 = (rb_state1.var_0) - (0x15acec3e);
                    rb_state1.var_1 = (rb_state1.var_1) + (3311 < rb_input_size ? (uint32_t)rb_input[3311] : 0x55cce186);
                    rb_state1.var_2 = (rb_state1.var_2) + (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_0) : (0x4a6481d0));
                    rb_state1.var_3 = (rb_state1.var_3) - (0x552133c9);
                    rb_state1.var_3 = (rb_state1.var_3) + (rb_state1.var_0);
                    rb_state1.var_2 = (rb_state1.var_2) ^ (0x26a5301e);
                    if ((rb_state1.var_3) == (0x27f3ab5b)) {
                        rb_state1.var_14 = 0xb58f14cb;
                        rb_state1.var_15 = 0xc6ab3266;
                        rb_state1.var_16 = (rb_state1.var_16) ^ (((rb_state1.var_9) == (0x0)) ? (rb_state1.var_14) : (0xd4f9dbce));
                        rb_state1.var_17 = (rb_state1.var_17) ^ ((0x285cfc4c) - (rb_state1.var_10));
                        rb_state1.var_18 = (rb_state1.var_18) + (((((0xe9a7df0b) - (rb_state1.var_11)) - (rb_state1.var_8)) ^ (rb_state1.var_12)) ^ (rb_state1.var_15));
                        rb_state1.var_19 = (rb_state1.var_19) - (rb_state1.var_16);
                        rb_state1.var_20 = (rb_state1.var_20) - (((0xf69282ad) + (0x7889a9c1)) - (rb_state1.var_17));
                        rb_state1.var_21 = (rb_state1.var_21) ^ (((0x2f40afa8) ^ (rb_state1.var_18)) ^ (rb_state1.var_13));
                        rb_state1.var_22 = (rb_state1.var_22) - (((rb_state1.var_19) == (0x0)) ? (rb_state1.var_20) : (0xfe1f097));
                        rb_state1.var_23 = (rb_state1.var_23) + (((((0x522ad368) - (rb_state1.var_21)) ^ (rb_state1.var_14)) ^ (rb_state1.var_22)) + (0x1eebbb9d));
                        rb_state1.var_13 = (rb_state1.var_13) - (rb_state1.var_23);
                        rb_state1.var_4 = rb_state1.var_13;
                    }
                    if ((rb_state1.var_3) == (0x27f3ab5b)) {
                        pthread_mutex_lock(&(rb_state1.lock_37));
                        rb_state1.var_25 = 0x5a51902e;
                        rb_state1.var_26 = 0x53926ca3;
                        rb_state1.var_27 = (rb_state1.var_27) ^ (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_25) : (0x38be5793));
                        rb_state1.var_28 = (rb_state1.var_28) - (rb_state1.var_26);
                        rb_state1.var_29 = (rb_state1.var_29) + (((0x2ebd9991) - (rb_state1.var_27)) - (rb_state1.var_16));
                        rb_state1.var_30 = (rb_state1.var_30) ^ (((0x9ff39e70) ^ (rb_state1.var_28)) + (rb_state1.var_17));
                        rb_state1.var_31 = (rb_state1.var_31) ^ (rb_state1.var_29);
                        rb_state1.var_32 = (rb_state1.var_32) ^ (((0x9d37eea6) + (rb_state1.var_30)) + (rb_state1.var_18));
                        rb_state1.var_33 = (rb_state1.var_33) - (((0x1c3745ba) + (rb_state1.var_31)) ^ (rb_state1.var_19));
                        rb_state1.var_34 = (rb_state1.var_34) - (((0xc3fe70bf) - (rb_state1.var_32)) - (0x7c87de6a));
                        rb_state1.var_35 = (rb_state1.var_35) ^ (((0x111e1ed5) + (rb_state1.var_33)) ^ (0xe369c303));
                        rb_state1.var_36 = (rb_state1.var_36) + (((((0x32d567d5) ^ (rb_state1.var_20)) ^ (rb_state1.var_34)) ^ (rb_state1.var_21)) ^ (rb_state1.var_35));
                        rb_state1.var_24 = (rb_state1.var_24) ^ (rb_state1.var_36);
                        rb_state1.var_4 = rb_state1.var_24;
                        pthread_mutex_unlock(&(rb_state1.lock_37));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_2
                    rb_state2.var_0 = (rb_state2.var_0) ^ (0x76d237a8);
                    rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_0);
                    rb_state2.var_1 = (rb_state2.var_1) + (0xc9ed0dc);
                    rb_state2.var_2 = (rb_state2.var_2) ^ ((0x3dd0336b) - (rb_state2.var_2));
                    rb_state2.var_3 = (rb_state2.var_3) - (4215 < rb_input_size ? (uint32_t)rb_input[4215] : 0xbc7548e9);
                    rb_state2.var_3 = (rb_state2.var_3) + (((rb_state2.var_1) == (0x83710884)) ? (rb_state2.var_0) : (0x814fa7a6));
                    rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_0);
                    if ((rb_state2.var_1) == (0x6aaafba4)) {
                        if ((rb_state2.var_4) != (0x0)) {
                            if (!((rb_state2.var_4) == (rb_state2.var_28))) {
                                racebench_trigger(2);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_3
                    rb_state3.var_2 = (rb_state3.var_2) - (0x4477e5f2);
                    rb_state3.var_0 = (rb_state3.var_0) + (rb_state3.var_1);
                    if ((rb_state3.var_2) == (0x8d5ab086)) {
                        rb_state3.var_13 = (rb_state3.var_13) - (((0x730ece68) - (0x38b8e4a)) + (rb_state3.var_6));
                        rb_state3.var_14 = (rb_state3.var_14) + (((0xb4f46fb6) - (rb_state3.var_9)) ^ (rb_state3.var_7));
                        rb_state3.var_15 = (rb_state3.var_15) ^ ((0xab9f44f9) + (rb_state3.var_10));
                        rb_state3.var_16 = (rb_state3.var_16) - ((0xed482fb7) + (0xaa3527a2));
                        rb_state3.var_17 = (rb_state3.var_17) ^ (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_13) : (0xaf155aaa));
                        rb_state3.var_18 = (rb_state3.var_18) - (((rb_state3.var_11) == (0x0)) ? (rb_state3.var_14) : (0x7189fddf));
                        rb_state3.var_19 = (rb_state3.var_19) - (((rb_state3.var_12) == (0x0)) ? (rb_state3.var_15) : (0x10a4101));
                        rb_state3.var_20 = (rb_state3.var_20) - (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_17) : (0x420d2d20));
                        rb_state3.var_21 = (rb_state3.var_21) + (((0xc262d3bf) - (rb_state3.var_18)) + (0xdc72dc37));
                        rb_state3.var_22 = (rb_state3.var_22) ^ (((0x4ddc73ac) ^ (rb_state3.var_19)) - (rb_state3.var_13));
                        rb_state3.var_23 = (rb_state3.var_23) + (rb_state3.var_20);
                        rb_state3.var_24 = (rb_state3.var_24) + (((rb_state3.var_14) == (0x0)) ? (rb_state3.var_21) : (0x26bfb37e));
                        rb_state3.var_25 = (rb_state3.var_25) ^ (((0x1c480ad3) - (rb_state3.var_15)) ^ (rb_state3.var_22));
                        rb_state3.var_26 = (rb_state3.var_26) - (((rb_state3.var_16) == (0x0)) ? (rb_state3.var_23) : (0x7c8d9b88));
                        rb_state3.var_27 = (rb_state3.var_27) ^ (((rb_state3.var_24) == (0x0)) ? (rb_state3.var_25) : (0x1fdfa472));
                        rb_state3.var_28 = (rb_state3.var_28) + (((rb_state3.var_17) == (0x0)) ? (rb_state3.var_26) : (0xadf4bd07));
                        rb_state3.var_29 = (rb_state3.var_29) + (((((0x80ed2eb6) ^ (rb_state3.var_18)) ^ (rb_state3.var_27)) + (0x443cd43d)) - (rb_state3.var_28));
                        rb_state3.var_12 = (rb_state3.var_12) + (rb_state3.var_29);
                        rb_state3.var_3 = rb_state3.var_12;
                    }
                    if ((rb_state3.var_2) == (0x48e2ca94)) {
                        if ((rb_state3.var_3) != (0x0)) {
                            if (!((rb_state3.var_3) == (rb_state3.var_30))) {
                                racebench_trigger(3);
                            }
                        }
                    }
                    if ((rb_state3.var_2) == (0x48e2ca94)) {
                        pthread_mutex_lock(&(rb_state3.lock_36));
                        rb_state3.var_31 = 0x1731e62e;
                        rb_state3.var_32 = 0xc3521742;
                        rb_state3.var_33 = (rb_state3.var_33) - (((rb_state3.var_19) == (0x0)) ? (rb_state3.var_31) : (0x2c47f1f3));
                        rb_state3.var_34 = (rb_state3.var_34) + (rb_state3.var_32);
                        rb_state3.var_35 = (rb_state3.var_35) - (((((0x2d8160ee) + (rb_state3.var_34)) + (rb_state3.var_33)) + (0xcd4807b6)) - (rb_state3.var_20));
                        rb_state3.var_30 = (rb_state3.var_30) + (rb_state3.var_35);
                        rb_state3.var_3 = rb_state3.var_30;
                        pthread_mutex_unlock(&(rb_state3.lock_36));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_4
                    rb_state4.var_0 = (rb_state4.var_0) - (7793 < rb_input_size ? (uint32_t)rb_input[7793] : 0xbedbdbab);
                    rb_state4.var_1 = (rb_state4.var_1) + (0xaa48ddd6);
                    rb_state4.var_2 = (rb_state4.var_2) - (11723 < rb_input_size ? (uint32_t)rb_input[11723] : 0x5a413e1d);
                    rb_state4.var_3 = (rb_state4.var_3) ^ (rb_state4.var_0);
                    rb_state4.var_1 = (rb_state4.var_1) ^ (0x8ba9fe2c);
                    rb_state4.var_2 = (rb_state4.var_2) + ((0xe1eb80fa) + (0x7b85acbc));
                    rb_state4.var_3 = (rb_state4.var_3) ^ (0xfa358584);
                    rb_state4.var_0 = (rb_state4.var_0) ^ (0xc3bc82c4);
                    if ((rb_state4.var_1) == (0xdba700f2)) {
                        rb_state4.var_23 = 0xd2c3e365;
                        rb_state4.var_24 = 0xf500f44a;
                        rb_state4.var_25 = (rb_state4.var_25) + (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_12) : (0x899a492));
                        rb_state4.var_26 = (rb_state4.var_26) + ((0x5d1fa5d4) + (0x500cae64));
                        rb_state4.var_27 = (rb_state4.var_27) ^ (((((0xb310c1da) - (0x8033fb2c)) - (rb_state4.var_25)) + (rb_state4.var_23)) + (0xee067911));
                        rb_state4.var_28 = (rb_state4.var_28) ^ (((rb_state4.var_13) == (0x0)) ? (rb_state4.var_26) : (0xdfb4bda2));
                        rb_state4.var_29 = (rb_state4.var_29) - (((((0x71b0fe09) + (0x47c102e5)) ^ (0x8c0c7a3b)) ^ (rb_state4.var_27)) - (rb_state4.var_28));
                        rb_state4.var_4 = (rb_state4.var_4) + (rb_state4.var_29);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_5
                    rb_state5.var_1 = (rb_state5.var_1) + ((0x33f51f57) + (0x76437ce2));
                    rb_state5.var_2 = (rb_state5.var_2) + (0x84825e5d);
                    rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_2) == (0x84825e5d)) ? (rb_state5.var_1) : (0x2b88cbd9));
                    if ((rb_state5.var_3) == (0x586bad3f)) {
                        rb_state5.var_4 = rb_state5.var_0;
                    }
                    if ((rb_state5.var_3) == (0x17fd099)) {
                        if (!((rb_state5.var_0) == (rb_state5.var_4))) {
                            racebench_trigger(5);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_0 = (rb_state6.var_0) ^ ((0x50a4abdf) ^ (rb_state6.var_0));
                    rb_state6.var_0 = (rb_state6.var_0) - (rb_state6.var_0);
                    rb_state6.var_1 = (rb_state6.var_1) - (11641 < rb_input_size ? (uint32_t)rb_input[11641] : 0x86470b41);
                    rb_state6.var_2 = (rb_state6.var_2) + (0x7e9bbca3);
                    rb_state6.var_1 = (rb_state6.var_1) - (((rb_state6.var_1) == (0x0)) ? (rb_state6.var_2) : (0x7df7f35));
                    rb_state6.var_2 = (rb_state6.var_2) ^ (((rb_state6.var_1) == (0x8164435d)) ? (rb_state6.var_0) : (0x64e6b408));
                    rb_state6.var_2 = (rb_state6.var_2) ^ ((0xa262f6b7) + (rb_state6.var_0));
                    if ((rb_state6.var_1) == (0xf7a3bed1)) {
                        pthread_mutex_lock(&(rb_state6.lock_13));
                        rb_state6.var_5 = (rb_state6.var_5) ^ (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_4) : (0x134e5ff3));
                        rb_state6.var_6 = (rb_state6.var_6) + (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_5) : (0x98596c14));
                        rb_state6.var_4 = (rb_state6.var_4) - (rb_state6.var_6);
                        rb_state6.var_3 = rb_state6.var_4;
                        pthread_mutex_unlock(&(rb_state6.lock_13));
                    }
                    if ((rb_state6.var_1) == (0xefc43f9c)) {
                        rb_state6.var_7 = 0x2fdd97c;
                        rb_state6.var_8 = (rb_state6.var_8) + (0x5cce6af7);
                        rb_state6.var_9 = (rb_state6.var_9) - (((((0x5ee43bdf) ^ (0x27839ca5)) - (rb_state6.var_6)) - (rb_state6.var_7)) - (rb_state6.var_7));
                        rb_state6.var_10 = (rb_state6.var_10) - (((((0x60c483bf) - (0x4cc9844d)) + (rb_state6.var_4)) ^ (rb_state6.var_8)) - (rb_state6.var_8));
                        rb_state6.var_11 = (rb_state6.var_11) - (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_9) : (0x4096b348));
                        rb_state6.var_12 = (rb_state6.var_12) - (((((0x21c900e2) + (rb_state6.var_11)) + (rb_state6.var_10)) + (rb_state6.var_10)) + (rb_state6.var_11));
                        rb_state6.var_3 = (rb_state6.var_3) + (rb_state6.var_12);
                    }
                    if ((rb_state6.var_1) == (0xefc43f9c)) {
                        rb_state6.var_14 = !((rb_state6.var_3) == (rb_state6.var_4));
                    }
                    rb_state6.var_12 = (rb_state6.var_12) ^ (0x562ea504);
                    rb_state6.var_5 = (rb_state6.var_5) ^ (0x1bbf359a);
                    rb_state6.var_6 = (rb_state6.var_6) + (10999 < rb_input_size ? (uint32_t)rb_input[10999] : 0x5bb6d481);
                    rb_state6.var_8 = (rb_state6.var_8) + (13910 < rb_input_size ? (uint32_t)rb_input[13910] : 0xfe6275b2);
                    if ((rb_state6.var_14) == (0x1)) {
                        pthread_mutex_lock(&(rb_state6.lock_38));
                        rb_state6.var_28 = 0x2953b9e2;
                        rb_state6.var_29 = 0x8d5ed701;
                        rb_state6.var_30 = (rb_state6.var_30) - (((((0xdd42fa32) ^ (rb_state6.var_28)) - (rb_state6.var_21)) + (0x9d15e645)) - (rb_state6.var_29));
                        rb_state6.var_31 = (rb_state6.var_31) + (((((0x89ef3b4b) ^ (rb_state6.var_20)) - (0x4f091f48)) - (rb_state6.var_22)) ^ (rb_state6.var_30));
                        rb_state6.var_27 = (rb_state6.var_27) ^ (rb_state6.var_31);
                        rb_state6.var_15 = rb_state6.var_27;
                        pthread_mutex_unlock(&(rb_state6.lock_38));
                    }
                    if ((rb_state6.var_14) == (0x1)) {
                        if ((rb_state6.var_15) != (0x0)) {
                            if (!((rb_state6.var_15) == (rb_state6.var_32))) {
                                racebench_trigger(6);
                            }
                        }
                    }
                    if ((rb_state6.var_14) == (0x1)) {
                        pthread_mutex_lock(&(rb_state6.lock_38));
                        rb_state6.var_33 = 0x1dbbecd5;
                        rb_state6.var_34 = (rb_state6.var_34) ^ (9400 < rb_input_size ? (uint32_t)rb_input[9400] : 0x74f3814b);
                        rb_state6.var_35 = (rb_state6.var_35) ^ (((((0x231f83d5) + (0xe330dc87)) ^ (rb_state6.var_33)) ^ (rb_state6.var_23)) ^ (rb_state6.var_25));
                        rb_state6.var_36 = (rb_state6.var_36) ^ (((rb_state6.var_24) == (0x0)) ? (rb_state6.var_34) : (0xc3ba04d8));
                        rb_state6.var_37 = (rb_state6.var_37) ^ (((((0x9efa7763) ^ (rb_state6.var_35)) + (rb_state6.var_26)) ^ (rb_state6.var_36)) - (0xda414b24));
                        rb_state6.var_32 = (rb_state6.var_32) + (rb_state6.var_37);
                        rb_state6.var_15 = rb_state6.var_32;
                        pthread_mutex_unlock(&(rb_state6.lock_38));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_7
                    rb_state7.var_0 = (rb_state7.var_0) - (rb_state7.var_1);
                    rb_state7.var_2 = (rb_state7.var_2) ^ (((rb_state7.var_0) == (0x0)) ? (rb_state7.var_2) : (0x9cbefc9f));
                    rb_state7.var_3 = (rb_state7.var_3) - (6197 < rb_input_size ? (uint32_t)rb_input[6197] : 0x222e4e2f);
                    #endif
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_2 = (rb_state8.var_2) + (rb_state8.var_2);
                    rb_state8.var_1 = (rb_state8.var_1) + (8955 < rb_input_size ? (uint32_t)rb_input[8955] : 0xc83c3357);
                    rb_state8.var_2 = (rb_state8.var_2) + (rb_state8.var_0);
                    rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_0) == (0x0)) ? (rb_state8.var_2) : (0xe9051ca6));
                    if ((rb_state8.var_1) == (0x81df0dd6)) {
                        rb_state8.var_8 = 0x3a759e1a;
                        rb_state8.var_9 = (rb_state8.var_9) ^ (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_6) : (0x38604717));
                        rb_state8.var_10 = (rb_state8.var_10) + ((0x96b21fd9) ^ (rb_state8.var_8));
                        rb_state8.var_11 = (rb_state8.var_11) ^ (((0xae66291f) + (rb_state8.var_9)) - (rb_state8.var_7));
                        rb_state8.var_12 = (rb_state8.var_12) + (((0xa1b1edaf) + (rb_state8.var_10)) ^ (rb_state8.var_9));
                        rb_state8.var_13 = (rb_state8.var_13) + (((0x71c576b4) + (rb_state8.var_10)) + (0x72787dbc));
                        rb_state8.var_14 = (rb_state8.var_14) - (((((0x19acc271) - (rb_state8.var_12)) ^ (rb_state8.var_12)) - (rb_state8.var_11)) - (rb_state8.var_11));
                        rb_state8.var_15 = (rb_state8.var_15) ^ (((0x6c044aa8) - (rb_state8.var_13)) + (0x9fd3b070));
                        rb_state8.var_16 = (rb_state8.var_16) + (((((0x2d5b7c58) ^ (rb_state8.var_15)) + (0x2a0fd02f)) ^ (0x3d6a4ea)) + (rb_state8.var_14));
                        rb_state8.var_3 = (rb_state8.var_3) ^ (rb_state8.var_16);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_0 = (rb_state9.var_0) ^ (0xac220187);
                    rb_state9.var_0 = (rb_state9.var_0) ^ (13164 < rb_input_size ? (uint32_t)rb_input[13164] : 0xa6ce14d);
                    rb_state9.var_1 = (rb_state9.var_1) - (0x854caee3);
                    rb_state9.var_1 = (rb_state9.var_1) + ((0xff44f50) ^ (rb_state9.var_2));
                    rb_state9.var_0 = (rb_state9.var_0) + (((rb_state9.var_2) == (0x5d0dd4ce)) ? (rb_state9.var_1) : (0x502528a5));
                    rb_state9.var_0 = (rb_state9.var_0) ^ (rb_state9.var_1);
                    rb_state9.var_2 = (rb_state9.var_2) + ((0xbaf24a9d) ^ (rb_state9.var_0));
                    if ((rb_state9.var_1) == (0x2be4194b)) {
                        pthread_mutex_lock(&(rb_state9.lock_13));
                        rb_state9.var_4 = 0xca4b13dd;
                        rb_state9.var_5 = 0x70e0d341;
                        rb_state9.var_6 = (rb_state9.var_6) + ((0x2429acd1) + (rb_state9.var_4));
                        rb_state9.var_7 = (rb_state9.var_7) - (((0x38eb0768) - (rb_state9.var_5)) + (rb_state9.var_5));
                        rb_state9.var_8 = (rb_state9.var_8) - (((((0x5f8daf0f) - (rb_state9.var_3)) - (rb_state9.var_4)) ^ (0xd9816dd3)) - (rb_state9.var_6));
                        rb_state9.var_9 = (rb_state9.var_9) ^ (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_7) : (0xf5c946a7));
                        rb_state9.var_10 = (rb_state9.var_10) ^ (((0xd9eac9be) ^ (0x61cad04d)) - (rb_state9.var_8));
                        rb_state9.var_11 = (rb_state9.var_11) - (rb_state9.var_9);
                        rb_state9.var_12 = (rb_state9.var_12) ^ (((((0x1501b650) - (rb_state9.var_11)) - (rb_state9.var_10)) ^ (0x7d2615a0)) ^ (rb_state9.var_7));
                        rb_state9.var_2 = (rb_state9.var_2) ^ (rb_state9.var_12);
                        pthread_mutex_unlock(&(rb_state9.lock_13));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_0 = (rb_state10.var_0) ^ ((0xf020ae1) ^ (0x2c69e25d));
                    rb_state10.var_1 = (rb_state10.var_1) - (0x75694b8);
                    rb_state10.var_0 = (rb_state10.var_0) - (0x5530652b);
                    rb_state10.var_0 = (rb_state10.var_0) ^ (9510 < rb_input_size ? (uint32_t)rb_input[9510] : 0x43aa0f4c);
                    rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_3);
                    rb_state10.var_0 = (rb_state10.var_0) - (0x2b6a03fc);
                    if ((rb_state10.var_1) == (0xe971b330)) {
                        pthread_mutex_lock(&(rb_state10.lock_35));
                        if ((rb_state10.var_4) != (0x0)) {
                            if (!((rb_state10.var_4) == (rb_state10.var_25))) {
                                racebench_trigger(10);
                            }
                        }
                        pthread_mutex_unlock(&(rb_state10.lock_35));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_2 = (rb_state12.var_2) ^ (12617 < rb_input_size ? (uint32_t)rb_input[12617] : 0x54f1dae0);
                    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_1);
                    rb_state12.var_0 = (rb_state12.var_0) - ((0xea844400) + (0x6995df1b));
                    if ((rb_state12.var_1) == (0x94e55780)) {
                        pthread_mutex_lock(&(rb_state12.lock_13));
                        rb_state12.var_3 = rb_state12.var_2;
                        pthread_mutex_unlock(&(rb_state12.lock_13));
                    }
                    if ((rb_state12.var_1) == (0x94e55780)) {
                        rb_state12.var_4 = 0xe6bc2bc3;
                        rb_state12.var_5 = (rb_state12.var_5) + (0xb5cc1aed);
                        rb_state12.var_6 = (rb_state12.var_6) - (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_3) : (0x37543dcb));
                        rb_state12.var_7 = (rb_state12.var_7) - (1771 < rb_input_size ? (uint32_t)rb_input[1771] : 0x3b7dfd5d);
                        rb_state12.var_8 = (rb_state12.var_8) - (((((0x487b889f) ^ (0xbfad8563)) - (rb_state12.var_5)) - (rb_state12.var_5)) ^ (rb_state12.var_6));
                        rb_state12.var_9 = (rb_state12.var_9) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_7) : (0x52de395));
                        rb_state12.var_10 = (rb_state12.var_10) + (((0x99759e85) + (rb_state12.var_8)) ^ (rb_state12.var_7));
                        rb_state12.var_11 = (rb_state12.var_11) - (rb_state12.var_9);
                        rb_state12.var_12 = (rb_state12.var_12) + (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_11) : (0xeeebe404));
                        rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_12);
                    }
                    if ((rb_state12.var_1) == (0x94e55780)) {
                        if (!((rb_state12.var_2) == (rb_state12.var_3))) {
                            racebench_trigger(12);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_0 = (rb_state13.var_0) + (0x9e002b12);
                    rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_0);
                    rb_state13.var_2 = (rb_state13.var_2) - (10253 < rb_input_size ? (uint32_t)rb_input[10253] : 0xab22b756);
                    rb_state13.var_1 = (rb_state13.var_1) - ((0xc066ad7d) + (rb_state13.var_2));
                    rb_state13.var_3 = (rb_state13.var_3) ^ (((rb_state13.var_0) == (0x9e002b12)) ? (rb_state13.var_3) : (0xf83fea68));
                    rb_state13.var_3 = (rb_state13.var_3) - ((0xf56bda2c) + (0x63a5e362));
                    rb_state13.var_0 = (rb_state13.var_0) + ((0xa6f8eb2c) ^ (0xfd1bec68));
                    if ((rb_state13.var_1) == (0xe3af9815)) {
                        pthread_mutex_lock(&(rb_state13.lock_15));
                        rb_state13.var_4 = rb_state13.var_2;
                        pthread_mutex_unlock(&(rb_state13.lock_15));
                    }
                    if ((rb_state13.var_1) == (0xc954fc2f)) {
                        if (!((rb_state13.var_2) == (rb_state13.var_4))) {
                            racebench_trigger(13);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_14
                    rb_state14.var_0 = (rb_state14.var_0) + (2544 < rb_input_size ? (uint32_t)rb_input[2544] : 0x60561af);
                    rb_state14.var_0 = (rb_state14.var_0) ^ (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0xdbb0fa2f));
                    rb_state14.var_1 = (rb_state14.var_1) + (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_1) : (0x27e2a2e6));
                    rb_state14.var_2 = (rb_state14.var_2) + (0xcc3a6000);
                    rb_state14.var_2 = (rb_state14.var_2) - ((0x3320d7a9) - (rb_state14.var_2));
                    rb_state14.var_1 = (rb_state14.var_1) - (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_2) : (0x4e2ad7e7));
                    rb_state14.var_3 = (rb_state14.var_3) - (rb_state14.var_3);
                    rb_state14.var_3 = (rb_state14.var_3) ^ (rb_state14.var_3);
                    if ((rb_state14.var_1) == (0x556169de)) {
                        usleep(100);
                    }
                    if ((rb_state14.var_1) == (0xe297f51b)) {
                        pthread_mutex_lock(&(rb_state14.lock_7));
                        rb_state14.var_6 = (rb_state14.var_6) + (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_5) : (0x98d5ae29));
                        rb_state14.var_4 = (rb_state14.var_4) ^ (rb_state14.var_6);
                        rb_state14.var_5 = rb_state14.var_4;
                        pthread_mutex_unlock(&(rb_state14.lock_7));
                    }
                    if ((rb_state14.var_1) == (0xe297f51b)) {
                        rb_state14.var_8 = !((rb_state14.var_5) == (rb_state14.var_4));
                    }
                    rb_state14.var_6 = (rb_state14.var_6) ^ (0xc2321b45);
                    rb_state14.var_6 = (rb_state14.var_6) + (((rb_state14.var_5) == (0x0)) ? (rb_state14.var_8) : (0xabc6c0c4));
                    rb_state14.var_6 = (rb_state14.var_6) ^ (10595 < rb_input_size ? (uint32_t)rb_input[10595] : 0x72bf1c91);
                    rb_state14.var_2 = (rb_state14.var_2) ^ (rb_state14.var_8);
                    if ((rb_state14.var_4) == (0x9935d22b)) {
                        rb_state14.var_22 = 0xa6e2796a;
                        rb_state14.var_23 = 0x8b3014c0;
                        rb_state14.var_24 = (rb_state14.var_24) - (((rb_state14.var_22) == (0x0)) ? (rb_state14.var_23) : (0x15bd4780));
                        rb_state14.var_25 = (rb_state14.var_25) - (((rb_state14.var_16) == (0x0)) ? (rb_state14.var_24) : (0xd4bfc9ba));
                        rb_state14.var_21 = (rb_state14.var_21) + (rb_state14.var_25);
                        rb_state14.var_9 = rb_state14.var_21;
                    }
                    if ((rb_state14.var_4) == (0x9935d22b)) {
                        pthread_mutex_lock(&(rb_state14.lock_42));
                        rb_state14.var_27 = 0x673fee2c;
                        rb_state14.var_28 = 0xdafd395b;
                        rb_state14.var_29 = (rb_state14.var_29) + (0x216fb806);
                        rb_state14.var_30 = (rb_state14.var_30) + (((0xcd1d3e3) + (0x45e3613d)) - (rb_state14.var_17));
                        rb_state14.var_31 = (rb_state14.var_31) - (rb_state14.var_28);
                        rb_state14.var_32 = (rb_state14.var_32) - (((0x8c03c47f) ^ (rb_state14.var_27)) - (0x53466cfc));
                        rb_state14.var_33 = (rb_state14.var_33) ^ (((rb_state14.var_29) == (0x0)) ? (rb_state14.var_30) : (0xeaf0d75));
                        rb_state14.var_34 = (rb_state14.var_34) ^ (rb_state14.var_31);
                        rb_state14.var_35 = (rb_state14.var_35) ^ (((0x44ab2cb5) ^ (rb_state14.var_32)) - (0x811561dc));
                        rb_state14.var_36 = (rb_state14.var_36) ^ (rb_state14.var_33);
                        rb_state14.var_37 = (rb_state14.var_37) + (((rb_state14.var_18) == (0x0)) ? (rb_state14.var_34) : (0xf35096e9));
                        rb_state14.var_38 = (rb_state14.var_38) ^ (((0xbfb72e9f) ^ (rb_state14.var_19)) - (rb_state14.var_35));
                        rb_state14.var_39 = (rb_state14.var_39) ^ (rb_state14.var_36);
                        rb_state14.var_40 = (rb_state14.var_40) ^ (((((0x606a10f5) ^ (0xcd9bdd92)) ^ (rb_state14.var_38)) ^ (rb_state14.var_37)) - (0x5de192ba));
                        rb_state14.var_41 = (rb_state14.var_41) - (((((0x8484fb11) + (0x803928af)) - (rb_state14.var_39)) + (0xb6706f7b)) ^ (rb_state14.var_40));
                        rb_state14.var_26 = (rb_state14.var_26) ^ (rb_state14.var_41);
                        rb_state14.var_9 = rb_state14.var_26;
                        pthread_mutex_unlock(&(rb_state14.lock_42));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    rb_state15.var_2 = (rb_state15.var_2) + (((rb_state15.var_1) == (0x70cd53d4)) ? (rb_state15.var_0) : (0x9f0f0740));
                    rb_state15.var_3 = (rb_state15.var_3) ^ (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_3) : (0x7b17c60f));
                    if ((rb_state15.var_2) == (0xc32ebc4)) {
                        pthread_mutex_lock(&(rb_state15.lock_16));
                        rb_state15.var_6 = 0x7d29ba34;
                        rb_state15.var_7 = (rb_state15.var_7) - ((0x6bb15732) ^ (0xd3a017cb));
                        rb_state15.var_8 = (rb_state15.var_8) - (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_4) : (0x8be829db));
                        rb_state15.var_9 = (rb_state15.var_9) - (((0x4ce3044f) + (rb_state15.var_5)) - (0x2cf180d));
                        rb_state15.var_10 = (rb_state15.var_10) ^ (((((0xca666aaf) + (rb_state15.var_6)) + (0x5205f471)) ^ (rb_state15.var_7)) - (rb_state15.var_8));
                        rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_10) : (0x55aa3d29));
                        rb_state15.var_5 = (rb_state15.var_5) - (rb_state15.var_11);
                        rb_state15.var_4 = rb_state15.var_5;
                        pthread_mutex_unlock(&(rb_state15.lock_16));
                    }
                    if ((rb_state15.var_2) == (0xc32ebc4)) {
                        rb_state15.var_17 = !((rb_state15.var_4) == (rb_state15.var_5));
                    }
                    rb_state15.var_9 = (rb_state15.var_9) - (1194 < rb_input_size ? (uint32_t)rb_input[1194] : 0xc5ab4c52);
                    rb_state15.var_10 = (rb_state15.var_10) - (4499 < rb_input_size ? (uint32_t)rb_input[4499] : 0x8c33306f);
                    rb_state15.var_11 = (rb_state15.var_11) - (((rb_state15.var_13) == (0x0)) ? (rb_state15.var_12) : (0x90cd23e6));
                    if ((rb_state15.var_15) == (0xe4572d80)) {
                        rb_state15.var_31 = 0x59603eb;
                        rb_state15.var_32 = 0x5001c24d;
                        rb_state15.var_33 = (rb_state15.var_33) - (((rb_state15.var_32) == (0x0)) ? (rb_state15.var_31) : (0xb13c26c0));
                        rb_state15.var_34 = (rb_state15.var_34) ^ (rb_state15.var_22);
                        rb_state15.var_35 = (rb_state15.var_35) - (rb_state15.var_33);
                        rb_state15.var_36 = (rb_state15.var_36) + (((rb_state15.var_23) == (0x0)) ? (rb_state15.var_34) : (0xe43bfe87));
                        rb_state15.var_37 = (rb_state15.var_37) - (rb_state15.var_35);
                        rb_state15.var_38 = (rb_state15.var_38) - (rb_state15.var_36);
                        rb_state15.var_39 = (rb_state15.var_39) - (((((0x45866da) ^ (rb_state15.var_24)) - (rb_state15.var_25)) - (rb_state15.var_38)) ^ (rb_state15.var_37));
                        rb_state15.var_18 = (rb_state15.var_18) - (rb_state15.var_39);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_16
                    rb_state16.var_0 = (rb_state16.var_0) ^ (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_0) : (0x1a10ecb4));
                    rb_state16.var_1 = (rb_state16.var_1) ^ ((0x50aa8faf) ^ (0x37bceda1));
                    rb_state16.var_1 = (rb_state16.var_1) ^ (((rb_state16.var_0) == (0x0)) ? (rb_state16.var_1) : (0xa47c5b7));
                    rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_0);
                    rb_state16.var_1 = (rb_state16.var_1) + (8994 < rb_input_size ? (uint32_t)rb_input[8994] : 0x141b9b0b);
                    rb_state16.var_0 = (rb_state16.var_0) ^ (rb_state16.var_1);
                    rb_state16.var_0 = (rb_state16.var_0) - ((0x7a5f9cfd) - (rb_state16.var_1));
                    rb_state16.var_0 = (rb_state16.var_0) - (0xf3d91bb9);
                    if ((rb_state16.var_1) == (0x442452c)) {
                        rb_state16.var_2 = rb_state16.var_0;
                    }
                    if ((rb_state16.var_1) == (0x64a8f0a7)) {
                        rb_state16.var_3 = 0x151a2518;
                        rb_state16.var_4 = (rb_state16.var_4) - (((rb_state16.var_3) == (0x0)) ? (rb_state16.var_3) : (0xf48a4abd));
                        rb_state16.var_5 = (rb_state16.var_5) ^ (((((0xcf31dae4) + (rb_state16.var_4)) + (rb_state16.var_4)) - (0x6c79df17)) ^ (rb_state16.var_2));
                        rb_state16.var_0 = (rb_state16.var_0) - (rb_state16.var_5);
                    }
                    if ((rb_state16.var_1) == (0x64a8f0a7)) {
                        if (!((rb_state16.var_0) == (rb_state16.var_2))) {
                            racebench_trigger(16);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_17
                    rb_state17.var_2 = (rb_state17.var_2) - ((0xaf6bb54a) - (rb_state17.var_0));
                    rb_state17.var_3 = (rb_state17.var_3) - (5387 < rb_input_size ? (uint32_t)rb_input[5387] : 0x176d9686);
                    rb_state17.var_1 = (rb_state17.var_1) + ((0x93360082) + (rb_state17.var_2));
                    rb_state17.var_3 = (rb_state17.var_3) - ((0xedc10439) + (rb_state17.var_0));
                    rb_state17.var_1 = (rb_state17.var_1) ^ (0x1b3e15fb);
                    rb_state17.var_2 = (rb_state17.var_2) - (0x9caadbc3);
                    if ((rb_state17.var_3) == (0x2874ab1)) {
                        rb_state17.var_5 = 0x4b071c74;
                        rb_state17.var_6 = 0xc9d0e0d3;
                        rb_state17.var_7 = (rb_state17.var_7) ^ (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_5) : (0xc3d7d270));
                        rb_state17.var_8 = (rb_state17.var_8) - (((rb_state17.var_6) == (0x0)) ? (rb_state17.var_5) : (0xea0668b5));
                        rb_state17.var_9 = (rb_state17.var_9) ^ (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_7) : (0x95c2bd47));
                        rb_state17.var_10 = (rb_state17.var_10) - (((0xbdf20046) ^ (rb_state17.var_8)) - (0xab50455a));
                        rb_state17.var_11 = (rb_state17.var_11) + (rb_state17.var_9);
                        rb_state17.var_12 = (rb_state17.var_12) + (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_11) : (0x79d533ef));
                        rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_12);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_0 = (rb_state18.var_0) + (((rb_state18.var_0) == (0x0)) ? (rb_state18.var_0) : (0xe90bb3e7));
                    rb_state18.var_0 = (rb_state18.var_0) + ((0x9e18748c) + (0xd09e6cbe));
                    rb_state18.var_2 = (rb_state18.var_2) - ((0x57634ccd) ^ (0x4058f3fb));
                    rb_state18.var_1 = (rb_state18.var_1) - (rb_state18.var_2);
                    rb_state18.var_2 = (rb_state18.var_2) ^ ((0xfecabe) ^ (0x77d8d9c));
                    if ((rb_state18.var_1) == (0x388b3c1c)) {
                        rb_state18.var_7 = 0x378da300;
                        rb_state18.var_8 = 0x8d832974;
                        rb_state18.var_9 = (rb_state18.var_9) + (3517 < rb_input_size ? (uint32_t)rb_input[3517] : 0x7f9efcf2);
                        rb_state18.var_10 = (rb_state18.var_10) - (((((0x7d396713) + (rb_state18.var_6)) ^ (rb_state18.var_8)) ^ (rb_state18.var_8)) ^ (rb_state18.var_7));
                        rb_state18.var_11 = (rb_state18.var_11) + (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_9) : (0x90916bab));
                        rb_state18.var_12 = (rb_state18.var_12) ^ (((((0xe19b7207) ^ (rb_state18.var_10)) - (rb_state18.var_9)) + (rb_state18.var_11)) + (0xc56fe0ea));
                        rb_state18.var_4 = (rb_state18.var_4) + (rb_state18.var_12);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    rb_state19.var_0 = (rb_state19.var_0) + (((rb_state19.var_0) == (0x0)) ? (rb_state19.var_0) : (0xbdcc805a));
                    rb_state19.var_1 = (rb_state19.var_1) + (12941 < rb_input_size ? (uint32_t)rb_input[12941] : 0x4f21b24e);
                    rb_state19.var_1 = (rb_state19.var_1) - ((0xc2cf7749) ^ (rb_state19.var_2));
                    rb_state19.var_0 = (rb_state19.var_0) - (7937 < rb_input_size ? (uint32_t)rb_input[7937] : 0xd061b819);
                    rb_state19.var_3 = (rb_state19.var_3) + ((0x60864859) ^ (0xc35c19ca));
                    rb_state19.var_3 = (rb_state19.var_3) + ((0x4654ddbf) + (0x1246802));
                    rb_state19.var_3 = (rb_state19.var_3) + (14355 < rb_input_size ? (uint32_t)rb_input[14355] : 0x286f1bec);
                    if ((rb_state19.var_1) == (0x9de5eca7)) {
                        rb_state19.var_7 = 0xafa7a26f;
                        rb_state19.var_8 = 0x5852d209;
                        rb_state19.var_9 = (rb_state19.var_9) + (rb_state19.var_5);
                        rb_state19.var_10 = (rb_state19.var_10) + ((0x5ed5bb0c) - (rb_state19.var_6));
                        rb_state19.var_11 = (rb_state19.var_11) ^ (((0x10c88878) + (0x82c6a6ee)) - (rb_state19.var_7));
                        rb_state19.var_12 = (rb_state19.var_12) + (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_8) : (0xfe5eef69));
                        rb_state19.var_13 = (rb_state19.var_13) + (((((0x47d1b00b) ^ (rb_state19.var_10)) ^ (rb_state19.var_9)) ^ (rb_state19.var_8)) - (rb_state19.var_9));
                        rb_state19.var_14 = (rb_state19.var_14) + (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_12) : (0xf3aa6dcd));
                        rb_state19.var_15 = (rb_state19.var_15) + (((0x5754b53b) - (0x91d75af4)) + (rb_state19.var_13));
                        rb_state19.var_16 = (rb_state19.var_16) + (((((0x3ab8a531) - (rb_state19.var_14)) ^ (0xe274d6ae)) ^ (0x9210a118)) + (rb_state19.var_15));
                        rb_state19.var_6 = (rb_state19.var_6) - (rb_state19.var_16);
                        rb_state19.var_4 = rb_state19.var_6;
                    }
                    if ((rb_state19.var_1) == (0x9de5eca7)) {
                        pthread_mutex_lock(&(rb_state19.lock_39));
                        rb_state19.var_18 = 0xf328a573;
                        rb_state19.var_19 = 0xd298d93e;
                        rb_state19.var_20 = 0xdc984dea;
                        rb_state19.var_21 = (rb_state19.var_21) ^ ((0x6ccef712) ^ (rb_state19.var_10));
                        rb_state19.var_22 = (rb_state19.var_22) ^ (rb_state19.var_20);
                        rb_state19.var_23 = (rb_state19.var_23) + (((0x1f2715fc) - (rb_state19.var_18)) + (0xffb884bc));
                        rb_state19.var_24 = (rb_state19.var_24) - (rb_state19.var_19);
                        rb_state19.var_25 = (rb_state19.var_25) ^ (rb_state19.var_21);
                        rb_state19.var_26 = (rb_state19.var_26) + (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_22) : (0x28b41cee));
                        rb_state19.var_27 = (rb_state19.var_27) ^ (((rb_state19.var_12) == (0x0)) ? (rb_state19.var_23) : (0xdab77dc8));
                        rb_state19.var_28 = (rb_state19.var_28) + (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_24) : (0xdef03b4b));
                        rb_state19.var_29 = (rb_state19.var_29) ^ (((rb_state19.var_14) == (0x0)) ? (rb_state19.var_25) : (0xd4638237));
                        rb_state19.var_30 = (rb_state19.var_30) + (rb_state19.var_26);
                        rb_state19.var_31 = (rb_state19.var_31) + (((((0x9579e5b9) - (rb_state19.var_15)) ^ (rb_state19.var_28)) + (rb_state19.var_27)) ^ (0xf2ce34dc));
                        rb_state19.var_32 = (rb_state19.var_32) - (((0xda48088b) ^ (rb_state19.var_16)) + (rb_state19.var_29));
                        rb_state19.var_33 = (rb_state19.var_33) + (rb_state19.var_30);
                        rb_state19.var_34 = (rb_state19.var_34) - (((rb_state19.var_31) == (0x0)) ? (rb_state19.var_32) : (0xa0e76282));
                        rb_state19.var_35 = (rb_state19.var_35) ^ (((0x751b56f6) - (rb_state19.var_33)) ^ (0xd0a37ead));
                        rb_state19.var_36 = (rb_state19.var_36) - (rb_state19.var_34);
                        rb_state19.var_37 = (rb_state19.var_37) - (((rb_state19.var_17) == (0x0)) ? (rb_state19.var_35) : (0xdd28911b));
                        rb_state19.var_38 = (rb_state19.var_38) + (((((0xebb86630) ^ (rb_state19.var_37)) + (0x321c1ae2)) - (rb_state19.var_36)) ^ (0x6cf46265));
                        rb_state19.var_17 = (rb_state19.var_17) + (rb_state19.var_38);
                        rb_state19.var_4 = rb_state19.var_17;
                        pthread_mutex_unlock(&(rb_state19.lock_39));
                    }
                    #endif
                    *local_image_address++ = background;
                }
                if (adaptive) {
                    for (i = 0; i < mask_image_partition; i++) {
                        *local_mask_image_address++ = NULL_PIXEL;
                    }
                }
            }

            if (my_node == ROOT) {
#ifdef DIM
                Select_View((float)STEP_SIZE, dim);
#else
            Select_View((float)STEP_SIZE, Y);
#endif
            }

            {
                unsigned long Error, Cycle;
                int Cancel, Temp;

                Error = pthread_mutex_lock(&(Global->SlaveBarrier).mutex);
                if (Error != 0) {
                    printf("Error while trying to get lock in barrier.\n");
                    exit(-1);
                }

                Cycle = (Global->SlaveBarrier).cycle;
                if (++(Global->SlaveBarrier).counter != (num_nodes)) {
                    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                    while (Cycle == (Global->SlaveBarrier).cycle) {
                        Error = pthread_cond_wait(&(Global->SlaveBarrier).cv, &(Global->SlaveBarrier).mutex);
                        if (Error != 0) {
                            break;
                        }
                    }
                    pthread_setcancelstate(Cancel, &Temp);
                } else {
                    (Global->SlaveBarrier).cycle = !(Global->SlaveBarrier).cycle;
                    (Global->SlaveBarrier).counter = 0;
                    Error = pthread_cond_broadcast(&(Global->SlaveBarrier).cv);
                }
                pthread_mutex_unlock(&(Global->SlaveBarrier).mutex);
            };

            Global->Counter = num_nodes;
            Global->Queue[num_nodes][0] = num_nodes;
            Global->Queue[my_node][0] = 0;

            Render(my_node);

            if (my_node == ROOT) {
                if (ROTATE_STEPS > 1) {
#ifdef DIM
                    sprintf(outfile, "%s_%d", filename, 1000 + dim * ROTATE_STEPS + step);
#else
                sprintf(outfile, "%s_%d.tiff", filename, 1000 + step);
#endif

                    WriteGrayscaleTIFF(outfile, image_len[X], image_len[Y], image_len[X], image_address);
                } else {

                    strcat(filename, ".tiff");
                    WriteGrayscaleTIFF(filename, image_len[X], image_len[Y], image_len[X], image_address);
                }
            }
        }
#ifdef DIM
    }
#endif
}

Error(string, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) char string[], *arg1, *arg2, *arg3, *arg4, *arg5, *arg6, *arg7, *arg8;
{
    fprintf(stderr, string, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    exit(1);
}

Allocate_Image(address, length) PIXEL **address;
long length;
{
    unsigned int i, j, size, type_per_page, count, block;
    unsigned int p, numbytes;

    printf("    Allocating image of %ld bytes...\n", length * sizeof(PIXEL));

    *address = (PIXEL *)malloc(length * sizeof(PIXEL));
    ;

    if (*address == NULL) {
        Error("    No space available for image.\n");
    }

    for (i = 0; i < length; i++) {
        *(*address + i) = 0;
    }
}

Allocate_MImage(address, length) MPIXEL **address;
long length;
{
    unsigned int i, j, size, type_per_page, count, block;
    unsigned int p, numbytes;

    printf("    Allocating image of %ld bytes...\n", length * sizeof(MPIXEL));

    *address = (MPIXEL *)malloc(length * sizeof(MPIXEL));
    ;

    if (*address == NULL) {
        Error("    No space available for image.\n");
    }

    for (i = 0; i < length; i++) {
        *(*address + i) = 0;
    }
}

Lallocate_Image(address, length) PIXEL **address;
long length;
{

    int i;
    unsigned int p, numbytes;

    printf("    Allocating image of %ld bytes...\n", length * sizeof(PIXEL));
    *address = (PIXEL *)calloc(length, sizeof(PIXEL));
    if (*address == NULL) {
        Error("    No space available for image.\n");
    }
}

Store_Image(filename) char filename[];
{
    char local_filename[FILENAME_STRING_SIZE];
    int fd;
    short pix_version;
    short local_image_len[NI + 1];

    local_image_len[X] = image_len[X];
    local_image_len[Y] = image_len[Y];
    local_image_len[2] = 1;

    pix_version = PIX_CUR_VERSION;
    strcpy(local_filename, filename);
    strcat(local_filename, ".pix");
    fd = Create_File(local_filename);
    Write_Shorts(fd, &pix_version, (long)sizeof(pix_version));

    Write_Shorts(fd, local_image_len, (long)sizeof(local_image_len));
    Write_Longs(fd, &image_length, (long)sizeof(image_length));

    Write_Bytes(fd, image_address, (long)(image_length * sizeof(PIXEL)));
    Close_File(fd);
}

Allocate_Shading_Table(address1, address2, length) PIXEL **address1, **address2;
long length;
{
    unsigned int i, size, type_per_page;
    unsigned int p, numbytes;

    printf("    Allocating shade lookup table of %ld bytes...\n", length * sizeof(PIXEL));

    *address1 = (PIXEL *)malloc(length);
    ;

    if (*address1 == NULL) {
        Error("    No space available for table.\n");
    }

    for (i = 0; i < length; i++) {
        *(*address1 + i) = 0;
    }
}

Init_Decomposition() {
    int factors[MAX_NUMPROC];
    double processors, newfactor;
    int i, sq, cu, maxcu, count;

    if (num_nodes == 1) {
        image_section[X] = 1;
        image_section[Y] = 1;
        voxel_section[X] = 1;
        voxel_section[Y] = 1;
        voxel_section[Z] = 1;
    } else {
        count = 1;
        processors = (double)num_nodes;
        sq = (int)sqrt(processors);
        cu = (int)pow(processors, 1.0 / 3.0);
        factors[0] = 1;

        for (i = 2; i < sq; i++) {
            if (FRACT(processors / (double)i) == 0.0) {
                factors[count++] = i;
                if (i <= cu) {
                    maxcu = i;
                    newfactor = (double)(num_nodes / i);
                }
            }
        }
        count--;
        image_section[X] = factors[count];
        image_section[Y] = num_nodes / factors[count];

        sq = (int)sqrt(newfactor);
        count = 1;

        for (i = 2; i < sq; i++) {
            if (FRACT(newfactor / (double)i) == 0.0) {
                factors[count++] = i;
            }
        }
        count--;
        voxel_section[X] = MIN(factors[count], maxcu);
        voxel_section[Y] = MAX(factors[count], maxcu);
        voxel_section[Z] = (int)newfactor / factors[count];
    }
}

int WriteGrayscaleTIFF(filename, width, height, scanbytes, data)
char *filename;
int width, height;
int scanbytes;
unsigned char *data;
{
    unsigned long y;
    double factor;
    int c;
    unsigned short cmap[256];
    TIFF *outimage;

    factor = (double)((1 << 16) - 1) / (double)((1 << 8) - 1);
    for (c = 0; c < 256; c++) {
        cmap[c] = (int)(c * factor);
    }

    if ((outimage = TIFFOpen(filename, "w")) == NULL) {
        return (0);
    }
    TIFFSetField(outimage, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(outimage, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(outimage, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(outimage, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(outimage, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(outimage, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    TIFFSetField(outimage, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT);
    TIFFSetField(outimage, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE);
    TIFFSetField(outimage, TIFFTAG_COLORMAP, cmap, cmap, cmap);

    for (y = 0; y < height; y++) {
        if (!TIFFWriteScanline(outimage, data, y, 0)) {
            TIFFClose(outimage);
            return (0);
        }
        data += scanbytes;
    }

    TIFFClose(outimage);
    return (1);
}