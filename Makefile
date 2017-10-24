CC?=gcc
LLVM_CONFIG=llvm-config-3.9
CFLAGS=$(shell $(LLVM_CONFIG) --cflags)
LDFLAGS=$(shell $(LLVM_CONFIG) --libs --cflags --ldflags orcjit)

fac: fac.o
	$(CC) fac.o $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -rf fac.o fac

.PHONY: clean
