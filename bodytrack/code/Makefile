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
O_LINK_ARGS := "-std=c++98" "-fexceptions" "-fpermissive" "-Wall" "-Wno-unknown-pragmas" "-o" "bodytrack" "AnnealingFactor.o" "BodyGeometry.o" "BodyPose.o" "CameraModel.o" "CovarianceMatrix.o" "ImageMeasurements.o" "ImageProjection.o" "RandomGenerator.o" "TrackingModel.o" "main.o" "TrackingModelPthread.o" "AsyncIO.o" "../FlexImageLib/.libs/libflex.a" "-lm" "threads/.libs/libthreads.a" "-lpthread"
O_WORKDIR := TrackingBenchmark

LDFLAGS += -pthread
CFLAGS += -pthread

all: $(RB_OBJS)
	cd src && ./rb-build config
	cd src && ( ./rb-build build || true )
	cd src && cd $(O_WORKDIR) && $(O_LINKER) $(abspath $(RB_OBJS)) $(LDFLAGS) $(O_LINK_ARGS)

$(RB_OBJS): $(RB_SRC)
	$(CC) $(CFLAGS) $(subst .o,.c,$@) -c -o $@  

install:
	mkdir -p racebench 
	cp src/$(TARGET) racebench/
	cp -r sequenceB_1 racebench/

clean:
	rm -rf $(RB_OBJS)
	rm -rf racebench
	cd src && ./rb-build clean
