TARGET = water_nsquared

OBJS = water.o mdmain.o syscons.o kineti.o bndry.o cnstnt.o initia.o \
       interf.o intraf.o cshift.o poteng.o predcor.o
CFLAGS += -w -fcommon
LDFLAGS += -lm -pthread 


.SUFFIXES:
.SUFFIXES: .o .S .s .c .C .U .h .H

.c.o:
	$(CC) -c $(CFLAGS) $*.c

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf *.o $(TARGET)
