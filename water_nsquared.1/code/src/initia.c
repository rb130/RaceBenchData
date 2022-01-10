

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

#include "math.h"
#include "stdio.h"
#include "mdvar.h"
#include "water.h"
#include "cnst.h"
#include "fileio.h"
#include "parameters.h"
#include "mddata.h"
#include "split.h"

extern const char *input_file;

INITIA() {

    static double XMIN = 0;
    static double YMIN = 0;
    static double ZMIN = 0;
    FILE *random_numbers;

    double XMAS[4], XS, ZERO, WCOS, WSIN, XT[4], YT[4], Z;
    double SUX, SUY, SUZ, SUMX, SUMY, SUMZ, FAC;
    int mol = 0;
    int atom = 0;
    int deriv;
    double xrand();

    random_numbers = fopen(input_file, "r");
    if (random_numbers == NULL) {

        fprintf(stderr, "Error in opening file %s\n", input_file);
        fflush(stderr);
        exit(-1);
    }

    XMAS[1] = sqrt(OMAS * HMAS);
    XMAS[0] = HMAS;
    XMAS[2] = HMAS;

    {
        double NS = pow((double)NMOL, 1.0 / 3.0) - 0.00001;
        double XS = BOXL / NS;
        double ZERO = XS * 0.50;
        double WCOS = ROH * cos(ANGLE * 0.5);
        double WSIN = ROH * sin(ANGLE * 0.5);
        int i, j, k;

        printf("\nNS = %.16f\n", NS);
        printf("BOXL = %10f\n", BOXL);
        printf("CUTOFF = %10f\n", CUTOFF);
        printf("XS = %10f\n", XS);
        printf("ZERO = %g\n", ZERO);
        printf("WCOS = %f\n", WCOS);
        printf("WSIN = %f\n", WSIN);
        fflush(stdout);

#ifdef RANDOM

        srandom(1023);
        for (i = 0; i < NMOL; i++) {
            VAR[mol].F[DISP][XDIR][O] = xrand(0, BOXL);
            VAR[mol].F[DISP][XDIR][H1] = VAR[mol].F[DISP][XDIR][O] + WCOS;
            VAR[mol].F[DISP][XDIR][H2] = VAR[mol].F[DISP][XDIR][H1];
            VAR[mol].F[DISP][YDIR][O] = xrand(0, BOXL);
            VAR[mol].F[DISP][YDIR][H1] = VAR[mol].F[DISP][YDIR][O] + WSIN;
            VAR[mol].F[DISP][YDIR][H2] = VAR[mol].F[DISP][YDIR][O] - WSIN;
            VAR[mol].F[DISP][ZDIR][O] = xrand(0, BOXL);
            VAR[mol].F[DISP][ZDIR][H1] = VAR[mol].F[DISP][ZDIR][O];
            VAR[mol].F[DISP][ZDIR][H2] = VAR[mol].F[DISP][ZDIR][O];
        }
#else

        fprintf(six, "***** NEW RUN STARTING FROM REGULAR LATTICE *****\n");
        fflush(six);
        XT[2] = ZERO;
        mol = 0;
        for (i = 0; i < NS; i += 1) {
            XT[1] = XT[2] + WCOS;
            XT[3] = XT[1];
            YT[2] = ZERO;
            for (j = 0; j < NS; j += 1) {
                YT[1] = YT[2] + WSIN;
                YT[3] = YT[2] - WSIN;
                Z = ZERO;
                for (k = 0; k < NS; k++) {
                    for (atom = 0; atom < NATOMS; atom += 1) {
                        VAR[mol].F[DISP][XDIR][atom] = XT[atom + 1];
                        VAR[mol].F[DISP][YDIR][atom] = YT[atom + 1];
                        VAR[mol].F[DISP][ZDIR][atom] = Z;
                    }
                    mol += 1;
                    Z = Z + XS;
                }
                YT[2] = YT[2] + XS;
            }
            XT[2] = XT[2] + XS;
        }

        if (NMOL != mol) {
            printf("Lattice init error: total mol %d != NMOL %d\n", mol, NMOL);
            exit(-1);
        }
#endif
    }

    fscanf(random_numbers, "%lf", &SUX);

    SUMX = 0.0;
    SUMY = 0.0;
    SUMZ = 0.0;

    for (mol = 0; mol < NMOL; mol++) {
        for (atom = 0; atom < NATOMS; atom++) {
            fscanf(random_numbers, "%lf", &VAR[mol].F[VEL][XDIR][atom]);
            fscanf(random_numbers, "%lf", &VAR[mol].F[VEL][YDIR][atom]);
            fscanf(random_numbers, "%lf", &VAR[mol].F[VEL][ZDIR][atom]);
            SUMX = SUMX + VAR[mol].F[VEL][XDIR][atom];
            SUMY = SUMY + VAR[mol].F[VEL][YDIR][atom];
            SUMZ = SUMZ + VAR[mol].F[VEL][ZDIR][atom];
            for (deriv = ACC; deriv < MAXODR; deriv++) {
                VAR[mol].F[deriv][XDIR][atom] = 0.0;
                VAR[mol].F[deriv][YDIR][atom] = 0.0;
                VAR[mol].F[deriv][ZDIR][atom] = 0.0;
            }
        }
    }

    SUMX = SUMX / (NATOMS * NMOL);
    SUMY = SUMY / (NATOMS * NMOL);
    SUMZ = SUMZ / (NATOMS * NMOL);

    SUX = 0.0;
    SUY = 0.0;
    SUZ = 0.0;
    for (mol = 0; mol < NMOL; mol++) {
        SUX = SUX + (pow((VAR[mol].F[VEL][XDIR][H1] - SUMX), 2.0) + pow((VAR[mol].F[VEL][XDIR][H2] - SUMX), 2.0)) / HMAS + pow((VAR[mol].F[VEL][XDIR][O] - SUMX), 2.0) / OMAS;

        SUY = SUY + (pow((VAR[mol].F[VEL][YDIR][H1] - SUMY), 2.0) + pow((VAR[mol].F[VEL][YDIR][H2] - SUMY), 2.0)) / HMAS + pow((VAR[mol].F[VEL][YDIR][O] - SUMY), 2.0) / OMAS;

        SUZ = SUZ + (pow((VAR[mol].F[VEL][ZDIR][H1] - SUMZ), 2.0) + pow((VAR[mol].F[VEL][ZDIR][H2] - SUMZ), 2.0)) / HMAS + pow((VAR[mol].F[VEL][ZDIR][O] - SUMZ), 2.0) / OMAS;
    }
    FAC = BOLTZ * TEMP * NATMO / UNITM * pow((UNITT * TSTEP / UNITL), 2.0);
    SUX = sqrt(FAC / SUX);
    SUY = sqrt(FAC / SUY);
    SUZ = sqrt(FAC / SUZ);

    XMAS[1] = OMAS;
    for (mol = 0; mol < NMOL; mol++) {
        for (atom = 0; atom < NATOMS; atom++) {
            VAR[mol].F[VEL][XDIR][atom] = (VAR[mol].F[VEL][XDIR][atom] - SUMX) * SUX / XMAS[atom];
            VAR[mol].F[VEL][YDIR][atom] = (VAR[mol].F[VEL][YDIR][atom] - SUMY) * SUY / XMAS[atom];
            VAR[mol].F[VEL][ZDIR][atom] = (VAR[mol].F[VEL][ZDIR][atom] - SUMZ) * SUZ / XMAS[atom];
        }
    }

    fclose(random_numbers);
}

double xrand(xl, xh)
double xl, xh;
{
    long random();
    double x;

    x = (xl + (xh - xl) * ((double)random()) / 2147483647.0);
    return (x);
}
