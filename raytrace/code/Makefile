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
O_LINK_ARGS := "-fexceptions" "-fno-strict-aliasing" "-fno-align-labels" "-DNDEBUG" "-D_MM_NO_ALIGN_CHECK" "-rdynamic" "CMakeFiles/rtview.dir/rtview.o" "CMakeFiles/rtview.dir/__/RTTL/common/MapOptions.o" "-o" "../bin/rtview" "-L../lib" "../lib/libRTTL.a" "../lib/libLRT.a" "-lglut" "-lGLU" "-lGL" "-lstdc++" "-lrt" "../lib/libRTTL.a" "-lstdc++" "-lpthread" "-lrt" $(CXXFLAGS)
O_WORKDIR := MiniView

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