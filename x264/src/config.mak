prefix=/usr/local
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
libdir=${exec_prefix}/lib
includedir=${prefix}/include
ARCH=X86_64
SYS=LINUX
CC=gcc
CFLAGS=-g  -Wall -I.  -DHAVE_MALLOC_H -DARCH_X86_64 -DSYS_LINUX -DHAVE_PTHREAD -fPIC
ALTIVECFLAGS=
LDFLAGS=  -lm -lpthread -Wl,-Bsymbolic
AS=
ASFLAGS=-f elf -m amd64 -DPIC
EXE=
VIS=no
HAVE_GETOPT_LONG=1
DEVNULL=/dev/null
ECHON=echo -n
CONFIGURE_ARGS= '--enable-debug' '--enable-shared' '--enable-pthread' '--extra-asflags=' '--extra-cflags=' '--extra-ldflags=' '--disable-asm'
SOSUFFIX=so
SONAME=libx264.so.65
SOFLAGS=-Wl,-soname,$(SONAME)
default: $(SONAME)
