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
            #ifdef RACEBENCH_BUG_0
            rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x0)) ? (rb_state0.var_0) : (0xa37d079b));
            #endif
            #ifdef RACEBENCH_BUG_18
            rb_state18.var_0 = (rb_state18.var_0) + ((0x25e34c2e) - (0x768754c2));
            #endif
            Error = pthread_create(&PThreadTable[i], NULL, (void *(*)(void *))(Render_Loop), NULL);
            if (Error != 0) {
                printf("Error in pthread_create().\n");
                exit(-1);
            }
        }

        #ifdef RACEBENCH_BUG_0
        rb_state0.var_1 = (rb_state0.var_1) - (((rb_state0.var_1) == (0x0)) ? (rb_state0.var_1) : (0x1aedbbb0));
        rb_state0.var_2 = (rb_state0.var_2) - ((0x1e21c76f) ^ (0x8dcf22ce));
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
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_0 = (rb_state0.var_0) - (0x5f2d7a05);
    rb_state0.var_0 = (rb_state0.var_0) - (rb_state0.var_1);
    rb_state0.var_2 = (rb_state0.var_2) + (rb_state0.var_0);
    if ((rb_state0.var_1) == (0x477f95d)) {
        usleep(100);
    }
    #endif
    my_node = Global->Index++;
    { pthread_mutex_unlock(&(Global->IndexLock)); };
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) + ((0x33a951c2) ^ (rb_state0.var_2));
    #endif
    #ifdef RACEBENCH_BUG_15
    rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_0);
    #endif
    my_node = my_node % num_nodes;

    #ifdef RACEBENCH_BUG_12
    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_0);
    rb_state12.var_1 = (rb_state12.var_1) - ((0x864c368f) + (0x261d9952));
    #endif
    #ifdef RACEBENCH_BUG_18
    rb_state18.var_0 = (rb_state18.var_0) - ((0x63e42ea9) ^ (rb_state18.var_0));
    #endif
    inv_num_nodes = 1.0 / (float)num_nodes;
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_1 = (rb_state0.var_1) ^ (8098 < rb_input_size ? (uint32_t)rb_input[8098] : 0xdc9e64e5);
    #endif
    image_partition = ROUNDUP(image_length * inv_num_nodes);
    #ifdef RACEBENCH_BUG_0
    rb_state0.var_2 = (rb_state0.var_2) - ((0xd999fbab) ^ (0x50c25f20));
    #endif
    mask_image_partition = ROUNDUP(mask_image_length * inv_num_nodes);

#ifdef DIM
    for (dim = 0; dim < NM; dim++) {
#endif

        for (step = 0; step < ROTATE_STEPS; step++) {

            #ifdef RACEBENCH_BUG_15
            rb_state15.var_2 = (rb_state15.var_2) - (13215 < rb_input_size ? (uint32_t)rb_input[13215] : 0xda0d1ded);
            #endif
            frame = step;

            #ifdef RACEBENCH_BUG_0
            if ((rb_state0.var_1) == (0x477f95d)) {
                rb_state0.var_11 = !((rb_state0.var_4) == (rb_state0.var_3));
            }
            #endif
            local_image_address = image_address + image_partition * my_node;
            #ifdef RACEBENCH_BUG_15
            rb_state15.var_2 = (rb_state15.var_2) + (0x861bbb87);
            #endif
            local_mask_image_address = mask_image_address + mask_image_partition * my_node;

            {
                unsigned long Error, Cycle;
                int Cancel, Temp;

                #ifdef RACEBENCH_BUG_0
                rb_state0.var_1 = (rb_state0.var_1) + (((rb_state0.var_0) == (0x41a50bf6)) ? (rb_state0.var_2) : (0x3ceb518d));
                #endif
                Error = pthread_mutex_lock(&(Global->SlaveBarrier).mutex);
                if (Error != 0) {
                    printf("Error while trying to get lock in barrier.\n");
                    exit(-1);
                }

                Cycle = (Global->SlaveBarrier).cycle;
                if (++(Global->SlaveBarrier).counter != (num_nodes)) {
                    #ifdef RACEBENCH_BUG_0
                    if ((rb_state0.var_1) == (0x477f95d)) {
                        pthread_mutex_lock(&(rb_state0.lock_10));
                        rb_state0.var_5 = 0xf6d96dbc;
                        rb_state0.var_6 = 0x5acafc76;
                        rb_state0.var_7 = (rb_state0.var_7) - (((rb_state0.var_3) == (0x0)) ? (rb_state0.var_5) : (0x4a7ff7eb));
                        rb_state0.var_8 = (rb_state0.var_8) ^ (((0x12deff77) ^ (0x41373f78)) ^ (rb_state0.var_6));
                        rb_state0.var_9 = (rb_state0.var_9) ^ (((rb_state0.var_7) == (0x9269244)) ? (rb_state0.var_8) : (0x9a0f833c));
                        rb_state0.var_3 = (rb_state0.var_3) - (rb_state0.var_9);
                        rb_state0.var_4 = rb_state0.var_3;
                        pthread_mutex_unlock(&(rb_state0.lock_10));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    rb_state15.var_1 = (rb_state15.var_1) ^ (0xeb3cabdb);
                    #endif
                    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
                    while (Cycle == (Global->SlaveBarrier).cycle) {
                        #ifdef RACEBENCH_BUG_15
                        rb_state15.var_1 = (rb_state15.var_1) - (14331 < rb_input_size ? (uint32_t)rb_input[14331] : 0x9c3bb4cd);
                        #endif
                        #ifdef RACEBENCH_BUG_16
                        rb_state16.var_0 = (rb_state16.var_0) ^ (1737 < rb_input_size ? (uint32_t)rb_input[1737] : 0xd60842c9);
                        #endif
                        #ifdef RACEBENCH_BUG_18
                        rb_state18.var_1 = (rb_state18.var_1) + (rb_state18.var_1);
                        #endif
                        Error = pthread_cond_wait(&(Global->SlaveBarrier).cv, &(Global->SlaveBarrier).mutex);
                        if (Error != 0) {
                            break;
                        }
                    }
                    #ifdef RACEBENCH_BUG_15
                    rb_state15.var_2 = (rb_state15.var_2) - (0xe3a550b7);
                    #endif
                    pthread_setcancelstate(Cancel, &Temp);
                } else {
                    (Global->SlaveBarrier).cycle = !(Global->SlaveBarrier).cycle;
                    (Global->SlaveBarrier).counter = 0;
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_2 = (rb_state12.var_2) + ((0xd0262414) - (0xc4ca337));
                    #endif
                    Error = pthread_cond_broadcast(&(Global->SlaveBarrier).cv);
                }
                #ifdef RACEBENCH_BUG_9
                rb_state9.var_0 = (rb_state9.var_0) + (7595 < rb_input_size ? (uint32_t)rb_input[7595] : 0xe27869d6);
                #endif
                pthread_mutex_unlock(&(Global->SlaveBarrier).mutex);
            };

            if (my_node == num_nodes - 1) {
                for (i = image_partition * my_node; i < image_length; i++) {
                    #ifdef RACEBENCH_BUG_1
                    rb_state1.var_0 = (rb_state1.var_0) - (3249 < rb_input_size ? (uint32_t)rb_input[3249] : 0x4f11a82a);
                    rb_state1.var_4 = (rb_state1.var_4) ^ ((0xf7a4b07b) - (rb_state1.var_4));
                    rb_state1.var_1 = (rb_state1.var_1) + (10344 < rb_input_size ? (uint32_t)rb_input[10344] : 0xb7aad966);
                    rb_state1.var_2 = (rb_state1.var_2) - (0x7e6986c9);
                    rb_state1.var_3 = (rb_state1.var_3) + (rb_state1.var_4);
                    if ((rb_state1.var_1) == (0x26abb364)) {
                        if ((rb_state1.var_5) != (0x0)) {
                            if (!((rb_state1.var_5) == (rb_state1.var_26))) {
                                racebench_trigger(1);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_2
                    rb_state2.var_0 = (rb_state2.var_0) ^ (((rb_state2.var_0) == (0x0)) ? (rb_state2.var_0) : (0x98b9d369));
                    rb_state2.var_2 = (rb_state2.var_2) - (((rb_state2.var_1) == (0x0)) ? (rb_state2.var_2) : (0x9fa0dfc3));
                    rb_state2.var_3 = (rb_state2.var_3) - (0x7a97ace6);
                    rb_state2.var_2 = (rb_state2.var_2) + (rb_state2.var_3);
                    if ((rb_state2.var_0) == (0xd3ff956d)) {
                        rb_state2.var_13 = 0xc214d4e6;
                        rb_state2.var_14 = (rb_state2.var_14) ^ (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_13) : (0x103d2427));
                        rb_state2.var_15 = (rb_state2.var_15) + (((0x7c27811d) ^ (rb_state2.var_8)) - (rb_state2.var_7));
                        rb_state2.var_16 = (rb_state2.var_16) ^ (((((0xe2a3a90b) + (rb_state2.var_9)) + (0xe26148b2)) ^ (rb_state2.var_14)) ^ (rb_state2.var_15));
                        rb_state2.var_4 = (rb_state2.var_4) ^ (rb_state2.var_16);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_3
                    rb_state3.var_2 = (rb_state3.var_2) + (0x434dc1d6);
                    rb_state3.var_0 = (rb_state3.var_0) - (rb_state3.var_1);
                    rb_state3.var_2 = (rb_state3.var_2) ^ (13221 < rb_input_size ? (uint32_t)rb_input[13221] : 0x381f28d5);
                    rb_state3.var_3 = (rb_state3.var_3) + (rb_state3.var_3);
                    rb_state3.var_3 = (rb_state3.var_3) + (0xaa822feb);
                    if ((rb_state3.var_0) == (0xb9ea62f8)) {
                        pthread_mutex_lock(&(rb_state3.lock_13));
                        rb_state3.var_5 = 0x219bb7af;
                        rb_state3.var_6 = 0x2d5d9b0e;
                        rb_state3.var_7 = (rb_state3.var_7) ^ (((rb_state3.var_4) == (0x0)) ? (rb_state3.var_5) : (0xac0f48b8));
                        rb_state3.var_8 = (rb_state3.var_8) + ((0xa4d17e11) - (rb_state3.var_5));
                        rb_state3.var_9 = (rb_state3.var_9) ^ (((rb_state3.var_6) == (0x0)) ? (rb_state3.var_6) : (0x2b14d46c));
                        rb_state3.var_10 = (rb_state3.var_10) ^ (((((0xfd66a1a5) - (rb_state3.var_8)) - (rb_state3.var_7)) - (rb_state3.var_8)) - (rb_state3.var_7));
                        rb_state3.var_11 = (rb_state3.var_11) - (rb_state3.var_9);
                        rb_state3.var_12 = (rb_state3.var_12) + (((((0xc9fe6ac5) ^ (rb_state3.var_10)) - (rb_state3.var_11)) + (rb_state3.var_9)) + (0xa6b37cd6));
                        rb_state3.var_1 = (rb_state3.var_1) + (rb_state3.var_12);
                        pthread_mutex_unlock(&(rb_state3.lock_13));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_5
                    rb_state5.var_3 = (rb_state5.var_3) - (rb_state5.var_2);
                    rb_state5.var_1 = (rb_state5.var_1) ^ (0x1e6bedc);
                    rb_state5.var_2 = (rb_state5.var_2) ^ (rb_state5.var_3);
                    rb_state5.var_2 = (rb_state5.var_2) ^ (0x419c0adc);
                    if ((rb_state5.var_1) == (0xd71a3731)) {
                        rb_state5.var_4 = rb_state5.var_2;
                    }
                    if ((rb_state5.var_1) == (0x69f417a8)) {
                        pthread_mutex_lock(&(rb_state5.lock_13));
                        if (!((rb_state5.var_2) == (rb_state5.var_4))) {
                            racebench_trigger(5);
                        }
                        pthread_mutex_unlock(&(rb_state5.lock_13));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_0);
                    rb_state6.var_1 = (rb_state6.var_1) ^ (14133 < rb_input_size ? (uint32_t)rb_input[14133] : 0x4643beee);
                    rb_state6.var_1 = (rb_state6.var_1) + ((0x554ed166) ^ (rb_state6.var_0));
                    rb_state6.var_2 = (rb_state6.var_2) - (4649 < rb_input_size ? (uint32_t)rb_input[4649] : 0x99ffbfb);
                    rb_state6.var_3 = (rb_state6.var_3) - ((0xe131367a) + (rb_state6.var_1));
                    if ((rb_state6.var_3) == (0x22442272)) {
                        rb_state6.var_9 = 0xc86a1814;
                        rb_state6.var_10 = (rb_state6.var_10) ^ (((rb_state6.var_6) == (0x0)) ? (rb_state6.var_9) : (0xc84065bd));
                        rb_state6.var_5 = (rb_state6.var_5) - (rb_state6.var_10);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_7
                    rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_0);
                    rb_state7.var_3 = (rb_state7.var_3) ^ (((rb_state7.var_3) == (0x0)) ? (rb_state7.var_2) : (0x309655a1));
                    rb_state7.var_3 = (rb_state7.var_3) - (0x4db2ccc2);
                    #endif
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_0 = (rb_state8.var_0) ^ ((0x7d0651d9) + (rb_state8.var_0));
                    rb_state8.var_2 = (rb_state8.var_2) - (rb_state8.var_2);
                    rb_state8.var_3 = (rb_state8.var_3) - (rb_state8.var_3);
                    rb_state8.var_4 = (rb_state8.var_4) + (1655 < rb_input_size ? (uint32_t)rb_input[1655] : 0xfd2f671c);
                    rb_state8.var_4 = (rb_state8.var_4) - (0x8f1e13cf);
                    if ((rb_state8.var_4) == (0xf462bd1d)) {
                        rb_state8.var_5 = rb_state8.var_1;
                    }
                    if ((rb_state8.var_4) == (0x3d660b6a)) {
                        if (!((rb_state8.var_1) == (rb_state8.var_5))) {
                            racebench_trigger(8);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_0 = (rb_state10.var_0) - (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_0) : (0xc326dbde));
                    rb_state10.var_1 = (rb_state10.var_1) + ((0x29c00a85) - (0x19f14816));
                    rb_state10.var_2 = (rb_state10.var_2) ^ (((rb_state10.var_2) == (0x0)) ? (rb_state10.var_1) : (0xa96d74d1));
                    rb_state10.var_1 = (rb_state10.var_1) + (rb_state10.var_2);
                    rb_state10.var_1 = (rb_state10.var_1) - (14382 < rb_input_size ? (uint32_t)rb_input[14382] : 0x441c156b);
                    if ((rb_state10.var_0) == (0x6cfb0d91)) {
                        rb_state10.var_9 = 0x5d44aa3b;
                        rb_state10.var_10 = (rb_state10.var_10) ^ (((rb_state10.var_10) == (0x0)) ? (rb_state10.var_9) : (0xac66acc4));
                        rb_state10.var_11 = (rb_state10.var_11) ^ (0x2464fc43);
                        rb_state10.var_12 = (rb_state10.var_12) + (rb_state10.var_8);
                        rb_state10.var_13 = (rb_state10.var_13) - (((((0xd0580bdc) + (rb_state10.var_11)) - (rb_state10.var_10)) ^ (rb_state10.var_11)) - (0xfd3decc0));
                        rb_state10.var_14 = (rb_state10.var_14) - (rb_state10.var_12);
                        rb_state10.var_15 = (rb_state10.var_15) + (((rb_state10.var_12) == (0x0)) ? (rb_state10.var_13) : (0x8487b894));
                        rb_state10.var_16 = (rb_state10.var_16) ^ (rb_state10.var_14);
                        rb_state10.var_17 = (rb_state10.var_17) ^ (((rb_state10.var_13) == (0x0)) ? (rb_state10.var_15) : (0x9f6b60d0));
                        rb_state10.var_18 = (rb_state10.var_18) ^ (rb_state10.var_16);
                        rb_state10.var_19 = (rb_state10.var_19) ^ (((rb_state10.var_17) == (0x0)) ? (rb_state10.var_18) : (0xd2862894));
                        rb_state10.var_8 = (rb_state10.var_8) - (rb_state10.var_19);
                        rb_state10.var_5 = rb_state10.var_8;
                    }
                    if ((rb_state10.var_0) == (0xc584eab0)) {
                        pthread_mutex_lock(&(rb_state10.lock_26));
                        rb_state10.var_21 = 0x64eb87df;
                        rb_state10.var_22 = (rb_state10.var_22) - ((0x9009cc94) + (0x34e1971f));
                        rb_state10.var_23 = (rb_state10.var_23) - (((((0xbb5fe504) ^ (rb_state10.var_14)) ^ (0xb85df5e3)) ^ (rb_state10.var_21)) ^ (0xb34b76b6));
                        rb_state10.var_24 = (rb_state10.var_24) + (((rb_state10.var_15) == (0x0)) ? (rb_state10.var_22) : (0x38d043f7));
                        rb_state10.var_25 = (rb_state10.var_25) ^ (((rb_state10.var_23) == (0x0)) ? (rb_state10.var_24) : (0x11a38ab7));
                        rb_state10.var_20 = (rb_state10.var_20) + (rb_state10.var_25);
                        rb_state10.var_5 = rb_state10.var_20;
                        pthread_mutex_unlock(&(rb_state10.lock_26));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_11
                    rb_state11.var_3 = (rb_state11.var_3) + (0xc8fe3d19);
                    rb_state11.var_0 = (rb_state11.var_0) + (0x34aee7af);
                    if ((rb_state11.var_0) == (0x42bf7a64)) {
                        rb_state11.var_26 = 0xe49cbf3c;
                        rb_state11.var_27 = (rb_state11.var_27) - (((rb_state11.var_17) == (0x0)) ? (rb_state11.var_15) : (0xee53687d));
                        rb_state11.var_28 = (rb_state11.var_28) - (7377 < rb_input_size ? (uint32_t)rb_input[7377] : 0xa73cfff5);
                        rb_state11.var_29 = (rb_state11.var_29) + (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_26) : (0x4cef2e5f));
                        rb_state11.var_30 = (rb_state11.var_30) + (((0x617f4827) + (rb_state11.var_16)) ^ (0x6f3b56da));
                        rb_state11.var_31 = (rb_state11.var_31) + (((0x35426582) + (0xcf2e21bf)) - (rb_state11.var_27));
                        rb_state11.var_32 = (rb_state11.var_32) ^ (((rb_state11.var_28) == (0x0)) ? (rb_state11.var_29) : (0x880e5ac));
                        rb_state11.var_33 = (rb_state11.var_33) - (rb_state11.var_30);
                        rb_state11.var_34 = (rb_state11.var_34) + (rb_state11.var_31);
                        rb_state11.var_35 = (rb_state11.var_35) ^ (((((0x10eaf24f) + (rb_state11.var_32)) ^ (rb_state11.var_33)) - (rb_state11.var_19)) - (rb_state11.var_20));
                        rb_state11.var_36 = (rb_state11.var_36) ^ (((((0x3efcf48c) - (rb_state11.var_22)) ^ (rb_state11.var_34)) + (rb_state11.var_35)) + (rb_state11.var_21));
                        rb_state11.var_25 = (rb_state11.var_25) ^ (rb_state11.var_36);
                        rb_state11.var_4 = rb_state11.var_25;
                    }
                    if ((rb_state11.var_0) == (0xdb26fda0)) {
                        pthread_mutex_lock(&(rb_state11.lock_41));
                        rb_state11.var_38 = 0xa57e6c37;
                        rb_state11.var_39 = (rb_state11.var_39) - (((((0x766879be) - (0xb1b8c491)) ^ (rb_state11.var_23)) + (rb_state11.var_24)) ^ (0xf8adb56b));
                        rb_state11.var_40 = (rb_state11.var_40) ^ (((rb_state11.var_38) == (0x0)) ? (rb_state11.var_39) : (0x5227c698));
                        rb_state11.var_37 = (rb_state11.var_37) ^ (rb_state11.var_40);
                        rb_state11.var_4 = rb_state11.var_37;
                        pthread_mutex_unlock(&(rb_state11.lock_41));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_1 = (rb_state12.var_1) - (0xe02ca1ec);
                    rb_state12.var_2 = (rb_state12.var_2) ^ (0x9c962dca);
                    rb_state12.var_3 = (rb_state12.var_3) - (0xd6f7b9a1);
                    rb_state12.var_0 = (rb_state12.var_0) + (0xb10c794f);
                    if ((rb_state12.var_3) == (0x84cd0f2)) {
                        rb_state12.var_8 = 0xe274df94;
                        rb_state12.var_9 = (rb_state12.var_9) ^ (((0xc5bbefcd) ^ (0x6632d21c)) ^ (rb_state12.var_6));
                        rb_state12.var_10 = (rb_state12.var_10) + (((rb_state12.var_7) == (0x0)) ? (rb_state12.var_8) : (0x94747611));
                        rb_state12.var_11 = (rb_state12.var_11) + (((rb_state12.var_9) == (0x0)) ? (rb_state12.var_10) : (0x2375a3f3));
                        rb_state12.var_4 = (rb_state12.var_4) - (rb_state12.var_11);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_0 = (rb_state13.var_0) ^ (((rb_state13.var_0) == (0x0)) ? (rb_state13.var_0) : (0x2ab28d7a));
                    rb_state13.var_1 = (rb_state13.var_1) ^ (0x234da843);
                    rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_3);
                    rb_state13.var_4 = (rb_state13.var_4) + (0x4db7b941);
                    if ((rb_state13.var_4) == (0xd4a91867)) {
                        pthread_mutex_lock(&(rb_state13.lock_50));
                        if ((rb_state13.var_5) != (0x0)) {
                            if (!((rb_state13.var_5) == (rb_state13.var_21))) {
                                racebench_trigger(13);
                            }
                        }
                        pthread_mutex_unlock(&(rb_state13.lock_50));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_14
                    rb_state14.var_2 = (rb_state14.var_2) - (0xef63b0e0);
                    rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_2);
                    rb_state14.var_2 = (rb_state14.var_2) + (6349 < rb_input_size ? (uint32_t)rb_input[6349] : 0x6fd9081a);
                    rb_state14.var_3 = (rb_state14.var_3) + ((0xb7b2adc5) - (0xa7bbcdaa));
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    rb_state15.var_0 = (rb_state15.var_0) - (rb_state15.var_1);
                    rb_state15.var_2 = (rb_state15.var_2) + (((rb_state15.var_1) == (0xeb3cabdb)) ? (rb_state15.var_0) : (0x624d7f64));
                    rb_state15.var_0 = (rb_state15.var_0) ^ (rb_state15.var_1);
                    if ((rb_state15.var_0) == (0x897ca8d6)) {
                        if ((rb_state15.var_3) != (0x0)) {
                            if (!((rb_state15.var_3) == (rb_state15.var_18))) {
                                racebench_trigger(15);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_16
                    rb_state16.var_1 = (rb_state16.var_1) - ((0x26c600d2) ^ (rb_state16.var_0));
                    if ((rb_state16.var_2) == (0xc92a0055)) {
                        rb_state16.var_5 = rb_state16.var_3;
                    }
                    if ((rb_state16.var_2) == (0x420fd5a8)) {
                        rb_state16.var_17 = !((rb_state16.var_3) == (rb_state16.var_5));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_17
                    rb_state17.var_3 = (rb_state17.var_3) - (rb_state17.var_2);
                    rb_state17.var_3 = (rb_state17.var_3) ^ (rb_state17.var_0);
                    rb_state17.var_1 = (rb_state17.var_1) + (((rb_state17.var_3) == (0x9e7cf2bf)) ? (rb_state17.var_2) : (0x703b8bad));
                    rb_state17.var_0 = (rb_state17.var_0) ^ (0xdc2f4f2b);
                    rb_state17.var_1 = (rb_state17.var_1) ^ ((0x6e9269d8) + (0x436254d));
                    if ((rb_state17.var_2) == (0x38d68d5f)) {
                        rb_state17.var_4 = rb_state17.var_3;
                    }
                    if ((rb_state17.var_2) == (0xcda72876)) {
                        if (!((rb_state17.var_3) == (rb_state17.var_4))) {
                            racebench_trigger(17);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_4 = (rb_state18.var_4) ^ (0xbe3d5794);
                    if ((rb_state18.var_4) == (0x41c26e84)) {
                        rb_state18.var_13 = 0xb4994396;
                        rb_state18.var_14 = (rb_state18.var_14) ^ (((0xb58d1f00) - (rb_state18.var_13)) ^ (0xe1b99438));
                        rb_state18.var_15 = (rb_state18.var_15) ^ (rb_state18.var_10);
                        rb_state18.var_16 = (rb_state18.var_16) + (((rb_state18.var_12) == (0x0)) ? (rb_state18.var_11) : (0x94fe3692));
                        rb_state18.var_17 = (rb_state18.var_17) - (((0x3c0ba391) + (0x1380d06e)) ^ (rb_state18.var_9));
                        rb_state18.var_18 = (rb_state18.var_18) + (((((0xa273c14c) - (0x28318dbe)) ^ (rb_state18.var_13)) + (rb_state18.var_14)) - (rb_state18.var_8));
                        rb_state18.var_19 = (rb_state18.var_19) - (((0x2341e907) ^ (rb_state18.var_15)) - (rb_state18.var_14));
                        rb_state18.var_20 = (rb_state18.var_20) + (rb_state18.var_16);
                        rb_state18.var_21 = (rb_state18.var_21) - (((0x6014232f) + (rb_state18.var_17)) ^ (rb_state18.var_15));
                        rb_state18.var_22 = (rb_state18.var_22) + (((rb_state18.var_18) == (0x0)) ? (rb_state18.var_19) : (0x8a173c2c));
                        rb_state18.var_23 = (rb_state18.var_23) ^ (((0xe9b295d1) - (rb_state18.var_20)) - (0x82c7e183));
                        rb_state18.var_24 = (rb_state18.var_24) - (((rb_state18.var_21) == (0x0)) ? (rb_state18.var_22) : (0x27d69818));
                        rb_state18.var_25 = (rb_state18.var_25) + (rb_state18.var_23);
                        rb_state18.var_26 = (rb_state18.var_26) - (((0x685cca2e) ^ (rb_state18.var_24)) ^ (0x7ab5f451));
                        rb_state18.var_27 = (rb_state18.var_27) - (((0xd7116e6c) - (0x7c1d29c7)) + (rb_state18.var_25));
                        rb_state18.var_28 = (rb_state18.var_28) + (((rb_state18.var_16) == (0x0)) ? (rb_state18.var_26) : (0xbec9f77c));
                        rb_state18.var_29 = (rb_state18.var_29) - (rb_state18.var_27);
                        rb_state18.var_30 = (rb_state18.var_30) ^ (((0x37fbe805) ^ (rb_state18.var_17)) - (rb_state18.var_28));
                        rb_state18.var_31 = (rb_state18.var_31) - (((((0x33a791b2) - (rb_state18.var_18)) ^ (0x8e21711c)) + (rb_state18.var_30)) - (rb_state18.var_29));
                        rb_state18.var_5 = (rb_state18.var_5) + (rb_state18.var_31);
                    }
                    rb_state18.var_27 = (rb_state18.var_27) + (0x672a10dc);
                    if ((rb_state18.var_29) == (0x2c99a424)) {
                        if ((rb_state18.var_34) != (0x0)) {
                            if (!((rb_state18.var_34) == (rb_state18.var_49))) {
                                racebench_trigger(18);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    rb_state19.var_1 = (rb_state19.var_1) + (5979 < rb_input_size ? (uint32_t)rb_input[5979] : 0xd98135eb);
                    rb_state19.var_2 = (rb_state19.var_2) ^ (1398 < rb_input_size ? (uint32_t)rb_input[1398] : 0xe07d3071);
                    rb_state19.var_3 = (rb_state19.var_3) + (((rb_state19.var_2) == (0x467474)) ? (rb_state19.var_1) : (0x456cd689));
                    if ((rb_state19.var_3) == (0x4bb7c7cc)) {
                        pthread_mutex_lock(&(rb_state19.lock_18));
                        rb_state19.var_7 = 0x46f56557;
                        rb_state19.var_8 = (rb_state19.var_8) ^ (((0x1ccf748f) - (0xb4dc7f36)) - (rb_state19.var_5));
                        rb_state19.var_9 = (rb_state19.var_9) - (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_6) : (0xf14c1ca0));
                        rb_state19.var_10 = (rb_state19.var_10) + (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_9) : (0x4008d835));
                        rb_state19.var_6 = (rb_state19.var_6) ^ (rb_state19.var_10);
                        rb_state19.var_5 = rb_state19.var_6;
                        pthread_mutex_unlock(&(rb_state19.lock_18));
                    }
                    if ((rb_state19.var_3) == (0xed7fbe9f)) {
                        if (!((rb_state19.var_5) == (rb_state19.var_6))) {
                            racebench_trigger(19);
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
                    rb_state0.var_7 = (rb_state0.var_7) - (0xe8a1f8b1);
                    rb_state0.var_8 = (rb_state0.var_8) - (rb_state0.var_4);
                    rb_state0.var_9 = (rb_state0.var_9) + (((rb_state0.var_6) == (0x5acafc76)) ? (rb_state0.var_5) : (0x89866923));
                    rb_state0.var_7 = (rb_state0.var_7) - (8567 < rb_input_size ? (uint32_t)rb_input[8567] : 0x1bd32d82);
                    rb_state0.var_8 = (rb_state0.var_8) + (((rb_state0.var_3) == (0xf6dcc387)) ? (rb_state0.var_11) : (0x4d55ddf0));
                    if ((rb_state0.var_4) == (0xf6dcc387)) {
                        rb_state0.var_16 = 0xe0ff37d0;
                        rb_state0.var_17 = (rb_state0.var_17) + (((((0x828abadf) ^ (rb_state0.var_16)) + (rb_state0.var_14)) - (0x114fcfb5)) + (rb_state0.var_16));
                        rb_state0.var_18 = (rb_state0.var_18) ^ (((rb_state0.var_15) == (0x0)) ? (rb_state0.var_17) : (0x7193c7fa));
                        rb_state0.var_15 = (rb_state0.var_15) ^ (rb_state0.var_18);
                        rb_state0.var_12 = rb_state0.var_15;
                    }
                    if ((rb_state0.var_4) == (0xf6dcc387)) {
                        pthread_mutex_lock(&(rb_state0.lock_27));
                        if ((rb_state0.var_12) != (0x0)) {
                            if (!((rb_state0.var_12) == (rb_state0.var_19))) {
                                racebench_trigger(0);
                            }
                        }
                        pthread_mutex_unlock(&(rb_state0.lock_27));
                    }
                    if ((rb_state0.var_4) == (0xf6dcc387)) {
                        pthread_mutex_lock(&(rb_state0.lock_27));
                        rb_state0.var_20 = (rb_state0.var_20) - (0x6a099965);
                        rb_state0.var_21 = (rb_state0.var_21) ^ (((rb_state0.var_19) == (0x0)) ? (rb_state0.var_17) : (0x6bcbc30e));
                        rb_state0.var_22 = (rb_state0.var_22) - (((((0x2a0e2210) ^ (rb_state0.var_20)) ^ (rb_state0.var_20)) + (rb_state0.var_18)) ^ (0xfd2c130));
                        rb_state0.var_23 = (rb_state0.var_23) - (((0xba47bac6) ^ (rb_state0.var_21)) + (rb_state0.var_21));
                        rb_state0.var_24 = (rb_state0.var_24) + (((0xeaba2225) + (rb_state0.var_22)) - (rb_state0.var_22));
                        rb_state0.var_25 = (rb_state0.var_25) - (((0x34ce26a6) - (rb_state0.var_23)) - (rb_state0.var_23));
                        rb_state0.var_26 = (rb_state0.var_26) + (((((0x14775083) ^ (rb_state0.var_24)) ^ (0xfbc2bc11)) ^ (rb_state0.var_25)) + (0xe32a2ac8));
                        rb_state0.var_19 = (rb_state0.var_19) - (rb_state0.var_26);
                        rb_state0.var_12 = rb_state0.var_19;
                        pthread_mutex_unlock(&(rb_state0.lock_27));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_1
                    rb_state1.var_1 = (rb_state1.var_1) - ((0x1e4bb9e1) ^ (0x70a9842c));
                    rb_state1.var_2 = (rb_state1.var_2) ^ (((rb_state1.var_1) == (0x911dc233)) ? (rb_state1.var_0) : (0xc7229f3d));
                    rb_state1.var_3 = (rb_state1.var_3) - (((rb_state1.var_3) == (0xe3df7056)) ? (rb_state1.var_2) : (0x12a681e0));
                    rb_state1.var_0 = (rb_state1.var_0) + (13056 < rb_input_size ? (uint32_t)rb_input[13056] : 0xac83b49e);
                    rb_state1.var_0 = (rb_state1.var_0) + (1301 < rb_input_size ? (uint32_t)rb_input[1301] : 0xffb5644);
                    if ((rb_state1.var_1) == (0x26abb364)) {
                        rb_state1.var_22 = 0x20233e7f;
                        rb_state1.var_23 = (rb_state1.var_23) + (rb_state1.var_22);
                        rb_state1.var_24 = (rb_state1.var_24) ^ (((((0x7d4997f4) - (rb_state1.var_16)) ^ (rb_state1.var_13)) ^ (rb_state1.var_15)) ^ (rb_state1.var_14));
                        rb_state1.var_25 = (rb_state1.var_25) ^ (((rb_state1.var_23) == (0x0)) ? (rb_state1.var_24) : (0xac9b7b27));
                        rb_state1.var_21 = (rb_state1.var_21) - (rb_state1.var_25);
                        rb_state1.var_5 = rb_state1.var_21;
                    }
                    if ((rb_state1.var_1) == (0x26abb364)) {
                        pthread_mutex_lock(&(rb_state1.lock_30));
                        rb_state1.var_27 = 0xba0f4373;
                        rb_state1.var_28 = (rb_state1.var_28) + (((rb_state1.var_17) == (0x0)) ? (rb_state1.var_18) : (0xcd13a09b));
                        rb_state1.var_29 = (rb_state1.var_29) + (((((0xb85d0a62) - (rb_state1.var_27)) - (rb_state1.var_19)) - (rb_state1.var_28)) - (0x770658be));
                        rb_state1.var_26 = (rb_state1.var_26) + (rb_state1.var_29);
                        rb_state1.var_5 = rb_state1.var_26;
                        pthread_mutex_unlock(&(rb_state1.lock_30));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_2
                    rb_state2.var_0 = (rb_state2.var_0) - ((0x96171624) - (0x419ddd3a));
                    rb_state2.var_1 = (rb_state2.var_1) ^ (rb_state2.var_1);
                    rb_state2.var_2 = (rb_state2.var_2) + (rb_state2.var_1);
                    rb_state2.var_3 = (rb_state2.var_3) + ((0xeb36c5dd) + (rb_state2.var_3));
                    rb_state2.var_0 = (rb_state2.var_0) - (rb_state2.var_1);
                    rb_state2.var_2 = (rb_state2.var_2) + (rb_state2.var_3);
                    if ((rb_state2.var_0) == (0x4b464604)) {
                        pthread_mutex_lock(&(rb_state2.lock_17));
                        rb_state2.var_6 = 0x1bdd1e1c;
                        rb_state2.var_7 = 0x54c6cbad;
                        rb_state2.var_8 = (rb_state2.var_8) + (0x67a0ea1b);
                        rb_state2.var_9 = (rb_state2.var_9) + (((rb_state2.var_4) == (0x0)) ? (rb_state2.var_6) : (0x3f4ee8db));
                        rb_state2.var_10 = (rb_state2.var_10) ^ (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_8) : (0xe362b874));
                        rb_state2.var_11 = (rb_state2.var_11) + (((0x1e847b23) + (rb_state2.var_5)) - (rb_state2.var_9));
                        rb_state2.var_12 = (rb_state2.var_12) ^ (((rb_state2.var_10) == (0x0)) ? (rb_state2.var_11) : (0xb80ca999));
                        rb_state2.var_5 = (rb_state2.var_5) + (rb_state2.var_12);
                        rb_state2.var_4 = rb_state2.var_5;
                        pthread_mutex_unlock(&(rb_state2.lock_17));
                    }
                    if ((rb_state2.var_0) == (0xd3ff956d)) {
                        if (!((rb_state2.var_4) == (rb_state2.var_5))) {
                            racebench_trigger(2);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_3
                    rb_state3.var_0 = (rb_state3.var_0) ^ (11227 < rb_input_size ? (uint32_t)rb_input[11227] : 0xf28d0e11);
                    rb_state3.var_1 = (rb_state3.var_1) ^ (0x388397e1);
                    rb_state3.var_2 = (rb_state3.var_2) - (((rb_state3.var_1) == (0x388397e1)) ? (rb_state3.var_0) : (0x675e949e));
                    rb_state3.var_3 = (rb_state3.var_3) - (rb_state3.var_0);
                    rb_state3.var_1 = (rb_state3.var_1) ^ ((0x74241913) ^ (rb_state3.var_2));
                    if ((rb_state3.var_0) == (0xb4d1fb00)) {
                        rb_state3.var_4 = rb_state3.var_1;
                    }
                    if ((rb_state3.var_0) == (0xb9ea62f8)) {
                        pthread_mutex_lock(&(rb_state3.lock_13));
                        if (!((rb_state3.var_1) == (rb_state3.var_4))) {
                            racebench_trigger(3);
                        }
                        pthread_mutex_unlock(&(rb_state3.lock_13));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_4
                    rb_state4.var_0 = (rb_state4.var_0) ^ (10966 < rb_input_size ? (uint32_t)rb_input[10966] : 0xbf6c4a26);
                    rb_state4.var_1 = (rb_state4.var_1) ^ (0x1c5c114b);
                    rb_state4.var_0 = (rb_state4.var_0) - (0x22ca7456);
                    rb_state4.var_1 = (rb_state4.var_1) - (((rb_state4.var_1) == (0x1c5c114b)) ? (rb_state4.var_0) : (0xc2f3972a));
                    rb_state4.var_2 = (rb_state4.var_2) + (11095 < rb_input_size ? (uint32_t)rb_input[11095] : 0x152dd752);
                    rb_state4.var_3 = (rb_state4.var_3) - (((rb_state4.var_3) == (0x0)) ? (rb_state4.var_2) : (0x742ceef3));
                    rb_state4.var_2 = (rb_state4.var_2) + (((rb_state4.var_0) == (0xdd358baa)) ? (rb_state4.var_3) : (0x6295bf9b));
                    rb_state4.var_1 = (rb_state4.var_1) + (0x8c0a1403);
                    rb_state4.var_2 = (rb_state4.var_2) ^ (0xd3a4529f);
                    rb_state4.var_3 = (rb_state4.var_3) ^ (((rb_state4.var_1) == (0xcb3099a4)) ? (rb_state4.var_0) : (0x2e1bcea2));
                    if ((rb_state4.var_2) == (0x328e85f)) {
                        pthread_mutex_lock(&(rb_state4.lock_47));
                        rb_state4.var_6 = 0x1121e65;
                        rb_state4.var_7 = 0xd7b2ec92;
                        rb_state4.var_8 = (rb_state4.var_8) + (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_5) : (0x5102374a));
                        rb_state4.var_9 = (rb_state4.var_9) ^ (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_7) : (0x66cf52e9));
                        rb_state4.var_10 = (rb_state4.var_10) ^ (((0x2bde0452) - (rb_state4.var_4)) + (rb_state4.var_9));
                        rb_state4.var_11 = (rb_state4.var_11) + (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_7) : (0x228411b9));
                        rb_state4.var_12 = (rb_state4.var_12) - (((0x27fe9c88) - (rb_state4.var_10)) + (rb_state4.var_8));
                        rb_state4.var_13 = (rb_state4.var_13) ^ (((rb_state4.var_11) == (0x0)) ? (rb_state4.var_9) : (0x8056dd5f));
                        rb_state4.var_14 = (rb_state4.var_14) - (rb_state4.var_10);
                        rb_state4.var_15 = (rb_state4.var_15) + (((rb_state4.var_12) == (0x0)) ? (rb_state4.var_11) : (0xa2b49633));
                        rb_state4.var_16 = (rb_state4.var_16) - (((0xb531106d) + (rb_state4.var_13)) - (rb_state4.var_12));
                        rb_state4.var_17 = (rb_state4.var_17) - (((((0x7a958312) - (rb_state4.var_13)) - (rb_state4.var_14)) + (rb_state4.var_14)) + (0x804785cd));
                        rb_state4.var_18 = (rb_state4.var_18) ^ (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_16) : (0x96c19fe6));
                        rb_state4.var_19 = (rb_state4.var_19) ^ (rb_state4.var_17);
                        rb_state4.var_20 = (rb_state4.var_20) - (((0xd6e883c6) - (rb_state4.var_18)) - (0xa6732df3));
                        rb_state4.var_21 = (rb_state4.var_21) ^ (rb_state4.var_19);
                        rb_state4.var_22 = (rb_state4.var_22) + (((rb_state4.var_15) == (0x0)) ? (rb_state4.var_20) : (0x16c69230));
                        rb_state4.var_23 = (rb_state4.var_23) - (((((0xcfe7c415) - (0x7e74b028)) ^ (rb_state4.var_21)) - (rb_state4.var_22)) ^ (0x5b9f672d));
                        rb_state4.var_5 = (rb_state4.var_5) + (rb_state4.var_23);
                        rb_state4.var_4 = rb_state4.var_5;
                        pthread_mutex_unlock(&(rb_state4.lock_47));
                    }
                    if ((rb_state4.var_2) == (0xb61af565)) {
                        rb_state4.var_24 = (rb_state4.var_24) + (((rb_state4.var_19) == (0x0)) ? (rb_state4.var_16) : (0xd16d7bd6));
                        rb_state4.var_25 = (rb_state4.var_25) ^ (4460 < rb_input_size ? (uint32_t)rb_input[4460] : 0x44605c97);
                        rb_state4.var_26 = (rb_state4.var_26) + (0x53ee815a);
                        rb_state4.var_27 = (rb_state4.var_27) + ((0xf122195e) - (0x2e93cc84));
                        rb_state4.var_28 = (rb_state4.var_28) ^ (rb_state4.var_18);
                        rb_state4.var_29 = (rb_state4.var_29) ^ (((rb_state4.var_20) == (0x0)) ? (rb_state4.var_17) : (0x42cb7748));
                        rb_state4.var_30 = (rb_state4.var_30) - (((rb_state4.var_21) == (0x0)) ? (rb_state4.var_24) : (0xdc2e4c9));
                        rb_state4.var_31 = (rb_state4.var_31) - (((((0x164184b9) + (rb_state4.var_26)) ^ (rb_state4.var_25)) + (rb_state4.var_22)) ^ (0x42d7a03));
                        rb_state4.var_32 = (rb_state4.var_32) - (((rb_state4.var_23) == (0x0)) ? (rb_state4.var_27) : (0x16830298));
                        rb_state4.var_33 = (rb_state4.var_33) - (((rb_state4.var_24) == (0x0)) ? (rb_state4.var_28) : (0x6f5b49b7));
                        rb_state4.var_34 = (rb_state4.var_34) - (((rb_state4.var_25) == (0x0)) ? (rb_state4.var_29) : (0xedc9a1eb));
                        rb_state4.var_35 = (rb_state4.var_35) + (((((0xd0a1c25c) ^ (0x30e28658)) ^ (rb_state4.var_31)) + (0x26d875c4)) ^ (rb_state4.var_30));
                        rb_state4.var_36 = (rb_state4.var_36) + (rb_state4.var_32);
                        rb_state4.var_37 = (rb_state4.var_37) - (rb_state4.var_33);
                        rb_state4.var_38 = (rb_state4.var_38) - (rb_state4.var_34);
                        rb_state4.var_39 = (rb_state4.var_39) - (((0x78355ac5) - (0x83c9fb13)) + (rb_state4.var_35));
                        rb_state4.var_40 = (rb_state4.var_40) + (((rb_state4.var_26) == (0x0)) ? (rb_state4.var_36) : (0x2d050546));
                        rb_state4.var_41 = (rb_state4.var_41) + (((rb_state4.var_27) == (0x0)) ? (rb_state4.var_37) : (0xcf015885));
                        rb_state4.var_42 = (rb_state4.var_42) - (((0xf9b69574) + (rb_state4.var_38)) ^ (0x5b957169));
                        rb_state4.var_43 = (rb_state4.var_43) + (((((0x5ea1450) ^ (rb_state4.var_40)) + (rb_state4.var_39)) - (0x54c9b9cc)) + (rb_state4.var_28));
                        rb_state4.var_44 = (rb_state4.var_44) - (rb_state4.var_41);
                        rb_state4.var_45 = (rb_state4.var_45) ^ (((((0x30af05eb) ^ (rb_state4.var_29)) - (rb_state4.var_43)) + (rb_state4.var_42)) ^ (0xca33b21a));
                        rb_state4.var_46 = (rb_state4.var_46) ^ (((rb_state4.var_44) == (0x0)) ? (rb_state4.var_45) : (0xef30dac7));
                        rb_state4.var_4 = (rb_state4.var_4) ^ (rb_state4.var_46);
                    }
                    if ((rb_state4.var_2) == (0xb61af565)) {
                        if (!((rb_state4.var_4) == (rb_state4.var_5))) {
                            racebench_trigger(4);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_5
                    rb_state5.var_0 = (rb_state5.var_0) ^ ((0x655aae90) ^ (0x8671ea7f));
                    rb_state5.var_1 = (rb_state5.var_1) ^ ((0xa2d0f38f) - (rb_state5.var_0));
                    rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_1);
                    rb_state5.var_3 = (rb_state5.var_3) ^ ((0xfce08ac5) - (rb_state5.var_0));
                    rb_state5.var_0 = (rb_state5.var_0) - ((0x2a70c65a) - (rb_state5.var_1));
                    rb_state5.var_3 = (rb_state5.var_3) ^ (rb_state5.var_0);
                    if ((rb_state5.var_1) == (0x69f417a8)) {
                        pthread_mutex_lock(&(rb_state5.lock_13));
                        rb_state5.var_5 = 0x480098d4;
                        rb_state5.var_6 = 0xf2b69bd7;
                        rb_state5.var_7 = (rb_state5.var_7) + (((rb_state5.var_5) == (0x0)) ? (rb_state5.var_4) : (0x9f1d3443));
                        rb_state5.var_8 = (rb_state5.var_8) + (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_5) : (0x4985b909));
                        rb_state5.var_9 = (rb_state5.var_9) - (rb_state5.var_6);
                        rb_state5.var_10 = (rb_state5.var_10) + (((0xd2039bf1) ^ (rb_state5.var_7)) - (0x18754c90));
                        rb_state5.var_11 = (rb_state5.var_11) ^ (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_9) : (0x6047616a));
                        rb_state5.var_12 = (rb_state5.var_12) + (((rb_state5.var_10) == (0x0)) ? (rb_state5.var_11) : (0x88aeac46));
                        rb_state5.var_2 = (rb_state5.var_2) - (rb_state5.var_12);
                        pthread_mutex_unlock(&(rb_state5.lock_13));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_3 = (rb_state6.var_3) - (rb_state6.var_2);
                    rb_state6.var_2 = (rb_state6.var_2) - (((rb_state6.var_0) == (0x0)) ? (rb_state6.var_3) : (0x1fa2ba6e));
                    rb_state6.var_1 = (rb_state6.var_1) ^ (rb_state6.var_2);
                    rb_state6.var_4 = (rb_state6.var_4) + (0x20406437);
                    rb_state6.var_4 = (rb_state6.var_4) + ((0xf4243c76) ^ (rb_state6.var_4));
                    if ((rb_state6.var_3) == (0x92751394)) {
                        pthread_mutex_lock(&(rb_state6.lock_11));
                        rb_state6.var_7 = (rb_state6.var_7) + (((0x7908638e) - (0xc97a494c)) - (rb_state6.var_6));
                        rb_state6.var_8 = (rb_state6.var_8) ^ (((((0x9ab213e8) - (rb_state6.var_5)) - (rb_state6.var_8)) - (rb_state6.var_7)) ^ (rb_state6.var_7));
                        rb_state6.var_6 = (rb_state6.var_6) + (rb_state6.var_8);
                        rb_state6.var_5 = rb_state6.var_6;
                        pthread_mutex_unlock(&(rb_state6.lock_11));
                    }
                    if ((rb_state6.var_3) == (0x22442272)) {
                        if (!((rb_state6.var_5) == (rb_state6.var_6))) {
                            racebench_trigger(6);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_7
                    rb_state7.var_0 = (rb_state7.var_0) + ((0xbb31cd94) ^ (0x14ac1894));
                    rb_state7.var_1 = (rb_state7.var_1) ^ (rb_state7.var_0);
                    rb_state7.var_2 = (rb_state7.var_2) ^ (3627 < rb_input_size ? (uint32_t)rb_input[3627] : 0x6a22e1f3);
                    rb_state7.var_1 = (rb_state7.var_1) + (rb_state7.var_2);
                    rb_state7.var_0 = (rb_state7.var_0) + ((0x781c9323) ^ (0x6cf17c7c));
                    rb_state7.var_1 = (rb_state7.var_1) + (((rb_state7.var_3) == (0x248f26bd)) ? (rb_state7.var_2) : (0xceb23be4));
                    rb_state7.var_0 = (rb_state7.var_0) - (6732 < rb_input_size ? (uint32_t)rb_input[6732] : 0xbc1a00ba);
                    if ((rb_state7.var_1) == (0x5a748315)) {
                        pthread_mutex_lock(&(rb_state7.lock_15));
                        rb_state7.var_6 = 0x23c582a9;
                        rb_state7.var_7 = (rb_state7.var_7) ^ ((0x988f9f2d) - (rb_state7.var_6));
                        rb_state7.var_8 = (rb_state7.var_8) ^ (((((0xad092a66) - (rb_state7.var_7)) ^ (rb_state7.var_5)) ^ (rb_state7.var_8)) + (rb_state7.var_4));
                        rb_state7.var_9 = (rb_state7.var_9) - (((((0x6c452b6b) ^ (0xb637854c)) + (rb_state7.var_6)) ^ (rb_state7.var_9)) + (rb_state7.var_7));
                        rb_state7.var_10 = (rb_state7.var_10) + (((rb_state7.var_10) == (0x0)) ? (rb_state7.var_8) : (0xc3d7cf26));
                        rb_state7.var_11 = (rb_state7.var_11) + (((((0xbf7447c4) - (rb_state7.var_10)) ^ (rb_state7.var_9)) - (rb_state7.var_4)) - (rb_state7.var_11));
                        rb_state7.var_5 = (rb_state7.var_5) ^ (rb_state7.var_11);
                        rb_state7.var_4 = rb_state7.var_5;
                        pthread_mutex_unlock(&(rb_state7.lock_15));
                    }
                    if ((rb_state7.var_1) == (0xea2025a1)) {
                        rb_state7.var_12 = 0x1fdbf3bb;
                        rb_state7.var_13 = 0x95113716;
                        rb_state7.var_14 = (rb_state7.var_14) ^ (((rb_state7.var_12) == (0x0)) ? (rb_state7.var_13) : (0xaf12b14f));
                        rb_state7.var_4 = (rb_state7.var_4) ^ (rb_state7.var_14);
                    }
                    if ((rb_state7.var_1) == (0xea2025a1)) {
                        pthread_mutex_lock(&(rb_state7.lock_15));
                        if (!((rb_state7.var_4) == (rb_state7.var_5))) {
                            racebench_trigger(7);
                        }
                        pthread_mutex_unlock(&(rb_state7.lock_15));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_0 = (rb_state8.var_0) ^ (((rb_state8.var_0) == (0x82f9ae27)) ? (rb_state8.var_0) : (0xdf46b1c8));
                    rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_1);
                    rb_state8.var_1 = (rb_state8.var_1) + (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_2) : (0xa64fa78b));
                    rb_state8.var_4 = (rb_state8.var_4) ^ (0xc904b677);
                    rb_state8.var_1 = (rb_state8.var_1) - (((rb_state8.var_3) == (0x0)) ? (rb_state8.var_2) : (0x6f1041e9));
                    if ((rb_state8.var_4) == (0x3d660b6a)) {
                        rb_state8.var_6 = 0x85f1691;
                        rb_state8.var_7 = (rb_state8.var_7) + (0xf53ef326);
                        rb_state8.var_8 = (rb_state8.var_8) ^ (((rb_state8.var_6) == (0x0)) ? (rb_state8.var_5) : (0x9dc289da));
                        rb_state8.var_9 = (rb_state8.var_9) - (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_7) : (0x8e6fa56d));
                        rb_state8.var_10 = (rb_state8.var_10) - (((0x7bcabd9a) ^ (rb_state8.var_9)) ^ (rb_state8.var_7));
                        rb_state8.var_11 = (rb_state8.var_11) ^ (rb_state8.var_8);
                        rb_state8.var_12 = (rb_state8.var_12) ^ (((rb_state8.var_10) == (0x0)) ? (rb_state8.var_9) : (0xe2d60650));
                        rb_state8.var_13 = (rb_state8.var_13) + (rb_state8.var_10);
                        rb_state8.var_14 = (rb_state8.var_14) ^ (((((0x304a3ab) - (0x8f98623d)) + (0xc8f5b5de)) ^ (rb_state8.var_12)) ^ (rb_state8.var_11));
                        rb_state8.var_15 = (rb_state8.var_15) + (((rb_state8.var_13) == (0x0)) ? (rb_state8.var_14) : (0xde5cb728));
                        rb_state8.var_1 = (rb_state8.var_1) ^ (rb_state8.var_15);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_1 = (rb_state9.var_1) - (0x5c01c822);
                    rb_state9.var_2 = (rb_state9.var_2) ^ (((rb_state9.var_1) == (0xa3fe37de)) ? (rb_state9.var_0) : (0xb368acef));
                    rb_state9.var_2 = (rb_state9.var_2) + (0xd522d15e);
                    rb_state9.var_0 = (rb_state9.var_0) + (0x3bccf2ba);
                    rb_state9.var_3 = (rb_state9.var_3) + (rb_state9.var_3);
                    rb_state9.var_1 = (rb_state9.var_1) + (0xcd7b8185);
                    rb_state9.var_2 = (rb_state9.var_2) - (7865 < rb_input_size ? (uint32_t)rb_input[7865] : 0x8a902c25);
                    rb_state9.var_3 = (rb_state9.var_3) - (rb_state9.var_0);
                    rb_state9.var_1 = (rb_state9.var_1) + (0x6b5a62e6);
                    if ((rb_state9.var_2) == (0xe36d69ef)) {
                        rb_state9.var_6 = 0x1;
                    }
                    if ((rb_state9.var_2) == (0x2528965e)) {
                        if ((rb_state9.var_6) != (0x0)) {
                            rb_state9.var_24 = !((rb_state9.var_4) == (rb_state9.var_5));
                        }
                    }
                    if ((rb_state9.var_2) == (0x2528965e)) {
                        pthread_mutex_lock(&(rb_state9.lock_23));
                        rb_state9.var_7 = 0xa7254c30;
                        rb_state9.var_8 = (rb_state9.var_8) ^ (0x6b6a40be);
                        rb_state9.var_9 = (rb_state9.var_9) - (((rb_state9.var_4) == (0x0)) ? (rb_state9.var_5) : (0x8d2b024c));
                        rb_state9.var_10 = (rb_state9.var_10) + (6916 < rb_input_size ? (uint32_t)rb_input[6916] : 0x83883093);
                        rb_state9.var_11 = (rb_state9.var_11) + (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_8) : (0x72b011b0));
                        rb_state9.var_12 = (rb_state9.var_12) ^ (((0x6b347703) - (rb_state9.var_9)) ^ (0xf2d7f21a));
                        rb_state9.var_13 = (rb_state9.var_13) - (((rb_state9.var_6) == (0x0)) ? (rb_state9.var_10) : (0x6c75dc9e));
                        rb_state9.var_14 = (rb_state9.var_14) ^ (rb_state9.var_11);
                        rb_state9.var_15 = (rb_state9.var_15) - (((rb_state9.var_7) == (0x0)) ? (rb_state9.var_12) : (0xf65cc9fe));
                        rb_state9.var_16 = (rb_state9.var_16) + (((rb_state9.var_13) == (0x0)) ? (rb_state9.var_14) : (0x68519f5a));
                        rb_state9.var_17 = (rb_state9.var_17) + (rb_state9.var_15);
                        rb_state9.var_18 = (rb_state9.var_18) + (rb_state9.var_16);
                        rb_state9.var_19 = (rb_state9.var_19) ^ (rb_state9.var_17);
                        rb_state9.var_20 = (rb_state9.var_20) + (((rb_state9.var_8) == (0x0)) ? (rb_state9.var_18) : (0x7fcce495));
                        rb_state9.var_21 = (rb_state9.var_21) ^ (((0x8a2c153d) + (rb_state9.var_19)) + (0x5736202d));
                        rb_state9.var_22 = (rb_state9.var_22) ^ (((((0x4440a094) - (rb_state9.var_21)) - (rb_state9.var_20)) - (rb_state9.var_9)) ^ (0xc763835));
                        rb_state9.var_5 = (rb_state9.var_5) + (rb_state9.var_22);
                        rb_state9.var_4 = rb_state9.var_5;
                        pthread_mutex_unlock(&(rb_state9.lock_23));
                    }
                    rb_state9.var_10 = (rb_state9.var_10) ^ (((rb_state9.var_12) == (0x0)) ? (rb_state9.var_11) : (0x3fc6c95));
                    rb_state9.var_13 = (rb_state9.var_13) + ((0x9b138f37) - (rb_state9.var_14));
                    rb_state9.var_15 = (rb_state9.var_15) ^ ((0xac27897c) ^ (rb_state9.var_16));
                    rb_state9.var_17 = (rb_state9.var_17) + (0x6d96f09c);
                    rb_state9.var_18 = (rb_state9.var_18) ^ (((rb_state9.var_20) == (0x0)) ? (rb_state9.var_19) : (0x41741cf7));
                    if ((rb_state9.var_21) == (0xd80bcb14)) {
                        pthread_mutex_lock(&(rb_state9.lock_37));
                        rb_state9.var_27 = 0x3c61fca5;
                        rb_state9.var_28 = (rb_state9.var_28) ^ (((((0xf6dec081) + (rb_state9.var_25)) ^ (rb_state9.var_24)) + (0x7bf66723)) - (rb_state9.var_22));
                        rb_state9.var_29 = (rb_state9.var_29) + (((((0xb765d2c7) + (rb_state9.var_27)) ^ (0xbe210c3f)) - (rb_state9.var_28)) + (rb_state9.var_26));
                        rb_state9.var_26 = (rb_state9.var_26) ^ (rb_state9.var_29);
                        rb_state9.var_25 = rb_state9.var_26;
                        pthread_mutex_unlock(&(rb_state9.lock_37));
                    }
                    if ((rb_state9.var_21) == (0xd80bcb14)) {
                        rb_state9.var_30 = 0x98058538;
                        rb_state9.var_31 = (rb_state9.var_31) + (((((0xd16e313f) ^ (rb_state9.var_27)) - (0xbe94aad3)) - (rb_state9.var_28)) - (0x2ce1090));
                        rb_state9.var_32 = (rb_state9.var_32) + (0xd515bf51);
                        rb_state9.var_33 = (rb_state9.var_33) ^ (rb_state9.var_30);
                        rb_state9.var_34 = (rb_state9.var_34) ^ (((0x7dae8702) - (rb_state9.var_31)) ^ (rb_state9.var_29));
                        rb_state9.var_35 = (rb_state9.var_35) ^ (((((0xd16f93f2) + (0x36feddaa)) - (rb_state9.var_33)) - (0xac97e407)) - (rb_state9.var_32));
                        rb_state9.var_36 = (rb_state9.var_36) ^ (((rb_state9.var_34) == (0x0)) ? (rb_state9.var_35) : (0x512351f2));
                        rb_state9.var_25 = (rb_state9.var_25) ^ (rb_state9.var_36);
                    }
                    if ((rb_state9.var_21) == (0xd80bcb14)) {
                        if (!((rb_state9.var_25) == (rb_state9.var_26))) {
                            racebench_trigger(9);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_0 = (rb_state10.var_0) ^ ((0x460ea1a) - (0x5ae102f9));
                    rb_state10.var_3 = (rb_state10.var_3) + (rb_state10.var_3);
                    rb_state10.var_3 = (rb_state10.var_3) ^ (rb_state10.var_2);
                    rb_state10.var_4 = (rb_state10.var_4) - (rb_state10.var_4);
                    rb_state10.var_4 = (rb_state10.var_4) - (((rb_state10.var_4) == (0x0)) ? (rb_state10.var_3) : (0x9829e9c2));
                    if ((rb_state10.var_0) == (0xc584eab0)) {
                        if ((rb_state10.var_5) != (0x0)) {
                            if (!((rb_state10.var_5) == (rb_state10.var_20))) {
                                racebench_trigger(10);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_11
                    rb_state11.var_0 = (rb_state11.var_0) - (1775 < rb_input_size ? (uint32_t)rb_input[1775] : 0x55b43686);
                    rb_state11.var_1 = (rb_state11.var_1) + (((rb_state11.var_1) == (0x0)) ? (rb_state11.var_0) : (0xfcff9df5));
                    rb_state11.var_0 = (rb_state11.var_0) ^ ((0xba5b294e) ^ (rb_state11.var_1));
                    rb_state11.var_2 = (rb_state11.var_2) - (0x6a20676c);
                    rb_state11.var_2 = (rb_state11.var_2) ^ ((0x3d393c45) + (rb_state11.var_3));
                    rb_state11.var_2 = (rb_state11.var_2) + (14411 < rb_input_size ? (uint32_t)rb_input[14411] : 0x57468d62);
                    rb_state11.var_3 = (rb_state11.var_3) - ((0xca6e1621) - (0x6d4bdcf9));
                    rb_state11.var_1 = (rb_state11.var_1) + (((rb_state11.var_3) == (0x6c06e4c2)) ? (rb_state11.var_2) : (0x174dd153));
                    if ((rb_state11.var_0) == (0xdb26fda0)) {
                        if ((rb_state11.var_4) != (0x0)) {
                            if (!((rb_state11.var_4) == (rb_state11.var_37))) {
                                racebench_trigger(11);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_3 = (rb_state12.var_3) + ((0x35c48d8a) ^ (0xa1329e9));
                    rb_state12.var_1 = (rb_state12.var_1) ^ ((0xecf39635) + (0x85b3106a));
                    rb_state12.var_2 = (rb_state12.var_2) + (12010 < rb_input_size ? (uint32_t)rb_input[12010] : 0xc3ca6233);
                    if ((rb_state12.var_3) == (0xdf448a93)) {
                        pthread_mutex_lock(&(rb_state12.lock_12));
                        rb_state12.var_6 = (rb_state12.var_6) - (rb_state12.var_5);
                        rb_state12.var_7 = (rb_state12.var_7) - (((rb_state12.var_4) == (0x0)) ? (rb_state12.var_6) : (0xe6a62212));
                        rb_state12.var_5 = (rb_state12.var_5) - (rb_state12.var_7);
                        rb_state12.var_4 = rb_state12.var_5;
                        pthread_mutex_unlock(&(rb_state12.lock_12));
                    }
                    if ((rb_state12.var_3) == (0x84cd0f2)) {
                        pthread_mutex_lock(&(rb_state12.lock_12));
                        if (!((rb_state12.var_4) == (rb_state12.var_5))) {
                            racebench_trigger(12);
                        }
                        pthread_mutex_unlock(&(rb_state12.lock_12));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_1 = (rb_state13.var_1) - (rb_state13.var_1);
                    rb_state13.var_2 = (rb_state13.var_2) - ((0xda6d33f6) ^ (0xdefb6aae));
                    rb_state13.var_3 = (rb_state13.var_3) - (((rb_state13.var_3) == (0x0)) ? (rb_state13.var_2) : (0x40a429d8));
                    rb_state13.var_1 = (rb_state13.var_1) ^ (((rb_state13.var_3) == (0x4965958)) ? (rb_state13.var_2) : (0x7214f0c6));
                    rb_state13.var_0 = (rb_state13.var_0) - (9343 < rb_input_size ? (uint32_t)rb_input[9343] : 0x55d0ddfd);
                    rb_state13.var_4 = (rb_state13.var_4) + (2864 < rb_input_size ? (uint32_t)rb_input[2864] : 0x331042e9);
                    if ((rb_state13.var_4) == (0x86f15f26)) {
                        rb_state13.var_13 = 0xbc5de941;
                        rb_state13.var_14 = 0x393d6c57;
                        rb_state13.var_15 = (rb_state13.var_15) + ((0xef018660) + (rb_state13.var_9));
                        rb_state13.var_16 = (rb_state13.var_16) ^ (((((0x6fa5b5cf) + (0x17bca9b9)) ^ (rb_state13.var_8)) ^ (rb_state13.var_10)) - (rb_state13.var_13));
                        rb_state13.var_17 = (rb_state13.var_17) + (((rb_state13.var_14) == (0x0)) ? (rb_state13.var_15) : (0x5c3db47c));
                        rb_state13.var_18 = (rb_state13.var_18) ^ (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_16) : (0xdbf71c28));
                        rb_state13.var_19 = (rb_state13.var_19) - (rb_state13.var_17);
                        rb_state13.var_20 = (rb_state13.var_20) + (((rb_state13.var_18) == (0x0)) ? (rb_state13.var_19) : (0xbdefcfd3));
                        rb_state13.var_12 = (rb_state13.var_12) + (rb_state13.var_20);
                        rb_state13.var_5 = rb_state13.var_12;
                    }
                    if ((rb_state13.var_4) == (0xd4a91867)) {
                        pthread_mutex_lock(&(rb_state13.lock_50));
                        rb_state13.var_22 = 0x73b75512;
                        rb_state13.var_23 = 0xcb2626c0;
                        rb_state13.var_24 = (rb_state13.var_24) ^ (rb_state13.var_22);
                        rb_state13.var_25 = (rb_state13.var_25) + (rb_state13.var_13);
                        rb_state13.var_26 = (rb_state13.var_26) ^ (rb_state13.var_23);
                        rb_state13.var_27 = (rb_state13.var_27) ^ (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_24) : (0x3bf0df1a));
                        rb_state13.var_28 = (rb_state13.var_28) ^ (((0xd822afdb) - (rb_state13.var_14)) + (rb_state13.var_25));
                        rb_state13.var_29 = (rb_state13.var_29) + (((0xa71e8e4b) - (rb_state13.var_15)) + (rb_state13.var_26));
                        rb_state13.var_30 = (rb_state13.var_30) ^ (rb_state13.var_27);
                        rb_state13.var_31 = (rb_state13.var_31) + (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_28) : (0x2b3317b2));
                        rb_state13.var_32 = (rb_state13.var_32) ^ (rb_state13.var_29);
                        rb_state13.var_33 = (rb_state13.var_33) + (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_30) : (0x6472fb5f));
                        rb_state13.var_34 = (rb_state13.var_34) + (((rb_state13.var_18) == (0x0)) ? (rb_state13.var_31) : (0xc1bdbc8f));
                        rb_state13.var_35 = (rb_state13.var_35) + (rb_state13.var_32);
                        rb_state13.var_36 = (rb_state13.var_36) + (((0xd0f6f509) - (rb_state13.var_33)) ^ (0xaae10950));
                        rb_state13.var_37 = (rb_state13.var_37) ^ (((0x994fe71a) - (rb_state13.var_34)) + (0x2db94c5c));
                        rb_state13.var_38 = (rb_state13.var_38) + (((0xe5b956f0) + (rb_state13.var_35)) - (0x31fbb8b4));
                        rb_state13.var_39 = (rb_state13.var_39) ^ (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_36) : (0xe171a6b));
                        rb_state13.var_40 = (rb_state13.var_40) - (((0x6dd6d335) - (rb_state13.var_37)) - (rb_state13.var_20));
                        rb_state13.var_41 = (rb_state13.var_41) + (rb_state13.var_38);
                        rb_state13.var_42 = (rb_state13.var_42) ^ (((rb_state13.var_21) == (0x0)) ? (rb_state13.var_39) : (0xc31f6116));
                        rb_state13.var_43 = (rb_state13.var_43) ^ (((rb_state13.var_22) == (0x0)) ? (rb_state13.var_40) : (0x5c29eecb));
                        rb_state13.var_44 = (rb_state13.var_44) - (((rb_state13.var_23) == (0x0)) ? (rb_state13.var_41) : (0x9c60d220));
                        rb_state13.var_45 = (rb_state13.var_45) + (((0xed36357e) ^ (rb_state13.var_42)) ^ (rb_state13.var_24));
                        rb_state13.var_46 = (rb_state13.var_46) + (((((0x6dc9d003) - (0xb06142b1)) - (rb_state13.var_43)) + (rb_state13.var_25)) ^ (rb_state13.var_44));
                        rb_state13.var_47 = (rb_state13.var_47) ^ (((rb_state13.var_26) == (0x0)) ? (rb_state13.var_45) : (0x63e24083));
                        rb_state13.var_48 = (rb_state13.var_48) ^ (((rb_state13.var_27) == (0x0)) ? (rb_state13.var_46) : (0x9a279f7a));
                        rb_state13.var_49 = (rb_state13.var_49) ^ (((rb_state13.var_47) == (0x0)) ? (rb_state13.var_48) : (0xe448580a));
                        rb_state13.var_21 = (rb_state13.var_21) - (rb_state13.var_49);
                        rb_state13.var_5 = rb_state13.var_21;
                        pthread_mutex_unlock(&(rb_state13.lock_50));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_14
                    rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_0);
                    rb_state14.var_1 = (rb_state14.var_1) + (rb_state14.var_1);
                    rb_state14.var_0 = (rb_state14.var_0) - (0xae517ad5);
                    rb_state14.var_2 = (rb_state14.var_2) ^ (6615 < rb_input_size ? (uint32_t)rb_input[6615] : 0xae51a339);
                    rb_state14.var_0 = (rb_state14.var_0) + ((0x3a61d68c) + (rb_state14.var_1));
                    rb_state14.var_3 = (rb_state14.var_3) - ((0xbc7a54af) + (0xc7c4e81c));
                    if ((rb_state14.var_3) == (0xee69cfa8)) {
                        rb_state14.var_4 = rb_state14.var_0;
                    }
                    if ((rb_state14.var_3) == (0x6a2a92dd)) {
                        rb_state14.var_5 = 0xee4647c4;
                        rb_state14.var_6 = 0x6aaf955f;
                        rb_state14.var_7 = (rb_state14.var_7) + (((((0x5abed328) ^ (rb_state14.var_5)) + (rb_state14.var_5)) + (rb_state14.var_6)) - (0xdc7e5b5a));
                        rb_state14.var_8 = (rb_state14.var_8) - (((0x1b1adf81) + (0xb3ed47ba)) - (rb_state14.var_4));
                        rb_state14.var_9 = (rb_state14.var_9) + (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_8) : (0x7028692d));
                        rb_state14.var_0 = (rb_state14.var_0) - (rb_state14.var_9);
                    }
                    if ((rb_state14.var_3) == (0x6a2a92dd)) {
                        if (!((rb_state14.var_0) == (rb_state14.var_4))) {
                            racebench_trigger(14);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    rb_state15.var_2 = (rb_state15.var_2) + (3810 < rb_input_size ? (uint32_t)rb_input[3810] : 0xe433aca1);
                    if ((rb_state15.var_0) == (0x4d7caee8)) {
                        rb_state15.var_10 = 0x10789b3b;
                        rb_state15.var_11 = (rb_state15.var_11) ^ (rb_state15.var_9);
                        rb_state15.var_12 = (rb_state15.var_12) + (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_8) : (0x9ae36062));
                        rb_state15.var_13 = (rb_state15.var_13) + (rb_state15.var_7);
                        rb_state15.var_14 = (rb_state15.var_14) ^ (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_11) : (0x28dc1e07));
                        rb_state15.var_15 = (rb_state15.var_15) - (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_13) : (0x5a43aa30));
                        rb_state15.var_16 = (rb_state15.var_16) ^ (rb_state15.var_14);
                        rb_state15.var_17 = (rb_state15.var_17) - (((rb_state15.var_15) == (0x0)) ? (rb_state15.var_16) : (0x6145d859));
                        rb_state15.var_9 = (rb_state15.var_9) - (rb_state15.var_17);
                        rb_state15.var_3 = rb_state15.var_9;
                    }
                    if ((rb_state15.var_0) == (0x897ca8d6)) {
                        pthread_mutex_lock(&(rb_state15.lock_27));
                        rb_state15.var_19 = 0x25f83ded;
                        rb_state15.var_20 = 0xfba43d22;
                        rb_state15.var_21 = (rb_state15.var_21) ^ (rb_state15.var_11);
                        rb_state15.var_22 = (rb_state15.var_22) - (((rb_state15.var_19) == (0x0)) ? (rb_state15.var_20) : (0x70a3a4e4));
                        rb_state15.var_23 = (rb_state15.var_23) - (rb_state15.var_21);
                        rb_state15.var_24 = (rb_state15.var_24) + (rb_state15.var_22);
                        rb_state15.var_25 = (rb_state15.var_25) - (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_23) : (0xda5d33dc));
                        rb_state15.var_26 = (rb_state15.var_26) + (((rb_state15.var_24) == (0x0)) ? (rb_state15.var_25) : (0x98854f5d));
                        rb_state15.var_18 = (rb_state15.var_18) - (rb_state15.var_26);
                        rb_state15.var_3 = rb_state15.var_18;
                        pthread_mutex_unlock(&(rb_state15.lock_27));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_16
                    rb_state16.var_1 = (rb_state16.var_1) ^ (rb_state16.var_0);
                    rb_state16.var_2 = (rb_state16.var_2) ^ (0xd4c12cc2);
                    rb_state16.var_3 = (rb_state16.var_3) ^ (rb_state16.var_2);
                    rb_state16.var_4 = (rb_state16.var_4) - ((0xed4d8f7b) - (0xd890be35));
                    rb_state16.var_3 = (rb_state16.var_3) + ((0x37fe1435) ^ (rb_state16.var_4));
                    rb_state16.var_1 = (rb_state16.var_1) ^ (0x162b2a2f);
                    rb_state16.var_2 = (rb_state16.var_2) - (rb_state16.var_3);
                    rb_state16.var_4 = (rb_state16.var_4) + (((rb_state16.var_1) == (0x162b2ad0)) ? (rb_state16.var_0) : (0x190d5d8b));
                    if ((rb_state16.var_2) == (0x420fd5a8)) {
                        rb_state16.var_6 = 0x21c388d9;
                        rb_state16.var_7 = 0x24acbfaa;
                        rb_state16.var_8 = (rb_state16.var_8) ^ (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_6) : (0xa6673a1c));
                        rb_state16.var_9 = (rb_state16.var_9) ^ (rb_state16.var_7);
                        rb_state16.var_10 = (rb_state16.var_10) + (rb_state16.var_5);
                        rb_state16.var_11 = (rb_state16.var_11) + (rb_state16.var_6);
                        rb_state16.var_12 = (rb_state16.var_12) + (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_9) : (0xea4466dd));
                        rb_state16.var_13 = (rb_state16.var_13) - (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_10) : (0xf0faf6f2));
                        rb_state16.var_14 = (rb_state16.var_14) ^ (((rb_state16.var_11) == (0x0)) ? (rb_state16.var_12) : (0x34b4ee3b));
                        rb_state16.var_15 = (rb_state16.var_15) - (((((0x7a9cf070) + (rb_state16.var_14)) - (rb_state16.var_9)) + (rb_state16.var_13)) ^ (0x90a50752));
                        rb_state16.var_3 = (rb_state16.var_3) - (rb_state16.var_15);
                    }
                    rb_state16.var_10 = (rb_state16.var_10) ^ (6997 < rb_input_size ? (uint32_t)rb_input[6997] : 0x16241838);
                    rb_state16.var_11 = (rb_state16.var_11) + (((rb_state16.var_13) == (0x0)) ? (rb_state16.var_12) : (0x6d7e32f3));
                    rb_state16.var_14 = (rb_state16.var_14) + (rb_state16.var_15);
                    rb_state16.var_8 = (rb_state16.var_8) - (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_17) : (0x484bce93));
                    rb_state16.var_9 = (rb_state16.var_9) - (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_6) : (0xd0aa92c2));
                    if ((rb_state16.var_10) == (0x595f88f8)) {
                        pthread_mutex_lock(&(rb_state16.lock_38));
                        rb_state16.var_20 = 0x7d27c7ad;
                        rb_state16.var_21 = (rb_state16.var_21) ^ (rb_state16.var_19);
                        rb_state16.var_22 = (rb_state16.var_22) + (((0x333e83d5) ^ (0x6bb877a)) - (rb_state16.var_18));
                        rb_state16.var_23 = (rb_state16.var_23) + (((0xfe0968a8) - (rb_state16.var_20)) ^ (rb_state16.var_20));
                        rb_state16.var_24 = (rb_state16.var_24) ^ (rb_state16.var_21);
                        rb_state16.var_25 = (rb_state16.var_25) ^ (((((0x7f0f3363) ^ (rb_state16.var_21)) + (rb_state16.var_23)) + (0x13469f8f)) + (rb_state16.var_22));
                        rb_state16.var_26 = (rb_state16.var_26) - (((rb_state16.var_22) == (0x0)) ? (rb_state16.var_24) : (0x94e4b40a));
                        rb_state16.var_27 = (rb_state16.var_27) + (((((0x40214de0) + (rb_state16.var_23)) ^ (rb_state16.var_26)) ^ (rb_state16.var_25)) - (0xe90cdf8c));
                        rb_state16.var_19 = (rb_state16.var_19) ^ (rb_state16.var_27);
                        rb_state16.var_18 = rb_state16.var_19;
                        pthread_mutex_unlock(&(rb_state16.lock_38));
                    }
                    if ((rb_state16.var_10) == (0x595f88f8)) {
                        rb_state16.var_28 = 0xcbb7bf15;
                        rb_state16.var_29 = 0x9a4e18a8;
                        rb_state16.var_30 = (rb_state16.var_30) + (((rb_state16.var_25) == (0x0)) ? (rb_state16.var_29) : (0x4d8c2e3));
                        rb_state16.var_31 = (rb_state16.var_31) + (((0x2a623201) - (rb_state16.var_24)) + (rb_state16.var_26));
                        rb_state16.var_32 = (rb_state16.var_32) + (((0x376e1ac6) - (rb_state16.var_27)) + (rb_state16.var_28));
                        rb_state16.var_33 = (rb_state16.var_33) - (((rb_state16.var_28) == (0x0)) ? (rb_state16.var_30) : (0xb1afb4a6));
                        rb_state16.var_34 = (rb_state16.var_34) - (((rb_state16.var_29) == (0x0)) ? (rb_state16.var_31) : (0x16882be5));
                        rb_state16.var_35 = (rb_state16.var_35) + (((rb_state16.var_32) == (0x0)) ? (rb_state16.var_33) : (0xa4501d89));
                        rb_state16.var_36 = (rb_state16.var_36) - (rb_state16.var_34);
                        rb_state16.var_37 = (rb_state16.var_37) - (((rb_state16.var_35) == (0x0)) ? (rb_state16.var_36) : (0x7dcc5fcf));
                        rb_state16.var_18 = (rb_state16.var_18) - (rb_state16.var_37);
                    }
                    if ((rb_state16.var_10) == (0x595f88f8)) {
                        if (!((rb_state16.var_18) == (rb_state16.var_19))) {
                            racebench_trigger(16);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_17
                    rb_state17.var_0 = (rb_state17.var_0) + (0xa3fb9e36);
                    rb_state17.var_1 = (rb_state17.var_1) ^ ((0x594c194c) ^ (rb_state17.var_0));
                    rb_state17.var_2 = (rb_state17.var_2) - (((rb_state17.var_2) == (0x0)) ? (rb_state17.var_1) : (0x6b2f64e9));
                    rb_state17.var_0 = (rb_state17.var_0) + (rb_state17.var_1);
                    rb_state17.var_3 = (rb_state17.var_3) - (0xab2db7d4);
                    if ((rb_state17.var_2) == (0xcda72876)) {
                        rb_state17.var_3 = (rb_state17.var_3) + (rb_state17.var_4);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_2 = (rb_state18.var_2) + ((0x4597dcd7) - (rb_state18.var_2));
                    rb_state18.var_3 = (rb_state18.var_3) + (((rb_state18.var_1) == (0x0)) ? (rb_state18.var_3) : (0xcd78811f));
                    rb_state18.var_2 = (rb_state18.var_2) - (13314 < rb_input_size ? (uint32_t)rb_input[13314] : 0x8b69bf71);
                    rb_state18.var_3 = (rb_state18.var_3) ^ ((0xb2db1f13) - (rb_state18.var_0));
                    rb_state18.var_1 = (rb_state18.var_1) ^ (0xb668135e);
                    rb_state18.var_4 = (rb_state18.var_4) - (4732 < rb_input_size ? (uint32_t)rb_input[4732] : 0x30a6bdf2);
                    if ((rb_state18.var_4) == (0xffff3910)) {
                        pthread_mutex_lock(&(rb_state18.lock_32));
                        rb_state18.var_7 = 0xb01f8789;
                        rb_state18.var_8 = 0xc83915be;
                        rb_state18.var_9 = (rb_state18.var_9) ^ (rb_state18.var_7);
                        rb_state18.var_10 = (rb_state18.var_10) ^ (((((0x55529a61) ^ (rb_state18.var_6)) - (rb_state18.var_8)) ^ (rb_state18.var_5)) ^ (0x787214e6));
                        rb_state18.var_11 = (rb_state18.var_11) - (((rb_state18.var_7) == (0x0)) ? (rb_state18.var_9) : (0x5d0f72e5));
                        rb_state18.var_12 = (rb_state18.var_12) - (((((0x832e926a) + (0x5ac84289)) ^ (0xc667a87)) + (rb_state18.var_10)) + (rb_state18.var_11));
                        rb_state18.var_6 = (rb_state18.var_6) + (rb_state18.var_12);
                        rb_state18.var_5 = rb_state18.var_6;
                        pthread_mutex_unlock(&(rb_state18.lock_32));
                    }
                    if ((rb_state18.var_4) == (0x41c26e84)) {
                        pthread_mutex_lock(&(rb_state18.lock_32));
                        rb_state18.var_33 = !((rb_state18.var_5) == (rb_state18.var_6));
                        pthread_mutex_unlock(&(rb_state18.lock_32));
                    }
                    rb_state18.var_19 = (rb_state18.var_19) ^ (((rb_state18.var_21) == (0x0)) ? (rb_state18.var_20) : (0xea5d1af9));
                    rb_state18.var_22 = (rb_state18.var_22) - (((rb_state18.var_24) == (0x0)) ? (rb_state18.var_23) : (0xb4cdd768));
                    rb_state18.var_25 = (rb_state18.var_25) + (rb_state18.var_26);
                    rb_state18.var_28 = (rb_state18.var_28) ^ (10920 < rb_input_size ? (uint32_t)rb_input[10920] : 0x2d07db60);
                    if ((rb_state18.var_29) == (0x2c99a424)) {
                        rb_state18.var_46 = 0x8c4ad914;
                        rb_state18.var_47 = (rb_state18.var_47) ^ (((((0xf2141a1f) + (0xa479a086)) ^ (rb_state18.var_39)) + (rb_state18.var_40)) - (rb_state18.var_38));
                        rb_state18.var_48 = (rb_state18.var_48) + (((((0x7ce13986) ^ (rb_state18.var_47)) - (rb_state18.var_42)) - (rb_state18.var_41)) - (rb_state18.var_46));
                        rb_state18.var_45 = (rb_state18.var_45) ^ (rb_state18.var_48);
                        rb_state18.var_34 = rb_state18.var_45;
                    }
                    if ((rb_state18.var_29) == (0x2c99a424)) {
                        pthread_mutex_lock(&(rb_state18.lock_59));
                        rb_state18.var_50 = (rb_state18.var_50) - (rb_state18.var_43);
                        rb_state18.var_51 = (rb_state18.var_51) ^ (0xd5f55bef);
                        rb_state18.var_52 = (rb_state18.var_52) + (((((0x6f86b629) - (rb_state18.var_46)) + (rb_state18.var_45)) ^ (rb_state18.var_44)) + (0xdd9d4e9f));
                        rb_state18.var_53 = (rb_state18.var_53) - ((0x3b1d176) + (rb_state18.var_47));
                        rb_state18.var_54 = (rb_state18.var_54) - (((0xb7bb54b) + (rb_state18.var_50)) - (0xd6240150));
                        rb_state18.var_55 = (rb_state18.var_55) + (((rb_state18.var_51) == (0x0)) ? (rb_state18.var_52) : (0xc3b9d9d));
                        rb_state18.var_56 = (rb_state18.var_56) - (((rb_state18.var_53) == (0x0)) ? (rb_state18.var_54) : (0xac115492));
                        rb_state18.var_57 = (rb_state18.var_57) + (((0xc1e36497) - (0xb964c43b)) - (rb_state18.var_55));
                        rb_state18.var_58 = (rb_state18.var_58) - (((rb_state18.var_56) == (0x0)) ? (rb_state18.var_57) : (0x27c1732e));
                        rb_state18.var_49 = (rb_state18.var_49) ^ (rb_state18.var_58);
                        rb_state18.var_34 = rb_state18.var_49;
                        pthread_mutex_unlock(&(rb_state18.lock_59));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    rb_state19.var_0 = (rb_state19.var_0) + ((0x5a2f5420) - (0x85ad88d4));
                    rb_state19.var_0 = (rb_state19.var_0) ^ ((0x54da8c30) - (0x4ea49c67));
                    rb_state19.var_3 = (rb_state19.var_3) - (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_0) : (0x204e1bca));
                    rb_state19.var_2 = (rb_state19.var_2) - ((0x3437aef0) ^ (0x3dc4b8c4));
                    rb_state19.var_3 = (rb_state19.var_3) ^ (((rb_state19.var_1) == (0x0)) ? (rb_state19.var_0) : (0xc616129d));
                    rb_state19.var_2 = (rb_state19.var_2) + (14065 < rb_input_size ? (uint32_t)rb_input[14065] : 0x8a955ef1);
                    rb_state19.var_4 = (rb_state19.var_4) ^ (((rb_state19.var_4) == (0x0)) ? (rb_state19.var_4) : (0x38eb67fa));
                    if ((rb_state19.var_3) == (0xed7fbe9f)) {
                        rb_state19.var_11 = (rb_state19.var_11) + (((rb_state19.var_11) == (0x0)) ? (rb_state19.var_10) : (0x12352cb5));
                        rb_state19.var_12 = (rb_state19.var_12) ^ (((((0xd5f5157d) - (0xdf172e71)) - (rb_state19.var_8)) - (rb_state19.var_12)) + (rb_state19.var_9));
                        rb_state19.var_13 = (rb_state19.var_13) ^ (((rb_state19.var_5) == (0x0)) ? (rb_state19.var_13) : (0xa9b1a768));
                        rb_state19.var_14 = (rb_state19.var_14) ^ (rb_state19.var_7);
                        rb_state19.var_15 = (rb_state19.var_15) ^ (((((0xec4749e7) - (0x3ee972c6)) - (0x88da2c6)) ^ (rb_state19.var_11)) ^ (rb_state19.var_12));
                        rb_state19.var_16 = (rb_state19.var_16) ^ (((rb_state19.var_13) == (0x0)) ? (rb_state19.var_14) : (0xd43aa15));
                        rb_state19.var_17 = (rb_state19.var_17) - (((((0xdcc92537) ^ (rb_state19.var_16)) - (rb_state19.var_15)) - (0xadc09370)) + (0x96c7e476));
                        rb_state19.var_5 = (rb_state19.var_5) ^ (rb_state19.var_17);
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