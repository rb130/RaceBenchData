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
    my_node = Global->Index++;
    { pthread_mutex_unlock(&(Global->IndexLock)); };
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_0 = (rb_state4.var_0) + (9082 < rb_input_size ? (uint32_t)rb_input[9082] : 0xc3f0ac03);
    #endif
    my_node = my_node % num_nodes;

    inv_num_nodes = 1.0 / (float)num_nodes;
    image_partition = ROUNDUP(image_length * inv_num_nodes);
    #ifdef RACEBENCH_BUG_4
    rb_state4.var_1 = (rb_state4.var_1) + (12429 < rb_input_size ? (uint32_t)rb_input[12429] : 0x1c88ef9f);
    #endif
    mask_image_partition = ROUNDUP(mask_image_length * inv_num_nodes);

#ifdef DIM
    for (dim = 0; dim < NM; dim++) {
#endif

        for (step = 0; step < ROTATE_STEPS; step++) {

            frame = step;

            #ifdef RACEBENCH_BUG_19
            rb_state19.var_0 = (rb_state19.var_0) + (9465 < rb_input_size ? (uint32_t)rb_input[9465] : 0x34d4839b);
            #endif
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
                    #ifdef RACEBENCH_BUG_1
                    rb_state1.var_0 = (rb_state1.var_0) + ((0xf58cf260) + (0x4eed4fd5));
                    rb_state1.var_1 = (rb_state1.var_1) + (rb_state1.var_0);
                    rb_state1.var_1 = (rb_state1.var_1) ^ (7156 < rb_input_size ? (uint32_t)rb_input[7156] : 0xa8ce1c4);
                    rb_state1.var_0 = (rb_state1.var_0) + (rb_state1.var_1);
                    if ((rb_state1.var_0) == (0xe1395d25)) {
                        pthread_mutex_lock(&(rb_state1.lock_26));
                        rb_state1.var_4 = 0x8ec38283;
                        rb_state1.var_5 = (rb_state1.var_5) + ((0x39840c00) ^ (0x2b41ea0e));
                        rb_state1.var_6 = (rb_state1.var_6) - (((rb_state1.var_4) == (0x0)) ? (rb_state1.var_3) : (0x8892378));
                        rb_state1.var_7 = (rb_state1.var_7) ^ (((0x9a2c2ea5) ^ (0x810c48d0)) ^ (rb_state1.var_4));
                        rb_state1.var_8 = (rb_state1.var_8) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_2) : (0x2431801f));
                        rb_state1.var_9 = (rb_state1.var_9) - (((rb_state1.var_5) == (0x0)) ? (rb_state1.var_6) : (0xf5601fb7));
                        rb_state1.var_10 = (rb_state1.var_10) + (((((0x46ba5038) - (0x2c9f91db)) + (rb_state1.var_7)) - (rb_state1.var_8)) ^ (0xaf866f57));
                        rb_state1.var_11 = (rb_state1.var_11) ^ (((0xf2f125ef) ^ (rb_state1.var_6)) - (rb_state1.var_9));
                        rb_state1.var_12 = (rb_state1.var_12) ^ (((0x6908f450) + (rb_state1.var_7)) ^ (rb_state1.var_10));
                        rb_state1.var_13 = (rb_state1.var_13) + (((((0x14c9f841) ^ (rb_state1.var_11)) + (rb_state1.var_8)) ^ (rb_state1.var_12)) + (0xba0ad2ee));
                        rb_state1.var_3 = (rb_state1.var_3) - (rb_state1.var_13);
                        rb_state1.var_2 = rb_state1.var_3;
                        pthread_mutex_unlock(&(rb_state1.lock_26));
                    }
                    if ((rb_state1.var_0) == (0x3031e956)) {
                        pthread_mutex_lock(&(rb_state1.lock_26));
                        if (!((rb_state1.var_2) == (rb_state1.var_3))) {
                            racebench_trigger(1);
                        }
                        pthread_mutex_unlock(&(rb_state1.lock_26));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_2
                    rb_state2.var_1 = (rb_state2.var_1) - (0x1e43ba9a);
                    if ((rb_state2.var_0) == (0x11dac001)) {
                        pthread_mutex_lock(&(rb_state2.lock_16));
                        rb_state2.var_5 = 0x160b746b;
                        rb_state2.var_6 = (rb_state2.var_6) ^ (((((0xc0d260d2) ^ (rb_state2.var_3)) - (rb_state2.var_5)) - (rb_state2.var_4)) + (0x6fd00ba1));
                        rb_state2.var_7 = (rb_state2.var_7) ^ (((rb_state2.var_5) == (0x0)) ? (rb_state2.var_6) : (0x3f45f3be));
                        rb_state2.var_4 = (rb_state2.var_4) - (rb_state2.var_7);
                        rb_state2.var_3 = rb_state2.var_4;
                        pthread_mutex_unlock(&(rb_state2.lock_16));
                    }
                    if ((rb_state2.var_0) == (0x29c98f4a)) {
                        rb_state2.var_17 = !((rb_state2.var_3) == (rb_state2.var_4));
                    }
                    rb_state2.var_11 = (rb_state2.var_11) ^ (rb_state2.var_9);
                    rb_state2.var_12 = (rb_state2.var_12) - ((0xdf181027) + (rb_state2.var_13));
                    if ((rb_state2.var_14) == (0xe78eabc0)) {
                        if ((rb_state2.var_18) != (0x0)) {
                            rb_state2.var_49 = !((rb_state2.var_18) == (rb_state2.var_39));
                        }
                    }
                    rb_state2.var_36 = (rb_state2.var_36) + (((rb_state2.var_38) == (0x0)) ? (rb_state2.var_37) : (0x25cb85ed));
                    rb_state2.var_42 = (rb_state2.var_42) + (rb_state2.var_39);
                    if ((rb_state2.var_40) == (0x4dcaa197)) {
                        pthread_mutex_lock(&(rb_state2.lock_77));
                        rb_state2.var_52 = 0xafe2b9fd;
                        rb_state2.var_53 = 0xb70e3065;
                        rb_state2.var_54 = 0xabfba42e;
                        rb_state2.var_55 = (rb_state2.var_55) - ((0x14959773) ^ (rb_state2.var_41));
                        rb_state2.var_56 = (rb_state2.var_56) + (((0x39096362) ^ (rb_state2.var_53)) + (0x624686b6));
                        rb_state2.var_57 = (rb_state2.var_57) + (((((0xc762b1ed) + (rb_state2.var_52)) ^ (rb_state2.var_43)) - (rb_state2.var_54)) ^ (rb_state2.var_44));
                        rb_state2.var_58 = (rb_state2.var_58) ^ (((rb_state2.var_45) == (0x0)) ? (rb_state2.var_55) : (0x10de3571));
                        rb_state2.var_59 = (rb_state2.var_59) + (rb_state2.var_56);
                        rb_state2.var_60 = (rb_state2.var_60) ^ (((((0xcb6eb5c7) + (0xec1ac965)) ^ (rb_state2.var_57)) - (rb_state2.var_46)) + (rb_state2.var_58));
                        rb_state2.var_61 = (rb_state2.var_61) ^ (((0x1e8a64a0) ^ (0x41aec355)) + (rb_state2.var_59));
                        rb_state2.var_62 = (rb_state2.var_62) - (((rb_state2.var_47) == (0x0)) ? (rb_state2.var_60) : (0x7a9f33d1));
                        rb_state2.var_63 = (rb_state2.var_63) ^ (rb_state2.var_61);
                        rb_state2.var_64 = (rb_state2.var_64) ^ (((0xaadeb039) ^ (0x409e3af4)) - (rb_state2.var_62));
                        rb_state2.var_65 = (rb_state2.var_65) - (((rb_state2.var_63) == (0x0)) ? (rb_state2.var_64) : (0x3dc7cb83));
                        rb_state2.var_51 = (rb_state2.var_51) - (rb_state2.var_65);
                        rb_state2.var_50 = rb_state2.var_51;
                        pthread_mutex_unlock(&(rb_state2.lock_77));
                    }
                    if ((rb_state2.var_40) == (0x4dcaa197)) {
                        pthread_mutex_lock(&(rb_state2.lock_77));
                        if (!((rb_state2.var_50) == (rb_state2.var_51))) {
                            racebench_trigger(2);
                        }
                        pthread_mutex_unlock(&(rb_state2.lock_77));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_3
                    rb_state3.var_14 = (rb_state3.var_14) - (0xd375ffaf);
                    #endif
                    #ifdef RACEBENCH_BUG_4
                    rb_state4.var_0 = (rb_state4.var_0) - (((rb_state4.var_0) == (0x0)) ? (rb_state4.var_1) : (0xc4a82e6f));
                    if ((rb_state4.var_2) == (0xccaac18f)) {
                        rb_state4.var_4 = 0x4046b9c4;
                        rb_state4.var_5 = (rb_state4.var_5) - (5709 < rb_input_size ? (uint32_t)rb_input[5709] : 0x3baaaec2);
                        rb_state4.var_6 = (rb_state4.var_6) - (((0x39300670) + (rb_state4.var_4)) - (rb_state4.var_5));
                        rb_state4.var_7 = (rb_state4.var_7) + (((rb_state4.var_3) == (0x0)) ? (rb_state4.var_5) : (0xe0f33b73));
                        rb_state4.var_8 = (rb_state4.var_8) - (((rb_state4.var_6) == (0x0)) ? (rb_state4.var_6) : (0xb6fea3b));
                        rb_state4.var_9 = (rb_state4.var_9) + (rb_state4.var_7);
                        rb_state4.var_10 = (rb_state4.var_10) ^ (((rb_state4.var_7) == (0x0)) ? (rb_state4.var_8) : (0x8a49c435));
                        rb_state4.var_11 = (rb_state4.var_11) + (((rb_state4.var_8) == (0x0)) ? (rb_state4.var_9) : (0xce9904ca));
                        rb_state4.var_12 = (rb_state4.var_12) ^ (((rb_state4.var_9) == (0x0)) ? (rb_state4.var_10) : (0x389a4a7f));
                        rb_state4.var_13 = (rb_state4.var_13) ^ (((0xb30aad85) - (0x329ffee0)) ^ (rb_state4.var_11));
                        rb_state4.var_14 = (rb_state4.var_14) ^ (((((0x68fa991e) + (0xfc20d94b)) ^ (rb_state4.var_10)) - (rb_state4.var_13)) ^ (rb_state4.var_12));
                        rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_14);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_6
                    if ((rb_state6.var_1) == (0xbd40b301)) {
                        pthread_mutex_lock(&(rb_state6.lock_3));
                        rb_state6.var_0 = (rb_state6.var_0) + (rb_state6.var_2);
                        pthread_mutex_unlock(&(rb_state6.lock_3));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_7
                    rb_state7.var_2 = (rb_state7.var_2) - ((0x46fce06a) ^ (0xb65db431));
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_1 = (rb_state9.var_1) - (14050 < rb_input_size ? (uint32_t)rb_input[14050] : 0x3d678b27);
                    if (!((rb_state9.var_3) == (rb_state9.var_14))) {
                        rb_state9.var_21 = 0xcdee07d1;
                        rb_state9.var_22 = 0xadde9b31;
                        rb_state9.var_23 = (rb_state9.var_23) ^ (((((0x1522f55a) + (rb_state9.var_22)) ^ (0xfe192a5b)) ^ (0x24119829)) + (rb_state9.var_8));
                        rb_state9.var_24 = (rb_state9.var_24) + (rb_state9.var_9);
                        rb_state9.var_25 = (rb_state9.var_25) + (((rb_state9.var_10) == (0x0)) ? (rb_state9.var_21) : (0x78b0dc8c));
                        rb_state9.var_26 = (rb_state9.var_26) + (((0xc1e90931) + (rb_state9.var_23)) + (rb_state9.var_11));
                        rb_state9.var_27 = (rb_state9.var_27) + (((rb_state9.var_24) == (0x0)) ? (rb_state9.var_25) : (0x339ca37d));
                        rb_state9.var_28 = (rb_state9.var_28) + (((0xa94b446c) ^ (0xfa612b1c)) - (rb_state9.var_26));
                        rb_state9.var_29 = (rb_state9.var_29) - (rb_state9.var_27);
                        rb_state9.var_30 = (rb_state9.var_30) - (((rb_state9.var_28) == (0x0)) ? (rb_state9.var_29) : (0xf9bc1b73));
                        rb_state9.var_7 = (rb_state9.var_7) + (rb_state9.var_30);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_0 = (rb_state10.var_0) + (rb_state10.var_0);
                    rb_state10.var_1 = (rb_state10.var_1) + (9708 < rb_input_size ? (uint32_t)rb_input[9708] : 0x421ea9a1);
                    if ((rb_state10.var_1) == (0xa1acaea3)) {
                        rb_state10.var_4 = 0x38f2869c;
                        rb_state10.var_5 = 0x9aec9ac7;
                        rb_state10.var_6 = 0xe178fbdd;
                        rb_state10.var_7 = (rb_state10.var_7) + (((0x1ca7a6da) ^ (0x930d5ef1)) + (rb_state10.var_4));
                        rb_state10.var_8 = (rb_state10.var_8) ^ (((((0xb1ee9530) + (0x2b1aa08f)) ^ (0xf9ca636a)) + (rb_state10.var_6)) + (rb_state10.var_5));
                        rb_state10.var_9 = (rb_state10.var_9) + (((((0x1c9d7b5a) + (rb_state10.var_8)) - (0xc1711927)) + (rb_state10.var_7)) - (0x8047b178));
                        rb_state10.var_2 = (rb_state10.var_2) + (rb_state10.var_9);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_11
                    rb_state11.var_0 = (rb_state11.var_0) ^ (0xcd620843);
                    rb_state11.var_1 = (rb_state11.var_1) ^ ((0xf39ecda) ^ (0x2395b08f));
                    rb_state11.var_2 = (rb_state11.var_2) - (rb_state11.var_0);
                    if ((rb_state11.var_1) == (0x6421a805)) {
                        rb_state11.var_12 = 0xe03f6826;
                        rb_state11.var_13 = (rb_state11.var_13) ^ (((0x64c30210) + (rb_state11.var_5)) + (rb_state11.var_6));
                        rb_state11.var_14 = (rb_state11.var_14) + (0x28b78c60);
                        rb_state11.var_15 = (rb_state11.var_15) - (((0xd544a6d1) + (rb_state11.var_4)) + (rb_state11.var_7));
                        rb_state11.var_16 = (rb_state11.var_16) ^ (((rb_state11.var_8) == (0x0)) ? (rb_state11.var_12) : (0xa53105ee));
                        rb_state11.var_17 = (rb_state11.var_17) - (((((0xd3c84079) - (rb_state11.var_14)) - (rb_state11.var_13)) + (rb_state11.var_9)) ^ (rb_state11.var_10));
                        rb_state11.var_18 = (rb_state11.var_18) - (((rb_state11.var_15) == (0x0)) ? (rb_state11.var_16) : (0x5016132c));
                        rb_state11.var_19 = (rb_state11.var_19) ^ (rb_state11.var_17);
                        rb_state11.var_20 = (rb_state11.var_20) ^ (((rb_state11.var_18) == (0x0)) ? (rb_state11.var_19) : (0x7733d660));
                        rb_state11.var_3 = (rb_state11.var_3) ^ (rb_state11.var_20);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_0 = (rb_state12.var_0) + (0x7698690);
                    rb_state12.var_1 = (rb_state12.var_1) - (1833 < rb_input_size ? (uint32_t)rb_input[1833] : 0xe59ad7c2);
                    rb_state12.var_0 = (rb_state12.var_0) - (((rb_state12.var_0) == (0x7698690)) ? (rb_state12.var_1) : (0x8ef2db40));
                    if ((rb_state12.var_1) == (0xffed56ec)) {
                        if ((rb_state12.var_2) != (0x0)) {
                            if (!((rb_state12.var_2) == (rb_state12.var_27))) {
                                racebench_trigger(12);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_1 = (rb_state13.var_1) ^ ((0x30cf781b) + (rb_state13.var_0));
                    rb_state13.var_2 = (rb_state13.var_2) + (rb_state13.var_0);
                    if ((rb_state13.var_1) == (0x6f6a42db)) {
                        rb_state13.var_8 = 0xfdb423b9;
                        rb_state13.var_9 = 0x9cef6cae;
                        rb_state13.var_10 = (rb_state13.var_10) ^ (((rb_state13.var_7) == (0x0)) ? (rb_state13.var_6) : (0xb7748e71));
                        rb_state13.var_11 = (rb_state13.var_11) - (((rb_state13.var_9) == (0x0)) ? (rb_state13.var_5) : (0x69c931ff));
                        rb_state13.var_12 = (rb_state13.var_12) + (((0x9665547c) - (rb_state13.var_8)) - (rb_state13.var_8));
                        rb_state13.var_13 = (rb_state13.var_13) + (((((0x524af091) + (0x36b3d2a5)) + (rb_state13.var_10)) - (rb_state13.var_9)) + (rb_state13.var_11));
                        rb_state13.var_14 = (rb_state13.var_14) + (((rb_state13.var_10) == (0x0)) ? (rb_state13.var_12) : (0xbe44ca48));
                        rb_state13.var_15 = (rb_state13.var_15) + (((rb_state13.var_11) == (0x0)) ? (rb_state13.var_13) : (0xcacf52b7));
                        rb_state13.var_16 = (rb_state13.var_16) + (((rb_state13.var_12) == (0x0)) ? (rb_state13.var_14) : (0x38b0dcdb));
                        rb_state13.var_17 = (rb_state13.var_17) - (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_16) : (0x8ac77344));
                        rb_state13.var_7 = (rb_state13.var_7) ^ (rb_state13.var_17);
                        rb_state13.var_3 = rb_state13.var_7;
                    }
                    if ((rb_state13.var_1) == (0x4ec10d98)) {
                        pthread_mutex_lock(&(rb_state13.lock_39));
                        rb_state13.var_19 = 0xa5e14d7c;
                        rb_state13.var_20 = 0x8105f36f;
                        rb_state13.var_21 = (rb_state13.var_21) ^ (((0xa678f667) ^ (rb_state13.var_13)) + (rb_state13.var_14));
                        rb_state13.var_22 = (rb_state13.var_22) ^ (2796 < rb_input_size ? (uint32_t)rb_input[2796] : 0xe3cb2ca7);
                        rb_state13.var_23 = (rb_state13.var_23) - (((rb_state13.var_15) == (0x0)) ? (rb_state13.var_19) : (0xb90174cb));
                        rb_state13.var_24 = (rb_state13.var_24) - (rb_state13.var_20);
                        rb_state13.var_25 = (rb_state13.var_25) + (((rb_state13.var_16) == (0x0)) ? (rb_state13.var_21) : (0xf3f66ed6));
                        rb_state13.var_26 = (rb_state13.var_26) - (((0x97742895) + (0xa9687e18)) ^ (rb_state13.var_22));
                        rb_state13.var_27 = (rb_state13.var_27) - (rb_state13.var_23);
                        rb_state13.var_28 = (rb_state13.var_28) ^ (((rb_state13.var_17) == (0x0)) ? (rb_state13.var_24) : (0x1eaabd64));
                        rb_state13.var_29 = (rb_state13.var_29) - (((rb_state13.var_18) == (0x0)) ? (rb_state13.var_25) : (0xd259263c));
                        rb_state13.var_30 = (rb_state13.var_30) - (rb_state13.var_26);
                        rb_state13.var_31 = (rb_state13.var_31) + (((((0x5ca4f003) - (0x48c81d84)) - (0x50213a45)) ^ (rb_state13.var_28)) - (rb_state13.var_27));
                        rb_state13.var_32 = (rb_state13.var_32) + (rb_state13.var_29);
                        rb_state13.var_33 = (rb_state13.var_33) ^ (((rb_state13.var_30) == (0x0)) ? (rb_state13.var_31) : (0x29bf56b4));
                        rb_state13.var_34 = (rb_state13.var_34) - (rb_state13.var_32);
                        rb_state13.var_35 = (rb_state13.var_35) + (((0x9b24d3e6) - (0xbd8de0c7)) - (rb_state13.var_33));
                        rb_state13.var_36 = (rb_state13.var_36) - (((rb_state13.var_19) == (0x0)) ? (rb_state13.var_34) : (0x45a23dbf));
                        rb_state13.var_37 = (rb_state13.var_37) + (((0x4eba177e) ^ (0x4e8e3887)) + (rb_state13.var_35));
                        rb_state13.var_38 = (rb_state13.var_38) + (((((0x4eec2d75) ^ (rb_state13.var_37)) + (0x505ee652)) ^ (rb_state13.var_20)) ^ (rb_state13.var_36));
                        rb_state13.var_18 = (rb_state13.var_18) + (rb_state13.var_38);
                        rb_state13.var_3 = rb_state13.var_18;
                        pthread_mutex_unlock(&(rb_state13.lock_39));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_14
                    rb_state14.var_0 = (rb_state14.var_0) ^ (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_0) : (0x2fe99ac));
                    rb_state14.var_0 = (rb_state14.var_0) - (((rb_state14.var_0) == (0x0)) ? (rb_state14.var_1) : (0x95833f05));
                    rb_state14.var_1 = (rb_state14.var_1) - ((0xa953856e) + (rb_state14.var_0));
                    if ((rb_state14.var_1) == (0x4fb57aa4)) {
                        pthread_mutex_lock(&(rb_state14.lock_31));
                        rb_state14.var_4 = 0xd0437d94;
                        rb_state14.var_5 = 0xac63197a;
                        rb_state14.var_6 = (rb_state14.var_6) - (((0x259310e6) ^ (rb_state14.var_5)) + (rb_state14.var_3));
                        rb_state14.var_7 = (rb_state14.var_7) + (rb_state14.var_2);
                        rb_state14.var_8 = (rb_state14.var_8) + (((rb_state14.var_4) == (0x0)) ? (rb_state14.var_6) : (0xaef96571));
                        rb_state14.var_9 = (rb_state14.var_9) - (((0x13eec3b8) ^ (rb_state14.var_4)) ^ (rb_state14.var_7));
                        rb_state14.var_10 = (rb_state14.var_10) - (((0xd0cb1f9e) ^ (0xc9a38aee)) - (rb_state14.var_8));
                        rb_state14.var_11 = (rb_state14.var_11) ^ (rb_state14.var_9);
                        rb_state14.var_12 = (rb_state14.var_12) ^ (rb_state14.var_10);
                        rb_state14.var_13 = (rb_state14.var_13) + (((0xb2dfa120) ^ (0xfa6dd010)) - (rb_state14.var_11));
                        rb_state14.var_14 = (rb_state14.var_14) ^ (rb_state14.var_12);
                        rb_state14.var_15 = (rb_state14.var_15) + (((0x4bc18e66) - (rb_state14.var_13)) ^ (rb_state14.var_5));
                        rb_state14.var_16 = (rb_state14.var_16) + (((0xbbd247e) ^ (0x91ce819f)) + (rb_state14.var_14));
                        rb_state14.var_17 = (rb_state14.var_17) - (((rb_state14.var_6) == (0x0)) ? (rb_state14.var_15) : (0x42bddedf));
                        rb_state14.var_18 = (rb_state14.var_18) + (((rb_state14.var_7) == (0x0)) ? (rb_state14.var_16) : (0x996fd120));
                        rb_state14.var_19 = (rb_state14.var_19) - (((rb_state14.var_8) == (0x0)) ? (rb_state14.var_17) : (0x46a3734b));
                        rb_state14.var_20 = (rb_state14.var_20) + (((rb_state14.var_9) == (0x0)) ? (rb_state14.var_18) : (0x317442ca));
                        rb_state14.var_21 = (rb_state14.var_21) - (((rb_state14.var_10) == (0x0)) ? (rb_state14.var_19) : (0x4ef9e295));
                        rb_state14.var_22 = (rb_state14.var_22) - (rb_state14.var_20);
                        rb_state14.var_23 = (rb_state14.var_23) ^ (((((0x59fb3970) + (rb_state14.var_11)) - (rb_state14.var_12)) + (rb_state14.var_21)) + (rb_state14.var_22));
                        rb_state14.var_3 = (rb_state14.var_3) ^ (rb_state14.var_23);
                        rb_state14.var_2 = rb_state14.var_3;
                        pthread_mutex_unlock(&(rb_state14.lock_31));
                    }
                    if ((rb_state14.var_1) == (0x1db624bf)) {
                        pthread_mutex_lock(&(rb_state14.lock_31));
                        rb_state14.var_32 = !((rb_state14.var_2) == (rb_state14.var_3));
                        pthread_mutex_unlock(&(rb_state14.lock_31));
                    }
                    if ((rb_state14.var_22) == (0xce8bbd36)) {
                        pthread_mutex_lock(&(rb_state14.lock_48));
                        rb_state14.var_35 = (rb_state14.var_35) + (((((0xb07c7372) - (0xac8eaa2a)) ^ (rb_state14.var_24)) ^ (rb_state14.var_23)) - (rb_state14.var_26));
                        rb_state14.var_36 = (rb_state14.var_36) + (((rb_state14.var_25) == (0x0)) ? (rb_state14.var_35) : (0x1300097));
                        rb_state14.var_34 = (rb_state14.var_34) - (rb_state14.var_36);
                        rb_state14.var_33 = rb_state14.var_34;
                        pthread_mutex_unlock(&(rb_state14.lock_48));
                    }
                    if ((rb_state14.var_22) == (0xce8bbd36)) {
                        if (!((rb_state14.var_33) == (rb_state14.var_34))) {
                            racebench_trigger(14);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    rb_state15.var_0 = (rb_state15.var_0) + (4749 < rb_input_size ? (uint32_t)rb_input[4749] : 0xd2da12ce);
                    rb_state15.var_1 = (rb_state15.var_1) - ((0x50086740) + (0x87571ff6));
                    rb_state15.var_2 = (rb_state15.var_2) + ((0x7ba4fe55) - (0x25edeeb9));
                    if ((rb_state15.var_1) == (0xfe792d22)) {
                        if ((rb_state15.var_3) != (0x0)) {
                            if (!((rb_state15.var_3) == (rb_state15.var_22))) {
                                racebench_trigger(15);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_16
                    rb_state16.var_2 = (rb_state16.var_2) ^ (((rb_state16.var_2) == (0x0)) ? (rb_state16.var_1) : (0x3ac563e));
                    rb_state16.var_1 = (rb_state16.var_1) + (11253 < rb_input_size ? (uint32_t)rb_input[11253] : 0x73ee297e);
                    rb_state16.var_2 = (rb_state16.var_2) ^ (((rb_state16.var_1) == (0x0)) ? (rb_state16.var_0) : (0xff6b438));
                    #endif
                    #ifdef RACEBENCH_BUG_17
                    rb_state17.var_0 = (rb_state17.var_0) ^ (rb_state17.var_0);
                    rb_state17.var_1 = (rb_state17.var_1) ^ (0x817b5474);
                    rb_state17.var_1 = (rb_state17.var_1) - (((rb_state17.var_1) == (0x817b5474)) ? (rb_state17.var_0) : (0x4deb3379));
                    if ((rb_state17.var_1) == (0x7653ef85)) {
                        rb_state17.var_11 = 0x4426168c;
                        rb_state17.var_12 = 0x8ccaa433;
                        rb_state17.var_13 = (rb_state17.var_13) ^ (((((0xb0b0319d) ^ (rb_state17.var_9)) ^ (rb_state17.var_7)) ^ (rb_state17.var_8)) - (rb_state17.var_12));
                        rb_state17.var_14 = (rb_state17.var_14) - (0x8791e749);
                        rb_state17.var_15 = (rb_state17.var_15) - (((rb_state17.var_10) == (0x0)) ? (rb_state17.var_11) : (0x72119129));
                        rb_state17.var_16 = (rb_state17.var_16) + (((((0x1a10a7c) + (rb_state17.var_11)) - (rb_state17.var_13)) - (0x4df51d15)) - (rb_state17.var_14));
                        rb_state17.var_17 = (rb_state17.var_17) + (((rb_state17.var_15) == (0x0)) ? (rb_state17.var_16) : (0x5865f60c));
                        rb_state17.var_2 = (rb_state17.var_2) - (rb_state17.var_17);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_2 = (rb_state18.var_2) - (0x34b96a28);
                    if ((rb_state18.var_1) == (0xe952f9ff)) {
                        pthread_mutex_lock(&(rb_state18.lock_17));
                        rb_state18.var_5 = 0x5593c52c;
                        rb_state18.var_6 = 0xb47b3170;
                        rb_state18.var_7 = (rb_state18.var_7) + (((0x7b70c218) - (rb_state18.var_3)) - (0x7d1c09dd));
                        rb_state18.var_8 = (rb_state18.var_8) - (((rb_state18.var_5) == (0x0)) ? (rb_state18.var_6) : (0xc80e30e8));
                        rb_state18.var_9 = (rb_state18.var_9) + (((rb_state18.var_4) == (0x0)) ? (rb_state18.var_7) : (0x2bb1817f));
                        rb_state18.var_10 = (rb_state18.var_10) - (((((0xc418088b) + (rb_state18.var_5)) ^ (rb_state18.var_8)) - (rb_state18.var_9)) + (rb_state18.var_6));
                        rb_state18.var_4 = (rb_state18.var_4) ^ (rb_state18.var_10);
                        rb_state18.var_3 = rb_state18.var_4;
                        pthread_mutex_unlock(&(rb_state18.lock_17));
                    }
                    if ((rb_state18.var_1) == (0xd9714138)) {
                        if (!((rb_state18.var_3) == (rb_state18.var_4))) {
                            racebench_trigger(18);
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
                    rb_state0.var_0 = (rb_state0.var_0) + (8497 < rb_input_size ? (uint32_t)rb_input[8497] : 0x61338d8a);
                    rb_state0.var_0 = (rb_state0.var_0) ^ (1305 < rb_input_size ? (uint32_t)rb_input[1305] : 0x4cac885c);
                    rb_state0.var_1 = (rb_state0.var_1) + ((0xfe1b5e3e) ^ (0x7a7428c));
                    rb_state0.var_1 = (rb_state0.var_1) - (0xeed5dae6);
                    rb_state0.var_0 = (rb_state0.var_0) + (((rb_state0.var_0) == (0x4f)) ? (rb_state0.var_1) : (0x8dad5d42));
                    rb_state0.var_1 = (rb_state0.var_1) ^ (5369 < rb_input_size ? (uint32_t)rb_input[5369] : 0x6954165e);
                    if ((rb_state0.var_0) == (0x89a8cd97)) {
                        pthread_mutex_lock(&(rb_state0.lock_20));
                        rb_state0.var_4 = 0xff457dfc;
                        rb_state0.var_5 = (rb_state0.var_5) - (((rb_state0.var_2) == (0x0)) ? (rb_state0.var_3) : (0x31906499));
                        rb_state0.var_6 = (rb_state0.var_6) + (0xd97b702a);
                        rb_state0.var_7 = (rb_state0.var_7) + (((0x9ff1ec18) ^ (rb_state0.var_4)) + (rb_state0.var_4));
                        rb_state0.var_8 = (rb_state0.var_8) - (((rb_state0.var_5) == (0x0)) ? (rb_state0.var_6) : (0x32fabd45));
                        rb_state0.var_9 = (rb_state0.var_9) + (((((0x604e0ade) - (rb_state0.var_8)) ^ (rb_state0.var_7)) + (rb_state0.var_6)) - (rb_state0.var_5));
                        rb_state0.var_3 = (rb_state0.var_3) + (rb_state0.var_9);
                        rb_state0.var_2 = rb_state0.var_3;
                        pthread_mutex_unlock(&(rb_state0.lock_20));
                    }
                    if ((rb_state0.var_0) == (0x17562be8)) {
                        rb_state0.var_10 = 0x27aeb6ee;
                        rb_state0.var_11 = 0xd471f810;
                        rb_state0.var_12 = (rb_state0.var_12) ^ (((rb_state0.var_8) == (0x0)) ? (rb_state0.var_10) : (0x84722773));
                        rb_state0.var_13 = (rb_state0.var_13) + (rb_state0.var_11);
                        rb_state0.var_14 = (rb_state0.var_14) ^ (((rb_state0.var_9) == (0x0)) ? (rb_state0.var_7) : (0x837f05b8));
                        rb_state0.var_15 = (rb_state0.var_15) - (rb_state0.var_12);
                        rb_state0.var_16 = (rb_state0.var_16) + (((rb_state0.var_13) == (0x0)) ? (rb_state0.var_14) : (0xa08ba8be));
                        rb_state0.var_17 = (rb_state0.var_17) - (rb_state0.var_15);
                        rb_state0.var_18 = (rb_state0.var_18) - (((rb_state0.var_10) == (0x0)) ? (rb_state0.var_16) : (0x62c25ff6));
                        rb_state0.var_19 = (rb_state0.var_19) + (((((0x6505485) + (rb_state0.var_17)) - (rb_state0.var_11)) ^ (rb_state0.var_12)) ^ (rb_state0.var_18));
                        rb_state0.var_2 = (rb_state0.var_2) ^ (rb_state0.var_19);
                    }
                    if ((rb_state0.var_0) == (0x17562be8)) {
                        pthread_mutex_lock(&(rb_state0.lock_20));
                        rb_state0.var_21 = !((rb_state0.var_2) == (rb_state0.var_3));
                        pthread_mutex_unlock(&(rb_state0.lock_20));
                    }
                    rb_state0.var_13 = (rb_state0.var_13) - (9208 < rb_input_size ? (uint32_t)rb_input[9208] : 0x3e9a3b8d);
                    rb_state0.var_14 = (rb_state0.var_14) + (rb_state0.var_15);
                    rb_state0.var_16 = (rb_state0.var_16) ^ (5786 < rb_input_size ? (uint32_t)rb_input[5786] : 0xbf18d796);
                    if ((rb_state0.var_17) == (0x8e44ee6)) {
                        rb_state0.var_22 = rb_state0.var_18;
                    }
                    if ((rb_state0.var_17) == (0x8e44ee6)) {
                        pthread_mutex_lock(&(rb_state0.lock_42));
                        rb_state0.var_23 = 0x118a6e12;
                        rb_state0.var_24 = 0xf818f92a;
                        rb_state0.var_25 = (rb_state0.var_25) + (rb_state0.var_21);
                        rb_state0.var_26 = (rb_state0.var_26) ^ (((rb_state0.var_23) == (0x0)) ? (rb_state0.var_22) : (0x7fb24534));
                        rb_state0.var_27 = (rb_state0.var_27) ^ (((0xa20efc61) ^ (rb_state0.var_24)) + (rb_state0.var_19));
                        rb_state0.var_28 = (rb_state0.var_28) ^ (((0x8375432) - (rb_state0.var_25)) ^ (rb_state0.var_23));
                        rb_state0.var_29 = (rb_state0.var_29) + (((rb_state0.var_24) == (0x0)) ? (rb_state0.var_25) : (0x4ddcf4ea));
                        rb_state0.var_30 = (rb_state0.var_30) - (rb_state0.var_26);
                        rb_state0.var_31 = (rb_state0.var_31) + (((rb_state0.var_27) == (0x0)) ? (rb_state0.var_28) : (0x4e791af3));
                        rb_state0.var_32 = (rb_state0.var_32) - (((rb_state0.var_26) == (0x0)) ? (rb_state0.var_29) : (0x171725b0));
                        rb_state0.var_33 = (rb_state0.var_33) + (((rb_state0.var_27) == (0x0)) ? (rb_state0.var_30) : (0x9b3ea3ad));
                        rb_state0.var_34 = (rb_state0.var_34) + (((rb_state0.var_31) == (0x0)) ? (rb_state0.var_32) : (0x77a73d16));
                        rb_state0.var_35 = (rb_state0.var_35) - (rb_state0.var_33);
                        rb_state0.var_36 = (rb_state0.var_36) ^ (((0x791fa960) ^ (0xdd54aee4)) + (rb_state0.var_34));
                        rb_state0.var_37 = (rb_state0.var_37) ^ (((0xf79e8ebf) ^ (rb_state0.var_28)) + (rb_state0.var_35));
                        rb_state0.var_38 = (rb_state0.var_38) + (((0xc755ba0) + (rb_state0.var_29)) + (rb_state0.var_36));
                        rb_state0.var_39 = (rb_state0.var_39) ^ (rb_state0.var_37);
                        rb_state0.var_40 = (rb_state0.var_40) + (((0xa313b2ac) + (rb_state0.var_38)) ^ (rb_state0.var_30));
                        rb_state0.var_41 = (rb_state0.var_41) ^ (((rb_state0.var_39) == (0x0)) ? (rb_state0.var_40) : (0xafce5059));
                        rb_state0.var_18 = (rb_state0.var_18) + (rb_state0.var_41);
                        pthread_mutex_unlock(&(rb_state0.lock_42));
                    }
                    if ((rb_state0.var_17) == (0x8e44ee6)) {
                        if (!((rb_state0.var_18) == (rb_state0.var_22))) {
                            racebench_trigger(0);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_1
                    rb_state1.var_0 = (rb_state1.var_0) - (0xb10773cf);
                    rb_state1.var_1 = (rb_state1.var_1) + (((rb_state1.var_1) == (0x0)) ? (rb_state1.var_0) : (0x2f153d81));
                    if ((rb_state1.var_0) == (0x3031e956)) {
                        rb_state1.var_14 = 0xb6dacdad;
                        rb_state1.var_15 = 0x53f047ec;
                        rb_state1.var_16 = (rb_state1.var_16) - (((0x4a7e14aa) + (0xeec0b73b)) + (rb_state1.var_14));
                        rb_state1.var_17 = (rb_state1.var_17) ^ (((rb_state1.var_11) == (0x0)) ? (rb_state1.var_10) : (0xab984253));
                        rb_state1.var_18 = (rb_state1.var_18) ^ (rb_state1.var_15);
                        rb_state1.var_19 = (rb_state1.var_19) - (((((0xd2a522d) + (rb_state1.var_9)) - (rb_state1.var_16)) ^ (rb_state1.var_13)) + (rb_state1.var_12));
                        rb_state1.var_20 = (rb_state1.var_20) - (((0x3e9d0c39) + (rb_state1.var_14)) - (rb_state1.var_17));
                        rb_state1.var_21 = (rb_state1.var_21) - (rb_state1.var_18);
                        rb_state1.var_22 = (rb_state1.var_22) + (((rb_state1.var_15) == (0x0)) ? (rb_state1.var_19) : (0xf45c7871));
                        rb_state1.var_23 = (rb_state1.var_23) ^ (((rb_state1.var_16) == (0x0)) ? (rb_state1.var_20) : (0xe72d6d0a));
                        rb_state1.var_24 = (rb_state1.var_24) ^ (((rb_state1.var_21) == (0x0)) ? (rb_state1.var_22) : (0x93342baf));
                        rb_state1.var_25 = (rb_state1.var_25) ^ (((((0x6c6c89cd) - (rb_state1.var_24)) ^ (rb_state1.var_18)) ^ (rb_state1.var_17)) ^ (rb_state1.var_23));
                        rb_state1.var_2 = (rb_state1.var_2) - (rb_state1.var_25);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_2
                    rb_state2.var_0 = (rb_state2.var_0) - ((0xf9365c63) ^ (0x112773b4));
                    rb_state2.var_0 = (rb_state2.var_0) - (4876 < rb_input_size ? (uint32_t)rb_input[4876] : 0xdee8f4f7);
                    rb_state2.var_1 = (rb_state2.var_1) ^ (5693 < rb_input_size ? (uint32_t)rb_input[5693] : 0x5c3c09c7);
                    rb_state2.var_2 = (rb_state2.var_2) ^ (7434 < rb_input_size ? (uint32_t)rb_input[7434] : 0x322ebf5b);
                    rb_state2.var_2 = (rb_state2.var_2) ^ (169 < rb_input_size ? (uint32_t)rb_input[169] : 0x6a566c66);
                    if ((rb_state2.var_0) == (0x29c98f4a)) {
                        pthread_mutex_lock(&(rb_state2.lock_16));
                        rb_state2.var_8 = 0x247699c4;
                        rb_state2.var_9 = 0x965df363;
                        rb_state2.var_10 = (rb_state2.var_10) + (((rb_state2.var_6) == (0x0)) ? (rb_state2.var_9) : (0xdff64d60));
                        rb_state2.var_11 = (rb_state2.var_11) + (((rb_state2.var_7) == (0x0)) ? (rb_state2.var_8) : (0x21dfd32d));
                        rb_state2.var_12 = (rb_state2.var_12) + (((0x4a2653b6) - (rb_state2.var_10)) + (0xc0d196cb));
                        rb_state2.var_13 = (rb_state2.var_13) - (rb_state2.var_11);
                        rb_state2.var_14 = (rb_state2.var_14) - (rb_state2.var_12);
                        rb_state2.var_15 = (rb_state2.var_15) + (((rb_state2.var_13) == (0x0)) ? (rb_state2.var_14) : (0x83e4950b));
                        rb_state2.var_3 = (rb_state2.var_3) ^ (rb_state2.var_15);
                        pthread_mutex_unlock(&(rb_state2.lock_16));
                    }
                    rb_state2.var_10 = (rb_state2.var_10) + ((0x241ce667) + (rb_state2.var_8));
                    if ((rb_state2.var_14) == (0xe78eabc0)) {
                        rb_state2.var_25 = (rb_state2.var_25) ^ (0x557d69da);
                        rb_state2.var_26 = (rb_state2.var_26) + (0x7c849c9f);
                        rb_state2.var_27 = (rb_state2.var_27) + (((rb_state2.var_24) == (0x0)) ? (rb_state2.var_22) : (0xd70d83f0));
                        rb_state2.var_28 = (rb_state2.var_28) + (rb_state2.var_21);
                        rb_state2.var_29 = (rb_state2.var_29) + (((rb_state2.var_23) == (0x0)) ? (rb_state2.var_25) : (0xa9ab2786));
                        rb_state2.var_30 = (rb_state2.var_30) - (((((0xc1b57216) ^ (0x7853f920)) ^ (rb_state2.var_25)) ^ (rb_state2.var_26)) ^ (rb_state2.var_27));
                        rb_state2.var_31 = (rb_state2.var_31) ^ (rb_state2.var_28);
                        rb_state2.var_32 = (rb_state2.var_32) ^ (rb_state2.var_29);
                        rb_state2.var_33 = (rb_state2.var_33) ^ (((((0x8b7f559f) ^ (rb_state2.var_31)) - (rb_state2.var_30)) - (rb_state2.var_26)) + (rb_state2.var_27));
                        rb_state2.var_34 = (rb_state2.var_34) - (((rb_state2.var_28) == (0x0)) ? (rb_state2.var_32) : (0x9e4d24cd));
                        rb_state2.var_35 = (rb_state2.var_35) - (((0x841ed06d) ^ (rb_state2.var_33)) - (0xd9edcb12));
                        rb_state2.var_36 = (rb_state2.var_36) + (((0x136e35ac) - (0xf4e4de9d)) ^ (rb_state2.var_34));
                        rb_state2.var_37 = (rb_state2.var_37) ^ (((0x631837ac) - (rb_state2.var_29)) + (rb_state2.var_35));
                        rb_state2.var_38 = (rb_state2.var_38) + (((((0xc0278492) - (rb_state2.var_36)) - (rb_state2.var_37)) + (rb_state2.var_30)) + (0x3eb0756c));
                        rb_state2.var_24 = (rb_state2.var_24) ^ (rb_state2.var_38);
                        rb_state2.var_18 = rb_state2.var_24;
                    }
                    if ((rb_state2.var_14) == (0xe78eabc0)) {
                        pthread_mutex_lock(&(rb_state2.lock_48));
                        rb_state2.var_40 = 0x4dcaa197;
                        rb_state2.var_41 = 0x31ebe0c7;
                        rb_state2.var_42 = (rb_state2.var_42) + (((rb_state2.var_33) == (0x0)) ? (rb_state2.var_32) : (0x387ce140));
                        rb_state2.var_43 = (rb_state2.var_43) + (((rb_state2.var_35) == (0x0)) ? (rb_state2.var_34) : (0x7288a01c));
                        rb_state2.var_44 = (rb_state2.var_44) - (((((0x8c1efb2) ^ (rb_state2.var_41)) + (0x402d394c)) - (rb_state2.var_40)) + (0x4955462e));
                        rb_state2.var_45 = (rb_state2.var_45) + (((((0xbf436b8f) ^ (rb_state2.var_31)) ^ (rb_state2.var_42)) ^ (0x8be8022d)) - (0xfcc36e38));
                        rb_state2.var_46 = (rb_state2.var_46) ^ (((rb_state2.var_43) == (0x0)) ? (rb_state2.var_44) : (0xc5a5f84f));
                        rb_state2.var_47 = (rb_state2.var_47) + (((rb_state2.var_45) == (0x0)) ? (rb_state2.var_46) : (0x4dcc6ebb));
                        rb_state2.var_39 = (rb_state2.var_39) - (rb_state2.var_47);
                        rb_state2.var_18 = rb_state2.var_39;
                        pthread_mutex_unlock(&(rb_state2.lock_48));
                    }
                    if ((rb_state2.var_40) == (0x4dcaa197)) {
                        rb_state2.var_66 = 0x62aefddb;
                        rb_state2.var_67 = 0xaa63afda;
                        rb_state2.var_68 = (rb_state2.var_68) ^ (13518 < rb_input_size ? (uint32_t)rb_input[13518] : 0x5a3ec147);
                        rb_state2.var_69 = (rb_state2.var_69) + (((rb_state2.var_50) == (0x0)) ? (rb_state2.var_66) : (0x516a7e36));
                        rb_state2.var_70 = (rb_state2.var_70) + (((rb_state2.var_51) == (0x0)) ? (rb_state2.var_49) : (0x3c0f9e96));
                        rb_state2.var_71 = (rb_state2.var_71) + (((rb_state2.var_67) == (0x0)) ? (rb_state2.var_68) : (0xa5e96237));
                        rb_state2.var_72 = (rb_state2.var_72) - (rb_state2.var_69);
                        rb_state2.var_73 = (rb_state2.var_73) ^ (rb_state2.var_70);
                        rb_state2.var_74 = (rb_state2.var_74) - (((rb_state2.var_71) == (0x0)) ? (rb_state2.var_72) : (0x40a64eb4));
                        rb_state2.var_75 = (rb_state2.var_75) - (rb_state2.var_73);
                        rb_state2.var_76 = (rb_state2.var_76) ^ (((rb_state2.var_74) == (0x0)) ? (rb_state2.var_75) : (0xfb2b4f14));
                        rb_state2.var_50 = (rb_state2.var_50) + (rb_state2.var_76);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_3
                    rb_state3.var_0 = (rb_state3.var_0) + (((rb_state3.var_0) == (0x0)) ? (rb_state3.var_0) : (0xba913ff7));
                    rb_state3.var_0 = (rb_state3.var_0) - (0xab1d374a);
                    rb_state3.var_1 = (rb_state3.var_1) + (8814 < rb_input_size ? (uint32_t)rb_input[8814] : 0x9a772b73);
                    rb_state3.var_2 = (rb_state3.var_2) ^ ((0x8aa64189) + (rb_state3.var_1));
                    rb_state3.var_2 = (rb_state3.var_2) - (0x13a37e9);
                    rb_state3.var_1 = (rb_state3.var_1) + (((rb_state3.var_1) == (0xf6)) ? (rb_state3.var_2) : (0xdd106c75));
                    if ((rb_state3.var_2) == (0xc1c37c84)) {
                        pthread_mutex_lock(&(rb_state3.lock_25));
                        rb_state3.var_5 = 0x5bded856;
                        rb_state3.var_6 = 0x5373f0de;
                        rb_state3.var_7 = 0x2b195b0e;
                        rb_state3.var_8 = (rb_state3.var_8) + (0xcb7cd40e);
                        rb_state3.var_9 = (rb_state3.var_9) + (rb_state3.var_6);
                        rb_state3.var_10 = (rb_state3.var_10) + (rb_state3.var_7);
                        rb_state3.var_11 = (rb_state3.var_11) - (((rb_state3.var_3) == (0x0)) ? (rb_state3.var_5) : (0x3cd8ccc3));
                        rb_state3.var_12 = (rb_state3.var_12) ^ (((((0x1f0250b6) + (rb_state3.var_9)) ^ (rb_state3.var_8)) + (rb_state3.var_4)) - (rb_state3.var_5));
                        rb_state3.var_13 = (rb_state3.var_13) + (((((0x11446c49) + (rb_state3.var_10)) - (rb_state3.var_11)) - (rb_state3.var_6)) + (0x3e86a263));
                        rb_state3.var_14 = (rb_state3.var_14) ^ (rb_state3.var_12);
                        rb_state3.var_15 = (rb_state3.var_15) + (((rb_state3.var_13) == (0x0)) ? (rb_state3.var_14) : (0x506755fe));
                        rb_state3.var_4 = (rb_state3.var_4) + (rb_state3.var_15);
                        rb_state3.var_3 = rb_state3.var_4;
                        pthread_mutex_unlock(&(rb_state3.lock_25));
                    }
                    if ((rb_state3.var_2) == (0x87f1f475)) {
                        rb_state3.var_16 = 0x19b22dac;
                        rb_state3.var_17 = 0xc8d53216;
                        rb_state3.var_18 = (rb_state3.var_18) - (((0x4c1b0ce2) + (rb_state3.var_7)) - (rb_state3.var_8));
                        rb_state3.var_19 = (rb_state3.var_19) ^ (0x3f316602);
                        rb_state3.var_20 = (rb_state3.var_20) - (((0xfd521251) ^ (rb_state3.var_16)) ^ (0x3e8414ca));
                        rb_state3.var_21 = (rb_state3.var_21) - (((rb_state3.var_17) == (0x0)) ? (rb_state3.var_18) : (0x8ea1f66f));
                        rb_state3.var_22 = (rb_state3.var_22) ^ (rb_state3.var_19);
                        rb_state3.var_23 = (rb_state3.var_23) + (((((0xee39cce1) + (rb_state3.var_21)) + (0xa0beb40c)) - (rb_state3.var_9)) + (rb_state3.var_20));
                        rb_state3.var_24 = (rb_state3.var_24) - (((rb_state3.var_22) == (0x0)) ? (rb_state3.var_23) : (0x112fd90b));
                        rb_state3.var_3 = (rb_state3.var_3) - (rb_state3.var_24);
                    }
                    if ((rb_state3.var_2) == (0x87f1f475)) {
                        rb_state3.var_26 = !((rb_state3.var_3) == (rb_state3.var_4));
                    }
                    rb_state3.var_10 = (rb_state3.var_10) + (8033 < rb_input_size ? (uint32_t)rb_input[8033] : 0x53c53f69);
                    rb_state3.var_11 = (rb_state3.var_11) - (((rb_state3.var_13) == (0x0)) ? (rb_state3.var_12) : (0x4b30e546));
                    if ((rb_state3.var_15) == (0x506755fe)) {
                        pthread_mutex_lock(&(rb_state3.lock_68));
                        rb_state3.var_43 = 0x2e818b0f;
                        rb_state3.var_44 = (rb_state3.var_44) - (((rb_state3.var_26) == (0x0)) ? (rb_state3.var_23) : (0xecf707d1));
                        rb_state3.var_45 = (rb_state3.var_45) + (((rb_state3.var_43) == (0x0)) ? (rb_state3.var_24) : (0x6e3e2402));
                        rb_state3.var_46 = (rb_state3.var_46) ^ (((((0x52890079) ^ (0xc99f284f)) - (rb_state3.var_44)) - (rb_state3.var_45)) - (0xc52b2caa));
                        rb_state3.var_42 = (rb_state3.var_42) ^ (rb_state3.var_46);
                        rb_state3.var_27 = rb_state3.var_42;
                        pthread_mutex_unlock(&(rb_state3.lock_68));
                    }
                    if ((rb_state3.var_15) == (0x506755fe)) {
                        if ((rb_state3.var_27) != (0x0)) {
                            if (!((rb_state3.var_27) == (rb_state3.var_47))) {
                                racebench_trigger(3);
                            }
                        }
                    }
                    if ((rb_state3.var_15) == (0x506755fe)) {
                        pthread_mutex_lock(&(rb_state3.lock_68));
                        rb_state3.var_48 = 0xcb56a96;
                        rb_state3.var_49 = (rb_state3.var_49) - (((0xb90a5d8f) + (0x28260bf0)) - (rb_state3.var_27));
                        rb_state3.var_50 = (rb_state3.var_50) - (((0x3be6c8e) - (rb_state3.var_28)) + (rb_state3.var_29));
                        rb_state3.var_51 = (rb_state3.var_51) + (((rb_state3.var_31) == (0x0)) ? (rb_state3.var_30) : (0x3e85b067));
                        rb_state3.var_52 = (rb_state3.var_52) - (((rb_state3.var_32) == (0x0)) ? (rb_state3.var_48) : (0x30cb2cfb));
                        rb_state3.var_53 = (rb_state3.var_53) + (((0xc3f4289f) ^ (rb_state3.var_33)) + (rb_state3.var_49));
                        rb_state3.var_54 = (rb_state3.var_54) + (((0xd7ea24fa) + (rb_state3.var_50)) - (rb_state3.var_34));
                        rb_state3.var_55 = (rb_state3.var_55) - (((0x608b68a7) - (0x98b7401d)) + (rb_state3.var_51));
                        rb_state3.var_56 = (rb_state3.var_56) - (((((0xb402239b) ^ (rb_state3.var_52)) - (0xe77e7a06)) ^ (rb_state3.var_53)) - (0x6d995546));
                        rb_state3.var_57 = (rb_state3.var_57) - (((0xa1231af6) ^ (rb_state3.var_54)) - (rb_state3.var_35));
                        rb_state3.var_58 = (rb_state3.var_58) ^ (((0x2854154d) - (0xc7392fa3)) + (rb_state3.var_55));
                        rb_state3.var_59 = (rb_state3.var_59) ^ (rb_state3.var_56);
                        rb_state3.var_60 = (rb_state3.var_60) - (rb_state3.var_57);
                        rb_state3.var_61 = (rb_state3.var_61) ^ (((rb_state3.var_36) == (0x0)) ? (rb_state3.var_58) : (0xa753efaf));
                        rb_state3.var_62 = (rb_state3.var_62) - (((0x88ad6568) ^ (0x87311d8)) ^ (rb_state3.var_59));
                        rb_state3.var_63 = (rb_state3.var_63) + (((rb_state3.var_37) == (0x0)) ? (rb_state3.var_60) : (0x57f9fcc0));
                        rb_state3.var_64 = (rb_state3.var_64) ^ (((((0x1334d14) - (rb_state3.var_62)) - (0x22a598c2)) - (rb_state3.var_61)) + (rb_state3.var_38));
                        rb_state3.var_65 = (rb_state3.var_65) + (((0x1ae1820d) - (0x15872f6a)) + (rb_state3.var_63));
                        rb_state3.var_66 = (rb_state3.var_66) - (((0xf3b48a37) ^ (rb_state3.var_39)) - (rb_state3.var_64));
                        rb_state3.var_67 = (rb_state3.var_67) ^ (((rb_state3.var_65) == (0x0)) ? (rb_state3.var_66) : (0x46fc5820));
                        rb_state3.var_47 = (rb_state3.var_47) - (rb_state3.var_67);
                        rb_state3.var_27 = rb_state3.var_47;
                        pthread_mutex_unlock(&(rb_state3.lock_68));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_4
                    rb_state4.var_2 = (rb_state4.var_2) + ((0x9587127) + (0xab7aa262));
                    rb_state4.var_2 = (rb_state4.var_2) - (((rb_state4.var_2) == (0xb4d31389)) ? (rb_state4.var_1) : (0x6b31f0a3));
                    rb_state4.var_0 = (rb_state4.var_0) + (rb_state4.var_1);
                    if ((rb_state4.var_2) == (0xccaac18f)) {
                        rb_state4.var_3 = rb_state4.var_0;
                    }
                    if ((rb_state4.var_2) == (0xccaac18f)) {
                        if (!((rb_state4.var_0) == (rb_state4.var_3))) {
                            racebench_trigger(4);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_5
                    rb_state5.var_0 = (rb_state5.var_0) - (0x9b5e6c30);
                    rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_1) == (0x0)) ? (rb_state5.var_0) : (0x20a5f50f));
                    rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_2);
                    rb_state5.var_0 = (rb_state5.var_0) - ((0x227898af) ^ (0x9464ee35));
                    rb_state5.var_1 = (rb_state5.var_1) ^ (((rb_state5.var_0) == (0xae851d36)) ? (rb_state5.var_2) : (0x27145f54));
                    rb_state5.var_1 = (rb_state5.var_1) - (((rb_state5.var_0) == (0xae851d36)) ? (rb_state5.var_2) : (0x7be947c2));
                    if ((rb_state5.var_1) == (0x38d813d4)) {
                        rb_state5.var_3 = rb_state5.var_2;
                    }
                    if ((rb_state5.var_1) == (0xc38071cd)) {
                        pthread_mutex_lock(&(rb_state5.lock_11));
                        rb_state5.var_4 = 0x9f4d72a;
                        rb_state5.var_5 = 0x767df8fa;
                        rb_state5.var_6 = (rb_state5.var_6) ^ (rb_state5.var_4);
                        rb_state5.var_7 = (rb_state5.var_7) + (((0x12eca96f) + (0xc86e56c6)) ^ (rb_state5.var_5));
                        rb_state5.var_8 = (rb_state5.var_8) + (((0x8c0b5c3c) ^ (0xb9cb09af)) ^ (rb_state5.var_3));
                        rb_state5.var_9 = (rb_state5.var_9) - (((rb_state5.var_6) == (0x0)) ? (rb_state5.var_7) : (0x762151c6));
                        rb_state5.var_10 = (rb_state5.var_10) + (((rb_state5.var_8) == (0x0)) ? (rb_state5.var_9) : (0x1c8fa803));
                        rb_state5.var_2 = (rb_state5.var_2) + (rb_state5.var_10);
                        pthread_mutex_unlock(&(rb_state5.lock_11));
                    }
                    if ((rb_state5.var_1) == (0xc38071cd)) {
                        if (!((rb_state5.var_2) == (rb_state5.var_3))) {
                            racebench_trigger(5);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_6
                    rb_state6.var_0 = (rb_state6.var_0) ^ (3538 < rb_input_size ? (uint32_t)rb_input[3538] : 0xaf3ccdcf);
                    rb_state6.var_1 = (rb_state6.var_1) + (0x952157a8);
                    rb_state6.var_0 = (rb_state6.var_0) + (0x1c8e4e30);
                    rb_state6.var_1 = (rb_state6.var_1) - (rb_state6.var_0);
                    rb_state6.var_1 = (rb_state6.var_1) ^ (0x268b3e11);
                    rb_state6.var_0 = (rb_state6.var_0) - (0x5f74b8cb);
                    if ((rb_state6.var_1) == (0xbd40b301)) {
                        pthread_mutex_lock(&(rb_state6.lock_3));
                        rb_state6.var_2 = rb_state6.var_0;
                        pthread_mutex_unlock(&(rb_state6.lock_3));
                    }
                    if ((rb_state6.var_1) == (0xbd40b301)) {
                        if (!((rb_state6.var_0) == (rb_state6.var_2))) {
                            racebench_trigger(6);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_7
                    rb_state7.var_0 = (rb_state7.var_0) ^ ((0x38c4ddec) - (rb_state7.var_0));
                    rb_state7.var_0 = (rb_state7.var_0) - ((0xcf38a369) - (rb_state7.var_0));
                    rb_state7.var_1 = (rb_state7.var_1) ^ ((0x718961a4) ^ (0x9cae4e95));
                    rb_state7.var_1 = (rb_state7.var_1) - (((rb_state7.var_1) == (0xed272f31)) ? (rb_state7.var_1) : (0x1a055bb3));
                    rb_state7.var_2 = (rb_state7.var_2) - (2417 < rb_input_size ? (uint32_t)rb_input[2417] : 0x807902cd);
                    if ((rb_state7.var_2) == (0x26b11973)) {
                        rb_state7.var_13 = 0x6d5f18c;
                        rb_state7.var_14 = 0xd65cbb31;
                        rb_state7.var_15 = (rb_state7.var_15) - ((0x12878e94) - (rb_state7.var_7));
                        rb_state7.var_16 = (rb_state7.var_16) + (((rb_state7.var_14) == (0x0)) ? (rb_state7.var_6) : (0xed06a15f));
                        rb_state7.var_17 = (rb_state7.var_17) ^ (((((0x1e3be1c4) + (rb_state7.var_13)) - (rb_state7.var_15)) + (0x8be84d9e)) ^ (0x59592ebe));
                        rb_state7.var_18 = (rb_state7.var_18) - (((rb_state7.var_8) == (0x0)) ? (rb_state7.var_16) : (0xc1f5f293));
                        rb_state7.var_19 = (rb_state7.var_19) ^ (((rb_state7.var_17) == (0x0)) ? (rb_state7.var_18) : (0x71440b34));
                        rb_state7.var_12 = (rb_state7.var_12) + (rb_state7.var_19);
                        rb_state7.var_3 = rb_state7.var_12;
                    }
                    if ((rb_state7.var_2) == (0x26b11973)) {
                        pthread_mutex_lock(&(rb_state7.lock_31));
                        if ((rb_state7.var_3) != (0x0)) {
                            if (!((rb_state7.var_3) == (rb_state7.var_20))) {
                                racebench_trigger(7);
                            }
                        }
                        pthread_mutex_unlock(&(rb_state7.lock_31));
                    }
                    if ((rb_state7.var_2) == (0x26b11973)) {
                        pthread_mutex_lock(&(rb_state7.lock_31));
                        rb_state7.var_21 = 0x709e72b2;
                        rb_state7.var_22 = (rb_state7.var_22) + ((0x4393a959) + (0xdad3add));
                        rb_state7.var_23 = (rb_state7.var_23) + (((((0x596bf200) - (rb_state7.var_10)) - (0x45cbf8c4)) ^ (rb_state7.var_21)) ^ (rb_state7.var_11));
                        rb_state7.var_24 = (rb_state7.var_24) ^ (((0x4f692a86) ^ (rb_state7.var_12)) - (rb_state7.var_9));
                        rb_state7.var_25 = (rb_state7.var_25) + (((rb_state7.var_13) == (0x0)) ? (rb_state7.var_22) : (0x5b6e19d));
                        rb_state7.var_26 = (rb_state7.var_26) ^ (((rb_state7.var_23) == (0x0)) ? (rb_state7.var_24) : (0xcbdbc1c0));
                        rb_state7.var_27 = (rb_state7.var_27) ^ (((0x9c5c3aa6) ^ (rb_state7.var_25)) ^ (rb_state7.var_14));
                        rb_state7.var_28 = (rb_state7.var_28) ^ (rb_state7.var_26);
                        rb_state7.var_29 = (rb_state7.var_29) + (rb_state7.var_27);
                        rb_state7.var_30 = (rb_state7.var_30) + (((rb_state7.var_28) == (0x0)) ? (rb_state7.var_29) : (0xcd19f09b));
                        rb_state7.var_20 = (rb_state7.var_20) ^ (rb_state7.var_30);
                        rb_state7.var_3 = rb_state7.var_20;
                        pthread_mutex_unlock(&(rb_state7.lock_31));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_8
                    rb_state8.var_0 = (rb_state8.var_0) ^ (0x6c37ec4c);
                    rb_state8.var_1 = (rb_state8.var_1) - (0x8e157fb0);
                    rb_state8.var_0 = (rb_state8.var_0) ^ (0xd2d7f5aa);
                    rb_state8.var_1 = (rb_state8.var_1) - (rb_state8.var_0);
                    rb_state8.var_1 = (rb_state8.var_1) + ((0x8122de59) + (0x125e8f22));
                    rb_state8.var_0 = (rb_state8.var_0) + (((rb_state8.var_0) == (0xbee019e6)) ? (rb_state8.var_1) : (0xde124ce1));
                    if ((rb_state8.var_1) == (0x102f0cf7)) {
                        rb_state8.var_14 = 0x14d11938;
                        rb_state8.var_15 = (rb_state8.var_15) + (((rb_state8.var_5) == (0x0)) ? (rb_state8.var_6) : (0x2242e11e));
                        rb_state8.var_16 = (rb_state8.var_16) ^ (((rb_state8.var_7) == (0x0)) ? (rb_state8.var_14) : (0xdbfbd8fb));
                        rb_state8.var_17 = (rb_state8.var_17) - (((rb_state8.var_8) == (0x0)) ? (rb_state8.var_15) : (0xde86b1c7));
                        rb_state8.var_18 = (rb_state8.var_18) - (((0x70b8d333) ^ (rb_state8.var_16)) + (0x6ebf9776));
                        rb_state8.var_19 = (rb_state8.var_19) - (((rb_state8.var_17) == (0x0)) ? (rb_state8.var_18) : (0x967a62c7));
                        rb_state8.var_13 = (rb_state8.var_13) ^ (rb_state8.var_19);
                        rb_state8.var_2 = rb_state8.var_13;
                    }
                    if ((rb_state8.var_1) == (0x96ebd925)) {
                        if ((rb_state8.var_2) != (0x0)) {
                            if (!((rb_state8.var_2) == (rb_state8.var_20))) {
                                racebench_trigger(8);
                            }
                        }
                    }
                    if ((rb_state8.var_1) == (0x96ebd925)) {
                        pthread_mutex_lock(&(rb_state8.lock_27));
                        rb_state8.var_21 = 0x755bdf68;
                        rb_state8.var_22 = 0x2ee2a9db;
                        rb_state8.var_23 = (rb_state8.var_23) + (((rb_state8.var_21) == (0x0)) ? (rb_state8.var_9) : (0xcb358498));
                        rb_state8.var_24 = (rb_state8.var_24) ^ (((0xa1be0daf) - (rb_state8.var_22)) - (0xa0a5b018));
                        rb_state8.var_25 = (rb_state8.var_25) ^ (((0xf8d1856a) ^ (rb_state8.var_10)) - (rb_state8.var_23));
                        rb_state8.var_26 = (rb_state8.var_26) ^ (((rb_state8.var_24) == (0x0)) ? (rb_state8.var_25) : (0x7e75ad88));
                        rb_state8.var_20 = (rb_state8.var_20) ^ (rb_state8.var_26);
                        rb_state8.var_2 = rb_state8.var_20;
                        pthread_mutex_unlock(&(rb_state8.lock_27));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_9
                    rb_state9.var_0 = (rb_state9.var_0) + ((0xd01844d) - (0x5ab9f5b9));
                    rb_state9.var_2 = (rb_state9.var_2) - (0x8065d3f1);
                    rb_state9.var_0 = (rb_state9.var_0) - (3301 < rb_input_size ? (uint32_t)rb_input[3301] : 0x88ad97ff);
                    rb_state9.var_1 = (rb_state9.var_1) - ((0xbe8364e5) + (0xf74909e9));
                    rb_state9.var_2 = (rb_state9.var_2) + (rb_state9.var_0);
                    if ((rb_state9.var_1) == (0x5ccb5374)) {
                        rb_state9.var_10 = 0xd16a81e7;
                        rb_state9.var_11 = 0xf3454042;
                        rb_state9.var_12 = (rb_state9.var_12) + (((rb_state9.var_3) == (0x0)) ? (rb_state9.var_10) : (0xbc30d6a2));
                        rb_state9.var_13 = (rb_state9.var_13) ^ (((rb_state9.var_11) == (0x0)) ? (rb_state9.var_12) : (0x6ee581cf));
                        rb_state9.var_9 = (rb_state9.var_9) - (rb_state9.var_13);
                        rb_state9.var_3 = rb_state9.var_9;
                    }
                    if ((rb_state9.var_1) == (0xa6fee4a6)) {
                        if ((rb_state9.var_3) != (0x0)) {
                            if (!((rb_state9.var_3) == (rb_state9.var_14))) {
                                rb_state9.var_20 = rb_state9.var_7;
                            }
                            if (!((rb_state9.var_3) == (rb_state9.var_14))) {
                                if (!((rb_state9.var_7) == (rb_state9.var_20))) {
                                    racebench_trigger(9);
                                }
                            }
                        }
                    }
                    if ((rb_state9.var_1) == (0xa6fee4a6)) {
                        pthread_mutex_lock(&(rb_state9.lock_19));
                        rb_state9.var_15 = 0xfcd2371f;
                        rb_state9.var_16 = 0xfc8d8066;
                        rb_state9.var_17 = (rb_state9.var_17) - (((((0x296feabd) - (0xc911ec13)) ^ (rb_state9.var_4)) + (0xf15d0f4c)) - (rb_state9.var_15));
                        rb_state9.var_18 = (rb_state9.var_18) - (((((0x90b99891) + (rb_state9.var_5)) ^ (rb_state9.var_16)) - (rb_state9.var_17)) + (rb_state9.var_6));
                        rb_state9.var_14 = (rb_state9.var_14) + (rb_state9.var_18);
                        rb_state9.var_3 = rb_state9.var_14;
                        pthread_mutex_unlock(&(rb_state9.lock_19));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_10
                    rb_state10.var_1 = (rb_state10.var_1) - ((0x616c740e) - (0x680aa7d9));
                    rb_state10.var_2 = (rb_state10.var_2) ^ (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_2) : (0xd615d900));
                    rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_2) : (0xc72d791e));
                    rb_state10.var_1 = (rb_state10.var_1) + (((rb_state10.var_0) == (0x0)) ? (rb_state10.var_2) : (0xcfdfb09e));
                    if ((rb_state10.var_1) == (0xa1acaea3)) {
                        rb_state10.var_3 = rb_state10.var_2;
                    }
                    if ((rb_state10.var_1) == (0xa1acaea3)) {
                        if (!((rb_state10.var_2) == (rb_state10.var_3))) {
                            racebench_trigger(10);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_11
                    rb_state11.var_1 = (rb_state11.var_1) - (rb_state11.var_0);
                    rb_state11.var_2 = (rb_state11.var_2) + (5741 < rb_input_size ? (uint32_t)rb_input[5741] : 0x1587610d);
                    rb_state11.var_2 = (rb_state11.var_2) - (((rb_state11.var_1) == (0x5f4a5412)) ? (rb_state11.var_0) : (0xa5cf60a6));
                    if ((rb_state11.var_1) == (0x488df450)) {
                        pthread_mutex_lock(&(rb_state11.lock_21));
                        rb_state11.var_5 = 0x59c2994b;
                        rb_state11.var_6 = 0xdd28329b;
                        rb_state11.var_7 = 0x9c1f59b1;
                        rb_state11.var_8 = (rb_state11.var_8) + ((0x4bef2de3) ^ (0xad933bd7));
                        rb_state11.var_9 = (rb_state11.var_9) - (((((0xeda33055) ^ (0xbb99bf2a)) ^ (rb_state11.var_5)) - (rb_state11.var_6)) - (0xad589530));
                        rb_state11.var_10 = (rb_state11.var_10) ^ (((rb_state11.var_7) == (0x0)) ? (rb_state11.var_8) : (0xa786231b));
                        rb_state11.var_11 = (rb_state11.var_11) - (((((0x75674cdf) - (0xfcce27a7)) ^ (rb_state11.var_10)) ^ (rb_state11.var_9)) ^ (rb_state11.var_3));
                        rb_state11.var_4 = (rb_state11.var_4) + (rb_state11.var_11);
                        rb_state11.var_3 = rb_state11.var_4;
                        pthread_mutex_unlock(&(rb_state11.lock_21));
                    }
                    if ((rb_state11.var_1) == (0x6421a805)) {
                        if (!((rb_state11.var_3) == (rb_state11.var_4))) {
                            racebench_trigger(11);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_12
                    rb_state12.var_1 = (rb_state12.var_1) - (4228 < rb_input_size ? (uint32_t)rb_input[4228] : 0xc8562311);
                    rb_state12.var_0 = (rb_state12.var_0) - (rb_state12.var_1);
                    rb_state12.var_0 = (rb_state12.var_0) ^ (((rb_state12.var_0) == (0x9dc86717)) ? (rb_state12.var_1) : (0x47b9ccd9));
                    if ((rb_state12.var_1) == (0xffed57eb)) {
                        pthread_mutex_lock(&(rb_state12.lock_47));
                        rb_state12.var_16 = 0xbd7edb46;
                        rb_state12.var_17 = (rb_state12.var_17) ^ (((0x9914a275) + (rb_state12.var_16)) - (0xdfbc44b));
                        rb_state12.var_18 = (rb_state12.var_18) - (((rb_state12.var_10) == (0x0)) ? (rb_state12.var_8) : (0xcdcbb03));
                        rb_state12.var_19 = (rb_state12.var_19) ^ (0xc220db8);
                        rb_state12.var_20 = (rb_state12.var_20) + (((((0x174c6467) + (rb_state12.var_11)) + (rb_state12.var_17)) ^ (rb_state12.var_12)) ^ (rb_state12.var_9));
                        rb_state12.var_21 = (rb_state12.var_21) - (((rb_state12.var_13) == (0x0)) ? (rb_state12.var_18) : (0x8c0a4d65));
                        rb_state12.var_22 = (rb_state12.var_22) - (((rb_state12.var_14) == (0x0)) ? (rb_state12.var_19) : (0xbbf988c));
                        rb_state12.var_23 = (rb_state12.var_23) + (rb_state12.var_20);
                        rb_state12.var_24 = (rb_state12.var_24) ^ (((((0x551e71a6) + (rb_state12.var_21)) + (rb_state12.var_22)) - (rb_state12.var_15)) ^ (0x54fa97ba));
                        rb_state12.var_25 = (rb_state12.var_25) ^ (rb_state12.var_23);
                        rb_state12.var_26 = (rb_state12.var_26) - (((rb_state12.var_24) == (0x0)) ? (rb_state12.var_25) : (0xedf934ab));
                        rb_state12.var_15 = (rb_state12.var_15) - (rb_state12.var_26);
                        rb_state12.var_2 = rb_state12.var_15;
                        pthread_mutex_unlock(&(rb_state12.lock_47));
                    }
                    if ((rb_state12.var_1) == (0xffed56ec)) {
                        pthread_mutex_lock(&(rb_state12.lock_47));
                        rb_state12.var_28 = 0xe5137937;
                        rb_state12.var_29 = 0x492c0620;
                        rb_state12.var_30 = 0x2ef61c88;
                        rb_state12.var_31 = (rb_state12.var_31) + (((rb_state12.var_17) == (0x0)) ? (rb_state12.var_16) : (0x5164a08d));
                        rb_state12.var_32 = (rb_state12.var_32) - (rb_state12.var_29);
                        rb_state12.var_33 = (rb_state12.var_33) - (((rb_state12.var_18) == (0x0)) ? (rb_state12.var_30) : (0x60d7d44b));
                        rb_state12.var_34 = (rb_state12.var_34) - (((rb_state12.var_28) == (0x0)) ? (rb_state12.var_31) : (0xea2c21f9));
                        rb_state12.var_35 = (rb_state12.var_35) ^ (((0x7f69b139) - (rb_state12.var_19)) - (rb_state12.var_32));
                        rb_state12.var_36 = (rb_state12.var_36) + (((rb_state12.var_20) == (0x0)) ? (rb_state12.var_33) : (0x47a3e3b0));
                        rb_state12.var_37 = (rb_state12.var_37) ^ (((0x248c4575) - (rb_state12.var_21)) - (rb_state12.var_34));
                        rb_state12.var_38 = (rb_state12.var_38) ^ (((0x41e6b755) ^ (rb_state12.var_22)) + (rb_state12.var_35));
                        rb_state12.var_39 = (rb_state12.var_39) ^ (((rb_state12.var_23) == (0x0)) ? (rb_state12.var_36) : (0x79809e39));
                        rb_state12.var_40 = (rb_state12.var_40) - (((((0x794cf508) ^ (rb_state12.var_38)) ^ (rb_state12.var_24)) + (rb_state12.var_37)) + (0x7239e347));
                        rb_state12.var_41 = (rb_state12.var_41) - (((rb_state12.var_25) == (0x0)) ? (rb_state12.var_39) : (0xe39c4468));
                        rb_state12.var_42 = (rb_state12.var_42) ^ (((rb_state12.var_26) == (0x0)) ? (rb_state12.var_40) : (0x1ea5b781));
                        rb_state12.var_43 = (rb_state12.var_43) - (rb_state12.var_41);
                        rb_state12.var_44 = (rb_state12.var_44) ^ (((0xb509bba6) - (rb_state12.var_42)) - (0xf2426841));
                        rb_state12.var_45 = (rb_state12.var_45) ^ (rb_state12.var_43);
                        rb_state12.var_46 = (rb_state12.var_46) + (((rb_state12.var_44) == (0x0)) ? (rb_state12.var_45) : (0xd961fdde));
                        rb_state12.var_27 = (rb_state12.var_27) - (rb_state12.var_46);
                        rb_state12.var_2 = rb_state12.var_27;
                        pthread_mutex_unlock(&(rb_state12.lock_47));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_13
                    rb_state13.var_0 = (rb_state13.var_0) + ((0xf6657e4) ^ (0x491ab5f7));
                    rb_state13.var_2 = (rb_state13.var_2) - (rb_state13.var_1);
                    rb_state13.var_2 = (rb_state13.var_2) ^ (rb_state13.var_0);
                    rb_state13.var_1 = (rb_state13.var_1) + (0xdf56cabd);
                    if ((rb_state13.var_1) == (0x4ec10d98)) {
                        if ((rb_state13.var_3) != (0x0)) {
                            if (!((rb_state13.var_3) == (rb_state13.var_18))) {
                                racebench_trigger(13);
                            }
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_14
                    rb_state14.var_1 = (rb_state14.var_1) + (((rb_state14.var_1) == (0xcd6d16ef)) ? (rb_state14.var_1) : (0xa4891db6));
                    rb_state14.var_0 = (rb_state14.var_0) ^ ((0x6cdb60f7) ^ (rb_state14.var_1));
                    rb_state14.var_1 = (rb_state14.var_1) - (rb_state14.var_0);
                    if ((rb_state14.var_1) == (0x1db624bf)) {
                        pthread_mutex_lock(&(rb_state14.lock_31));
                        rb_state14.var_24 = 0x2e5a9957;
                        rb_state14.var_25 = 0xb3d3b369;
                        rb_state14.var_26 = (rb_state14.var_26) + (((rb_state14.var_14) == (0x0)) ? (rb_state14.var_24) : (0x94bab386));
                        rb_state14.var_27 = (rb_state14.var_27) ^ (((0x69ff9b7c) - (0xc5eae792)) - (rb_state14.var_13));
                        rb_state14.var_28 = (rb_state14.var_28) + (((rb_state14.var_15) == (0x0)) ? (rb_state14.var_25) : (0x8c265429));
                        rb_state14.var_29 = (rb_state14.var_29) + (((((0xde40aad) - (rb_state14.var_27)) ^ (rb_state14.var_16)) + (rb_state14.var_26)) - (0xe2d8d96d));
                        rb_state14.var_30 = (rb_state14.var_30) ^ (((((0x39a0c6af) + (rb_state14.var_28)) - (0x9f53aa29)) + (rb_state14.var_29)) - (0xf3cdac07));
                        rb_state14.var_2 = (rb_state14.var_2) ^ (rb_state14.var_30);
                        pthread_mutex_unlock(&(rb_state14.lock_31));
                    }
                    rb_state14.var_17 = (rb_state14.var_17) + ((0xb1ac5645) ^ (rb_state14.var_18));
                    rb_state14.var_19 = (rb_state14.var_19) + ((0x9e683bd1) + (rb_state14.var_20));
                    rb_state14.var_21 = (rb_state14.var_21) + (9023 < rb_input_size ? (uint32_t)rb_input[9023] : 0x47d125d0);
                    if ((rb_state14.var_22) == (0xce8bbd36)) {
                        rb_state14.var_37 = 0x31b48add;
                        rb_state14.var_38 = (rb_state14.var_38) + (((rb_state14.var_29) == (0x0)) ? (rb_state14.var_37) : (0x4c93a561));
                        rb_state14.var_39 = (rb_state14.var_39) - (((rb_state14.var_28) == (0x0)) ? (rb_state14.var_27) : (0x5818832d));
                        rb_state14.var_40 = (rb_state14.var_40) - (rb_state14.var_30);
                        rb_state14.var_41 = (rb_state14.var_41) ^ (((0xf37b6361) ^ (rb_state14.var_38)) - (0x8be607bb));
                        rb_state14.var_42 = (rb_state14.var_42) - (rb_state14.var_39);
                        rb_state14.var_43 = (rb_state14.var_43) ^ (((0x792eaa3c) - (rb_state14.var_40)) + (rb_state14.var_32));
                        rb_state14.var_44 = (rb_state14.var_44) ^ (((rb_state14.var_41) == (0x0)) ? (rb_state14.var_42) : (0xd0f9c08f));
                        rb_state14.var_45 = (rb_state14.var_45) + (((rb_state14.var_33) == (0x0)) ? (rb_state14.var_43) : (0x9bcd07f7));
                        rb_state14.var_46 = (rb_state14.var_46) + (((0xbd5f52b3) ^ (rb_state14.var_44)) - (rb_state14.var_34));
                        rb_state14.var_47 = (rb_state14.var_47) ^ (((rb_state14.var_45) == (0x0)) ? (rb_state14.var_46) : (0x402cf733));
                        rb_state14.var_33 = (rb_state14.var_33) - (rb_state14.var_47);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_15
                    rb_state15.var_0 = (rb_state15.var_0) ^ ((0x103608a5) - (0xdc40f900));
                    rb_state15.var_1 = (rb_state15.var_1) - (2075 < rb_input_size ? (uint32_t)rb_input[2075] : 0x2cbf7798);
                    rb_state15.var_2 = (rb_state15.var_2) + (rb_state15.var_0);
                    if ((rb_state15.var_1) == (0xd5d8b458)) {
                        rb_state15.var_9 = 0x64f1c479;
                        rb_state15.var_10 = 0x5c190a6a;
                        rb_state15.var_11 = (rb_state15.var_11) + (((rb_state15.var_8) == (0x0)) ? (rb_state15.var_7) : (0x5fa9271e));
                        rb_state15.var_12 = (rb_state15.var_12) + (((rb_state15.var_6) == (0x0)) ? (rb_state15.var_10) : (0xc118a160));
                        rb_state15.var_13 = (rb_state15.var_13) ^ (((rb_state15.var_9) == (0x0)) ? (rb_state15.var_9) : (0xb2fc81d7));
                        rb_state15.var_14 = (rb_state15.var_14) ^ (((rb_state15.var_11) == (0x0)) ? (rb_state15.var_12) : (0x97e4e833));
                        rb_state15.var_15 = (rb_state15.var_15) ^ (rb_state15.var_13);
                        rb_state15.var_16 = (rb_state15.var_16) - (((rb_state15.var_10) == (0x0)) ? (rb_state15.var_14) : (0xfb016ade));
                        rb_state15.var_17 = (rb_state15.var_17) + (((0x7fe1dea0) ^ (0xf3425943)) - (rb_state15.var_15));
                        rb_state15.var_18 = (rb_state15.var_18) + (rb_state15.var_16);
                        rb_state15.var_19 = (rb_state15.var_19) - (((0xce67d579) - (rb_state15.var_17)) ^ (rb_state15.var_11));
                        rb_state15.var_20 = (rb_state15.var_20) - (((rb_state15.var_12) == (0x0)) ? (rb_state15.var_18) : (0xf116369e));
                        rb_state15.var_21 = (rb_state15.var_21) ^ (((((0x450bd996) - (rb_state15.var_13)) ^ (0x270e44dd)) + (rb_state15.var_19)) + (rb_state15.var_20));
                        rb_state15.var_8 = (rb_state15.var_8) - (rb_state15.var_21);
                        rb_state15.var_3 = rb_state15.var_8;
                    }
                    if ((rb_state15.var_1) == (0xfe792d22)) {
                        pthread_mutex_lock(&(rb_state15.lock_32));
                        rb_state15.var_23 = 0xc5087c9f;
                        rb_state15.var_24 = (rb_state15.var_24) - ((0x578ea65f) ^ (rb_state15.var_16));
                        rb_state15.var_25 = (rb_state15.var_25) + (rb_state15.var_15);
                        rb_state15.var_26 = (rb_state15.var_26) ^ (((((0xf98e5450) - (rb_state15.var_14)) + (0xcfd8bf29)) - (0xfa319587)) ^ (rb_state15.var_23));
                        rb_state15.var_27 = (rb_state15.var_27) - (rb_state15.var_24);
                        rb_state15.var_28 = (rb_state15.var_28) + (rb_state15.var_25);
                        rb_state15.var_29 = (rb_state15.var_29) ^ (((((0x8263aa7d) - (rb_state15.var_26)) - (rb_state15.var_17)) + (0x86a3fc15)) ^ (rb_state15.var_27));
                        rb_state15.var_30 = (rb_state15.var_30) ^ (rb_state15.var_28);
                        rb_state15.var_31 = (rb_state15.var_31) - (((rb_state15.var_29) == (0x0)) ? (rb_state15.var_30) : (0xa8515a73));
                        rb_state15.var_22 = (rb_state15.var_22) + (rb_state15.var_31);
                        rb_state15.var_3 = rb_state15.var_22;
                        pthread_mutex_unlock(&(rb_state15.lock_32));
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_16
                    rb_state16.var_0 = (rb_state16.var_0) + (0xb704f6ff);
                    rb_state16.var_0 = (rb_state16.var_0) + (rb_state16.var_0);
                    rb_state16.var_1 = (rb_state16.var_1) + (0xabb12b81);
                    if ((rb_state16.var_2) == (0x54ce013c)) {
                        pthread_mutex_lock(&(rb_state16.lock_20));
                        rb_state16.var_5 = 0xb18eee05;
                        rb_state16.var_6 = (rb_state16.var_6) + (((0x8cd9b1bd) - (0xa3b033b9)) ^ (rb_state16.var_5));
                        rb_state16.var_7 = (rb_state16.var_7) - (((rb_state16.var_5) == (0x0)) ? (rb_state16.var_4) : (0x67db2ec2));
                        rb_state16.var_8 = (rb_state16.var_8) ^ (((((0x361ab971) + (0x6212ed87)) ^ (rb_state16.var_3)) + (rb_state16.var_6)) ^ (rb_state16.var_6));
                        rb_state16.var_9 = (rb_state16.var_9) + (((rb_state16.var_7) == (0x0)) ? (rb_state16.var_8) : (0xddf60254));
                        rb_state16.var_4 = (rb_state16.var_4) ^ (rb_state16.var_9);
                        rb_state16.var_3 = rb_state16.var_4;
                        pthread_mutex_unlock(&(rb_state16.lock_20));
                    }
                    if ((rb_state16.var_2) == (0x54ce013c)) {
                        rb_state16.var_10 = 0x235b9003;
                        rb_state16.var_11 = (rb_state16.var_11) ^ (((rb_state16.var_9) == (0x0)) ? (rb_state16.var_10) : (0x71b192df));
                        rb_state16.var_12 = (rb_state16.var_12) + (((rb_state16.var_8) == (0x0)) ? (rb_state16.var_7) : (0x95d347ae));
                        rb_state16.var_13 = (rb_state16.var_13) + (((rb_state16.var_10) == (0x0)) ? (rb_state16.var_11) : (0x4179f856));
                        rb_state16.var_14 = (rb_state16.var_14) - (((0x305ac775) - (rb_state16.var_11)) - (rb_state16.var_12));
                        rb_state16.var_15 = (rb_state16.var_15) + (rb_state16.var_13);
                        rb_state16.var_16 = (rb_state16.var_16) - (rb_state16.var_14);
                        rb_state16.var_17 = (rb_state16.var_17) + (rb_state16.var_15);
                        rb_state16.var_18 = (rb_state16.var_18) + (((rb_state16.var_12) == (0x0)) ? (rb_state16.var_16) : (0xe8f2973a));
                        rb_state16.var_19 = (rb_state16.var_19) ^ (((((0x94524d3a) - (rb_state16.var_18)) + (0x38187aba)) + (rb_state16.var_17)) ^ (0xfd2a0a27));
                        rb_state16.var_3 = (rb_state16.var_3) ^ (rb_state16.var_19);
                    }
                    if ((rb_state16.var_2) == (0x54ce013c)) {
                        if (!((rb_state16.var_3) == (rb_state16.var_4))) {
                            racebench_trigger(16);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_17
                    rb_state17.var_0 = (rb_state17.var_0) - ((0xc002363d) ^ (0x450c144c));
                    rb_state17.var_1 = (rb_state17.var_1) + (((rb_state17.var_0) == (0x7af1dd8f)) ? (rb_state17.var_1) : (0xd46d7700));
                    rb_state17.var_0 = (rb_state17.var_0) + (((rb_state17.var_0) == (0x7af1dd8f)) ? (rb_state17.var_1) : (0x80c2f8a9));
                    if ((rb_state17.var_1) == (0x4544768a)) {
                        pthread_mutex_lock(&(rb_state17.lock_18));
                        rb_state17.var_4 = 0x7ac5c1d8;
                        rb_state17.var_5 = 0x57cac2e3;
                        rb_state17.var_6 = (rb_state17.var_6) - (((((0x8058365b) + (rb_state17.var_4)) + (rb_state17.var_2)) ^ (rb_state17.var_4)) - (rb_state17.var_3));
                        rb_state17.var_7 = (rb_state17.var_7) - (rb_state17.var_5);
                        rb_state17.var_8 = (rb_state17.var_8) ^ (rb_state17.var_5);
                        rb_state17.var_9 = (rb_state17.var_9) ^ (((((0x9e4b31f4) ^ (0xe36abdcd)) - (rb_state17.var_7)) + (rb_state17.var_6)) - (rb_state17.var_6));
                        rb_state17.var_10 = (rb_state17.var_10) - (((rb_state17.var_8) == (0x0)) ? (rb_state17.var_9) : (0x6d6568b3));
                        rb_state17.var_3 = (rb_state17.var_3) - (rb_state17.var_10);
                        rb_state17.var_2 = rb_state17.var_3;
                        pthread_mutex_unlock(&(rb_state17.lock_18));
                    }
                    if ((rb_state17.var_1) == (0x7653ef85)) {
                        if (!((rb_state17.var_2) == (rb_state17.var_3))) {
                            racebench_trigger(17);
                        }
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_18
                    rb_state18.var_0 = (rb_state18.var_0) - ((0x23511817) ^ (rb_state18.var_0));
                    rb_state18.var_1 = (rb_state18.var_1) - ((0x877d27d3) + (0x957ae57d));
                    rb_state18.var_2 = (rb_state18.var_2) + (rb_state18.var_1);
                    rb_state18.var_0 = (rb_state18.var_0) + (0x97358a5d);
                    rb_state18.var_1 = (rb_state18.var_1) ^ (((rb_state18.var_0) == (0x73e47246)) ? (rb_state18.var_2) : (0x152bad97));
                    if ((rb_state18.var_1) == (0xd9714138)) {
                        rb_state18.var_11 = 0xa20bdc33;
                        rb_state18.var_12 = 0x246bfbaf;
                        rb_state18.var_13 = (rb_state18.var_13) + (((((0x2b0888e5) + (rb_state18.var_8)) - (rb_state18.var_7)) - (rb_state18.var_9)) - (rb_state18.var_11));
                        rb_state18.var_14 = (rb_state18.var_14) - (((rb_state18.var_10) == (0x0)) ? (rb_state18.var_12) : (0x829acf69));
                        rb_state18.var_15 = (rb_state18.var_15) ^ (rb_state18.var_13);
                        rb_state18.var_16 = (rb_state18.var_16) ^ (((rb_state18.var_14) == (0x0)) ? (rb_state18.var_15) : (0xcb64e0c7));
                        rb_state18.var_3 = (rb_state18.var_3) + (rb_state18.var_16);
                    }
                    #endif
                    #ifdef RACEBENCH_BUG_19
                    rb_state19.var_0 = (rb_state19.var_0) ^ ((0xf0a69073) - (0x7f870868));
                    rb_state19.var_1 = (rb_state19.var_1) - (9245 < rb_input_size ? (uint32_t)rb_input[9245] : 0x43538ac5);
                    rb_state19.var_1 = (rb_state19.var_1) - ((0x7016f48) + (rb_state19.var_0));
                    rb_state19.var_1 = (rb_state19.var_1) + (0x6b46df80);
                    rb_state19.var_0 = (rb_state19.var_0) - (rb_state19.var_1);
                    if ((rb_state19.var_0) == (0x3376ea32)) {
                        pthread_mutex_lock(&(rb_state19.lock_23));
                        rb_state19.var_2 = rb_state19.var_1;
                        pthread_mutex_unlock(&(rb_state19.lock_23));
                    }
                    if ((rb_state19.var_0) == (0xb9d7d283)) {
                        rb_state19.var_3 = 0xa3d48293;
                        rb_state19.var_4 = (rb_state19.var_4) + (((0x244a555a) ^ (rb_state19.var_3)) + (0x2f5b3096));
                        rb_state19.var_5 = (rb_state19.var_5) - ((0xe5bc1924) + (0xa604684a));
                        rb_state19.var_6 = (rb_state19.var_6) + (((rb_state19.var_2) == (0x0)) ? (rb_state19.var_4) : (0x187cfb30));
                        rb_state19.var_7 = (rb_state19.var_7) ^ (rb_state19.var_5);
                        rb_state19.var_8 = (rb_state19.var_8) + (((0x6b653786) + (rb_state19.var_4)) ^ (rb_state19.var_6));
                        rb_state19.var_9 = (rb_state19.var_9) ^ (((0x82a2e4fe) + (rb_state19.var_7)) + (0x9ac53a67));
                        rb_state19.var_10 = (rb_state19.var_10) - (rb_state19.var_8);
                        rb_state19.var_11 = (rb_state19.var_11) + (((0xd509f27d) ^ (rb_state19.var_9)) + (rb_state19.var_5));
                        rb_state19.var_12 = (rb_state19.var_12) ^ (((rb_state19.var_6) == (0x0)) ? (rb_state19.var_10) : (0xaddb4ab8));
                        rb_state19.var_13 = (rb_state19.var_13) - (((rb_state19.var_7) == (0x0)) ? (rb_state19.var_11) : (0xf6c90140));
                        rb_state19.var_14 = (rb_state19.var_14) + (((rb_state19.var_8) == (0x0)) ? (rb_state19.var_12) : (0x4d64fff8));
                        rb_state19.var_15 = (rb_state19.var_15) - (((0x83c7c1b0) + (0xa87cce8c)) - (rb_state19.var_13));
                        rb_state19.var_16 = (rb_state19.var_16) ^ (rb_state19.var_14);
                        rb_state19.var_17 = (rb_state19.var_17) - (((rb_state19.var_9) == (0x0)) ? (rb_state19.var_15) : (0xdb3cf5a6));
                        rb_state19.var_18 = (rb_state19.var_18) - (((0x2b203d39) ^ (rb_state19.var_10)) ^ (rb_state19.var_16));
                        rb_state19.var_19 = (rb_state19.var_19) + (((0x4bd329a7) ^ (0xc7bc0174)) ^ (rb_state19.var_17));
                        rb_state19.var_20 = (rb_state19.var_20) + (rb_state19.var_18);
                        rb_state19.var_21 = (rb_state19.var_21) + (rb_state19.var_19);
                        rb_state19.var_22 = (rb_state19.var_22) ^ (((((0xa359f412) ^ (0xd7ef8fb4)) - (rb_state19.var_21)) - (rb_state19.var_11)) ^ (rb_state19.var_20));
                        rb_state19.var_1 = (rb_state19.var_1) ^ (rb_state19.var_22);
                    }
                    if ((rb_state19.var_0) == (0xb9d7d283)) {
                        rb_state19.var_24 = !((rb_state19.var_1) == (rb_state19.var_2));
                    }
                    rb_state19.var_12 = (rb_state19.var_12) + (rb_state19.var_13);
                    rb_state19.var_14 = (rb_state19.var_14) ^ (420 < rb_input_size ? (uint32_t)rb_input[420] : 0x3ad05081);
                    rb_state19.var_15 = (rb_state19.var_15) + (2489 < rb_input_size ? (uint32_t)rb_input[2489] : 0xb7f6cd59);
                    if ((rb_state19.var_16) == (0xd7ad0008)) {
                        pthread_mutex_lock(&(rb_state19.lock_36));
                        rb_state19.var_27 = 0xa6c8e587;
                        rb_state19.var_28 = (rb_state19.var_28) ^ (rb_state19.var_27);
                        rb_state19.var_29 = (rb_state19.var_29) + (((((0xdaeaa59d) + (0x782e13fa)) + (rb_state19.var_17)) ^ (0xe90717db)) - (rb_state19.var_18));
                        rb_state19.var_30 = (rb_state19.var_30) ^ (((rb_state19.var_28) == (0x0)) ? (rb_state19.var_29) : (0x6035e1aa));
                        rb_state19.var_26 = (rb_state19.var_26) ^ (rb_state19.var_30);
                        rb_state19.var_25 = rb_state19.var_26;
                        pthread_mutex_unlock(&(rb_state19.lock_36));
                    }
                    if ((rb_state19.var_16) == (0xd7ad0008)) {
                        rb_state19.var_31 = 0xc0d25cc;
                        rb_state19.var_32 = 0x67083d89;
                        rb_state19.var_33 = (rb_state19.var_33) ^ (rb_state19.var_32);
                        rb_state19.var_34 = (rb_state19.var_34) + (((rb_state19.var_19) == (0x0)) ? (rb_state19.var_31) : (0x346bedd6));
                        rb_state19.var_35 = (rb_state19.var_35) ^ (((rb_state19.var_33) == (0x0)) ? (rb_state19.var_34) : (0x596d18a4));
                        rb_state19.var_25 = (rb_state19.var_25) + (rb_state19.var_35);
                    }
                    if ((rb_state19.var_16) == (0xd7ad0008)) {
                        if (!((rb_state19.var_25) == (rb_state19.var_26))) {
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