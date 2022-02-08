CSRC    = $(SRC)
TARGET  = blackscholes
MACROS  = c.m4.pthreads

SRC	= blackscholes.c

M4_SRC    = blackscholes.m4.cpp
CSRC      = $(M4_SRC)
MT        = -DENABLE_THREADS
CXXFLAGS += -pthread

OBJ	= blackscholes.o


# Default build single precision version
NCO     = -DNCO=4

CXXFLAGS += $(MT) $(NCO)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $(OBJ) -o $(TARGET)

$(OBJ): $(CSRC)
	$(CXX) $(CXXFLAGS) $(CSRC) -o $(OBJ) -c

clean:
	rm -f $(TARGET) $(OBJ)
