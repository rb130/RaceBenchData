TARGET = volrend
OBJS = main.o file.o option.o map.o normal.o opacity.o octree.o view.o \
       render.o adaptive.o raytrace.o
CFLAGS += -w -DPRESHD $(CDEBUG) -Ilibtiff -fcommon
CCOPTS = -S -I -I$(SIMDIR) -I$(IDIR) -I/usr/include -DAUG $(CFLAGS)
LDFLAGS += -w -pthread -lm -Ilibtiff -Llibtiff -ltiff
mylibtiff = libtiff.a

all: $(mylibtiff) $(TARGET)
	@echo Done

$(mylibtiff):
	make -C libtiff

$(TARGET): $(OBJS)  
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET) $(LDFLAGS)


clean:
	rm -rf *.o $(TARGET)
	make -C libtiff clean

.SUFFIXES:
.SUFFIXES:	.o .c .C .h .H

.c.o:	
	$(CC) -c $(CFLAGS) $*.c
