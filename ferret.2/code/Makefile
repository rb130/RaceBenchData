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

O_LINKER := $(CXX)
O_LINK_ARGS := "-Iinclude" "-Lparsec/lib" "parsec/obj/ferret-pthreads.o" "parsec/lib/libcass.a" "parsec/lib/libcassimage.a" "-o" "parsec/bin/ferret-pthreads" "-lrt" "-lm" "-lcass" "-lgsl" "-lgslcblas" "-ljpeg" "-lstdc++" "-lpthread" $(CXXFLAGS)
O_WORKDIR := .

REQUIRED_FLAGS := -pthread -I$(abspath .)

LDFLAGS += $(REQUIRED_FLAGS)
CFLAGS += $(REQUIRED_FLAGS)
CXXFLAGS += $(REQUIRED_FLAGS)

export LDFLAGS
export CFLAGS
export CXXFLAGS

RB_CFLAGS := $(CFLAGS) -std=c99

all: $(RB_OBJS)
	cd src && ./rb-build config
	cd src && ( ./rb-build build 2>&1 || true )
	cd src && cd $(O_WORKDIR) && $(O_LINKER) $(abspath $(RB_OBJS)) $(LDFLAGS) $(O_LINK_ARGS)

$(RB_OBJS): $(RB_SRC)
	$(CC) $(RB_CFLAGS) $(subst .o,.c,$@) -c -o $@  

install:
	mkdir -p racebench 
	cp -f src/$(TARGET) racebench/
	cp -r corel racebench/

clean:
	rm -rf $(RB_OBJS)
	rm -rf racebench
	cd src && ./rb-build clean
