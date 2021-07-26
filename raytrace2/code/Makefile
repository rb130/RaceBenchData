.PHONY: all install clean

LDFLAGS += -pthread

ifneq ("$(wildcard racebench.c)","")
  RB_SRCS += racebench.c
endif
ifneq ("$(wildcard racebench_bugs.c)","")
  RB_SRCS += racebench_bugs.c
endif 
RB_OBJS := $(addsuffix .o,$(basename $(RB_SRCS)))

TARGET := $(shell cd src && ./rb-build binary)

O_LINKER := $(CC)
O_LINK_ARGS := "bbox.o" "cr.o" "env.o" "fbuf.o" "geo.o" "huprn.o" "husetup.o" "hutv.o" "isect.o" "main.o" "matrix.o" "memory.o" "poly.o" "raystack.o" "shade.o" "sph.o" "trace.o" "tri.o" "workpool.o" "-fcommon" "-std=c99" "-w" "-o" "raytrace" "-lpthread" "-lm" $(CFLAGS)
O_WORKDIR := .

RB_CFLAGS := $(CFLAGS) -pthread -std=c99

all: $(RB_OBJS)
	cd src && ./rb-build config
	cd src && ( ./rb-build build 2>&1 || true )
	cd src && cd $(O_WORKDIR) && $(O_LINKER) $(abspath $(RB_OBJS)) $(LDFLAGS) $(O_LINK_ARGS)

$(RB_OBJS): $(RB_SRC)
	$(CC) $(RB_CFLAGS) $(subst .o,.c,$@) -c -o $@  

install:
	mkdir -p racebench 
	cp -f src/$(TARGET) racebench/
	tar xf input_test.tar --directory=racebench

clean:
	rm -rf $(RB_OBJS)
	rm -rf racebench
	cd src && ./rb-build clean
