CC?=gcc
LLVM_CONFIG=llvm-config-3.9
CFLAGS=$(shell $(LLVM_CONFIG) --cflags)
LDFLAGS=$(shell $(LLVM_CONFIG) --libs --cflags --ldflags orcjit)

all: fac minimal

fac: fac.o
	$(CC) fac.o $(LDFLAGS) -o $@

minimal: minimal.o
	$(CC) minimal.o $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -rf *.o fac minimal

.PHONY: clean
