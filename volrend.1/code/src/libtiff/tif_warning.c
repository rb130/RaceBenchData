

#ifndef lint
static char rcsid[] = "$Header: /cvs/bao-parsec/ext/splash2/apps/volrend/src/libtiff/tif_warning.c,v 1.1.1.1 2012/03/29 17:22:40 uid42307 Exp $";
#endif

#include <stdio.h>
#include "tiffioP.h"
#include "prototypes.h"

static void DECLARE3(defaultHandler, char *, module, char *, fmt, va_list, ap) {
    if (module != NULL) {
        fprintf(stderr, "%s: ", module);
    }
    fprintf(stderr, "Warning, ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}

static TIFFErrorHandler _warningHandler = defaultHandler;

TIFFErrorHandler DECLARE1(TIFFSetWarningHandler, TIFFErrorHandler, handler) {
    TIFFErrorHandler prev = _warningHandler;
    _warningHandler = handler;
    return (prev);
}

void
#if USE_PROTOTYPES
TIFFWarning(char *module, char *fmt, ...)
#else

    TIFFWarning(module, fmt, va_alist) char *module;
char *fmt;
va_dcl
#endif
{
    if (_warningHandler) {
        va_list ap;
        VA_START(ap, fmt);
        (*_warningHandler)(module, fmt, ap);
        va_end(ap);
    }
}
