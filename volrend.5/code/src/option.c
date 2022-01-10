

#include "incl.h"

int block_xlen, block_ylen;
BOOLEAN adaptive;

int density_epsilon;

int magnitude_epsilon;

float density_opacity[MAX_DENSITY + 1];
float magnitude_opacity[MAX_MAGNITUDE + 1];

PIXEL background;

float light[NM];

float ambient_color;
float diffuse_color;
float specular_color;
float specular_exponent;

float depth_hither;
float depth_yon;
float depth_exponent;

float opacity_epsilon;

float opacity_cutoff;

int highest_sampling_boxlen;

int lowest_volume_boxlen;

int volume_color_difference;

int pyr_highest_level;

int pyr_lowest_level;

float angle[NM];

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

Init_Options() {

    norm_address = NULL;
    opc_address = NULL;
    pyr_address[0] = NULL;

    background = NULL_PIXEL;

    Init_Opacity();
    Init_Lighting();

    angle[X] = 90.0;
    angle[Y] = -36.0;
    angle[Z] = 0.0;
    Init_Parallelization();

    opacity_epsilon = 0.0;
    opacity_cutoff = 0.95;
    highest_sampling_boxlen = HBOXLEN;

    lowest_volume_boxlen = 1;

    volume_color_difference = 16;
    pyr_highest_level = 5;
    pyr_lowest_level = 2;
}

Init_Opacity() {
    int i;
    float increment;

    density_epsilon = 96;
    magnitude_epsilon = 1;

    density_opacity[MIN_DENSITY] = 0.0;
    density_opacity[95] = 0.0;
    density_opacity[135] = 1.0;
    density_opacity[MAX_DENSITY] = 1.0;
    for (i = MIN_DENSITY; i < 95; i++) {
        density_opacity[i] = 0.0;
    }
    increment = 1.0 / (135.0 - 95.0);
    for (i = 95; i < 134; i++) {
        density_opacity[i + 1] = density_opacity[i] + increment;
    }
    for (i = 135; i < MAX_DENSITY; i++) {
        density_opacity[i] = 1.0;
    }
    magnitude_opacity[MIN_MAGNITUDE] = 0.0;
    magnitude_opacity[70] = 1.0;
    magnitude_opacity[MAX_MAGNITUDE] = 1.0;
    increment = 1.0 / (70.0 - (float)MIN_MAGNITUDE);
    for (i = MIN_MAGNITUDE; i < 69; i++) {
        magnitude_opacity[i + 1] = magnitude_opacity[i] + increment;
    }
    for (i = 70; i < MAX_MAGNITUDE - 1; i++) {
        magnitude_opacity[i] = 1.0;
    }
}

Init_Lighting() {
    float inv_magnitude;

    light[X] = 0.5;
    light[Y] = 0.7;
    light[Z] = -1.0;
    inv_magnitude = 1.0 / sqrt(light[X] * light[X] + light[Y] * light[Y] + light[Z] * light[Z]);
    light[X] = light[X] * inv_magnitude;
    light[Y] = light[Y] * inv_magnitude;
    light[Z] = light[Z] * inv_magnitude;

    ambient_color = 30.0;
    diffuse_color = 100.0;
    specular_color = 130.0;
    specular_exponent = 10.0;

    depth_hither = 1.0;
    depth_yon = 0.4;
    depth_exponent = 1.0;
}

Init_Parallelization() {
    block_xlen = BLOCK_LEN;
    block_ylen = BLOCK_LEN;
#ifdef RENDER_ONLY
    printf("Rendering only:  Starting from .norm, .opc and .pyr files\n");
#else
#ifdef PREPROCESS
    printf("Preprocessing only: From .den file to .norm, .opc, and .pyr files; no rendering\n");
#else
    printf("Both computing normals, opacities and octree as well as rendering: starting from .den file\n");
#endif
#ifdef SERIAL_PREPROC
    printf("NOTE: Preprocessing (computing normals, opacities and octree) is done serially by only one processor\n");
#endif
#endif

    printf("Gouraud shading from lookup tables used\n");
    printf("\t%d processes\n", num_nodes);
    printf("\t%dx%d image block size\n", block_xlen, block_ylen);

    if (adaptive) {
        printf("\tdoing adaptive rendering\n");
    } else {
        printf("\tdoing nonadaptive rendering\n");
    }

    printf("\tusing precomputed normals and opacities in separate array\n");

    printf("\tusing opacity octree\n");

    printf("\tstarting angle at (%.1f, %.1f, %.1f) with rotation step of %d\n", angle[X], angle[Y], angle[Z], STEP_SIZE);
#ifdef DIM
    printf("rotating %d steps in each of the three Cartesian directions\n", ROTATE_STEPS);
#else
    printf("rotating %d steps in the Y direction\n", ROTATE_STEPS);
#endif

    printf("\n");
}
