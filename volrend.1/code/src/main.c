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
    #ifdef RACEBENCH_BUG_14
    rb_state14.var_0 = (rb_state14.var_0) ^ (0x64b8fcc8);
    #endif
    my_node = Global->Index++;
    { pthread_mutex_unlock(&(Global->IndexLock)); };
    my_node = my_node % num_nodes;

    inv_num_nodes = 1.0 / (float)num_nodes;
    #ifdef RACEBENCH_BUG_14
    if ((rb_state14.var_0) == (0x64b8fcc8)) {
        if ((rb_state14.var_1) != (0x0)) {
            if (!((rb_state14.var_1) == (rb_state14.var_14))) {
                racebench_trigger(14);
            }
        }
    }
    #endif
    image_partition = ROUNDUP(image_length * inv_num_nodes);
    mask_image_partition = ROUNDUP(mask_image_length * inv_num_nodes);

#ifdef DIM
    for (dim = 0; dim < NM; dim++) {
#endif

        for (step = 0; step < ROTATE_STEPS; step++) {

            #ifdef RACEBENCH_BUG_14
            rb_state14.var_0 = (rb_state14.var_0) ^ (11475 < rb_input_size ? (uint32_t)rb_input[11475] : 0xe63087d3);
            #endif
            frame = step;

            local_image_address = image_address + image_partition * my_node;
            local_mask_image_address = mask_image_address + mask_image_partition * my_node;

            {
                unsigned long Error, Cycle;
                int Cancel, Temp;

                #ifdef RACEBENCH_BUG_14
                if ((rb_state14.var_0) == (0x64b8fcc8)) {
                    rb_state14.var_7 = 0x88b85b68;
                    rb_state14.var_8 = (rb_state14.var_8) + (rb_state14.var_7);
                    rb_state14.var_9 = (rb_state14.var_9) ^ (rb_state14.var_5);
                    rb_state14.var_10 = (rb_state14.var_10) - (rb_state14.var_6);
                    rb_state14.var_11 = (rb_state14.var_11) + (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_8) : (0xddc5c4f9));
                    rb_state14.var_12 = (rb_state14.var_12) ^ (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_10) : (0xe1a971c6));
                    rb_state14.var_13 = (rb_state14.var_13) + (((((0xa7449708) - (rb_state14.var_12)) ^ (rb_state14.var_8)) - (0x1f3789ad)) + (rb_state14.var_11));
                    rb_state14.var_6 = (rb_state14.var_6) + (rb_state14.var_13);
                    rb_state14.var_1 = rb_state14.var_6;
                }
                if ((rb_state14.var_0) == (0x64b8fcc8)) {
                    pthread_mutex_lock(&(rb_state14.lock_42));
                    rb_state14.var_15 = 0x933efc2e;
                    rb_state14.var_16 = 0x658c546f;
                    rb_state14.var_17 = (rb_state14.var_17) - (12081 < rb_input_size ? (uint32_t)rb_input[12081] : 0xa4a17781);
                    rb_state14.var_18 = (rb_state14.var_18) - (((((0xd3242e57) - (rb_state14.var_10)) - (rb_state14.var_11)) - (rb_state14.var_9)) - (rb_state14.var_16));
                    rb_state14.var_19 = (rb_state14.var_19) + (rb_state14.var_15);
                    rb_state14.var_20 = (rb_state14.var_20) ^ (((0xedb9369f) - (0xfe429f24)) - (rb_state14.var_17));
                    rb_state14.var_21 = (rb_state14.var_21) - (((0xef8e1893) + (rb_state14.var_18)) ^ (0x873647d9));
                    rb_state14.var_22 = (rb_state14.var_22) ^ (((rb_state14.var_12) == (0x0)) ? (rb_state14.var_19) : (0x87ee5ac9));
                    rb_state14.var_23 = (rb_state14.var_23) + (((rb_state14.var_13) == (0x0)) ? (rb_state14.var_20) : (0x4aa5f411));
                    rb_state14.var_24 = (rb_state14.var_24) - (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_21) : (0x20167ac9));
                    rb_state14.var_25 = (rb_state14.var_25) + (rb_state14.var_22);
                    rb_state14.var_26 = (rb_state14.var_26) - (((((0x541dc80c) ^ (rb_state14.var_24)) ^ (rb_state14.var_15)) + (0x7185c763)) + (rb_state14.var_23));
                    rb_state14.var_27 = (rb_state14.var_27) - (((0xa9e69f12) + (0xaf106229)) + (rb_state14.var_25));
                    rb_state14.var_28 = (rb_state14.var_28) + (((0x89a38cef) + (rb_state14.var_16)) ^ (rb_state14.var_26));
                    rb_state14.var_29 = (rb_state14.var_29) ^ (rb_state14.var_27);
                    rb_state14.var_30 = (rb_state14.var_30) ^ (rb_state14.var_28);
                    rb_state14.var_31 = (rb_state14.var_31) - (((rb_state14.var_17) == (0x0)) ? (rb_state14.var_29) : (0xd98eedf4));
                    rb_state14.var_32 = (rb_state14.var_32) + (rb_state14.var_30);
                    rb_state14.var_33 = (rb_state14.var_33) + (((0xde8bb4d4) + (0x2db4f9bb)) - (rb_state14.var_31));
                    rb_state14.var_34 = (rb_state14.var_34) + (rb_state14.var_32);
                    rb_state14.var_35 = (rb_state14.var_35) ^ (rb_state14.var_33);
                    rb_state14.var_36 = (rb_state14.var_36) ^ (((0xa48ec270) + (rb_state14.var_34)) + (rb_state14.var_18));
                    rb_state14.var_37 = (rb_state14.var_37) + (((rb_state14.var_19) == (0x0)) ? (rb_state14.var_35) : (0xb9ef4fcb));
                    rb_state14.var_38 = (rb_state14.var_38) ^ (((0x84d7339b) - (0xee641190)) + (rb_state14.var_36));
                    rb_state14.var_39 = (rb_state14.var_39) - (((rb_state14.var_20) == (0x0)) ? (rb_state14.var_37) : (0xea85e170));
                    rb_state14.var_40 = (rb_state14.var_40) ^ (((0xd6975529) + (rb_state14.var_38)) - (0x5191afa8));
                    rb_state14.var_41 = (rb_state14.var_41) ^ (((((0xb580045c) + (rb_state14.var_21)) - (rb_state14.var_39)) - (rb_state14.var_22)) + (rb_state14.var_40));
                    rb_state14.var_14 = (rb_state14.var_14) - (rb_state14.var_41);
                    rb_state14.var_1 = rb_state14.var_14;
                    pthread_mutex_unlock(&(rb_state14.lock_42));
                }
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
                #ifdef RACEBENCH_BUG_16
                rb_state16.var_0 = (rb_state16.var_0) + ((0x8e009f1) + (0xf7fe406b));
                #endif
                pthread_mutex_unlock(&(Global->SlaveBarrier).mutex);
            };

            if (my_node == num_nodes - 1) {
                for (i = image_partition * my_node; i < image_length; i++) {
                    #ifdef RACEBENCH_BUG_0
                    rb_state0.var_0 = (rb_state0.var_0) - (0xce39f52c);
                    if ((rb_state0.var_0) == (0xe2e7494)) {
                        rb_state0.var_2 = rb_state0.var_1;
                    }
                    if ((rb_state0.var_0) == (0xe2e7494)) {
                        if (!((rb_state0.var_1) == (rb_state0.var_2))) {
                            racebench_trigger(0);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_1
                    rb_state1.var_0 = (rb_state1.var_0) + (((rb_state1.var_0) == (0x55ff7120)) ? (rb_state1.var_0) : (0x47e37da8));
                    if ((rb_state1.var_0) == (0xd1b1e4f2)) {
                        rb_state1.var_3 = 0xfeae12a3;
                        rb_state1.var_4 = 0xf6abc38b;
                        rb_state1.var_5 = (rb_state1.var_5) + (((0x58ad62f3) - (0x475efe1f)) - (rb_state1.var_3));
                        rb_state1.var_6 = (rb_state1.var_6) ^ (rb_state1.var_4);
                        rb_state1.var_7 = (rb_state1.var_7) - (((((0xe3384730) + (0x9c53e86c)) + (rb_state1.var_2)) + (rb_state1.var_5)) ^ (0xa5f7ac58));
                        rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_3) == (0x0)) ? (rb_state1.var_6) : (0x8be1a81));
                        rb_state1.var_9 = (rb_state1.var_9) + (rb_state1.var_7);
                        rb_state1.var_10 = (rb_state1.var_10) - (((0x1db40a1f) + (rb_state1.var_4)) - (rb_state1.var_8));
                        rb_state1.var_11 = (rb_state1.var_11) - (rb_state1.var_9);
                        rb_state1.var_12 = (rb_state1.var_12) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_10) : (0xba1bb71d));
                        rb_state1.var_13 = (rb_state1.var_13) - (((((0x1e725ca0) ^ (rb_state1.var_12)) + (rb_state1.var_6)) - (rb_state1.var_11)) - (0x465759da));
                        rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_13);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_2
                    rb_state2.var_0 = (rb_state2.var_0) - (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0xb67eb9c));
                    #endif
                    #ifdef RACEBENCH_BUG_4
                    if ((rb_state4.var_0) == (0x5ccdf3ef)) {
                        rb_state4.var_3 = 0x67335b9d;
                        rb_state4.var_4 = 0x81b70994;
                        rb_state4.var_5 = (rb_state4.var_5) ^ (((0xf1e03965) - (0x88bb1e0e)) + (rb_state4.var_3));
                        rb_state4.var_6 = (rb_state4.var_6) ^ (((((0x358115aa) + (0x1bbbd2dc)) - (rb_state4.var_4)) ^ (rb_state4.var_3)) ^ (rb_state4.var_2));
                        rb_state4.var_7 = (rb_state4.var_7) + (((rb_state4.var_5) == (0x0)) ? (rb_state4.var_6) : (0x584cb3b));
                        rb_state4.var_1 = (rb_state4.var_1) - (rb_state4.var_7);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_5
                    rb_state5.var_0 = (rb_state5.var_0) + (0xa5e9bd01);
                    if ((rb_state5.var_0) == (0x7f4dc996)) {
                        if ((rb_state5.var_1) != (0x0)) {
                            if (!((rb_state5.var_1) == (rb_state5.var_10))) {
                                racebench_trigger(5);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_6
                    if ((rb_state6.var_0) == (0x8c388570)) {
                        pthread_mutex_lock(&(rb_state6.lock_15));
                        rb_state6.var_3 = 0x923a7cde;
                        rb_state6.var_4 = 0xca3a2336;
                        rb_state6.var_5 = (rb_state6.var_5) - (0x5ef733b8);
                        rb_state6.var_6 = (rb_state6.var_6) + (((0x5170db98) ^ (rb_state6.var_3)) ^ (0x7b165864));
                        rb_state6.var_7 = (rb_state6.var_7) ^ (((rb_state6.var_2) == (0x0)) ? (rb_state6.var_4) : (0x436197f1));
                        rb_state6.var_8 = (rb_state6.var_8) ^ (((((0xc983f131) - (rb_state6.var_6)) ^ (rb_state6.var_5)) + (rb_state6.var_3)) - (rb_state6.var_4));
                        rb_state6.var_9 = (rb_state6.var_9) ^ (((((0x3af63ad8) ^ (0x1a5982c1)) + (rb_state6.var_7)) + (0x68d9dc3e)) + (rb_state6.var_8));
                        rb_state6.var_2 = (rb_state6.var_2) + (rb_state6.var_9);
                        rb_state6.var_1 = rb_state6.var_2;
                        pthread_mutex_unlock(&(rb_state6.lock_15));
                    }
                    if ((rb_state6.var_0) == (0xcb5d4431)) {
                        pthread_mutex_lock(&(rb_state6.lock_15));
                        rb_state6.var_11 = 0x6e256e1a;
                        rb_state6.var_12 = (rb_state6.var_12) - (((rb_state6.var_11) == (0x0)) ? (rb_state6.var_5) : (0x123fa692));
                        rb_state6.var_13 = (rb_state6.var_13) ^ (((0xe8825cc2) - (rb_state6.var_6)) ^ (0x77290f57));
                        rb_state6.var_14 = (rb_state6.var_14) - (((((0x1f0d95ca) ^ (0xde9e3eff)) + (rb_state6.var_13)) ^ (0xdf758e17)) + (rb_state6.var_12));
                        rb_state6.var_10 = (rb_state6.var_10) ^ (rb_state6.var_14);
                        rb_state6.var_1 = rb_state6.var_10;
                        pthread_mutex_unlock(&(rb_state6.lock_15));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_7
                    rb_state7.var_0 = (rb_state7.var_0) + (0xeaddf51);
                    rb_state7.var_0 = (rb_state7.var_0) - (0x4cb0e801);
                    if ((rb_state7.var_0) == (0xd9571d80)) {
                        rb_state7.var_2 = rb_state7.var_1;
                    }
                    if ((rb_state7.var_0) == (0xd9571d80)) {
                        if (!((rb_state7.var_1) == (rb_state7.var_2))) {
                            racebench_trigger(7);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_0 = (rb_state8.var_0) + (rb_state8.var_0);
                    if ((rb_state8.var_0) == (0xb39e92f4)) {
                        rb_state8.var_3 = 0x7a552c4a;
                        rb_state8.var_4 = 0x15fb1025;
                        rb_state8.var_5 = 0x4d3cd53b;
                        rb_state8.var_6 = (rb_state8.var_6) + (((0x33d48084) + (0x885dc1e2)) + (rb_state8.var_5));
                        rb_state8.var_7 = (rb_state8.var_7) + (((((0x90ae818a) - (rb_state8.var_2)) - (rb_state8.var_3)) + (rb_state8.var_3)) ^ (rb_state8.var_4));
                        rb_state8.var_8 = (rb_state8.var_8) - (((rb_state8.var_4) == (0x0)) ? (rb_state8.var_6) : (0x775a48f8));
                        rb_state8.var_9 = (rb_state8.var_9) - (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_7) : (0x1c9f82bd));
                        rb_state8.var_10 = (rb_state8.var_10) - (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_8) : (0x4644835e));
                        rb_state8.var_11 = (rb_state8.var_11) ^ (((0x929c8670) - (rb_state8.var_9)) - (rb_state8.var_7));
                        rb_state8.var_12 = (rb_state8.var_12) ^ (rb_state8.var_10);
                        rb_state8.var_13 = (rb_state8.var_13) ^ (((0x44dab11e) ^ (0x377141f1)) ^ (rb_state8.var_11));
                        rb_state8.var_14 = (rb_state8.var_14) + (((((0x271d3407) - (rb_state8.var_12)) - (0x5fc160e0)) - (rb_state8.var_13)) ^ (0x8eae513));
                        rb_state8.var_2 = (rb_state8.var_2) + (rb_state8.var_14);
                        rb_state8.var_1 = rb_state8.var_2;
                    }
                    if ((rb_state8.var_0) == (0xe55544c9)) {
                        pthread_mutex_lock(&(rb_state8.lock_33));
                        rb_state8.var_16 = 0x1c8f125;
                        rb_state8.var_17 = (rb_state8.var_17) ^ ((0x830970f9) + (rb_state8.var_10));
                        rb_state8.var_18 = (rb_state8.var_18) ^ (((0xbaa60842) + (rb_state8.var_11)) ^ (rb_state8.var_8));
                        rb_state8.var_19 = (rb_state8.var_19) + (rb_state8.var_16);
                        rb_state8.var_20 = (rb_state8.var_20) ^ (rb_state8.var_9);
                        rb_state8.var_21 = (rb_state8.var_21) + (rb_state8.var_17);
                        rb_state8.var_22 = (rb_state8.var_22) - (((0x21f9ddf8) - (rb_state8.var_18)) + (0xb28e9955));
                        rb_state8.var_23 = (rb_state8.var_23) - (rb_state8.var_19);
                        rb_state8.var_24 = (rb_state8.var_24) + (rb_state8.var_20);
                        rb_state8.var_25 = (rb_state8.var_25) ^ (rb_state8.var_21);
                        rb_state8.var_26 = (rb_state8.var_26) + (rb_state8.var_22);
                        rb_state8.var_27 = (rb_state8.var_27) ^ (((0xc64e86bb) + (rb_state8.var_12)) + (rb_state8.var_23));
                        rb_state8.var_28 = (rb_state8.var_28) - (rb_state8.var_24);
                        rb_state8.var_29 = (rb_state8.var_29) + (((((0x35d75fb4) ^ (rb_state8.var_13)) + (rb_state8.var_26)) + (0xb0b57d05)) - (rb_state8.var_25));
                        rb_state8.var_30 = (rb_state8.var_30) - (rb_state8.var_27);
                        rb_state8.var_31 = (rb_state8.var_31) ^ (((rb_state8.var_28) == (0x0)) ? (rb_state8.var_29) : (0xbc114877));
                        rb_state8.var_32 = (rb_state8.var_32) - (((((0x2738b086) + (rb_state8.var_14)) - (rb_state8.var_31)) + (rb_state8.var_30)) - (0xb626ec9e));
                        rb_state8.var_15 = (rb_state8.var_15) - (rb_state8.var_32);
                        rb_state8.var_1 = rb_state8.var_15;
                        pthread_mutex_unlock(&(rb_state8.lock_33));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_0 = (rb_state9.var_0) + (9961 < rb_input_size ? (uint32_t)rb_input[9961] : 0xd168e26b);
                    rb_state9.var_6 = (rb_state9.var_6) ^ (4514 < rb_input_size ? (uint32_t)rb_input[4514] : 0x19ea81c7);
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_0 = (rb_state10.var_0) - (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0x4ea929b5));
                    if ((rb_state10.var_0) == (0x889e61c9)) {
                        if ((rb_state10.var_1) != (0x0)) {
                            if (!((rb_state10.var_1) == (rb_state10.var_14))) {
                                racebench_trigger(10);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_11
                    if ((rb_state11.var_0) == (0x7abea7f4)) {
                        if ((rb_state11.var_1) != (0x0)) {
                            if (!((rb_state11.var_1) == (rb_state11.var_33))) {
                                racebench_trigger(11);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_0 = (rb_state12.var_0) ^ ((0xebec6c22) + (rb_state12.var_0));
                    if ((rb_state12.var_0) == (0x3414942e)) {
                        pthread_mutex_lock(&(rb_state12.lock_23));
                        rb_state12.var_3 = 0xc8ceb2df;
                        rb_state12.var_4 = 0x8b209ae5;
                        rb_state12.var_5 = (rb_state12.var_5) ^ (((rb_state12.var_2) == (0x0)) ? (rb_state12.var_3) : (0xe0a0acc9));
                        rb_state12.var_6 = (rb_state12.var_6) - (((rb_state12.var_1) == (0x0)) ? (rb_state12.var_4) : (0xcf39028c));
                        rb_state12.var_7 = (rb_state12.var_7) - (((0xc1f52bf0) ^ (0xf611f4fd)) ^ (rb_state12.var_5));
                        rb_state12.var_8 = (rb_state12.var_8) ^ (((0xdb49cf01) - (rb_state12.var_3)) ^ (rb_state12.var_6));
                        rb_state12.var_9 = (rb_state12.var_9) + (rb_state12.var_7);
                        rb_state12.var_10 = (rb_state12.var_10) - (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_8) : (0xf43fe696));
                        rb_state12.var_11 = (rb_state12.var_11) + (rb_state12.var_9);
                        rb_state12.var_12 = (rb_state12.var_12) - (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_11) : (0x7a017ed3));
                        rb_state12.var_2 = (rb_state12.var_2) + (rb_state12.var_12);
                        rb_state12.var_1 = rb_state12.var_2;
                        pthread_mutex_unlock(&(rb_state12.lock_23));
                    }
                    if ((rb_state12.var_0) == (0x11b1cd25)) {
                        if (!((rb_state12.var_1) == (rb_state12.var_2))) {
                            racebench_trigger(12);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_0 = (rb_state13.var_0) - (0x6d492185);
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    rb_state15.var_0 = (rb_state15.var_0) - (0x856a0bc1);
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_0 = (rb_state18.var_0) + ((0xea100f7c) ^ (0x2249c8bc));
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    rb_state19.var_0 = (rb_state19.var_0) - (0xaf6d9c17);
                    if ((rb_state19.var_0) == (0x705ba871)) {
                        pthread_mutex_lock(&(rb_state19.lock_8));
                        rb_state19.var_3 = 0x1425258e;
                        rb_state19.var_4 = 0x8717c080;
                        rb_state19.var_5 = (rb_state19.var_5) ^ (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_2) : (0xd0a79e03));
                        rb_state19.var_6 = (rb_state19.var_6) - (((rb_state19.var_3) == (0x0)) ? (rb_state19.var_4) : (0xb913b375));
                        rb_state19.var_7 = (rb_state19.var_7) + (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_6) : (0x4b2c9424));
                        rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_7);
                        pthread_mutex_unlock(&(rb_state19.lock_8));
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
                    rb_state0.var_0 = (rb_state0.var_0) ^ (12457 < rb_input_size ? (uint32_t)rb_input[12457] : 0xf1862e0e);
                    if ((rb_state0.var_0) == (0xe2e7494)) {
                        pthread_mutex_lock(&(rb_state0.lock_11));
                        rb_state0.var_3 = 0xdd34be4a;
                        rb_state0.var_4 = 0x1c2fcab;
                        rb_state0.var_5 = 0x6109ec94;
                        rb_state0.var_6 = (rb_state0.var_6) ^ (((0x8d89cbd3) + (0x4c13c619)) ^ (rb_state0.var_3));
                        rb_state0.var_7 = (rb_state0.var_7) ^ (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_4) : (0x6802421a));
                        rb_state0.var_8 = (rb_state0.var_8) ^ (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_6) : (0x740adfba));
                        rb_state0.var_9 = (rb_state0.var_9) ^ (((0xde794a1a) ^ (rb_state0.var_7)) - (0x31469c3b));
                        rb_state0.var_10 = (rb_state0.var_10) + (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_9) : (0x5af5d246));
                        rb_state0.var_1 = (rb_state0.var_1) - (rb_state0.var_10);
                        pthread_mutex_unlock(&(rb_state0.lock_11));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_1
                    rb_state1.var_0 = (rb_state1.var_0) + (0x50e8ee12);
                    if ((rb_state1.var_0) == (0x89ce674a)) {
                        rb_state1.var_2 = rb_state1.var_1;
                    }
                    if ((rb_state1.var_0) == (0xd1b1e4f2)) {
                        if (!((rb_state1.var_1) == (rb_state1.var_2))) {
                            racebench_trigger(1);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_2
                    rb_state2.var_0 = (rb_state2.var_0) ^ ((0xc22247a3) ^ (0xa612a0b4));
                    if ((rb_state2.var_0) == (0x230689d0)) {
                        pthread_mutex_lock(&(rb_state2.lock_25));
                        rb_state2.var_3 = 0x61ec7ec1;
                        rb_state2.var_4 = 0xe4addcd8;
                        rb_state2.var_5 = (rb_state2.var_5) + (rb_state2.var_1);
                        rb_state2.var_6 = (rb_state2.var_6) + (((((0x12062589) - (rb_state2.var_2)) - (rb_state2.var_4)) - (rb_state2.var_3)) + (rb_state2.var_3));
                        rb_state2.var_7 = (rb_state2.var_7) + (rb_state2.var_5);
                        rb_state2.var_8 = (rb_state2.var_8) - (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_6) : (0xef1a3c45));
                        rb_state2.var_9 = (rb_state2.var_9) + (((((0x7d1b68d5) - (rb_state2.var_8)) ^ (rb_state2.var_5)) + (rb_state2.var_7)) ^ (0xfa75c376));
                        rb_state2.var_2 = (rb_state2.var_2) ^ (rb_state2.var_9);
                        rb_state2.var_1 = rb_state2.var_2;
                        pthread_mutex_unlock(&(rb_state2.lock_25));
                    }
                    if ((rb_state2.var_0) == (0x47366ec7)) {
                        rb_state2.var_10 = 0x1d8554a5;
                        rb_state2.var_11 = 0xc3ce4070;
                        rb_state2.var_12 = (rb_state2.var_12) + (((rb_state2.var_8) == (0x0)) ? (rb_state2.var_7) : (0xd76beaf4));
                        rb_state2.var_13 = (rb_state2.var_13) - (((rb_state2.var_9) == (0x0)) ? (rb_state2.var_6) : (0xf411e22b));
                        rb_state2.var_14 = (rb_state2.var_14) + (rb_state2.var_10);
                        rb_state2.var_15 = (rb_state2.var_15) - (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_11) : (0x47be3c5));
                        rb_state2.var_16 = (rb_state2.var_16) + (rb_state2.var_12);
                        rb_state2.var_17 = (rb_state2.var_17) ^ (((((0x4f78bdb9) - (rb_state2.var_14)) - (0x794de85)) + (rb_state2.var_11)) - (rb_state2.var_13));
                        rb_state2.var_18 = (rb_state2.var_18) - (((rb_state2.var_12) == (0x0)) ? (rb_state2.var_15) : (0x7df30194));
                        rb_state2.var_19 = (rb_state2.var_19) + (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_16) : (0x16052967));
                        rb_state2.var_20 = (rb_state2.var_20) + (((0xcab39517) - (rb_state2.var_14)) - (rb_state2.var_17));
                        rb_state2.var_21 = (rb_state2.var_21) ^ (((0xbcc6fab8) - (rb_state2.var_15)) + (rb_state2.var_18));
                        rb_state2.var_22 = (rb_state2.var_22) - (((((0x5c0874ee) + (rb_state2.var_16)) - (rb_state2.var_20)) ^ (rb_state2.var_19)) + (0x21251a9e));
                        rb_state2.var_23 = (rb_state2.var_23) ^ (((0xd2b016b2) ^ (rb_state2.var_21)) + (rb_state2.var_17));
                        rb_state2.var_24 = (rb_state2.var_24) ^ (((((0xea8b0c8a) ^ (rb_state2.var_18)) - (rb_state2.var_19)) + (rb_state2.var_22)) ^ (rb_state2.var_23));
                        rb_state2.var_1 = (rb_state2.var_1) - (rb_state2.var_24);
                    }
                    if ((rb_state2.var_0) == (0x47366ec7)) {
                        if (!((rb_state2.var_1) == (rb_state2.var_2))) {
                            racebench_trigger(2);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_3
                    rb_state3.var_0 = (rb_state3.var_0) ^ (0x5925e738);
                    rb_state3.var_0 = (rb_state3.var_0) - ((0xd54c96) ^ (0x989e51fc));
                    if ((rb_state3.var_0) == (0xb8f3810c)) {
                        rb_state3.var_2 = rb_state3.var_1;
                    }
                    if ((rb_state3.var_0) == (0xb8f3810c)) {
                        if (!((rb_state3.var_1) == (rb_state3.var_2))) {
                            racebench_trigger(3);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_4
                    rb_state4.var_0 = (rb_state4.var_0) ^ (0x308e06fa);
                    rb_state4.var_0 = (rb_state4.var_0) - (0x97c21349);
                    if ((rb_state4.var_0) == (0x5ccdf3ef)) {
                        pthread_mutex_lock(&(rb_state4.lock_8));
                        rb_state4.var_2 = rb_state4.var_1;
                        pthread_mutex_unlock(&(rb_state4.lock_8));
                    }
                    if ((rb_state4.var_0) == (0x5ccdf3ef)) {
                        if (!((rb_state4.var_1) == (rb_state4.var_2))) {
                            racebench_trigger(4);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_5
                    rb_state5.var_0 = (rb_state5.var_0) + (4205 < rb_input_size ? (uint32_t)rb_input[4205] : 0xf7a7aa6);
                    if ((rb_state5.var_0) == (0xd9640c95)) {
                        rb_state5.var_7 = 0x653a1baa;
                        rb_state5.var_8 = (rb_state5.var_8) + (((rb_state5.var_7) == (0x0)) ? (rb_state5.var_6) : (0x85c2b385));
                        rb_state5.var_9 = (rb_state5.var_9) + (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_8) : (0x1e18cfd8));
                        rb_state5.var_6 = (rb_state5.var_6) + (rb_state5.var_9);
                        rb_state5.var_1 = rb_state5.var_6;
                    }
                    if ((rb_state5.var_0) == (0x7f4dc996)) {
                        pthread_mutex_lock(&(rb_state5.lock_15));
                        rb_state5.var_11 = 0xed93ab0f;
                        rb_state5.var_12 = 0xfa57e07f;
                        rb_state5.var_13 = (rb_state5.var_13) + (((rb_state5.var_11) == (0x0)) ? (rb_state5.var_12) : (0x59d35ab8));
                        rb_state5.var_14 = (rb_state5.var_14) + (((((0xf9f75bdc) - (rb_state5.var_13)) + (rb_state5.var_8)) ^ (0x6f5a512c)) + (rb_state5.var_7));
                        rb_state5.var_10 = (rb_state5.var_10) + (rb_state5.var_14);
                        rb_state5.var_1 = rb_state5.var_10;
                        pthread_mutex_unlock(&(rb_state5.lock_15));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_0 = (rb_state6.var_0) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_0) : (0x54a9299a));
                    rb_state6.var_0 = (rb_state6.var_0) + (0x93cde85b);
                    if ((rb_state6.var_0) == (0xcb5d4431)) {
                        if ((rb_state6.var_1) != (0x0)) {
                            if (!((rb_state6.var_1) == (rb_state6.var_10))) {
                                racebench_trigger(6);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_7
                    if ((rb_state7.var_0) == (0xd9571d80)) {
                        pthread_mutex_lock(&(rb_state7.lock_5));
                        rb_state7.var_3 = 0x5e28fe2;
                        rb_state7.var_4 = (rb_state7.var_4) ^ (((rb_state7.var_2) == (0x0)) ? (rb_state7.var_3) : (0x9ef0e894));
                        rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_4);
                        pthread_mutex_unlock(&(rb_state7.lock_5));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_0 = (rb_state8.var_0) + ((0xdca1945b) ^ (0xed17258e));
                    if ((rb_state8.var_0) == (0xe55544c9)) {
                        pthread_mutex_lock(&(rb_state8.lock_33));
                        if ((rb_state8.var_1) != (0x0)) {
                            rb_state8.var_34 = !((rb_state8.var_1) == (rb_state8.var_15));
                        }
                        pthread_mutex_unlock(&(rb_state8.lock_33));
                    }
                    rb_state8.var_17 = (rb_state8.var_17) - ((0x2c2ebd8e) - (rb_state8.var_15));
                    if ((rb_state8.var_16) == (0x1c8f125)) {
                        pthread_mutex_lock(&(rb_state8.lock_88));
                        rb_state8.var_63 = 0x20e11459;
                        rb_state8.var_64 = (rb_state8.var_64) - (rb_state8.var_34);
                        rb_state8.var_65 = (rb_state8.var_65) - (((((0xb4f59be5) ^ (rb_state8.var_31)) + (0x4a3c3eb9)) + (rb_state8.var_63)) + (rb_state8.var_35));
                        rb_state8.var_66 = (rb_state8.var_66) - (0xe4b975b);
                        rb_state8.var_67 = (rb_state8.var_67) - (((0xbe509d85) - (rb_state8.var_32)) + (rb_state8.var_36));
                        rb_state8.var_68 = (rb_state8.var_68) ^ (((0x8287bf06) + (rb_state8.var_37)) ^ (rb_state8.var_64));
                        rb_state8.var_69 = (rb_state8.var_69) ^ (rb_state8.var_65);
                        rb_state8.var_70 = (rb_state8.var_70) ^ (rb_state8.var_66);
                        rb_state8.var_71 = (rb_state8.var_71) ^ (rb_state8.var_67);
                        rb_state8.var_72 = (rb_state8.var_72) ^ (((0x77e28f17) - (rb_state8.var_38)) ^ (rb_state8.var_68));
                        rb_state8.var_73 = (rb_state8.var_73) - (((rb_state8.var_39) == (0x0)) ? (rb_state8.var_69) : (0x9d522a05));
                        rb_state8.var_74 = (rb_state8.var_74) ^ (((rb_state8.var_40) == (0x0)) ? (rb_state8.var_70) : (0xbb3bfaa8));
                        rb_state8.var_75 = (rb_state8.var_75) + (((rb_state8.var_41) == (0x0)) ? (rb_state8.var_71) : (0x678eee60));
                        rb_state8.var_76 = (rb_state8.var_76) - (((rb_state8.var_72) == (0x0)) ? (rb_state8.var_73) : (0xaae4de2));
                        rb_state8.var_77 = (rb_state8.var_77) - (rb_state8.var_74);
                        rb_state8.var_78 = (rb_state8.var_78) - (((rb_state8.var_75) == (0x0)) ? (rb_state8.var_76) : (0x25f8b18f));
                        rb_state8.var_79 = (rb_state8.var_79) - (((((0x82db2bf9) - (0x4fceb750)) + (rb_state8.var_78)) - (rb_state8.var_77)) + (0x234cd52e));
                        rb_state8.var_62 = (rb_state8.var_62) + (rb_state8.var_79);
                        rb_state8.var_35 = rb_state8.var_62;
                        pthread_mutex_unlock(&(rb_state8.lock_88));
                    }
                    if ((rb_state8.var_16) == (0x1c8f125)) {
                        if ((rb_state8.var_35) != (0x0)) {
                            if (!((rb_state8.var_35) == (rb_state8.var_80))) {
                                racebench_trigger(8);
                            }
                        }
                    }
                    if ((rb_state8.var_16) == (0x1c8f125)) {
                        pthread_mutex_lock(&(rb_state8.lock_88));
                        rb_state8.var_81 = 0x4ca576d0;
                        rb_state8.var_82 = (rb_state8.var_82) + (((0xb796fa51) + (rb_state8.var_81)) - (rb_state8.var_44));
                        rb_state8.var_83 = (rb_state8.var_83) + (((0x94540786) - (rb_state8.var_45)) - (rb_state8.var_42));
                        rb_state8.var_84 = (rb_state8.var_84) + (rb_state8.var_43);
                        rb_state8.var_85 = (rb_state8.var_85) + (((((0x5810d616) - (rb_state8.var_82)) + (rb_state8.var_83)) ^ (0x61b9ed15)) ^ (rb_state8.var_46));
                        rb_state8.var_86 = (rb_state8.var_86) ^ (((rb_state8.var_47) == (0x0)) ? (rb_state8.var_84) : (0x9b5ef3a9));
                        rb_state8.var_87 = (rb_state8.var_87) ^ (((((0x9a86ac6c) + (rb_state8.var_86)) + (rb_state8.var_85)) ^ (rb_state8.var_48)) - (rb_state8.var_49));
                        rb_state8.var_80 = (rb_state8.var_80) + (rb_state8.var_87);
                        rb_state8.var_35 = rb_state8.var_80;
                        pthread_mutex_unlock(&(rb_state8.lock_88));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_0 = (rb_state9.var_0) - (((rb_state9.var_0) == (0x0)) ? (rb_state9.var_0) : (0x109f8075));
                    if ((rb_state9.var_0) == (0xdb0aa21)) {
                        usleep(100);
                    }
                    if ((rb_state9.var_0) == (0xfd1129ac)) {
                        pthread_mutex_lock(&(rb_state9.lock_12));
                        rb_state9.var_3 = 0xbb76cce2;
                        rb_state9.var_4 = 0x2b170f68;
                        rb_state9.var_5 = 0x4769894e;
                        rb_state9.var_6 = (rb_state9.var_6) + (rb_state9.var_1);
                        rb_state9.var_7 = (rb_state9.var_7) ^ (rb_state9.var_3);
                        rb_state9.var_8 = (rb_state9.var_8) + (((0xdf62987f) ^ (0xc7d12adf)) - (rb_state9.var_5));
                        rb_state9.var_9 = (rb_state9.var_9) - (((((0xec77f7c) - (rb_state9.var_6)) + (rb_state9.var_2)) ^ (rb_state9.var_4)) ^ (rb_state9.var_3));
                        rb_state9.var_10 = (rb_state9.var_10) + (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_8) : (0xf7cfa09e));
                        rb_state9.var_11 = (rb_state9.var_11) + (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_10) : (0x7f075894));
                        rb_state9.var_1 = (rb_state9.var_1) + (rb_state9.var_11);
                        rb_state9.var_2 = rb_state9.var_1;
                        pthread_mutex_unlock(&(rb_state9.lock_12));
                    }
                    if ((rb_state9.var_0) == (0xfd1129ac)) {
                        rb_state9.var_13 = !((rb_state9.var_2) == (rb_state9.var_1));
                    }
                    if ((rb_state9.var_4) == (0x2b170f68)) {
                        pthread_mutex_lock(&(rb_state9.lock_38));
                        rb_state9.var_16 = 0x75eb7f6e;
                        rb_state9.var_17 = 0xe35f0f26;
                        rb_state9.var_18 = 0x9dd569d8;
                        rb_state9.var_19 = (rb_state9.var_19) + (rb_state9.var_5);
                        rb_state9.var_20 = (rb_state9.var_20) ^ (((0xc6e3642) ^ (rb_state9.var_18)) + (0xd3945b68));
                        rb_state9.var_21 = (rb_state9.var_21) - (((0xc1965274) + (rb_state9.var_7)) - (rb_state9.var_17));
                        rb_state9.var_22 = (rb_state9.var_22) + (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_16) : (0x21f0406c));
                        rb_state9.var_23 = (rb_state9.var_23) + (((rb_state9.var_9) == (0x0)) ? (rb_state9.var_19) : (0x8a5ad063));
                        rb_state9.var_24 = (rb_state9.var_24) - (((((0x8ff40866) ^ (rb_state9.var_20)) ^ (rb_state9.var_10)) + (rb_state9.var_11)) + (rb_state9.var_21));
                        rb_state9.var_25 = (rb_state9.var_25) + (((rb_state9.var_22) == (0x0)) ? (rb_state9.var_23) : (0x698d27eb));
                        rb_state9.var_26 = (rb_state9.var_26) + (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_24) : (0xf367a7b7));
                        rb_state9.var_27 = (rb_state9.var_27) ^ (((0x3ac70f1f) - (0x41c44b94)) - (rb_state9.var_25));
                        rb_state9.var_28 = (rb_state9.var_28) ^ (((rb_state9.var_14) == (0x0)) ? (rb_state9.var_26) : (0xdf7ff408));
                        rb_state9.var_29 = (rb_state9.var_29) ^ (((rb_state9.var_27) == (0x0)) ? (rb_state9.var_28) : (0x85d4c6a2));
                        rb_state9.var_15 = (rb_state9.var_15) - (rb_state9.var_29);
                        rb_state9.var_14 = rb_state9.var_15;
                        pthread_mutex_unlock(&(rb_state9.lock_38));
                    }
                    if ((rb_state9.var_4) == (0x2b170f68)) {
                        rb_state9.var_30 = 0x8f11ce82;
                        rb_state9.var_31 = 0x773264d3;
                        rb_state9.var_32 = (rb_state9.var_32) - (rb_state9.var_15);
                        rb_state9.var_33 = (rb_state9.var_33) + (((rb_state9.var_16) == (0x0)) ? (rb_state9.var_31) : (0x7c069a52));
                        rb_state9.var_34 = (rb_state9.var_34) - (((0xd26ac2b0) ^ (rb_state9.var_30)) + (rb_state9.var_17));
                        rb_state9.var_35 = (rb_state9.var_35) ^ (((rb_state9.var_18) == (0x0)) ? (rb_state9.var_32) : (0x406f3771));
                        rb_state9.var_36 = (rb_state9.var_36) ^ (((rb_state9.var_33) == (0x0)) ? (rb_state9.var_34) : (0x4d43ba50));
                        rb_state9.var_37 = (rb_state9.var_37) - (((((0x2e1562fa) ^ (0x9eb3fdd1)) - (0x83ac2a7f)) + (rb_state9.var_36)) - (rb_state9.var_35));
                        rb_state9.var_14 = (rb_state9.var_14) + (rb_state9.var_37);
                    }
                    if ((rb_state9.var_4) == (0x2b170f68)) {
                        if (!((rb_state9.var_14) == (rb_state9.var_15))) {
                            racebench_trigger(9);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_0 = (rb_state10.var_0) ^ ((0xe5b841c1) - (0x5b4e490e));
                    if ((rb_state10.var_0) == (0xd7478b7e)) {
                        rb_state10.var_9 = 0x513fd14d;
                        rb_state10.var_10 = (rb_state10.var_10) + (((rb_state10.var_7) == (0x0)) ? (rb_state10.var_5) : (0x6461b945));
                        rb_state10.var_11 = (rb_state10.var_11) - (((((0xdf8a7ddf) + (rb_state10.var_8)) - (rb_state10.var_6)) + (0x8b1f715a)) + (rb_state10.var_9));
                        rb_state10.var_12 = (rb_state10.var_12) ^ (((rb_state10.var_9) == (0x0)) ? (rb_state10.var_10) : (0x1f531ddb));
                        rb_state10.var_13 = (rb_state10.var_13) + (((((0x15f8f3dd) - (0x1ab081e2)) + (rb_state10.var_11)) ^ (rb_state10.var_12)) + (0xa0b9c421));
                        rb_state10.var_8 = (rb_state10.var_8) + (rb_state10.var_13);
                        rb_state10.var_1 = rb_state10.var_8;
                    }
                    if ((rb_state10.var_0) == (0x889e61c9)) {
                        pthread_mutex_lock(&(rb_state10.lock_28));
                        rb_state10.var_15 = 0x6fad06c1;
                        rb_state10.var_16 = (rb_state10.var_16) ^ (rb_state10.var_11);
                        rb_state10.var_17 = (rb_state10.var_17) + (((0x9188bf00) + (0xa17003dd)) ^ (rb_state10.var_10));
                        rb_state10.var_18 = (rb_state10.var_18) ^ (0x33fb47fc);
                        rb_state10.var_19 = (rb_state10.var_19) - (((((0x24807368) ^ (rb_state10.var_15)) ^ (0x61baa748)) + (rb_state10.var_16)) + (0xa6c3d114));
                        rb_state10.var_20 = (rb_state10.var_20) - (((0x92acfe14) + (rb_state10.var_17)) + (0xfa99e9d4));
                        rb_state10.var_21 = (rb_state10.var_21) + (((((0x484c815f) + (rb_state10.var_19)) - (0x4101bbe2)) - (rb_state10.var_18)) - (rb_state10.var_12));
                        rb_state10.var_22 = (rb_state10.var_22) ^ (rb_state10.var_20);
                        rb_state10.var_23 = (rb_state10.var_23) ^ (((0xa81e0d50) - (rb_state10.var_13)) + (rb_state10.var_21));
                        rb_state10.var_24 = (rb_state10.var_24) + (rb_state10.var_22);
                        rb_state10.var_25 = (rb_state10.var_25) - (rb_state10.var_23);
                        rb_state10.var_26 = (rb_state10.var_26) + (rb_state10.var_24);
                        rb_state10.var_27 = (rb_state10.var_27) + (((((0x929f284b) - (0xb2648312)) + (rb_state10.var_25)) - (rb_state10.var_26)) ^ (0xcfc10f8));
                        rb_state10.var_14 = (rb_state10.var_14) - (rb_state10.var_27);
                        rb_state10.var_1 = rb_state10.var_14;
                        pthread_mutex_unlock(&(rb_state10.lock_28));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_11
                    rb_state11.var_0 = (rb_state11.var_0) + (0xac504ca2);
                    rb_state11.var_0 = (rb_state11.var_0) ^ ((0x1959aec4) - (0x65a562cc));
                    if ((rb_state11.var_0) == (0x7abea7f4)) {
                        rb_state11.var_11 = (rb_state11.var_11) - (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_6) : (0x81778107));
                        rb_state11.var_12 = (rb_state11.var_12) ^ (((rb_state11.var_9) == (0x0)) ? (rb_state11.var_5) : (0xe3ff9d70));
                        rb_state11.var_13 = (rb_state11.var_13) + (((rb_state11.var_10) == (0x0)) ? (rb_state11.var_7) : (0x9c78c126));
                        rb_state11.var_14 = (rb_state11.var_14) + (((0xeb5d1b2b) - (rb_state11.var_11)) ^ (rb_state11.var_11));
                        rb_state11.var_15 = (rb_state11.var_15) - (((rb_state11.var_12) == (0x0)) ? (rb_state11.var_12) : (0xc15acc70));
                        rb_state11.var_16 = (rb_state11.var_16) ^ (rb_state11.var_13);
                        rb_state11.var_17 = (rb_state11.var_17) + (((rb_state11.var_13) == (0x0)) ? (rb_state11.var_14) : (0x871548d4));
                        rb_state11.var_18 = (rb_state11.var_18) + (((0xdb4b6ead) + (rb_state11.var_14)) + (rb_state11.var_15));
                        rb_state11.var_19 = (rb_state11.var_19) + (rb_state11.var_16);
                        rb_state11.var_20 = (rb_state11.var_20) - (((0x2679fd0f) + (rb_state11.var_15)) + (rb_state11.var_17));
                        rb_state11.var_21 = (rb_state11.var_21) + (((0xef8e74db) - (rb_state11.var_18)) - (rb_state11.var_16));
                        rb_state11.var_22 = (rb_state11.var_22) ^ (((0x8f39f01b) ^ (rb_state11.var_19)) - (0x50fdc3fe));
                        rb_state11.var_23 = (rb_state11.var_23) - (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_20) : (0x3e526228));
                        rb_state11.var_24 = (rb_state11.var_24) + (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_21) : (0x7df1cb97));
                        rb_state11.var_25 = (rb_state11.var_25) - (((0xad746f12) ^ (rb_state11.var_22)) + (rb_state11.var_19));
                        rb_state11.var_26 = (rb_state11.var_26) + (rb_state11.var_23);
                        rb_state11.var_27 = (rb_state11.var_27) + (((0x58c6dcee) + (rb_state11.var_24)) + (0x4e3d9b41));
                        rb_state11.var_28 = (rb_state11.var_28) ^ (((((0x1853c0ad) ^ (rb_state11.var_20)) ^ (rb_state11.var_25)) ^ (rb_state11.var_26)) + (0xf8d52ed8));
                        rb_state11.var_29 = (rb_state11.var_29) ^ (((rb_state11.var_21) == (0x0)) ? (rb_state11.var_27) : (0x37135412));
                        rb_state11.var_30 = (rb_state11.var_30) ^ (((0x59a484c0) - (rb_state11.var_22)) ^ (rb_state11.var_28));
                        rb_state11.var_31 = (rb_state11.var_31) ^ (rb_state11.var_29);
                        rb_state11.var_32 = (rb_state11.var_32) + (((((0x9cf2cfac) + (rb_state11.var_30)) ^ (0x1084d4ed)) - (0x62f73e2f)) + (rb_state11.var_31));
                        rb_state11.var_10 = (rb_state11.var_10) ^ (rb_state11.var_32);
                        rb_state11.var_1 = rb_state11.var_10;
                    }
                    if ((rb_state11.var_0) == (0x7abea7f4)) {
                        pthread_mutex_lock(&(rb_state11.lock_48));
                        rb_state11.var_34 = 0x4d38dd35;
                        rb_state11.var_35 = 0x89659cb3;
                        rb_state11.var_36 = 0xfff3a44;
                        rb_state11.var_37 = (rb_state11.var_37) - (rb_state11.var_34);
                        rb_state11.var_38 = (rb_state11.var_38) ^ (rb_state11.var_36);
                        rb_state11.var_39 = (rb_state11.var_39) - (((0xec708acd) ^ (rb_state11.var_23)) - (rb_state11.var_35));
                        rb_state11.var_40 = (rb_state11.var_40) - (rb_state11.var_37);
                        rb_state11.var_41 = (rb_state11.var_41) + (((rb_state11.var_38) == (0x0)) ? (rb_state11.var_39) : (0xdc051de8));
                        rb_state11.var_42 = (rb_state11.var_42) + (((rb_state11.var_24) == (0x0)) ? (rb_state11.var_40) : (0xb7b3c230));
                        rb_state11.var_43 = (rb_state11.var_43) ^ (((0x9e6be05c) + (0xd056c0e9)) ^ (rb_state11.var_41));
                        rb_state11.var_44 = (rb_state11.var_44) ^ (((rb_state11.var_25) == (0x0)) ? (rb_state11.var_42) : (0x66f23572));
                        rb_state11.var_45 = (rb_state11.var_45) ^ (rb_state11.var_43);
                        rb_state11.var_46 = (rb_state11.var_46) + (rb_state11.var_44);
                        rb_state11.var_47 = (rb_state11.var_47) - (((((0x6b854c5d) ^ (rb_state11.var_26)) ^ (rb_state11.var_27)) - (rb_state11.var_46)) ^ (rb_state11.var_45));
                        rb_state11.var_33 = (rb_state11.var_33) + (rb_state11.var_47);
                        rb_state11.var_1 = rb_state11.var_33;
                        pthread_mutex_unlock(&(rb_state11.lock_48));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_0 = (rb_state12.var_0) - (0x2262c709);
                    if ((rb_state12.var_0) == (0x11b1cd25)) {
                        rb_state12.var_13 = 0x15537037;
                        rb_state12.var_14 = (rb_state12.var_14) ^ (rb_state12.var_7);
                        rb_state12.var_15 = (rb_state12.var_15) ^ (((rb_state12.var_8) == (0x0)) ? (rb_state12.var_5) : (0x9e4a6d2b));
                        rb_state12.var_16 = (rb_state12.var_16) + (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_9) : (0x305c78ba));
                        rb_state12.var_17 = (rb_state12.var_17) - (((((0x2c3d22a4) + (0xb555a231)) + (rb_state12.var_13)) ^ (rb_state12.var_6)) + (0x6f92618b));
                        rb_state12.var_18 = (rb_state12.var_18) ^ (((0xc56cdb91) - (0xb0989ee5)) ^ (rb_state12.var_14));
                        rb_state12.var_19 = (rb_state12.var_19) + (((rb_state12.var_15) == (0x0)) ? (rb_state12.var_16) : (0x2d4abcb1));
                        rb_state12.var_20 = (rb_state12.var_20) ^ (((rb_state12.var_11) == (0x0)) ? (rb_state12.var_17) : (0xec80a24f));
                        rb_state12.var_21 = (rb_state12.var_21) - (((rb_state12.var_18) == (0x0)) ? (rb_state12.var_19) : (0x1c17f7a9));
                        rb_state12.var_22 = (rb_state12.var_22) + (((rb_state12.var_20) == (0x0)) ? (rb_state12.var_21) : (0xc2337eea));
                        rb_state12.var_1 = (rb_state12.var_1) ^ (rb_state12.var_22);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_0 = (rb_state13.var_0) + (((rb_state13.var_0) == (0x376e3d24)) ? (rb_state13.var_0) : (0x66feda11));
                    if ((rb_state13.var_0) == (0xd3a451b2)) {
                        rb_state13.var_3 = 0x1;
                    }
                    if ((rb_state13.var_0) == (0x3aa32bc3)) {
                        if ((rb_state13.var_3) != (0x0)) {
                            rb_state13.var_8 = !((rb_state13.var_1) == (rb_state13.var_2));
                        }
                    }
                    if ((rb_state13.var_0) == (0x3aa32bc3)) {
                        pthread_mutex_lock(&(rb_state13.lock_7));
                        rb_state13.var_4 = 0xd89feccd;
                        rb_state13.var_5 = (rb_state13.var_5) + (((((0x4da62703) ^ (rb_state13.var_3)) + (0xc5d5f6fc)) ^ (rb_state13.var_2)) - (rb_state13.var_1));
                        rb_state13.var_6 = (rb_state13.var_6) - (((((0x404b27cb) + (0x116c8510)) + (rb_state13.var_4)) - (rb_state13.var_5)) ^ (rb_state13.var_4));
                        rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_6);
                        rb_state13.var_1 = rb_state13.var_2;
                        pthread_mutex_unlock(&(rb_state13.lock_7));
                    }
                    rb_state13.var_5 = (rb_state13.var_5) + (3756 < rb_input_size ? (uint32_t)rb_input[3756] : 0x2bc98511);
                    if ((rb_state13.var_6) == (0x5242bc1c)) {
                        pthread_mutex_lock(&(rb_state13.lock_51));
                        rb_state13.var_16 = 0x4e8d8645;
                        rb_state13.var_17 = 0xde5f4fdc;
                        rb_state13.var_18 = (rb_state13.var_18) - (0xbd22217e);
                        rb_state13.var_19 = (rb_state13.var_19) + (((((0xf4dec9de) - (rb_state13.var_13)) + (rb_state13.var_16)) - (rb_state13.var_12)) - (rb_state13.var_17));
                        rb_state13.var_20 = (rb_state13.var_20) - (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_18) : (0x4453ed18));
                        rb_state13.var_21 = (rb_state13.var_21) - (rb_state13.var_19);
                        rb_state13.var_22 = (rb_state13.var_22) - (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_20) : (0xbeb6ec8f));
                        rb_state13.var_23 = (rb_state13.var_23) - (rb_state13.var_21);
                        rb_state13.var_24 = (rb_state13.var_24) ^ (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_22) : (0x72cb7708));
                        rb_state13.var_25 = (rb_state13.var_25) + (rb_state13.var_23);
                        rb_state13.var_26 = (rb_state13.var_26) - (((0x5182ecf8) - (0x3b01649)) ^ (rb_state13.var_24));
                        rb_state13.var_27 = (rb_state13.var_27) + (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_25) : (0x8c79e8de));
                        rb_state13.var_28 = (rb_state13.var_28) - (rb_state13.var_26);
                        rb_state13.var_29 = (rb_state13.var_29) ^ (rb_state13.var_27);
                        rb_state13.var_30 = (rb_state13.var_30) + (((rb_state13.var_28) == (0x0)) ? (rb_state13.var_29) : (0xaf1f59ff));
                        rb_state13.var_15 = (rb_state13.var_15) - (rb_state13.var_30);
                        rb_state13.var_9 = rb_state13.var_15;
                        pthread_mutex_unlock(&(rb_state13.lock_51));
                    }
                    if ((rb_state13.var_6) == (0x5242bc1c)) {
                        if ((rb_state13.var_9) != (0x0)) {
                            if (!((rb_state13.var_9) == (rb_state13.var_31))) {
                                racebench_trigger(13);
                            }
                        }
                    }
                    if ((rb_state13.var_6) == (0x5242bc1c)) {
                        pthread_mutex_lock(&(rb_state13.lock_51));
                        rb_state13.var_32 = 0x1fab000e;
                        rb_state13.var_33 = (rb_state13.var_33) - (((((0xa800d405) - (0x46a0ab6a)) ^ (rb_state13.var_32)) ^ (0x3b93594c)) - (rb_state13.var_17));
                        rb_state13.var_34 = (rb_state13.var_34) - (((rb_state13.var_20) == (0x0)) ? (rb_state13.var_19) : (0x916d8155));
                        rb_state13.var_35 = (rb_state13.var_35) + (((rb_state13.var_22) == (0x0)) ? (rb_state13.var_21) : (0xbbb40a89));
                        rb_state13.var_36 = (rb_state13.var_36) ^ (rb_state13.var_18);
                        rb_state13.var_37 = (rb_state13.var_37) + (rb_state13.var_33);
                        rb_state13.var_38 = (rb_state13.var_38) + (rb_state13.var_34);
                        rb_state13.var_39 = (rb_state13.var_39) + (((rb_state13.var_23) == (0x0)) ? (rb_state13.var_35) : (0x92d58359));
                        rb_state13.var_40 = (rb_state13.var_40) ^ (((rb_state13.var_24) == (0x0)) ? (rb_state13.var_36) : (0x23d844b6));
                        rb_state13.var_41 = (rb_state13.var_41) ^ (((((0xdf4808fc) - (rb_state13.var_25)) ^ (0xd85ca680)) + (rb_state13.var_37)) + (rb_state13.var_38));
                        rb_state13.var_42 = (rb_state13.var_42) - (rb_state13.var_39);
                        rb_state13.var_43 = (rb_state13.var_43) ^ (((rb_state13.var_26) == (0x0)) ? (rb_state13.var_40) : (0xea014732));
                        rb_state13.var_44 = (rb_state13.var_44) + (((0xec4dd620) ^ (rb_state13.var_41)) + (0x5a79b371));
                        rb_state13.var_45 = (rb_state13.var_45) - (((rb_state13.var_27) == (0x0)) ? (rb_state13.var_42) : (0x8cc97b31));
                        rb_state13.var_46 = (rb_state13.var_46) - (((rb_state13.var_28) == (0x0)) ? (rb_state13.var_43) : (0xca8226d1));
                        rb_state13.var_47 = (rb_state13.var_47) ^ (((0xa778245f) + (0x95e9e773)) ^ (rb_state13.var_44));
                        rb_state13.var_48 = (rb_state13.var_48) ^ (((rb_state13.var_29) == (0x0)) ? (rb_state13.var_45) : (0xc9a2a8f2));
                        rb_state13.var_49 = (rb_state13.var_49) - (((rb_state13.var_46) == (0x0)) ? (rb_state13.var_47) : (0x5d0249eb));
                        rb_state13.var_50 = (rb_state13.var_50) + (((rb_state13.var_48) == (0x0)) ? (rb_state13.var_49) : (0x3c744c40));
                        rb_state13.var_31 = (rb_state13.var_31) + (rb_state13.var_50);
                        rb_state13.var_9 = rb_state13.var_31;
                        pthread_mutex_unlock(&(rb_state13.lock_51));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    rb_state15.var_0 = (rb_state15.var_0) ^ ((0x9c2e48ce) - (0xcecb7662));
                    if ((rb_state15.var_0) == (0x44b1d748)) {
                        rb_state15.var_2 = rb_state15.var_1;
                    }
                    if ((rb_state15.var_0) == (0x89d30524)) {
                        rb_state15.var_3 = 0x24a07722;
                        rb_state15.var_4 = (rb_state15.var_4) - (((rb_state15.var_4) == (0x0)) ? (rb_state15.var_3) : (0x2ded5fa6));
                        rb_state15.var_5 = (rb_state15.var_5) ^ (((rb_state15.var_2) == (0x0)) ? (rb_state15.var_4) : (0xaf56a837));
                        rb_state15.var_1 = (rb_state15.var_1) ^ (rb_state15.var_5);
                    }
                    if ((rb_state15.var_0) == (0x89d30524)) {
                        pthread_mutex_lock(&(rb_state15.lock_6));
                        if (!((rb_state15.var_1) == (rb_state15.var_2))) {
                            racebench_trigger(15);
                        }
                        pthread_mutex_unlock(&(rb_state15.lock_6));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_16
                    rb_state16.var_0 = (rb_state16.var_0) - (((rb_state16.var_0) == (0x1bc94b8)) ? (rb_state16.var_0) : (0x41eebace));
                    if ((rb_state16.var_0) == (0x2a240f42)) {
                        pthread_mutex_lock(&(rb_state16.lock_17));
                        rb_state16.var_3 = 0x1b8cca0f;
                        rb_state16.var_4 = (rb_state16.var_4) + (((((0xa46b4072) ^ (0x4101f1ba)) - (rb_state16.var_1)) - (rb_state16.var_2)) ^ (rb_state16.var_3));
                        rb_state16.var_5 = (rb_state16.var_5) - (rb_state16.var_4);
                        rb_state16.var_6 = (rb_state16.var_6) - (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_3) : (0xe94dac9));
                        rb_state16.var_7 = (rb_state16.var_7) - (((0x17ac1abc) - (rb_state16.var_6)) - (rb_state16.var_4));
                        rb_state16.var_8 = (rb_state16.var_8) + (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_6) : (0x3a7c0b82));
                        rb_state16.var_9 = (rb_state16.var_9) + (rb_state16.var_7);
                        rb_state16.var_10 = (rb_state16.var_10) - (rb_state16.var_8);
                        rb_state16.var_11 = (rb_state16.var_11) ^ (((((0x5d401295) + (rb_state16.var_9)) + (rb_state16.var_10)) - (0x52fc41)) ^ (rb_state16.var_7));
                        rb_state16.var_2 = (rb_state16.var_2) - (rb_state16.var_11);
                        rb_state16.var_1 = rb_state16.var_2;
                        pthread_mutex_unlock(&(rb_state16.lock_17));
                    }
                    if ((rb_state16.var_0) == (0xe8355474)) {
                        pthread_mutex_lock(&(rb_state16.lock_17));
                        rb_state16.var_12 = 0x804ab423;
                        rb_state16.var_13 = (rb_state16.var_13) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_12) : (0x7cb19940));
                        rb_state16.var_14 = (rb_state16.var_14) + (((0xb3e16147) + (rb_state16.var_8)) ^ (rb_state16.var_11));
                        rb_state16.var_15 = (rb_state16.var_15) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_13) : (0x634724bb));
                        rb_state16.var_16 = (rb_state16.var_16) - (((rb_state16.var_14) == (0x0)) ? (rb_state16.var_15) : (0x532da81f));
                        rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_16);
                        pthread_mutex_unlock(&(rb_state16.lock_17));
                    }
                    if ((rb_state16.var_0) == (0xe8355474)) {
                        if (!((rb_state16.var_1) == (rb_state16.var_2))) {
                            racebench_trigger(16);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_17
                    rb_state17.var_0 = (rb_state17.var_0) ^ (0xeac9e2fb);
                    rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0xeac9e2fb)) ? (rb_state17.var_0) : (0xcaaea8c0));
                    if ((rb_state17.var_0) == (0x4f8af576)) {
                        rb_state17.var_5 = (rb_state17.var_5) ^ (((0xff537fff) ^ (rb_state17.var_3)) + (rb_state17.var_5));
                        rb_state17.var_6 = (rb_state17.var_6) - (((rb_state17.var_2) == (0x0)) ? (rb_state17.var_4) : (0xa0f28866));
                        rb_state17.var_7 = (rb_state17.var_7) + (rb_state17.var_6);
                        rb_state17.var_8 = (rb_state17.var_8) - (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_7) : (0x284e3b5a));
                        rb_state17.var_9 = (rb_state17.var_9) ^ (rb_state17.var_5);
                        rb_state17.var_10 = (rb_state17.var_10) ^ (((0x58e4d8ac) ^ (rb_state17.var_9)) + (rb_state17.var_6));
                        rb_state17.var_11 = (rb_state17.var_11) - (rb_state17.var_7);
                        rb_state17.var_12 = (rb_state17.var_12) - (rb_state17.var_8);
                        rb_state17.var_13 = (rb_state17.var_13) + (((((0x2b01738e) ^ (rb_state17.var_11)) - (rb_state17.var_10)) - (rb_state17.var_10)) - (rb_state17.var_9));
                        rb_state17.var_14 = (rb_state17.var_14) - (((0x228d02ea) ^ (rb_state17.var_11)) + (rb_state17.var_12));
                        rb_state17.var_15 = (rb_state17.var_15) ^ (((0x43973176) ^ (rb_state17.var_12)) - (rb_state17.var_13));
                        rb_state17.var_16 = (rb_state17.var_16) + (rb_state17.var_13);
                        rb_state17.var_17 = (rb_state17.var_17) - (((rb_state17.var_14) == (0x0)) ? (rb_state17.var_14) : (0x8f4179a));
                        rb_state17.var_18 = (rb_state17.var_18) - (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_15) : (0xb8c704a2));
                        rb_state17.var_19 = (rb_state17.var_19) ^ (((0x8ef67bf1) + (rb_state17.var_16)) ^ (rb_state17.var_16));
                        rb_state17.var_20 = (rb_state17.var_20) - (rb_state17.var_17);
                        rb_state17.var_21 = (rb_state17.var_21) + (((0x757405c1) - (rb_state17.var_17)) - (rb_state17.var_18));
                        rb_state17.var_22 = (rb_state17.var_22) - (((0x36dc712a) ^ (rb_state17.var_18)) + (rb_state17.var_19));
                        rb_state17.var_23 = (rb_state17.var_23) ^ (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_21) : (0x4bfe0505));
                        rb_state17.var_24 = (rb_state17.var_24) - (((rb_state17.var_22) == (0x0)) ? (rb_state17.var_23) : (0xbb5c5b6));
                        rb_state17.var_4 = (rb_state17.var_4) + (rb_state17.var_24);
                        rb_state17.var_1 = rb_state17.var_4;
                    }
                    if ((rb_state17.var_0) == (0x6ff1c04d)) {
                        if ((rb_state17.var_1) != (0x0)) {
                            if (!((rb_state17.var_1) == (rb_state17.var_25))) {
                                racebench_trigger(17);
                            }
                        }
                    }
                    if ((rb_state17.var_0) == (0x6ff1c04d)) {
                        pthread_mutex_lock(&(rb_state17.lock_32));
                        rb_state17.var_26 = 0xff9e9b04;
                        rb_state17.var_27 = 0x7a542c17;
                        rb_state17.var_28 = (rb_state17.var_28) - (((rb_state17.var_20) == (0x0)) ? (rb_state17.var_26) : (0x59c29038));
                        rb_state17.var_29 = (rb_state17.var_29) + (rb_state17.var_19);
                        rb_state17.var_30 = (rb_state17.var_30) - (((((0xab71f708) ^ (rb_state17.var_28)) + (0x403a9fab)) - (rb_state17.var_27)) + (rb_state17.var_21));
                        rb_state17.var_31 = (rb_state17.var_31) - (((rb_state17.var_29) == (0x0)) ? (rb_state17.var_30) : (0x442eb854));
                        rb_state17.var_25 = (rb_state17.var_25) ^ (rb_state17.var_31);
                        rb_state17.var_1 = rb_state17.var_25;
                        pthread_mutex_unlock(&(rb_state17.lock_32));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_0 = (rb_state18.var_0) ^ (((rb_state18.var_0) == (0x90b38f80)) ? (rb_state18.var_0) : (0xa0a04365));
                    if ((rb_state18.var_0) == (0xc55751e5)) {
                        rb_state18.var_7 = 0x9564cf15;
                        rb_state18.var_8 = 0x3d6b77cc;
                        rb_state18.var_9 = (rb_state18.var_9) - (((0x6ce49bd3) ^ (rb_state18.var_5)) ^ (rb_state18.var_8));
                        rb_state18.var_10 = (rb_state18.var_10) ^ (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_7) : (0x27225e53));
                        rb_state18.var_11 = (rb_state18.var_11) + (((((0x7ce85023) - (rb_state18.var_6)) - (rb_state18.var_9)) ^ (rb_state18.var_10)) ^ (0xdf9064a2));
                        rb_state18.var_6 = (rb_state18.var_6) ^ (rb_state18.var_11);
                        rb_state18.var_1 = rb_state18.var_6;
                    }
                    if ((rb_state18.var_0) == (0x65f71280)) {
                        if ((rb_state18.var_1) != (0x0)) {
                            if (!((rb_state18.var_1) == (rb_state18.var_12))) {
                                racebench_trigger(18);
                            }
                        }
                    }
                    if ((rb_state18.var_0) == (0x65f71280)) {
                        pthread_mutex_lock(&(rb_state18.lock_19));
                        rb_state18.var_13 = 0xc6650a1f;
                        rb_state18.var_14 = (rb_state18.var_14) - (((rb_state18.var_9) == (0x0)) ? (rb_state18.var_13) : (0xb1d3469f));
                        rb_state18.var_15 = (rb_state18.var_15) - ((0x59dc70ec) + (0xa1a69a10));
                        rb_state18.var_16 = (rb_state18.var_16) ^ (((((0x36bcb5fb) - (rb_state18.var_7)) - (0x15f237bb)) + (rb_state18.var_8)) ^ (0x1a315e72));
                        rb_state18.var_17 = (rb_state18.var_17) ^ (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_15) : (0x7052e9b5));
                        rb_state18.var_18 = (rb_state18.var_18) ^ (((rb_state18.var_16) == (0x0)) ? (rb_state18.var_17) : (0xbfd5beae));
                        rb_state18.var_12 = (rb_state18.var_12) ^ (rb_state18.var_18);
                        rb_state18.var_1 = rb_state18.var_12;
                        pthread_mutex_unlock(&(rb_state18.lock_19));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    rb_state19.var_0 = (rb_state19.var_0) ^ (0x277264f6);
                    if ((rb_state19.var_0) == (0x1fc94488)) {
                        rb_state19.var_2 = rb_state19.var_1;
                    }
                    if ((rb_state19.var_0) == (0x705ba871)) {
                        if (!((rb_state19.var_1) == (rb_state19.var_2))) {
                            racebench_trigger(19);
                        }
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