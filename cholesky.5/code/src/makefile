TARGET = cholesky
OBJS = amal.o assign.o bfac.o bksolve.o block2.o fo.o mf.o numLL.o \
     parts.o seg.o solve.o tree.o util.o malloc.o 

CFLAGS += -w -fcommon
LDFLAGS += -lpthread -lm

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf *.o $(TARGET)

.SUFFIXES:
.SUFFIXES:	.o .c .C .h .H

.c.o:
	$(CC) -c $(CFLAGS) $*.c
