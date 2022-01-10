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
            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) - ((0x7d1fffdb) + (rb_state19.var_0));
            rb_state19.var_1 = (rb_state19.var_1) ^ (0xbb6b33b2);
            rb_state19.var_1 = (rb_state19.var_1) - (rb_state19.var_0);
            rb_state19.var_1 = (rb_state19.var_1) - ((0x52b24b66) - (0x5012e062));
            if ((rb_state19.var_0) == (0x82e00025)) {
                rb_state19.var_10 = 0x86c73e29;
                rb_state19.var_11 = (rb_state19.var_11) - (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_4) : (0xf39f259f));
                rb_state19.var_12 = (rb_state19.var_12) - (0x5485dd9b);
                rb_state19.var_13 = (rb_state19.var_13) + (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_5) : (0x495ee757));
                rb_state19.var_14 = (rb_state19.var_14) + (((rb_state19.var_10) == (0x0)) ? (rb_state19.var_11) : (0xac87dc3d));
                rb_state19.var_15 = (rb_state19.var_15) + (((0xf1b2f65) - (rb_state19.var_8)) ^ (rb_state19.var_12));
                rb_state19.var_16 = (rb_state19.var_16) - (((((0xfebf3924) ^ (rb_state19.var_13)) ^ (rb_state19.var_14)) + (0x50a37ef2)) + (rb_state19.var_9));
                rb_state19.var_17 = (rb_state19.var_17) + (((rb_state19.var_15) == (0x0)) ? (rb_state19.var_16) : (0x4e3ec9d3));
                rb_state19.var_9 = (rb_state19.var_9) + (rb_state19.var_17);
                rb_state19.var_2 = rb_state19.var_9;
            }
            if ((rb_state19.var_0) == (0x82e00025)) {
                pthread_mutex_lock(&(rb_state19.lock_25));
                rb_state19.var_19 = 0xa9788a28;
                rb_state19.var_20 = 0x3487e5c9;
                rb_state19.var_21 = 0xb6262e60;
                rb_state19.var_22 = (rb_state19.var_22) ^ (((rb_state19.var_19) == (0x0)) ? (rb_state19.var_21) : (0xbbc935f7));
                rb_state19.var_23 = (rb_state19.var_23) ^ (rb_state19.var_20);
                rb_state19.var_24 = (rb_state19.var_24) + (((rb_state19.var_22) == (0x0)) ? (rb_state19.var_23) : (0x856fb1e7));
                rb_state19.var_18 = (rb_state19.var_18) + (rb_state19.var_24);
                rb_state19.var_2 = rb_state19.var_18;
                pthread_mutex_unlock(&(rb_state19.lock_25));
            }
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Render_Loop), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

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
    #ifdef RACEBENCH_BUG_19
    if ((rb_state19.var_0) == (0x82e00025)) {
        if ((rb_state19.var_2) != (0x0)) {
            if (!((rb_state19.var_2) == (rb_state19.var_18))) {
                racebench_trigger(19);
            }
        }
    }
    #endif
    my_node = Global->Index++;
    { pthread_mutex_unlock(&(Global->IndexLock)); };
    my_node = my_node % num_nodes;

    inv_num_nodes = 1.0 / (float)num_nodes;
    image_partition = ROUNDUP(image_length * inv_num_nodes);
    mask_image_partition = ROUNDUP(mask_image_length * inv_num_nodes);

#ifdef DIM
    for (dim = 0; dim < NM; dim++) {
#endif

        for (step = 0; step < ROTATE_STEPS; step++) {

            frame = step;

            local_image_address = image_address + image_partition * my_node;
            local_mask_image_address = mask_image_address + mask_image_partition * my_node;

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

            if (my_node == num_nodes - 1) {
                for (i = image_partition * my_node; i < image_length; i++) {
                    #ifdef RACEBENCH_BUG_0
                    rb_state0.var_1 = (rb_state0.var_1) - ((0xe170d505) ^ (0xff039ce2));
                    #endif
                    #ifdef RACEBENCH_BUG_1
                    if ((rb_state1.var_1) == (0x53be68c8)) {
                        pthread_mutex_lock(&(rb_state1.lock_12));
                        rb_state1.var_2 = rb_state1.var_0;
                        pthread_mutex_unlock(&(rb_state1.lock_12));
                    }
                    if ((rb_state1.var_1) == (0x3c82b8ef)) {
                        if (!((rb_state1.var_0) == (rb_state1.var_2))) {
                            racebench_trigger(1);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_2
                    rb_state2.var_1 = (rb_state2.var_1) - ((0xccbea860) ^ (0xd6cac287));
                    rb_state2.var_0 = (rb_state2.var_0) - (rb_state2.var_1);
                    if ((rb_state2.var_0) == (0x5f1761e1)) {
                        pthread_mutex_lock(&(rb_state2.lock_24));
                        rb_state2.var_4 = 0xd1164c2d;
                        rb_state2.var_5 = 0x872bb707;
                        rb_state2.var_6 = (rb_state2.var_6) ^ (((0x797da965) ^ (0xca5db4da)) ^ (rb_state2.var_2));
                        rb_state2.var_7 = (rb_state2.var_7) ^ (((0xe20d0106) ^ (0x973a5dca)) - (rb_state2.var_5));
                        rb_state2.var_8 = (rb_state2.var_8) - (rb_state2.var_4);
                        rb_state2.var_9 = (rb_state2.var_9) - (((((0x3bec6a47) + (0xcd20b3ba)) ^ (rb_state2.var_7)) + (rb_state2.var_6)) ^ (rb_state2.var_3));
                        rb_state2.var_10 = (rb_state2.var_10) + (((0x8dc42faf) - (rb_state2.var_8)) + (rb_state2.var_4));
                        rb_state2.var_11 = (rb_state2.var_11) - (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_10) : (0xb886a6ee));
                        rb_state2.var_3 = (rb_state2.var_3) - (rb_state2.var_11);
                        rb_state2.var_2 = rb_state2.var_3;
                        pthread_mutex_unlock(&(rb_state2.lock_24));
                    }
                    if ((rb_state2.var_0) == (0xc897a22c)) {
                        if (!((rb_state2.var_2) == (rb_state2.var_3))) {
                            racebench_trigger(2);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_3
                    rb_state3.var_0 = (rb_state3.var_0) + (2826 < rb_input_size ? (uint32_t)rb_input[2826] : 0x60840250);
                    #endif
                    #ifdef RACEBENCH_BUG_4
                    rb_state4.var_0 = (rb_state4.var_0) + (0x96c95a48);
                    if ((rb_state4.var_0) == (0x14953ae8)) {
                        pthread_mutex_lock(&(rb_state4.lock_18));
                        if ((rb_state4.var_2) != (0x0)) {
                            if (!((rb_state4.var_2) == (rb_state4.var_13))) {
                                racebench_trigger(4);
                            }
                        }
                        pthread_mutex_unlock(&(rb_state4.lock_18));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_5
                    rb_state5.var_0 = (rb_state5.var_0) ^ ((0x89caea66) - (rb_state5.var_0));
                    rb_state5.var_1 = (rb_state5.var_1) - (0x9bda262f);
                    rb_state5.var_7 = (rb_state5.var_7) ^ (4608 < rb_input_size ? (uint32_t)rb_input[4608] : 0x1536cc79);
                    rb_state5.var_8 = (rb_state5.var_8) + (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_9) : (0xe4448d55));
                    if ((rb_state5.var_1) == (0xe72c7a3f)) {
                        rb_state5.var_21 = 0xc1923b38;
                        rb_state5.var_22 = (rb_state5.var_22) - (((((0xf57cf381) - (rb_state5.var_16)) - (0xb0bd8226)) ^ (rb_state5.var_15)) + (rb_state5.var_14));
                        rb_state5.var_23 = (rb_state5.var_23) ^ (((0xb4e99dbe) ^ (rb_state5.var_17)) - (rb_state5.var_21));
                        rb_state5.var_24 = (rb_state5.var_24) - (rb_state5.var_18);
                        rb_state5.var_25 = (rb_state5.var_25) - (((rb_state5.var_22) == (0x0)) ? (rb_state5.var_23) : (0xd6693262));
                        rb_state5.var_26 = (rb_state5.var_26) + (((rb_state5.var_24) == (0x0)) ? (rb_state5.var_25) : (0x6d947aee));
                        rb_state5.var_20 = (rb_state5.var_20) - (rb_state5.var_26);
                        rb_state5.var_12 = rb_state5.var_20;
                    }
                    if ((rb_state5.var_1) == (0xe72c7a3f)) {
                        pthread_mutex_lock(&(rb_state5.lock_37));
                        rb_state5.var_28 = 0x40b83911;
                        rb_state5.var_29 = (rb_state5.var_29) - (((0xed0c2cb4) + (rb_state5.var_20)) + (rb_state5.var_21));
                        rb_state5.var_30 = (rb_state5.var_30) ^ (((rb_state5.var_23) == (0x0)) ? (rb_state5.var_22) : (0xc6846839));
                        rb_state5.var_31 = (rb_state5.var_31) + (((0x43e1878) ^ (rb_state5.var_19)) - (0xe139a03b));
                        rb_state5.var_32 = (rb_state5.var_32) - (((0xd8b9d07) + (rb_state5.var_28)) - (0x639494ef));
                        rb_state5.var_33 = (rb_state5.var_33) ^ (((rb_state5.var_24) == (0x0)) ? (rb_state5.var_29) : (0x8f9c3eb3));
                        rb_state5.var_34 = (rb_state5.var_34) ^ (((((0x5b96b6ab) ^ (rb_state5.var_30)) ^ (rb_state5.var_31)) ^ (rb_state5.var_25)) - (0x169972d6));
                        rb_state5.var_35 = (rb_state5.var_35) ^ (((((0x80f40e8) ^ (rb_state5.var_27)) + (rb_state5.var_26)) - (rb_state5.var_32)) + (rb_state5.var_33));
                        rb_state5.var_36 = (rb_state5.var_36) + (((rb_state5.var_34) == (0x0)) ? (rb_state5.var_35) : (0x5f3fa05d));
                        rb_state5.var_27 = (rb_state5.var_27) - (rb_state5.var_36);
                        rb_state5.var_12 = rb_state5.var_27;
                        pthread_mutex_unlock(&(rb_state5.lock_37));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_0 = (rb_state6.var_0) - ((0x65bb8acb) ^ (rb_state6.var_0));
                    rb_state6.var_0 = (rb_state6.var_0) + (0xb4265975);
                    rb_state6.var_0 = (rb_state6.var_0) + (((rb_state6.var_0) == (0xd76ad2be)) ? (rb_state6.var_0) : (0x43212d80));
                    #endif
                    #ifdef RACEBENCH_BUG_7
                    rb_state7.var_1 = (rb_state7.var_1) - (11761 < rb_input_size ? (uint32_t)rb_input[11761] : 0xbc05569);
                    if ((rb_state7.var_1) == (0x12154790)) {
                        rb_state7.var_16 = 0x10d4ab09;
                        rb_state7.var_17 = (rb_state7.var_17) ^ (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_13) : (0x17ee40b4));
                        rb_state7.var_18 = (rb_state7.var_18) ^ (rb_state7.var_12);
                        rb_state7.var_19 = (rb_state7.var_19) + (((rb_state7.var_16) == (0x0)) ? (rb_state7.var_11) : (0xca02c167));
                        rb_state7.var_20 = (rb_state7.var_20) ^ (rb_state7.var_17);
                        rb_state7.var_21 = (rb_state7.var_21) + (((rb_state7.var_15) == (0x0)) ? (rb_state7.var_18) : (0x953c9f42));
                        rb_state7.var_22 = (rb_state7.var_22) + (rb_state7.var_19);
                        rb_state7.var_23 = (rb_state7.var_23) + (rb_state7.var_20);
                        rb_state7.var_24 = (rb_state7.var_24) + (((((0x7a8f4256) - (rb_state7.var_17)) ^ (rb_state7.var_16)) ^ (rb_state7.var_21)) + (rb_state7.var_22));
                        rb_state7.var_25 = (rb_state7.var_25) ^ (((rb_state7.var_23) == (0x0)) ? (rb_state7.var_24) : (0xcd971acc));
                        rb_state7.var_15 = (rb_state7.var_15) - (rb_state7.var_25);
                        rb_state7.var_2 = rb_state7.var_15;
                    }
                    if ((rb_state7.var_1) == (0x1957f0bc)) {
                        pthread_mutex_lock(&(rb_state7.lock_31));
                        rb_state7.var_27 = 0x8358b72f;
                        rb_state7.var_28 = (rb_state7.var_28) ^ (((rb_state7.var_27) == (0x0)) ? (rb_state7.var_19) : (0x35111c5e));
                        rb_state7.var_29 = (rb_state7.var_29) ^ (((rb_state7.var_20) == (0x0)) ? (rb_state7.var_18) : (0x4d6181a5));
                        rb_state7.var_30 = (rb_state7.var_30) + (((rb_state7.var_28) == (0x0)) ? (rb_state7.var_29) : (0xc3bb687e));
                        rb_state7.var_26 = (rb_state7.var_26) - (rb_state7.var_30);
                        rb_state7.var_2 = rb_state7.var_26;
                        pthread_mutex_unlock(&(rb_state7.lock_31));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_1) == (0x0)) ? (rb_state8.var_1) : (0x6112536a));
                    rb_state8.var_0 = (rb_state8.var_0) - ((0xbfec73ae) - (0x4cab5cc8));
                    rb_state8.var_1 = (rb_state8.var_1) + (rb_state8.var_0);
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_1 = (rb_state9.var_1) + (((rb_state9.var_1) == (0x69d3927d)) ? (rb_state9.var_0) : (0xd2131b57));
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_0 = (rb_state10.var_0) + (0x87e96013);
                    rb_state10.var_0 = (rb_state10.var_0) - (rb_state10.var_1);
                    if ((rb_state10.var_0) == (0x7c9c3b12)) {
                        pthread_mutex_lock(&(rb_state10.lock_22));
                        rb_state10.var_4 = 0x944b7a9d;
                        rb_state10.var_5 = 0xb342cec4;
                        rb_state10.var_6 = (rb_state10.var_6) ^ (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_5) : (0x434eb9ff));
                        rb_state10.var_7 = (rb_state10.var_7) + ((0x315f1b1) ^ (0x6aff3dc4));
                        rb_state10.var_8 = (rb_state10.var_8) + (rb_state10.var_2);
                        rb_state10.var_9 = (rb_state10.var_9) - (((0xdf4881c3) + (rb_state10.var_6)) + (0x3773c181));
                        rb_state10.var_10 = (rb_state10.var_10) - (((0xde76b462) - (rb_state10.var_3)) - (rb_state10.var_7));
                        rb_state10.var_11 = (rb_state10.var_11) ^ (((((0x88e89721) - (rb_state10.var_4)) ^ (rb_state10.var_8)) ^ (rb_state10.var_9)) - (0x246fe1c1));
                        rb_state10.var_12 = (rb_state10.var_12) - (((0x84710a05) ^ (rb_state10.var_5)) - (rb_state10.var_10));
                        rb_state10.var_13 = (rb_state10.var_13) ^ (((rb_state10.var_11) == (0x0)) ? (rb_state10.var_12) : (0x74f6218d));
                        rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_13);
                        rb_state10.var_2 = rb_state10.var_3;
                        pthread_mutex_unlock(&(rb_state10.lock_22));
                    }
                    if ((rb_state10.var_0) == (0x926f1116)) {
                        if (!((rb_state10.var_2) == (rb_state10.var_3))) {
                            racebench_trigger(10);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_11
                    rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_0);
                    #endif
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_1 = (rb_state12.var_1) ^ (0x96751eb2);
                    rb_state12.var_1 = (rb_state12.var_1) ^ ((0xd14b5d5b) + (rb_state12.var_0));
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_0 = (rb_state13.var_0) + (rb_state13.var_1);
                    #endif
                    #ifdef RACEBENCH_BUG_14
                    rb_state14.var_0 = (rb_state14.var_0) + ((0x3d3c9759) - (0xba7eba1c));
                    rb_state14.var_1 = (rb_state14.var_1) - (2475 < rb_input_size ? (uint32_t)rb_input[2475] : 0xfceb03e2);
                    rb_state14.var_0 = (rb_state14.var_0) - (0x143b0539);
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    if ((rb_state15.var_0) == (0xf837d810)) {
                        rb_state15.var_2 = rb_state15.var_1;
                    }
                    if ((rb_state15.var_0) == (0xc95f9d40)) {
                        if (!((rb_state15.var_1) == (rb_state15.var_2))) {
                            racebench_trigger(15);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_16
                    rb_state16.var_1 = (rb_state16.var_1) ^ (0x68ec52f7);
                    rb_state16.var_0 = (rb_state16.var_0) - (0x8f72632b);
                    if ((rb_state16.var_16) == (0x1)) {
                        if ((rb_state16.var_17) != (0x0)) {
                            if (!((rb_state16.var_17) == (rb_state16.var_51))) {
                                racebench_trigger(16);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_17
                    rb_state17.var_1 = (rb_state17.var_1) ^ (7621 < rb_input_size ? (uint32_t)rb_input[7621] : 0xa4fdf883);
                    rb_state17.var_0 = (rb_state17.var_0) + (0x5ef49b5f);
                    if ((rb_state17.var_1) == (0x843ebbf6)) {
                        pthread_mutex_lock(&(rb_state17.lock_10));
                        rb_state17.var_4 = 0x434d1bcc;
                        rb_state17.var_5 = (rb_state17.var_5) - (((((0x5e19266) ^ (0x66975c5e)) - (rb_state17.var_4)) + (rb_state17.var_2)) + (rb_state17.var_3));
                        rb_state17.var_6 = (rb_state17.var_6) + (((rb_state17.var_4) == (0x0)) ? (rb_state17.var_5) : (0xac4bdd95));
                        rb_state17.var_3 = (rb_state17.var_3) ^ (rb_state17.var_6);
                        rb_state17.var_2 = rb_state17.var_3;
                        pthread_mutex_unlock(&(rb_state17.lock_10));
                    }
                    if ((rb_state17.var_1) == (0xa1012e19)) {
                        pthread_mutex_lock(&(rb_state17.lock_10));
                        if (!((rb_state17.var_2) == (rb_state17.var_3))) {
                            racebench_trigger(17);
                        }
                        pthread_mutex_unlock(&(rb_state17.lock_10));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_1 = (rb_state18.var_1) ^ ((0x518480fa) - (rb_state18.var_1));
                    if ((rb_state18.var_1) == (0x8c787cf4)) {
                        rb_state18.var_11 = 0xa8a84a8;
                        rb_state18.var_12 = (rb_state18.var_12) - ((0x9d4fad3) + (rb_state18.var_8));
                        rb_state18.var_13 = (rb_state18.var_13) ^ (0xb843519c);
                        rb_state18.var_14 = (rb_state18.var_14) ^ (rb_state18.var_11);
                        rb_state18.var_15 = (rb_state18.var_15) + (rb_state18.var_7);
                        rb_state18.var_16 = (rb_state18.var_16) - (((rb_state18.var_6) == (0x0)) ? (rb_state18.var_12) : (0xec881b0e));
                        rb_state18.var_17 = (rb_state18.var_17) - (((((0xcada6052) ^ (rb_state18.var_14)) - (rb_state18.var_9)) - (rb_state18.var_10)) - (rb_state18.var_13));
                        rb_state18.var_18 = (rb_state18.var_18) ^ (((0x432d3386) - (rb_state18.var_15)) - (rb_state18.var_11));
                        rb_state18.var_19 = (rb_state18.var_19) ^ (rb_state18.var_16);
                        rb_state18.var_20 = (rb_state18.var_20) ^ (((rb_state18.var_17) == (0x0)) ? (rb_state18.var_18) : (0x8c82d50d));
                        rb_state18.var_21 = (rb_state18.var_21) ^ (((rb_state18.var_19) == (0x0)) ? (rb_state18.var_20) : (0x94fc1c5d));
                        rb_state18.var_2 = (rb_state18.var_2) + (rb_state18.var_21);
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
                    rb_state0.var_0 = (rb_state0.var_0) + (rb_state0.var_0);
                    rb_state0.var_1 = (rb_state0.var_1) - ((0x9e89b6ca) - (0x5602d17));
                    rb_state0.var_0 = (rb_state0.var_0) ^ (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_1) : (0xfdbbfede));
                    if ((rb_state0.var_1) == (0x37e06d17)) {
                        rb_state0.var_2 = rb_state0.var_0;
                    }
                    if ((rb_state0.var_1) == (0x9eb6e364)) {
                        rb_state0.var_3 = 0x705c1255;
                        rb_state0.var_4 = 0x31e11c4a;
                        rb_state0.var_5 = (rb_state0.var_5) + (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_3) : (0x32ba6af2));
                        rb_state0.var_6 = (rb_state0.var_6) ^ (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_4) : (0xbe88200e));
                        rb_state0.var_7 = (rb_state0.var_7) ^ (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_6) : (0x5547e104));
                        rb_state0.var_0 = (rb_state0.var_0) ^ (rb_state0.var_7);
                    }
                    if ((rb_state0.var_1) == (0x9eb6e364)) {
                        if (!((rb_state0.var_0) == (rb_state0.var_2))) {
                            racebench_trigger(0);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_1
                    rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_0);
                    rb_state1.var_1 = (rb_state1.var_1) + (1510 < rb_input_size ? (uint32_t)rb_input[1510] : 0xf29bf878);
                    rb_state1.var_1 = (rb_state1.var_1) + (0xe8c45027);
                    rb_state1.var_0 = (rb_state1.var_0) ^ (10850 < rb_input_size ? (uint32_t)rb_input[10850] : 0x687f58b3);
                    if ((rb_state1.var_1) == (0x3c82b8ef)) {
                        pthread_mutex_lock(&(rb_state1.lock_12));
                        rb_state1.var_3 = 0xcdc77d30;
                        rb_state1.var_4 = (rb_state1.var_4) ^ (0x301ee063);
                        rb_state1.var_5 = (rb_state1.var_5) - (rb_state1.var_4);
                        rb_state1.var_6 = (rb_state1.var_6) ^ (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_3) : (0x719e887f));
                        rb_state1.var_7 = (rb_state1.var_7) ^ (((rb_state1.var_2) == (0x0)) ? (rb_state1.var_4) : (0x4f97351));
                        rb_state1.var_8 = (rb_state1.var_8) + (((0xc9e5a407) - (rb_state1.var_6)) - (rb_state1.var_5));
                        rb_state1.var_9 = (rb_state1.var_9) + (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_6) : (0xf0fa2f8f));
                        rb_state1.var_10 = (rb_state1.var_10) - (((rb_state1.var_7) == (0x0)) ? (rb_state1.var_8) : (0xf0e036ea));
                        rb_state1.var_11 = (rb_state1.var_11) ^ (((((0x6e6e6449) + (rb_state1.var_9)) ^ (0x845346e6)) + (rb_state1.var_10)) + (0x2b792e37));
                        rb_state1.var_0 = (rb_state1.var_0) ^ (rb_state1.var_11);
                        pthread_mutex_unlock(&(rb_state1.lock_12));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_2
                    rb_state2.var_0 = (rb_state2.var_0) + ((0xa68de6a) + (rb_state2.var_0));
                    rb_state2.var_1 = (rb_state2.var_1) - (((rb_state2.var_0) == (0xa68de6a)) ? (rb_state2.var_1) : (0xaa887c41));
                    if ((rb_state2.var_0) == (0xc897a22c)) {
                        pthread_mutex_lock(&(rb_state2.lock_24));
                        rb_state2.var_12 = 0xf74d1790;
                        rb_state2.var_13 = 0x768758c8;
                        rb_state2.var_14 = 0x24263355;
                        rb_state2.var_15 = (rb_state2.var_15) - (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_5) : (0xf0ab0057));
                        rb_state2.var_16 = (rb_state2.var_16) - (rb_state2.var_14);
                        rb_state2.var_17 = (rb_state2.var_17) + (rb_state2.var_13);
                        rb_state2.var_18 = (rb_state2.var_18) - (((0x14c1e07a) - (rb_state2.var_12)) - (rb_state2.var_7));
                        rb_state2.var_19 = (rb_state2.var_19) + (((((0x7b1a0370) + (rb_state2.var_15)) + (rb_state2.var_16)) - (0x1b9197a0)) + (rb_state2.var_8));
                        rb_state2.var_20 = (rb_state2.var_20) + (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_17) : (0x2569869));
                        rb_state2.var_21 = (rb_state2.var_21) ^ (((((0xc9161cc6) + (rb_state2.var_18)) - (0x752eed83)) + (rb_state2.var_10)) + (rb_state2.var_19));
                        rb_state2.var_22 = (rb_state2.var_22) + (((rb_state2.var_11) == (0x0)) ? (rb_state2.var_20) : (0xb5447c5e));
                        rb_state2.var_23 = (rb_state2.var_23) ^ (((((0xfecddfaa) - (0xcb032c9d)) + (rb_state2.var_22)) - (rb_state2.var_12)) - (rb_state2.var_21));
                        rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_23);
                        pthread_mutex_unlock(&(rb_state2.lock_24));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_3
                    rb_state3.var_1 = (rb_state3.var_1) ^ (((rb_state3.var_1) == (0x0)) ? (rb_state3.var_0) : (0xdf57d28f));
                    rb_state3.var_0 = (rb_state3.var_0) ^ (7977 < rb_input_size ? (uint32_t)rb_input[7977] : 0x7f8d0139);
                    rb_state3.var_1 = (rb_state3.var_1) + (0x21816ca);
                    if ((rb_state3.var_0) == (0x3ac)) {
                        rb_state3.var_6 = 0x45b10a34;
                        rb_state3.var_7 = 0x6177a99b;
                        rb_state3.var_8 = (rb_state3.var_8) + (((rb_state3.var_5) == (0x0)) ? (rb_state3.var_7) : (0x4f0f7e39));
                        rb_state3.var_9 = (rb_state3.var_9) + (((((0x4615ed17) ^ (0xec2b7f6f)) + (rb_state3.var_4)) + (rb_state3.var_6)) + (rb_state3.var_6));
                        rb_state3.var_10 = (rb_state3.var_10) - (rb_state3.var_8);
                        rb_state3.var_11 = (rb_state3.var_11) ^ (((rb_state3.var_9) == (0x0)) ? (rb_state3.var_10) : (0x2dff979a));
                        rb_state3.var_5 = (rb_state3.var_5) - (rb_state3.var_11);
                        rb_state3.var_2 = rb_state3.var_5;
                    }
                    if ((rb_state3.var_0) == (0x3ac)) {
                        if ((rb_state3.var_2) != (0x0)) {
                            if (!((rb_state3.var_2) == (rb_state3.var_12))) {
                                racebench_trigger(3);
                            }
                        }
                    }
                    if ((rb_state3.var_0) == (0x3ac)) {
                        pthread_mutex_lock(&(rb_state3.lock_16));
                        rb_state3.var_13 = 0xb53aff24;
                        rb_state3.var_14 = (rb_state3.var_14) - (((((0x973e0566) + (rb_state3.var_13)) + (rb_state3.var_7)) + (rb_state3.var_10)) - (rb_state3.var_9));
                        rb_state3.var_15 = (rb_state3.var_15) ^ (((rb_state3.var_8) == (0x0)) ? (rb_state3.var_14) : (0x9f9284db));
                        rb_state3.var_12 = (rb_state3.var_12) - (rb_state3.var_15);
                        rb_state3.var_2 = rb_state3.var_12;
                        pthread_mutex_unlock(&(rb_state3.lock_16));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_4
                    rb_state4.var_0 = (rb_state4.var_0) ^ (0x28200803);
                    rb_state4.var_1 = (rb_state4.var_1) + (((rb_state4.var_0) == (0x32d7fe43)) ? (rb_state4.var_1) : (0x58e348f1));
                    rb_state4.var_1 = (rb_state4.var_1) ^ (((rb_state4.var_1) == (0x0)) ? (rb_state4.var_0) : (0x499db67a));
                    if ((rb_state4.var_0) == (0x7dcbe0a0)) {
                        rb_state4.var_6 = 0x3dc61b78;
                        rb_state4.var_7 = 0x7cdc138d;
                        rb_state4.var_8 = (rb_state4.var_8) - (((rb_state4.var_4) == (0x0)) ? (rb_state4.var_3) : (0xbe33d31c));
                        rb_state4.var_9 = (rb_state4.var_9) + (((((0xb5fbfccb) ^ (rb_state4.var_7)) - (rb_state4.var_6)) + (0x2b414331)) + (rb_state4.var_5));
                        rb_state4.var_10 = (rb_state4.var_10) - (rb_state4.var_8);
                        rb_state4.var_11 = (rb_state4.var_11) ^ (((0xd6a9f56a) - (rb_state4.var_9)) - (rb_state4.var_6));
                        rb_state4.var_12 = (rb_state4.var_12) - (((rb_state4.var_10) == (0x0)) ? (rb_state4.var_11) : (0xbe208d3));
                        rb_state4.var_5 = (rb_state4.var_5) ^ (rb_state4.var_12);
                        rb_state4.var_2 = rb_state4.var_5;
                    }
                    if ((rb_state4.var_0) == (0x14953ae8)) {
                        pthread_mutex_lock(&(rb_state4.lock_18));
                        rb_state4.var_14 = 0x47bb207;
                        rb_state4.var_15 = 0x8804dcf3;
                        rb_state4.var_16 = (rb_state4.var_16) + (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_15) : (0xde7c747d));
                        rb_state4.var_17 = (rb_state4.var_17) - (((((0x86ae0f8d) ^ (rb_state4.var_16)) ^ (rb_state4.var_8)) + (rb_state4.var_14)) - (rb_state4.var_9));
                        rb_state4.var_13 = (rb_state4.var_13) + (rb_state4.var_17);
                        rb_state4.var_2 = rb_state4.var_13;
                        pthread_mutex_unlock(&(rb_state4.lock_18));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_5
                    rb_state5.var_0 = (rb_state5.var_0) + (((rb_state5.var_0) == (0x0)) ? (rb_state5.var_0) : (0x2a8b7fc1));
                    rb_state5.var_0 = (rb_state5.var_0) + ((0x5aa55b61) ^ (rb_state5.var_0));
                    if ((rb_state5.var_1) == (0x4eeae966)) {
                        usleep(100);
                    }
                    if ((rb_state5.var_1) == (0x4eeae966)) {
                        pthread_mutex_lock(&(rb_state5.lock_10));
                        rb_state5.var_4 = 0xfdacbd3;
                        rb_state5.var_5 = (rb_state5.var_5) ^ (((rb_state5.var_4) == (0x0)) ? (rb_state5.var_3) : (0x75124aca));
                        rb_state5.var_6 = (rb_state5.var_6) ^ (rb_state5.var_4);
                        rb_state5.var_7 = (rb_state5.var_7) - (((((0x8a41acc9) ^ (0x79d16106)) + (rb_state5.var_5)) - (rb_state5.var_2)) - (rb_state5.var_5));
                        rb_state5.var_8 = (rb_state5.var_8) ^ (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_6) : (0x924fcd93));
                        rb_state5.var_9 = (rb_state5.var_9) + (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_8) : (0x5871c879));
                        rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_9);
                        rb_state5.var_3 = rb_state5.var_2;
                        pthread_mutex_unlock(&(rb_state5.lock_10));
                    }
                    if ((rb_state5.var_1) == (0x4eeae966)) {
                        pthread_mutex_lock(&(rb_state5.lock_10));
                        rb_state5.var_11 = !((rb_state5.var_3) == (rb_state5.var_2));
                        pthread_mutex_unlock(&(rb_state5.lock_10));
                    }
                    if ((rb_state5.var_1) == (0xe72c7a3f)) {
                        pthread_mutex_lock(&(rb_state5.lock_37));
                        if ((rb_state5.var_12) != (0x0)) {
                            if (!((rb_state5.var_12) == (rb_state5.var_27))) {
                                racebench_trigger(5);
                            }
                        }
                        pthread_mutex_unlock(&(rb_state5.lock_37));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_0 = (rb_state6.var_0) + (0xe6bb7fc2);
                    if ((rb_state6.var_0) == (0x1f751132)) {
                        rb_state6.var_2 = rb_state6.var_1;
                    }
                    if ((rb_state6.var_0) == (0x63090f4)) {
                        rb_state6.var_3 = 0x4eb6ffa5;
                        rb_state6.var_4 = 0x7fcd91c6;
                        rb_state6.var_5 = (rb_state6.var_5) + (4252 < rb_input_size ? (uint32_t)rb_input[4252] : 0xd06d9a69);
                        rb_state6.var_6 = (rb_state6.var_6) ^ (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_4) : (0x505db33c));
                        rb_state6.var_7 = (rb_state6.var_7) - (((rb_state6.var_3) == (0x0)) ? (rb_state6.var_2) : (0x66150778));
                        rb_state6.var_8 = (rb_state6.var_8) ^ (((rb_state6.var_5) == (0x0)) ? (rb_state6.var_6) : (0xc1bf02c0));
                        rb_state6.var_9 = (rb_state6.var_9) ^ (rb_state6.var_7);
                        rb_state6.var_10 = (rb_state6.var_10) ^ (((rb_state6.var_4) == (0x0)) ? (rb_state6.var_8) : (0xd4f511b5));
                        rb_state6.var_11 = (rb_state6.var_11) ^ (((rb_state6.var_9) == (0x0)) ? (rb_state6.var_10) : (0xff16e647));
                        rb_state6.var_1 = (rb_state6.var_1) + (rb_state6.var_11);
                    }
                    if ((rb_state6.var_0) == (0x63090f4)) {
                        if (!((rb_state6.var_1) == (rb_state6.var_2))) {
                            racebench_trigger(6);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_7
                    rb_state7.var_0 = (rb_state7.var_0) + ((0x8ff14b67) ^ (rb_state7.var_0));
                    rb_state7.var_0 = (rb_state7.var_0) - ((0xbedc29fd) ^ (0x97973228));
                    rb_state7.var_1 = (rb_state7.var_1) - (((rb_state7.var_0) == (0x66a62f92)) ? (rb_state7.var_1) : (0xf8bd56d4));
                    if ((rb_state7.var_1) == (0x1957f0bc)) {
                        if ((rb_state7.var_2) != (0x0)) {
                            rb_state7.var_32 = !((rb_state7.var_2) == (rb_state7.var_26));
                        }
                    }
                    rb_state7.var_21 = (rb_state7.var_21) ^ (2103 < rb_input_size ? (uint32_t)rb_input[2103] : 0xa853d737);
                    rb_state7.var_22 = (rb_state7.var_22) + (((rb_state7.var_24) == (0x0)) ? (rb_state7.var_23) : (0x1c7f56f9));
                    if ((rb_state7.var_25) == (0x345eaac6)) {
                        pthread_mutex_lock(&(rb_state7.lock_47));
                        rb_state7.var_35 = 0x7d4a3c6f;
                        rb_state7.var_36 = (rb_state7.var_36) ^ (((((0x2e91323b) - (rb_state7.var_27)) ^ (0x5c91c790)) + (rb_state7.var_35)) - (rb_state7.var_28));
                        rb_state7.var_37 = (rb_state7.var_37) ^ (rb_state7.var_29);
                        rb_state7.var_38 = (rb_state7.var_38) + (((rb_state7.var_26) == (0x0)) ? (rb_state7.var_36) : (0xba3ee383));
                        rb_state7.var_39 = (rb_state7.var_39) + (((((0xb82452da) ^ (rb_state7.var_38)) + (rb_state7.var_37)) ^ (rb_state7.var_30)) + (rb_state7.var_32));
                        rb_state7.var_34 = (rb_state7.var_34) - (rb_state7.var_39);
                        rb_state7.var_33 = rb_state7.var_34;
                        pthread_mutex_unlock(&(rb_state7.lock_47));
                    }
                    if ((rb_state7.var_25) == (0x345eaac6)) {
                        rb_state7.var_40 = 0x474a234d;
                        rb_state7.var_41 = 0x3b86694a;
                        rb_state7.var_42 = (rb_state7.var_42) ^ (((((0xa4afd3c5) + (rb_state7.var_34)) - (rb_state7.var_40)) + (rb_state7.var_41)) - (rb_state7.var_35));
                        rb_state7.var_43 = (rb_state7.var_43) - (rb_state7.var_33);
                        rb_state7.var_44 = (rb_state7.var_44) ^ (rb_state7.var_42);
                        rb_state7.var_45 = (rb_state7.var_45) ^ (((rb_state7.var_36) == (0x0)) ? (rb_state7.var_43) : (0xacf72bc6));
                        rb_state7.var_46 = (rb_state7.var_46) + (((rb_state7.var_44) == (0x0)) ? (rb_state7.var_45) : (0x3a3209db));
                        rb_state7.var_33 = (rb_state7.var_33) - (rb_state7.var_46);
                    }
                    if ((rb_state7.var_25) == (0x345eaac6)) {
                        pthread_mutex_lock(&(rb_state7.lock_47));
                        if (!((rb_state7.var_33) == (rb_state7.var_34))) {
                            racebench_trigger(7);
                        }
                        pthread_mutex_unlock(&(rb_state7.lock_47));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_0) == (0x4fff2afa)) ? (rb_state8.var_0) : (0xfcd09d6c));
                    if ((rb_state8.var_1) == (0xb8e86ce2)) {
                        rb_state8.var_2 = rb_state8.var_0;
                    }
                    if ((rb_state8.var_1) == (0xb8e86ce2)) {
                        rb_state8.var_3 = 0xc5be4ee6;
                        rb_state8.var_4 = 0xb3adea99;
                        rb_state8.var_5 = 0xdc8e5d5;
                        rb_state8.var_6 = (rb_state8.var_6) - (3552 < rb_input_size ? (uint32_t)rb_input[3552] : 0x5ab6c6b5);
                        rb_state8.var_7 = (rb_state8.var_7) - (((0xa7dbe2b) - (rb_state8.var_5)) ^ (rb_state8.var_2));
                        rb_state8.var_8 = (rb_state8.var_8) ^ (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_4) : (0x3dce382));
                        rb_state8.var_9 = (rb_state8.var_9) ^ (rb_state8.var_3);
                        rb_state8.var_10 = (rb_state8.var_10) ^ (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_7) : (0xf9d674ce));
                        rb_state8.var_11 = (rb_state8.var_11) - (((0xcfe63225) ^ (0x4e7a1898)) ^ (rb_state8.var_8));
                        rb_state8.var_12 = (rb_state8.var_12) ^ (((0x33914128) ^ (rb_state8.var_4)) + (rb_state8.var_9));
                        rb_state8.var_13 = (rb_state8.var_13) ^ (((0x3fd57be0) ^ (rb_state8.var_5)) ^ (rb_state8.var_10));
                        rb_state8.var_14 = (rb_state8.var_14) - (((0x7b080fb2) - (rb_state8.var_6)) + (rb_state8.var_11));
                        rb_state8.var_15 = (rb_state8.var_15) + (((rb_state8.var_12) == (0x0)) ? (rb_state8.var_13) : (0xf522ad27));
                        rb_state8.var_16 = (rb_state8.var_16) - (((rb_state8.var_14) == (0x0)) ? (rb_state8.var_15) : (0x7a6bb22f));
                        rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_16);
                    }
                    if ((rb_state8.var_1) == (0xb8e86ce2)) {
                        pthread_mutex_lock(&(rb_state8.lock_17));
                        if (!((rb_state8.var_0) == (rb_state8.var_2))) {
                            racebench_trigger(8);
                        }
                        pthread_mutex_unlock(&(rb_state8.lock_17));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_0 = (rb_state9.var_0) - (0xb3e056c6);
                    rb_state9.var_1 = (rb_state9.var_1) - ((0x1d2c6e7b) + (rb_state9.var_0));
                    rb_state9.var_0 = (rb_state9.var_0) ^ (((rb_state9.var_0) == (0x4c1fa93a)) ? (rb_state9.var_1) : (0xa7b00d8a));
                    if ((rb_state9.var_1) == (0x33da080e)) {
                        pthread_mutex_lock(&(rb_state9.lock_22));
                        rb_state9.var_4 = 0x49f57683;
                        rb_state9.var_5 = 0x33b103f5;
                        rb_state9.var_6 = 0x4bbeb191;
                        rb_state9.var_7 = (rb_state9.var_7) - (((0xdaf25199) ^ (rb_state9.var_4)) + (0x6040763a));
                        rb_state9.var_8 = (rb_state9.var_8) ^ (((0xa2d8fd19) - (0x9b465acd)) + (rb_state9.var_6));
                        rb_state9.var_9 = (rb_state9.var_9) + (((((0x5212f5a) + (rb_state9.var_7)) + (rb_state9.var_5)) ^ (0x3dbca55f)) + (0x729e4810));
                        rb_state9.var_10 = (rb_state9.var_10) - (((((0x687cce24) - (rb_state9.var_3)) - (rb_state9.var_2)) - (rb_state9.var_8)) - (rb_state9.var_9));
                        rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_10);
                        rb_state9.var_2 = rb_state9.var_3;
                        pthread_mutex_unlock(&(rb_state9.lock_22));
                    }
                    if ((rb_state9.var_1) == (0x2bf25858)) {
                        rb_state9.var_11 = 0x2dafe321;
                        rb_state9.var_12 = 0xb62e290c;
                        rb_state9.var_13 = (rb_state9.var_13) - (0x1855dd25);
                        rb_state9.var_14 = (rb_state9.var_14) + (((0xce1575cc) - (rb_state9.var_5)) - (rb_state9.var_4));
                        rb_state9.var_15 = (rb_state9.var_15) - (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_12) : (0x36e0857));
                        rb_state9.var_16 = (rb_state9.var_16) ^ (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_11) : (0x94195aa));
                        rb_state9.var_17 = (rb_state9.var_17) ^ (((0x18294bf0) - (0x92bf645)) + (rb_state9.var_13));
                        rb_state9.var_18 = (rb_state9.var_18) + (rb_state9.var_14);
                        rb_state9.var_19 = (rb_state9.var_19) - (((((0xc05b4368) ^ (rb_state9.var_15)) + (rb_state9.var_8)) - (rb_state9.var_16)) - (0x7d41804e));
                        rb_state9.var_20 = (rb_state9.var_20) ^ (((((0x8a50a8b6) ^ (rb_state9.var_17)) ^ (rb_state9.var_18)) - (rb_state9.var_10)) + (rb_state9.var_9));
                        rb_state9.var_21 = (rb_state9.var_21) - (((((0x29fedcf8) + (rb_state9.var_19)) ^ (rb_state9.var_11)) ^ (0xb6568d98)) + (rb_state9.var_20));
                        rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_21);
                    }
                    if ((rb_state9.var_1) == (0x2bf25858)) {
                        pthread_mutex_lock(&(rb_state9.lock_22));
                        if (!((rb_state9.var_2) == (rb_state9.var_3))) {
                            racebench_trigger(9);
                        }
                        pthread_mutex_unlock(&(rb_state9.lock_22));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_1 = (rb_state10.var_1) ^ ((0x7af6beec) + (0x73fc6b18));
                    rb_state10.var_0 = (rb_state10.var_0) ^ (rb_state10.var_1);
                    if ((rb_state10.var_0) == (0x926f1116)) {
                        rb_state10.var_14 = 0x20c7c702;
                        rb_state10.var_15 = 0x36348963;
                        rb_state10.var_16 = 0x3a31828d;
                        rb_state10.var_17 = (rb_state10.var_17) - (0x26879dcd);
                        rb_state10.var_18 = (rb_state10.var_18) + (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_16) : (0x81504c31));
                        rb_state10.var_19 = (rb_state10.var_19) ^ (((0x48acaa40) ^ (rb_state10.var_6)) ^ (rb_state10.var_14));
                        rb_state10.var_20 = (rb_state10.var_20) ^ (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_18) : (0x344bfa48));
                        rb_state10.var_21 = (rb_state10.var_21) + (((rb_state10.var_19) == (0x0)) ? (rb_state10.var_20) : (0xc05ad6d9));
                        rb_state10.var_2 = (rb_state10.var_2) ^ (rb_state10.var_21);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_11
                    rb_state11.var_0 = (rb_state11.var_0) + (9669 < rb_input_size ? (uint32_t)rb_input[9669] : 0xc690a8f);
                    rb_state11.var_1 = (rb_state11.var_1) ^ (0xee99b144);
                    rb_state11.var_0 = (rb_state11.var_0) + ((0x887e5ea3) - (rb_state11.var_1));
                    if ((rb_state11.var_0) == (0x855ef9ba)) {
                        rb_state11.var_2 = rb_state11.var_1;
                    }
                    if ((rb_state11.var_0) == (0xaa02c417)) {
                        rb_state11.var_3 = 0x93574ec1;
                        rb_state11.var_4 = (rb_state11.var_4) + (0x20088dd6);
                        rb_state11.var_5 = (rb_state11.var_5) + (((rb_state11.var_3) == (0x0)) ? (rb_state11.var_2) : (0xa50dedbe));
                        rb_state11.var_6 = (rb_state11.var_6) + (((rb_state11.var_5) == (0x0)) ? (rb_state11.var_4) : (0x11ba1a48));
                        rb_state11.var_7 = (rb_state11.var_7) + (((rb_state11.var_6) == (0x0)) ? (rb_state11.var_3) : (0xdc309d65));
                        rb_state11.var_8 = (rb_state11.var_8) + (rb_state11.var_4);
                        rb_state11.var_9 = (rb_state11.var_9) + (((0xa0522248) + (rb_state11.var_5)) + (rb_state11.var_7));
                        rb_state11.var_10 = (rb_state11.var_10) - (((0xe669ba6e) - (rb_state11.var_6)) - (0xd9bd426));
                        rb_state11.var_11 = (rb_state11.var_11) ^ (rb_state11.var_7);
                        rb_state11.var_12 = (rb_state11.var_12) + (((((0x4aa2b7a5) - (rb_state11.var_9)) + (0x5d37cea6)) + (0xef2351aa)) - (rb_state11.var_8));
                        rb_state11.var_13 = (rb_state11.var_13) ^ (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_11) : (0xff61928b));
                        rb_state11.var_14 = (rb_state11.var_14) - (rb_state11.var_12);
                        rb_state11.var_15 = (rb_state11.var_15) ^ (rb_state11.var_13);
                        rb_state11.var_16 = (rb_state11.var_16) + (((0x342bf38a) - (rb_state11.var_14)) ^ (rb_state11.var_8));
                        rb_state11.var_17 = (rb_state11.var_17) + (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_16) : (0x24c5590));
                        rb_state11.var_1 = (rb_state11.var_1) ^ (rb_state11.var_17);
                    }
                    if ((rb_state11.var_0) == (0xaa02c417)) {
                        if (!((rb_state11.var_1) == (rb_state11.var_2))) {
                            racebench_trigger(11);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_0 = (rb_state12.var_0) ^ (0x3d764169);
                    rb_state12.var_0 = (rb_state12.var_0) + (0x64134d75);
                    if ((rb_state12.var_1) == (0x4716fe25)) {
                        pthread_mutex_lock(&(rb_state12.lock_32));
                        rb_state12.var_4 = 0xc1d86388;
                        rb_state12.var_5 = 0x9ff48343;
                        rb_state12.var_6 = (rb_state12.var_6) ^ (((rb_state12.var_3) == (0x0)) ? (rb_state12.var_2) : (0xf00caf55));
                        rb_state12.var_7 = (rb_state12.var_7) + ((0xf41b1e7f) ^ (rb_state12.var_4));
                        rb_state12.var_8 = (rb_state12.var_8) - (rb_state12.var_5);
                        rb_state12.var_9 = (rb_state12.var_9) ^ (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_6) : (0xa3025cca));
                        rb_state12.var_10 = (rb_state12.var_10) + (((((0x56b900bf) ^ (rb_state12.var_8)) + (rb_state12.var_5)) + (0x550d7daa)) ^ (rb_state12.var_7));
                        rb_state12.var_11 = (rb_state12.var_11) - (((rb_state12.var_6) == (0x0)) ? (rb_state12.var_9) : (0x631a59b2));
                        rb_state12.var_12 = (rb_state12.var_12) - (((0x9ad5834f) + (rb_state12.var_10)) + (rb_state12.var_7));
                        rb_state12.var_13 = (rb_state12.var_13) ^ (((0x145755c) ^ (rb_state12.var_11)) + (rb_state12.var_8));
                        rb_state12.var_14 = (rb_state12.var_14) ^ (((rb_state12.var_12) == (0x0)) ? (rb_state12.var_13) : (0x9cd6e1a2));
                        rb_state12.var_3 = (rb_state12.var_3) - (rb_state12.var_14);
                        rb_state12.var_2 = rb_state12.var_3;
                        pthread_mutex_unlock(&(rb_state12.lock_32));
                    }
                    if ((rb_state12.var_1) == (0x4716fe25)) {
                        pthread_mutex_lock(&(rb_state12.lock_32));
                        rb_state12.var_15 = 0xac43c4d5;
                        rb_state12.var_16 = 0xdd2d010a;
                        rb_state12.var_17 = (rb_state12.var_17) + (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_10) : (0x3a2049b2));
                        rb_state12.var_18 = (rb_state12.var_18) + (11693 < rb_input_size ? (uint32_t)rb_input[11693] : 0x5f7149f3);
                        rb_state12.var_19 = (rb_state12.var_19) + (((0x54f50a1a) + (rb_state12.var_9)) + (rb_state12.var_12));
                        rb_state12.var_20 = (rb_state12.var_20) + (((rb_state12.var_16) == (0x0)) ? (rb_state12.var_15) : (0xf54499b0));
                        rb_state12.var_21 = (rb_state12.var_21) + (rb_state12.var_17);
                        rb_state12.var_22 = (rb_state12.var_22) - (((rb_state12.var_18) == (0x0)) ? (rb_state12.var_19) : (0x41b1bacd));
                        rb_state12.var_23 = (rb_state12.var_23) + (((((0x9dfaca1f) ^ (rb_state12.var_13)) - (0x1da603a8)) ^ (rb_state12.var_20)) + (rb_state12.var_21));
                        rb_state12.var_24 = (rb_state12.var_24) ^ (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_22) : (0x23c88c40));
                        rb_state12.var_25 = (rb_state12.var_25) - (((0x84e3857f) ^ (rb_state12.var_23)) ^ (0xef3ad901));
                        rb_state12.var_26 = (rb_state12.var_26) + (rb_state12.var_24);
                        rb_state12.var_27 = (rb_state12.var_27) + (rb_state12.var_25);
                        rb_state12.var_28 = (rb_state12.var_28) ^ (rb_state12.var_26);
                        rb_state12.var_29 = (rb_state12.var_29) - (((0x44cf0d78) + (rb_state12.var_27)) + (0x525e3eaa));
                        rb_state12.var_30 = (rb_state12.var_30) - (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_28) : (0x5dd44c9b));
                        rb_state12.var_31 = (rb_state12.var_31) ^ (((rb_state12.var_29) == (0x0)) ? (rb_state12.var_30) : (0x15169425));
                        rb_state12.var_2 = (rb_state12.var_2) - (rb_state12.var_31);
                        pthread_mutex_unlock(&(rb_state12.lock_32));
                    }
                    if ((rb_state12.var_1) == (0x4716fe25)) {
                        if (!((rb_state12.var_2) == (rb_state12.var_3))) {
                            racebench_trigger(12);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_0 = (rb_state13.var_0) - ((0xb5e1a1ef) + (0xf0d94ec4));
                    rb_state13.var_1 = (rb_state13.var_1) ^ (0xcde30c5f);
                    rb_state13.var_0 = (rb_state13.var_0) - (rb_state13.var_1);
                    if ((rb_state13.var_0) == (0x40abbb38)) {
                        rb_state13.var_14 = 0x262d3789;
                        rb_state13.var_15 = 0xbd1b69bd;
                        rb_state13.var_16 = (rb_state13.var_16) + (((((0xcf9531b6) + (rb_state13.var_15)) + (0xa3391acd)) - (rb_state13.var_14)) + (0x105b08ae));
                        rb_state13.var_17 = (rb_state13.var_17) + (0xca91fbb9);
                        rb_state13.var_18 = (rb_state13.var_18) ^ (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_16) : (0xd7cafa01));
                        rb_state13.var_19 = (rb_state13.var_19) - (rb_state13.var_17);
                        rb_state13.var_20 = (rb_state13.var_20) - (rb_state13.var_18);
                        rb_state13.var_21 = (rb_state13.var_21) + (((0xa5fe3e8e) - (rb_state13.var_19)) + (0x1651157f));
                        rb_state13.var_22 = (rb_state13.var_22) ^ (((((0x257cbfb2) + (rb_state13.var_21)) + (rb_state13.var_8)) ^ (rb_state13.var_9)) + (rb_state13.var_20));
                        rb_state13.var_13 = (rb_state13.var_13) - (rb_state13.var_22);
                        rb_state13.var_2 = rb_state13.var_13;
                    }
                    if ((rb_state13.var_0) == (0xcc0dbe26)) {
                        if ((rb_state13.var_2) != (0x0)) {
                            if (!((rb_state13.var_2) == (rb_state13.var_23))) {
                                racebench_trigger(13);
                            }
                        }
                    }
                    if ((rb_state13.var_0) == (0xcc0dbe26)) {
                        pthread_mutex_lock(&(rb_state13.lock_31));
                        rb_state13.var_24 = (rb_state13.var_24) - (((0xe0c02544) - (rb_state13.var_13)) + (rb_state13.var_10));
                        rb_state13.var_25 = (rb_state13.var_25) ^ (((((0xd649bdd7) - (rb_state13.var_11)) + (rb_state13.var_15)) ^ (rb_state13.var_12)) ^ (rb_state13.var_14));
                        rb_state13.var_26 = (rb_state13.var_26) - ((0xbd5518bd) - (rb_state13.var_16));
                        rb_state13.var_27 = (rb_state13.var_27) + (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_24) : (0x91d4a819));
                        rb_state13.var_28 = (rb_state13.var_28) - (rb_state13.var_25);
                        rb_state13.var_29 = (rb_state13.var_29) + (((rb_state13.var_26) == (0x0)) ? (rb_state13.var_27) : (0x53fe4174));
                        rb_state13.var_30 = (rb_state13.var_30) + (((rb_state13.var_28) == (0x0)) ? (rb_state13.var_29) : (0x97d6e9bc));
                        rb_state13.var_23 = (rb_state13.var_23) - (rb_state13.var_30);
                        rb_state13.var_2 = rb_state13.var_23;
                        pthread_mutex_unlock(&(rb_state13.lock_31));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_14
                    rb_state14.var_1 = (rb_state14.var_1) ^ (((rb_state14.var_1) == (0x0)) ? (rb_state14.var_0) : (0x6982d2f2));
                    if ((rb_state14.var_0) == (0x2f0a34e4)) {
                        pthread_mutex_lock(&(rb_state14.lock_16));
                        rb_state14.var_4 = 0x5c0354bd;
                        rb_state14.var_5 = 0xe6b48d47;
                        rb_state14.var_6 = (rb_state14.var_6) ^ (((((0xfe63066c) ^ (0xb49db39f)) - (rb_state14.var_5)) - (rb_state14.var_3)) ^ (rb_state14.var_2));
                        rb_state14.var_7 = (rb_state14.var_7) ^ (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_6) : (0xc73e05b6));
                        rb_state14.var_3 = (rb_state14.var_3) + (rb_state14.var_7);
                        rb_state14.var_2 = rb_state14.var_3;
                        pthread_mutex_unlock(&(rb_state14.lock_16));
                    }
                    if ((rb_state14.var_0) == (0x2f0a34e4)) {
                        rb_state14.var_8 = 0xb8fc378a;
                        rb_state14.var_9 = 0x75a94c72;
                        rb_state14.var_10 = (rb_state14.var_10) - (rb_state14.var_5);
                        rb_state14.var_11 = (rb_state14.var_11) ^ (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_9) : (0x53605088));
                        rb_state14.var_12 = (rb_state14.var_12) - (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_10) : (0xca7ff8f7));
                        rb_state14.var_13 = (rb_state14.var_13) + (((0x76dfe7da) - (rb_state14.var_11)) ^ (rb_state14.var_6));
                        rb_state14.var_14 = (rb_state14.var_14) ^ (((0xd3cf1949) + (rb_state14.var_12)) - (0x7e560a5a));
                        rb_state14.var_15 = (rb_state14.var_15) - (((((0x59261576) ^ (0x4418cf9d)) + (rb_state14.var_14)) + (rb_state14.var_13)) ^ (rb_state14.var_7));
                        rb_state14.var_2 = (rb_state14.var_2) + (rb_state14.var_15);
                    }
                    if ((rb_state14.var_0) == (0x2f0a34e4)) {
                        if (!((rb_state14.var_2) == (rb_state14.var_3))) {
                            racebench_trigger(14);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    rb_state15.var_0 = (rb_state15.var_0) + (0xf4ea83a3);
                    rb_state15.var_0 = (rb_state15.var_0) + (3576 < rb_input_size ? (uint32_t)rb_input[3576] : 0x5c7fa296);
                    rb_state15.var_0 = (rb_state15.var_0) - (14414 < rb_input_size ? (uint32_t)rb_input[14414] : 0xe3d34e24);
                    rb_state15.var_0 = (rb_state15.var_0) - (((rb_state15.var_0) == (0xf4ea83a4)) ? (rb_state15.var_0) : (0x23c2be74));
                    if ((rb_state15.var_0) == (0xc95f9d40)) {
                        rb_state15.var_3 = 0xaa9124df;
                        rb_state15.var_4 = 0x84082445;
                        rb_state15.var_5 = (rb_state15.var_5) ^ (0xa3213f9a);
                        rb_state15.var_6 = (rb_state15.var_6) + (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_2) : (0x9c2373d0));
                        rb_state15.var_7 = (rb_state15.var_7) ^ (rb_state15.var_3);
                        rb_state15.var_8 = (rb_state15.var_8) ^ (((rb_state15.var_3) == (0x0)) ? (rb_state15.var_5) : (0x728bc92d));
                        rb_state15.var_9 = (rb_state15.var_9) - (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_6) : (0x4909c5be));
                        rb_state15.var_10 = (rb_state15.var_10) - (rb_state15.var_7);
                        rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_9) : (0x9e7044f7));
                        rb_state15.var_12 = (rb_state15.var_12) + (((0x55dda6b9) - (rb_state15.var_10)) + (0x1bbef109));
                        rb_state15.var_13 = (rb_state15.var_13) - (((rb_state15.var_5) == (0x0)) ? (rb_state15.var_11) : (0xfb5ed9ef));
                        rb_state15.var_14 = (rb_state15.var_14) + (((0x52f02096) + (rb_state15.var_12)) ^ (rb_state15.var_6));
                        rb_state15.var_15 = (rb_state15.var_15) - (((0x6ccd3edc) + (rb_state15.var_7)) ^ (rb_state15.var_13));
                        rb_state15.var_16 = (rb_state15.var_16) + (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_14) : (0xfc572541));
                        rb_state15.var_17 = (rb_state15.var_17) - (((0x5ff08c2c) + (0x7ba4cef5)) + (rb_state15.var_15));
                        rb_state15.var_18 = (rb_state15.var_18) + (((((0x659376e1) + (rb_state15.var_9)) + (0x1b6a11e9)) - (rb_state15.var_16)) + (rb_state15.var_17));
                        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_18);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_16
                    rb_state16.var_0 = (rb_state16.var_0) - (6773 < rb_input_size ? (uint32_t)rb_input[6773] : 0xd857b03b);
                    rb_state16.var_1 = (rb_state16.var_1) - (rb_state16.var_0);
                    if ((rb_state16.var_1) == (0xf0ddb4e4)) {
                        pthread_mutex_lock(&(rb_state16.lock_15));
                        rb_state16.var_4 = (rb_state16.var_4) + (((((0x31f0aff2) ^ (rb_state16.var_3)) + (0xc5d6f5cb)) ^ (rb_state16.var_2)) - (rb_state16.var_4));
                        rb_state16.var_3 = (rb_state16.var_3) + (rb_state16.var_4);
                        rb_state16.var_2 = rb_state16.var_3;
                        pthread_mutex_unlock(&(rb_state16.lock_15));
                    }
                    if ((rb_state16.var_1) == (0x28200c76)) {
                        rb_state16.var_5 = (rb_state16.var_5) ^ (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_4) : (0xf03e31e1));
                        rb_state16.var_6 = (rb_state16.var_6) + (rb_state16.var_6);
                        rb_state16.var_7 = (rb_state16.var_7) + (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_0) : (0x80fe87b));
                        rb_state16.var_8 = (rb_state16.var_8) ^ (rb_state16.var_8);
                        rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_3) : (0x61f30ed7));
                        rb_state16.var_10 = (rb_state16.var_10) - (((0x3162ee97) + (rb_state16.var_5)) ^ (0x36933800));
                        rb_state16.var_11 = (rb_state16.var_11) ^ (((((0x46499695) - (rb_state16.var_6)) - (0x43304cc4)) + (rb_state16.var_7)) - (0x30a66dd7));
                        rb_state16.var_12 = (rb_state16.var_12) + (((((0x417b8c6) - (0x87c2ef25)) - (rb_state16.var_8)) ^ (rb_state16.var_10)) - (rb_state16.var_9));
                        rb_state16.var_13 = (rb_state16.var_13) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_11) : (0x9c16bfe3));
                        rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_13) : (0x1c26b75d));
                        rb_state16.var_2 = (rb_state16.var_2) + (rb_state16.var_14);
                    }
                    if ((rb_state16.var_1) == (0x28200c76)) {
                        rb_state16.var_16 = !((rb_state16.var_2) == (rb_state16.var_3));
                    }
                    rb_state16.var_11 = (rb_state16.var_11) + (rb_state16.var_12);
                    rb_state16.var_13 = (rb_state16.var_13) - (rb_state16.var_14);
                    if ((rb_state16.var_16) == (0x1)) {
                        rb_state16.var_25 = 0x4a86363c;
                        rb_state16.var_26 = (rb_state16.var_26) ^ (((rb_state16.var_23) == (0x0)) ? (rb_state16.var_22) : (0xda484dce));
                        rb_state16.var_27 = (rb_state16.var_27) ^ ((0xbd5f0a66) + (rb_state16.var_24));
                        rb_state16.var_28 = (rb_state16.var_28) + (0x444d0653);
                        rb_state16.var_29 = (rb_state16.var_29) ^ (((rb_state16.var_20) == (0x0)) ? (rb_state16.var_25) : (0xd6c8c92));
                        rb_state16.var_30 = (rb_state16.var_30) + (((rb_state16.var_25) == (0x0)) ? (rb_state16.var_21) : (0xb93fc97e));
                        rb_state16.var_31 = (rb_state16.var_31) ^ (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_27) : (0xbd25fe9d));
                        rb_state16.var_32 = (rb_state16.var_32) - (((rb_state16.var_28) == (0x0)) ? (rb_state16.var_29) : (0xd68b3db9));
                        rb_state16.var_33 = (rb_state16.var_33) - (((rb_state16.var_26) == (0x0)) ? (rb_state16.var_30) : (0x2e4341bf));
                        rb_state16.var_34 = (rb_state16.var_34) ^ (rb_state16.var_31);
                        rb_state16.var_35 = (rb_state16.var_35) + (rb_state16.var_32);
                        rb_state16.var_36 = (rb_state16.var_36) - (((rb_state16.var_27) == (0x0)) ? (rb_state16.var_33) : (0xab3b08d7));
                        rb_state16.var_37 = (rb_state16.var_37) ^ (((rb_state16.var_28) == (0x0)) ? (rb_state16.var_34) : (0x35e43d1));
                        rb_state16.var_38 = (rb_state16.var_38) + (((rb_state16.var_29) == (0x0)) ? (rb_state16.var_35) : (0x83b0ff1a));
                        rb_state16.var_39 = (rb_state16.var_39) - (((0xb0af2994) - (rb_state16.var_36)) + (rb_state16.var_30));
                        rb_state16.var_40 = (rb_state16.var_40) - (rb_state16.var_37);
                        rb_state16.var_41 = (rb_state16.var_41) - (rb_state16.var_38);
                        rb_state16.var_42 = (rb_state16.var_42) + (rb_state16.var_39);
                        rb_state16.var_43 = (rb_state16.var_43) ^ (((0x28329517) + (rb_state16.var_31)) - (rb_state16.var_40));
                        rb_state16.var_44 = (rb_state16.var_44) - (rb_state16.var_41);
                        rb_state16.var_45 = (rb_state16.var_45) - (((rb_state16.var_32) == (0x0)) ? (rb_state16.var_42) : (0x2bb8ed89));
                        rb_state16.var_46 = (rb_state16.var_46) + (((rb_state16.var_33) == (0x0)) ? (rb_state16.var_43) : (0xf959c7d9));
                        rb_state16.var_47 = (rb_state16.var_47) + (((((0xab175d5a) + (rb_state16.var_44)) + (rb_state16.var_34)) + (0x2613f74e)) - (rb_state16.var_45));
                        rb_state16.var_48 = (rb_state16.var_48) ^ (((0x80a9dfd5) ^ (0x6bbf89ef)) ^ (rb_state16.var_46));
                        rb_state16.var_49 = (rb_state16.var_49) - (rb_state16.var_47);
                        rb_state16.var_50 = (rb_state16.var_50) + (((rb_state16.var_48) == (0x0)) ? (rb_state16.var_49) : (0xc84eeef6));
                        rb_state16.var_24 = (rb_state16.var_24) - (rb_state16.var_50);
                        rb_state16.var_17 = rb_state16.var_24;
                    }
                    if ((rb_state16.var_16) == (0x1)) {
                        pthread_mutex_lock(&(rb_state16.lock_61));
                        rb_state16.var_52 = 0xa3ba1ab7;
                        rb_state16.var_53 = 0x97d41a4d;
                        rb_state16.var_54 = (rb_state16.var_54) - (((rb_state16.var_35) == (0x0)) ? (rb_state16.var_53) : (0x31935cbd));
                        rb_state16.var_55 = (rb_state16.var_55) ^ (((rb_state16.var_37) == (0x0)) ? (rb_state16.var_36) : (0x6a8bb20));
                        rb_state16.var_56 = (rb_state16.var_56) + (((0x3e2615b0) + (0xb9f5d27e)) ^ (rb_state16.var_52));
                        rb_state16.var_57 = (rb_state16.var_57) ^ (rb_state16.var_54);
                        rb_state16.var_58 = (rb_state16.var_58) + (((rb_state16.var_38) == (0x0)) ? (rb_state16.var_55) : (0xd65ac1ec));
                        rb_state16.var_59 = (rb_state16.var_59) + (((((0x8b177187) ^ (rb_state16.var_39)) + (rb_state16.var_57)) - (rb_state16.var_56)) + (rb_state16.var_40));
                        rb_state16.var_60 = (rb_state16.var_60) ^ (((((0xd47f004a) ^ (rb_state16.var_41)) ^ (rb_state16.var_59)) + (0x6a2dfb15)) + (rb_state16.var_58));
                        rb_state16.var_51 = (rb_state16.var_51) + (rb_state16.var_60);
                        rb_state16.var_17 = rb_state16.var_51;
                        pthread_mutex_unlock(&(rb_state16.lock_61));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_17
                    rb_state17.var_0 = (rb_state17.var_0) + ((0xc99b784a) + (rb_state17.var_0));
                    rb_state17.var_1 = (rb_state17.var_1) + (0x1cc27223);
                    if ((rb_state17.var_1) == (0xa1012e19)) {
                        rb_state17.var_7 = 0xda5d4026;
                        rb_state17.var_8 = (rb_state17.var_8) + (((((0xc4689ba5) - (rb_state17.var_7)) + (rb_state17.var_6)) ^ (rb_state17.var_7)) + (rb_state17.var_8));
                        rb_state17.var_9 = (rb_state17.var_9) ^ (((rb_state17.var_5) == (0x0)) ? (rb_state17.var_8) : (0x972f0487));
                        rb_state17.var_2 = (rb_state17.var_2) ^ (rb_state17.var_9);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_0 = (rb_state18.var_0) + ((0xeab636f3) - (rb_state18.var_0));
                    rb_state18.var_0 = (rb_state18.var_0) ^ (13236 < rb_input_size ? (uint32_t)rb_input[13236] : 0x86536215);
                    rb_state18.var_1 = (rb_state18.var_1) ^ (rb_state18.var_0);
                    if ((rb_state18.var_1) == (0xe2ce4a07)) {
                        pthread_mutex_lock(&(rb_state18.lock_22));
                        rb_state18.var_4 = 0xd0066516;
                        rb_state18.var_5 = (rb_state18.var_5) ^ (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_4) : (0xb8b70b0));
                        rb_state18.var_6 = (rb_state18.var_6) ^ (rb_state18.var_3);
                        rb_state18.var_7 = (rb_state18.var_7) ^ (((rb_state18.var_2) == (0x0)) ? (rb_state18.var_5) : (0xff5b0db8));
                        rb_state18.var_8 = (rb_state18.var_8) + (rb_state18.var_6);
                        rb_state18.var_9 = (rb_state18.var_9) + (((0xe88b9ea7) + (0x9b5338fc)) ^ (rb_state18.var_7));
                        rb_state18.var_10 = (rb_state18.var_10) ^ (((((0xf08cf014) - (0x17eb16f3)) - (rb_state18.var_5)) - (rb_state18.var_9)) + (rb_state18.var_8));
                        rb_state18.var_3 = (rb_state18.var_3) ^ (rb_state18.var_10);
                        rb_state18.var_2 = rb_state18.var_3;
                        pthread_mutex_unlock(&(rb_state18.lock_22));
                    }
                    if ((rb_state18.var_1) == (0x8c787cf4)) {
                        pthread_mutex_lock(&(rb_state18.lock_22));
                        if (!((rb_state18.var_2) == (rb_state18.var_3))) {
                            racebench_trigger(18);
                        }
                        pthread_mutex_unlock(&(rb_state18.lock_22));
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