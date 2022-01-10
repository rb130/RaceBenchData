

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/mkg3states.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include <stdio.h>
#include "prototypes.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define WHITE 0
#define BLACK 1

#define MODE_NULL 0
#define MODE_PASS 1
#define MODE_HORIZ 2
#define MODE_VERT_VL3 3
#define MODE_VERT_VL2 4
#define MODE_VERT_VL1 5
#define MODE_VERT_V0 6
#define MODE_VERT_VR1 7
#define MODE_VERT_VR2 8
#define MODE_VERT_VR3 9
#define MODE_UNCOMP 10
#define MODE_ERROR 11
#define MODE_ERROR_1 12

unsigned long DECLARE1(append_0, unsigned long, prefix) {
    return (prefix + (1L << 16));
}

unsigned long DECLARE1(append_1, unsigned long, prefix) {
    static unsigned short prefix_bit[16] = {0x8000, 0x4000, 0x2000, 0x1000, 0x0800, 0x0400, 0x0200, 0x0100, 0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001};
    unsigned char len = (prefix >> 16) & 0xf;
    return (append_0(prefix) + prefix_bit[len]);
}

#define G3CODES
#include "t4.h"

short
#if defined(__STDC__) || defined(__EXTENDED__) || USE_CONST
DECLARE3(search_table, unsigned long, prefix, tableentry const*, tab, int, n)
#else
DECLARE3(search_table, unsigned long, prefix, tableentry*, tab, int, n)
#endif
{
    unsigned short len = (prefix >> 16) & 0xf;
    unsigned short code = (prefix & 0xffff) >> (16 - len);

    while (n-- > 0) {
        if (tab->length == len && tab->code == code) {
            return ((short)tab->runlen);
        }
        tab++;
    }
    return (G3CODE_INCOMP);
}

#define NCODES(a) (sizeof(a) / sizeof(a[0]))
short DECLARE1(white_run_length, unsigned long, prefix) {
    return (search_table(prefix, TIFFFaxWhiteCodes, NCODES(TIFFFaxWhiteCodes)));
}

short DECLARE1(black_run_length, unsigned long, prefix) {
    return (search_table(prefix, TIFFFaxBlackCodes, NCODES(TIFFFaxBlackCodes)));
}
#undef NCODES

#define MAX_NULLPREFIX 200
typedef unsigned char NullModeTable[MAX_NULLPREFIX][256];
#define MAX_HORIZPREFIX 250
typedef unsigned char HorizModeTable[MAX_HORIZPREFIX][256];

long null_mode_prefix[MAX_NULLPREFIX];
NullModeTable null_mode;
NullModeTable null_mode_next_state;

short null_mode_prefix_count = 0;

#define UNCOMP_INCOMP 0

#define UNCOMP_RUN0 1
#define UNCOMP_RUN1 2
#define UNCOMP_RUN2 3
#define UNCOMP_RUN3 4
#define UNCOMP_RUN4 5
#define UNCOMP_RUN5 6
#define UNCOMP_RUN6 7

#define UNCOMP_TRUN0 8
#define UNCOMP_TRUN1 9
#define UNCOMP_TRUN2 10
#define UNCOMP_TRUN3 11
#define UNCOMP_TRUN4 12

#define UNCOMP_EOF 13

#define UNCOMP_INVALID 14

long uncomp_mode_prefix[MAX_NULLPREFIX];
NullModeTable uncomp_mode;
NullModeTable uncomp_mode_next_state;
short uncomp_mode_prefix_count = 0;

#define ACT_INCOMP 0
#define ACT_INVALID 1
#define ACT_WRUNT 2
#define ACT_WRUN 65
#define ACT_BRUNT 106
#define ACT_BRUN 169
#define ACT_EOL 210
HorizModeTable horiz_mode;

short DECLARE1(horiz_mode_code_black, short, runlen) {
    return (runlen < 64 ? runlen + ACT_BRUNT : (runlen / 64) + ACT_BRUN);
}

short DECLARE1(horiz_mode_code_white, short, runlen) {
    return (runlen < 64 ? runlen + ACT_WRUNT : (runlen / 64) + ACT_WRUN);
}

HorizModeTable horiz_mode_next_state;

long horiz_mode_prefix[MAX_HORIZPREFIX];

char horiz_mode_color[MAX_HORIZPREFIX];
short horiz_mode_prefix_count = 0;

static unsigned char bit_mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

#if USE_PROTOTYPES
void build_null_mode_tables(void);
short find_horiz_mode_prefix(long, char);
short find_null_mode_prefix(long);
short null_mode_type(long);
void build_horiz_mode_tables(void);
short horiz_mode_code_black(short);
short horiz_mode_code_white(short);
void build_uncomp_mode_tables(void);
void write_tables(FILE *);
#else
void build_null_mode_tables();
short find_horiz_mode_prefix();
short find_null_mode_prefix();
short null_mode_type();
void build_horiz_mode_tables();
short horiz_mode_code_black();
short horiz_mode_code_white();
void build_uncomp_mode_tables();
void write_tables();
#endif

int verbose = FALSE;
char *storage_class = "";

int DECLARE2(main, int, argc, char **, argv) {
    while (argc > 1 && argv[1][0] == '-') {
        if (strcmp(argv[1], "-v") == 0) {
            verbose = TRUE;
            argc--, argv++;
        } else if (strcmp(argv[1], "-c") == 0) {
            storage_class = "const ";
            argc--, argv++;
        }
    }
    build_null_mode_tables();
    if (verbose) {
        fprintf(stderr, "%d null mode prefixes defined\n", (int)null_mode_prefix_count);
        fprintf(stderr, "building uncompressed mode scripts...\n");
    }
    build_uncomp_mode_tables();
    if (verbose) {
        fprintf(stderr, "%d uncompressed mode prefixes defined\n", (int)uncomp_mode_prefix_count);
        fprintf(stderr, "building 1D scripts...\n");
    }
    build_horiz_mode_tables();
    if (verbose) {
        fprintf(stderr, "%d incomplete prefixes defined\n", (int)horiz_mode_prefix_count);
    }
    write_tables(stdout);
    exit(0);
}

void DECLARE3(write_null_mode_table, FILE *, fd, NullModeTable, table, char *, name) {
    int i, j;
    char *outersep;
    char *sep;

    fprintf(fd, "%su_char\t%s[%d][256] = {", storage_class, name, (int)null_mode_prefix_count);
    outersep = "";
    for (i = 0; i < null_mode_prefix_count; i++) {
        fprintf(fd, "%s\n/* prefix %d */ {\n", outersep, i);
        sep = "    ";
        for (j = 0; j < 256; j++) {
            fprintf(fd, "%s%2d", sep, (int)table[i][j]);
            if (((j + 1) % 16) == 0) {
                fprintf(fd, ", /* %3d-%3d */\n", j - 15, j);
                sep = "    ";
            } else {
                sep = ",";
            }
        }
        fprintf(fd, "}");
        outersep = ",";
    }
    fprintf(fd, "\n};\n");
}

void DECLARE3(write_horiz_mode_table, FILE *, fd, HorizModeTable, table, char *, name) {
    int i, j;
    char *outersep;
    char *sep;

    fprintf(fd, "%s u_char\t%s[%d][256] = {", storage_class, name, (int)horiz_mode_prefix_count);
    outersep = "";
    for (i = 0; i < horiz_mode_prefix_count; i++) {
        fprintf(fd, "%s\n/* prefix %d */ {\n", outersep, i);
        sep = "    ";
        for (j = 0; j < 256; j++) {
            fprintf(fd, "%s%3d", sep, (int)table[i][j]);
            if (((j + 1) % 14) == 0) {
                fprintf(fd, ", /* %3d-%3d */\n", j - 13, j);
                sep = "    ";
            } else {
                sep = ",";
            }
        }
        fprintf(fd, "\n}");
        outersep = ",";
    }
    fprintf(fd, "\n};\n");
}

void write_define(fd, name, value, comment) FILE *fd;
char *name;
int value;
char *comment;
{
    fprintf(fd, "#define\t%s\t%d", name, value);
    if (comment) {
        fprintf(fd, "\t/* %s */", comment);
    }
    fprintf(fd, "\n");
}

void write_preamble(fd) FILE *fd;
{
    fprintf(fd, "%s\n", "/* DO NOT EDIT THIS FILE, IT WAS AUTOMATICALLY CREATED BY mkg3state */");
    write_define(fd, "ACT_INCOMP", ACT_INCOMP, "incompletely decoded code");
    write_define(fd, "ACT_INVALID", ACT_INVALID, "invalide code");
    write_define(fd, "ACT_WRUNT", ACT_WRUNT, "terminating white run code");
    write_define(fd, "ACT_WRUN", ACT_WRUN, "non-terminating white run code");
    write_define(fd, "ACT_BRUNT", ACT_BRUNT, "terminating black run code");
    write_define(fd, "ACT_BRUN", ACT_BRUN, "non-terminating black run code");
    write_define(fd, "ACT_EOL", ACT_EOL, "end-of-line code");
    fprintf(fd, "\n");
    fprintf(fd, "/* modes that the decoder can be in */\n");
    write_define(fd, "MODE_NULL", MODE_NULL, NULL);
    write_define(fd, "MODE_PASS", MODE_PASS, NULL);
    write_define(fd, "MODE_HORIZ", MODE_HORIZ, NULL);
    write_define(fd, "MODE_VERT_V0", MODE_VERT_V0, NULL);
    write_define(fd, "MODE_VERT_VR1", MODE_VERT_VR1, NULL);
    write_define(fd, "MODE_VERT_VR2", MODE_VERT_VR2, NULL);
    write_define(fd, "MODE_VERT_VR3", MODE_VERT_VR3, NULL);
    write_define(fd, "MODE_VERT_VL1", MODE_VERT_VL1, NULL);
    write_define(fd, "MODE_VERT_VL2", MODE_VERT_VL2, NULL);
    write_define(fd, "MODE_VERT_VL3", MODE_VERT_VL3, NULL);
    write_define(fd, "MODE_UNCOMP", MODE_UNCOMP, NULL);
    write_define(fd, "MODE_ERROR", MODE_ERROR, NULL);
    write_define(fd, "MODE_ERROR_1", MODE_ERROR_1, NULL);
    fprintf(fd, "\n");
    fprintf(fd, "#define\tRUNLENGTH(ix)	(TIFFFaxWhiteCodes[ix].runlen)\n");
    fprintf(fd, "\n");
    write_define(fd, "UNCOMP_INCOMP", UNCOMP_INCOMP, NULL);
    fprintf(fd, "/* runs of [0]*1 */\n");
    write_define(fd, "UNCOMP_RUN0", UNCOMP_RUN0, NULL);
    write_define(fd, "UNCOMP_RUN1", UNCOMP_RUN1, NULL);
    write_define(fd, "UNCOMP_RUN2", UNCOMP_RUN2, NULL);
    write_define(fd, "UNCOMP_RUN3", UNCOMP_RUN3, NULL);
    write_define(fd, "UNCOMP_RUN4", UNCOMP_RUN4, NULL);
    write_define(fd, "UNCOMP_RUN5", UNCOMP_RUN5, NULL);
    write_define(fd, "UNCOMP_RUN6", UNCOMP_RUN6, NULL);
    fprintf(fd, "/* runs of [0]* w/ terminating color */\n");
    write_define(fd, "UNCOMP_TRUN0", UNCOMP_TRUN0, NULL);
    write_define(fd, "UNCOMP_TRUN1", UNCOMP_TRUN1, NULL);
    write_define(fd, "UNCOMP_TRUN2", UNCOMP_TRUN2, NULL);
    write_define(fd, "UNCOMP_TRUN3", UNCOMP_TRUN3, NULL);
    write_define(fd, "UNCOMP_TRUN4", UNCOMP_TRUN4, NULL);
    fprintf(fd, "/* special code for unexpected EOF */\n");
    write_define(fd, "UNCOMP_EOF", UNCOMP_EOF, NULL);
    fprintf(fd, "/* invalid code encountered */\n");
    write_define(fd, "UNCOMP_INVALID", UNCOMP_INVALID, NULL);
    fprintf(fd, "/* codes >= terminate uncompress mode */\n");
    fprintf(fd, "#define\tUNCOMP_EXIT	UNCOMP_TRUN0\n");
    fprintf(fd, "\n");
}

void extern_table(fd, name) FILE *fd;
char *name;
{ fprintf(fd, "extern\t%su_char %s[][256];\n", storage_class, name); }

void write_tables(fd) FILE *fd;
{
    write_preamble(fd);
    fprintf(fd, "#ifdef G3STATES\n");
    write_null_mode_table(fd, null_mode, "TIFFFax2DMode");
    write_null_mode_table(fd, null_mode_next_state, "TIFFFax2DNextState");
    write_null_mode_table(fd, uncomp_mode, "TIFFFaxUncompAction");
    write_null_mode_table(fd, uncomp_mode_next_state, "TIFFFaxUncompNextState");
    write_horiz_mode_table(fd, horiz_mode, "TIFFFax1DAction");
    write_horiz_mode_table(fd, horiz_mode_next_state, "TIFFFax1DNextState");
    fprintf(fd, "#else\n");
    extern_table(fd, "TIFFFax2DMode");
    extern_table(fd, "TIFFFax2DNextState");
    extern_table(fd, "TIFFFaxUncompAction");
    extern_table(fd, "TIFFFaxUncompNextState");
    extern_table(fd, "TIFFFax1DAction");
    extern_table(fd, "TIFFFax1DNextState");
    fprintf(fd, "#endif\n");
}

short DECLARE1(find_null_mode_prefix, long, prefix) {
    short j1;

    if (prefix == 0L) {
        return (0);
    }
    for (j1 = 8; j1 < null_mode_prefix_count; j1++) {
        if (prefix == null_mode_prefix[j1]) {
            return (j1);
        }
    }
    if (null_mode_prefix_count == MAX_NULLPREFIX) {
        fprintf(stderr, "ERROR: null mode prefix table overflow\n");
        exit(1);
    }
    if (verbose) {
        fprintf(stderr, "adding null mode prefix[%d] 0x%lx\n", (int)null_mode_prefix_count, prefix);
    }
    null_mode_prefix[null_mode_prefix_count++] = prefix;
    return (null_mode_prefix_count - 1);
}

short DECLARE2(find_horiz_mode_prefix, long, prefix, char, color) {
    short j1;

    for (j1 = 0; j1 < horiz_mode_prefix_count; j1++) {
        if (prefix == horiz_mode_prefix[j1] && horiz_mode_color[j1] == color) {
            return (j1);
        }
    }

    if (horiz_mode_prefix_count == MAX_HORIZPREFIX) {
        fprintf(stderr, "ERROR: 1D prefix table overflow\n");
        exit(1);
    }

    if (verbose) {
        fprintf(stderr, "\nhoriz mode prefix %d, color %c = 0x%lx ", (int)horiz_mode_prefix_count, "WB"[color], prefix);
    }
    horiz_mode_prefix[horiz_mode_prefix_count] = prefix;
    horiz_mode_color[horiz_mode_prefix_count] = color;
    horiz_mode_prefix_count++;
    return (horiz_mode_prefix_count - 1);
}

short DECLARE1(find_uncomp_mode_prefix, long, prefix) {
    short j1;

    if (prefix == 0L) {
        return (0);
    }
    for (j1 = 8; j1 < uncomp_mode_prefix_count; j1++) {
        if (prefix == uncomp_mode_prefix[j1]) {
            return (j1);
        }
    }
    if (uncomp_mode_prefix_count == MAX_NULLPREFIX) {
        fprintf(stderr, "ERROR: uncomp mode prefix table overflow\n");
        exit(1);
    }
    if (verbose) {
        fprintf(stderr, "adding uncomp mode prefix[%d] 0x%lx\n", (int)uncomp_mode_prefix_count, prefix);
    }
    uncomp_mode_prefix[uncomp_mode_prefix_count++] = prefix;
    return (uncomp_mode_prefix_count - 1);
}

short DECLARE1(null_mode_type, long, prefix) {
    switch (prefix) {
    case 0x18000L:
        return (MODE_VERT_V0);
    case 0x36000L:
        return (MODE_VERT_VR1);
    case 0x34000L:
        return (MODE_VERT_VL1);
    case 0x32000L:
        return (MODE_HORIZ);
    case 0x41000L:
        return (MODE_PASS);
    case 0x60C00L:
        return (MODE_VERT_VR2);
    case 0x60800L:
        return (MODE_VERT_VL2);
    case 0x70600L:
        return (MODE_VERT_VR3);
    case 0x70400L:
        return (MODE_VERT_VL3);
    case 0x80200L:
        return (MODE_ERROR);
    case 0x90300L:
        return (MODE_ERROR);
    case 0xA0380L:
        return (MODE_ERROR);
    case 0xA03C0L:
        return (MODE_UNCOMP);

    case 0x70000L:
        return (MODE_ERROR_1);
    }
    return (-1);
}

short DECLARE1(uncomp_mode_type, long, prefix) {
    short code;
    short len;
    switch (prefix) {
    case 0x18000L:
        return (UNCOMP_RUN1);
    case 0x24000L:
        return (UNCOMP_RUN2);
    case 0x32000L:
        return (UNCOMP_RUN3);
    case 0x41000L:
        return (UNCOMP_RUN4);
    case 0x50800L:
        return (UNCOMP_RUN5);
    case 0x60400L:
        return (UNCOMP_RUN6);
    case 0x70200L:
        return (UNCOMP_TRUN0);
    case 0x80100L:
        return (UNCOMP_TRUN1);
    case 0x90080L:
        return (UNCOMP_TRUN2);
    case 0xA0040L:
        return (UNCOMP_TRUN3);
    case 0xB0020L:
        return (UNCOMP_TRUN4);
    }
    code = prefix & 0xffffL;
    len = (prefix >> 16) & 0xf;
    return ((code || len > 10) ? UNCOMP_INVALID : -1);
}

#define BASESTATE(b) ((unsigned char)((b)&0x7))

void build_null_mode_tables() {
    short prefix;

    null_mode_prefix_count = 8;
    for (prefix = 0; prefix < null_mode_prefix_count; prefix++) {
        short byte;
        for (byte = 0; byte < 256; byte++) {
            short firstbit;
            short bit;
            long curprefix;
            char found_code = FALSE;

            if (prefix < 8) {
                curprefix = 0L;
                firstbit = prefix;
            } else {
                curprefix = null_mode_prefix[prefix];
                firstbit = 0;
            }
            for (bit = firstbit; bit < 8 && !found_code; bit++) {
                short mode;

                if (bit_mask[bit] & byte) {
                    curprefix = append_1(curprefix);
                } else {
                    curprefix = append_0(curprefix);
                }
                switch (mode = null_mode_type(curprefix)) {
                case MODE_PASS:
                case MODE_HORIZ:
                case MODE_VERT_V0:
                case MODE_VERT_VR1:
                case MODE_VERT_VR2:
                case MODE_VERT_VR3:
                case MODE_VERT_VL1:
                case MODE_VERT_VL2:
                case MODE_VERT_VL3:
                case MODE_UNCOMP:
                case MODE_ERROR:
                case MODE_ERROR_1:

                    found_code = TRUE;
                    null_mode[prefix][byte] = (unsigned char)mode;
                    null_mode_next_state[prefix][byte] = BASESTATE(bit + 1);
                    break;
                }
            }
            if (!found_code) {
                null_mode_next_state[prefix][byte] = (unsigned char)find_null_mode_prefix(curprefix);

                null_mode[prefix][byte] = MODE_NULL;
            }
        }
    }
}

void build_horiz_mode_tables() {
    unsigned short byte;
    short prefix;

    horiz_mode_prefix_count = 16;
    for (prefix = 0; prefix < horiz_mode_prefix_count; prefix++) {
        for (byte = 0; byte < 256; byte++) {
            short bits_digested = 0;
            short bit;
            short firstbit;
            char color;
            unsigned long curprefix;

            if (prefix < 8) {
                color = WHITE;
                curprefix = 0L;
                firstbit = prefix;
            } else if (prefix < 16) {
                color = BLACK;
                curprefix = 0L;
                firstbit = prefix - 8;
            } else {
                color = horiz_mode_color[prefix];
                curprefix = horiz_mode_prefix[prefix];
                firstbit = 0;
            }
            for (bit = firstbit; bit < 8 && !bits_digested; bit++) {
                if (bit_mask[bit] & byte) {
                    curprefix = append_1(curprefix);
                } else {
                    curprefix = append_0(curprefix);
                }

                if (curprefix == 0xC0000L) {
                    curprefix = 0xB0000L;
                }
                if (color == WHITE) {
                    short runlength = white_run_length(curprefix);

                    if (runlength == G3CODE_INVALID) {
                        horiz_mode[prefix][byte] = (unsigned char)ACT_INVALID;
                        horiz_mode_next_state[prefix][byte] = (unsigned char)bit;
                        bits_digested = bit + 1;
                    } else if (runlength == G3CODE_EOL) {
                        horiz_mode[prefix][byte] = (unsigned char)ACT_EOL;
                        horiz_mode_next_state[prefix][byte] = BASESTATE(bit + 1);
                        bits_digested = bit + 1;
                    } else if (runlength != G3CODE_INCOMP) {
                        horiz_mode[prefix][byte] = (unsigned char)horiz_mode_code_white(runlength);
                        horiz_mode_next_state[prefix][byte] = BASESTATE(bit + 1);
                        bits_digested = bit + 1;
                    }
                } else {
                    short runlength = black_run_length(curprefix);

                    if (runlength == G3CODE_INVALID) {
                        horiz_mode[prefix][byte] = (unsigned char)ACT_INVALID;
                        horiz_mode_next_state[prefix][byte] = (unsigned char)(bit + 8);
                        bits_digested = bit + 1;
                    } else if (runlength == G3CODE_EOL) {
                        horiz_mode[prefix][byte] = (unsigned char)ACT_EOL;
                        horiz_mode_next_state[prefix][byte] = BASESTATE(bit + 1);
                        bits_digested = bit + 1;
                    } else if (runlength != G3CODE_INCOMP) {
                        horiz_mode[prefix][byte] = (unsigned char)horiz_mode_code_black(runlength);
                        horiz_mode_next_state[prefix][byte] = BASESTATE(bit + 1);
                        bits_digested = bit + 1;
                    }
                }
            }
            if (!bits_digested) {
                horiz_mode[prefix][byte] = (unsigned char)ACT_INCOMP;
                horiz_mode_next_state[prefix][byte] = (unsigned char)find_horiz_mode_prefix(curprefix, color);
            }
        }
    }
}

void build_uncomp_mode_tables() {
    short prefix;

    uncomp_mode_prefix_count = 8;
    for (prefix = 0; prefix < uncomp_mode_prefix_count; prefix++) {
        short byte;
        for (byte = 0; byte < 256; byte++) {
            short firstbit;
            short bit;
            long curprefix;
            char found_code = FALSE;

            if (prefix < 8) {
                curprefix = 0L;
                firstbit = prefix;
            } else {
                curprefix = uncomp_mode_prefix[prefix];
                firstbit = 0;
            }
            for (bit = firstbit; bit < 8 && !found_code; bit++) {
                short mode;

                if (bit_mask[bit] & byte) {
                    curprefix = append_1(curprefix);
                } else {
                    curprefix = append_0(curprefix);
                }
                mode = uncomp_mode_type(curprefix);
                if (mode != -1) {

                    found_code = TRUE;
                    uncomp_mode[prefix][byte] = (unsigned char)mode;
                    uncomp_mode_next_state[prefix][byte] = BASESTATE(bit + 1);
                    break;
                }
            }
            if (!found_code) {
                uncomp_mode_next_state[prefix][byte] = (unsigned char)find_uncomp_mode_prefix(curprefix);

                uncomp_mode[prefix][byte] = UNCOMP_INCOMP;
            }
        }
    }
}
