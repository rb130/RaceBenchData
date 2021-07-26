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
O_LINK_ARGS := "main.o" "file.o" "option.o" "map.o" "normal.o" "opacity.o" "octree.o" "view.o" "render.o" "adaptive.o" "raytrace.o" "-w" "-DPRESHD" "-Ilibtiff" "-fcommon" "-o" "volrend" "-w" "-pthread" "-lm" "-Ilibtiff" "-Llibtiff" "-ltiff" $(CFLAGS)
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
