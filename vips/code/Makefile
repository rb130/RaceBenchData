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
O_LINK_ARGS := "-g" "-O2" "-o" "vips" "vips.o" "-DG_DISABLE_CAST_CHECKS" "-pthread" "-I/usr/lib/glib-2.0/include" "-I/usr/include/libxml2" "-I/usr/include/glib-2.0" "-pthread" "-Wl,--export-dynamic" "-pthread" "../../libvips/.libs/libvips.a" "-lgthread-2.0" "-lgmodule-2.0" "-lxml2" "-lgobject-2.0" "-lglib-2.0" "-lm" "-pthread" $(CFLAGS)
O_WORKDIR := tools/iofuncs

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

clean:
	rm -rf $(RB_OBJS)
	rm -rf racebench
	cd src && ./rb-build clean