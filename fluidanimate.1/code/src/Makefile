PREFIX=.
TARGET=fluidanimate

version := pthreads

CXXFLAGS += -Wno-invalid-offsetof

all:
	$(MAKE) -f Makefile.${version} all

clean:
	$(MAKE) -f Makefile.${version} clean

install:
	mkdir -p $(PREFIX)/bin
	cp -f $(TARGET) $(PREFIX)/bin
	cp -f fluidcmp $(PREFIX)/bin
