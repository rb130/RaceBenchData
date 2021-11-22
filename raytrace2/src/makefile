TARGET = raytrace

OBJS = bbox.o cr.o env.o fbuf.o geo.o huprn.o husetup.o hutv.o isect.o \
	main.o matrix.o memory.o poly.o raystack.o shade.o sph.o trace.o \
	tri.o workpool.o
LDFLAGS += -lpthread -lm
CFLAGS += -fcommon -std=c99 -w 

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET) $(LDFLAGS)


clean:
	rm -rf *.o $(TARGET)


.SUFFIXES:
.SUFFIXES:      .o .c .C .h .H

.c.o:
	$(CC) -c $(CFLAGS) $*.c
