CSRC    = $(SRC)
TARGET  = blackscholes
MACROS  = c.m4.pthreads

SRC	= blackscholes.c

M4_SRC    = blackscholes.m4.cpp
CSRC      = $(M4_SRC)
MT        = -DENABLE_THREADS
CXXFLAGS += -pthread


# Default build single precision version
NCO     = -DNCO=4

CXXFLAGS += $(MT) $(NCO)

all: $(TARGET)

$(TARGET): clean $(CSRC)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $(CSRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
