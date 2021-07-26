
#include "incl.h"

#define MAP_CUR_VERSION 1
uint16_t map_version;

uint16_t orig_min[NM], orig_max[NM], orig_len[NM];

uint16_t extr_min[NM], extr_max[NM], extr_len[NM];

uint16_t map_min[NM], map_max[NM], map_len[NM];

uint16_t map_warps;

uint32_t map_length;

DENSITY *map_address;

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
extern pthread_t PThreadTable[];

Load_Map(filename) char filename[];
{
    char local_filename[FILENAME_STRING_SIZE];
    int fd;

    strcpy(local_filename, filename);
    strcat(local_filename, ".den");
    fd = Open_File(local_filename);

    Read_Shorts(fd, &map_version, (long)sizeof(map_version));
    if (map_version != MAP_CUR_VERSION) {
        Error("    Can't load version %d file\n", map_version);
    }

    Read_Shorts(fd, orig_min, (long)sizeof(orig_min));
    Read_Shorts(fd, orig_max, (long)sizeof(orig_max));
    Read_Shorts(fd, orig_len, (long)sizeof(orig_len));

    Read_Shorts(fd, extr_min, (long)sizeof(extr_min));
    Read_Shorts(fd, extr_max, (long)sizeof(extr_max));
    Read_Shorts(fd, extr_len, (long)sizeof(extr_len));

    Read_Shorts(fd, map_min, (long)sizeof(map_min));
    Read_Shorts(fd, map_max, (long)sizeof(map_max));
    Read_Shorts(fd, map_len, (long)sizeof(map_len));

    Read_Shorts(fd, &map_warps, (long)sizeof(map_warps));
    Read_Longs(fd, &map_length, (long)sizeof(map_length));

    Allocate_Map(&map_address, map_length);

    printf("    Loading map from .den file...\n");
    Read_Bytes(fd, map_address, (long)(map_length * sizeof(DENSITY)));
    Close_File(fd);
}

Allocate_Map(address, length) DENSITY **address;
long length;
{
    int i;
    unsigned int p, numbytes;

    printf("    Allocating density map of %ld bytes...\n", length * sizeof(DENSITY));

    *address = (DENSITY *)malloc(length * sizeof(DENSITY));
    ;

    if (*address == NULL) {
        Error("    No space available for map.\n");
    } else {
        for (i = 0; i < length; i++) {
            *(*address + i) = 0;
        }
    }
}

Deallocate_Map(address) DENSITY **address;
{
    printf("    Deallocating density map...\n");

    *address = NULL;
}
