.PHONY: all install clean

LDFLAGS += -pthread
CFLAGS += -pthread
CXXFLAGS += -pthread

ifneq ("$(wildcard racebench.c)","")
  RB_SRCS += racebench.c
endif
ifneq ("$(wildcard racebench_bugs.c)","")
  RB_SRCS += racebench_bugs.c
endif 
RB_OBJS := $(addsuffix .o,$(basename $(RB_SRCS)))

TARGET := $(shell cd src && ./rb-build binary)

O_LINKER := $(CXX)
O_LINK_ARGS := "-Wno-deprecated" "-fopenmp" "-O2" "fpgrowth.o" "data.o" "fp_tree.o" "buffer.o" "fsout.o" "fp_node.o" "wtime.o" "-o" "freqmine" $(CXXFLAGS)
O_WORKDIR := .

RB_CFLAGS := $(CFLAGS) -std=c99

all: $(RB_OBJS)
	cd src && ./rb-build config
	cd src && ( ./rb-build build 2>&1 || true )
	cd src && cd $(O_WORKDIR) && $(O_LINKER) $(abspath $(RB_OBJS)) $(LDFLAGS) $(O_LINK_ARGS)

$(RB_OBJS): $(RB_SRC)
	$(CC) $(RB_CFLAGS) $(subst .o,.c,$@) -c -o $@  

install:
	mkdir -p racebench 
	cp src/$(TARGET) racebench/

clean:
	rm -rf $(RB_OBJS)
	rm -rf racebench
	cd src && ./rb-build clean
