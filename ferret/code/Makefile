.PHONY: all install clean

ifneq ("$(wildcard racebench.c)","")
  RB_SRCS += racebench.c
endif
ifneq ("$(wildcard racebench_bugs.c)","")
  RB_SRCS += racebench_bugs.c
endif 
RB_OBJS := $(addsuffix .o,$(basename $(RB_SRCS)))

TARGET := $(shell cd src && ./rb-build binary)

O_LINKER := $(CXX)
O_LINK_ARGS := "-Iinclude" "-Lparsec/lib" "parsec/obj/ferret-pthreads.o" "parsec/lib/libcass.a" "parsec/lib/libcassimage.a" "-o" "parsec/bin/ferret-pthreads" "-lrt" "-lm" "-lcass" "-lgsl" "-lgslcblas" "-ljpeg" "-lstdc++" "-lpthread"
O_WORKDIR := .

LDFLAGS += -pthread
CFLAGS += -pthread

all: $(RB_OBJS)
	cd src && ./rb-build config
	cd src && ( ./rb-build build 2>&1 || true )
	cd src && cd $(O_WORKDIR) && $(O_LINKER) $(abspath $(RB_OBJS)) $(LDFLAGS) $(O_LINK_ARGS)

$(RB_OBJS): $(RB_SRC)
	$(CC) $(CFLAGS) $(subst .o,.c,$@) -c -o $@  

install:
	mkdir -p racebench 
	cp src/$(TARGET) racebench/
	cp -r corel queries racebench/

clean:
	rm -rf $(RB_OBJS)
	rm -rf racebench
	cd src && ./rb-build clean
