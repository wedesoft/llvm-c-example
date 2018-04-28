CC?=gcc
LLVM_CONFIG=llvm-config-3.9
CFLAGS=$(shell $(LLVM_CONFIG) --cflags)
LDFLAGS=$(shell $(LLVM_CONFIG) --libs --cflags --ldflags orcjit)

all: fac minimal branch select

fac: fac.o
	$(CC) $< $(LDFLAGS) -o $@

minimal: minimal.o
	$(CC) $< $(LDFLAGS) -o $@

branch: branch.o
	$(CC) $< $(LDFLAGS) -o $@

select: select.o
	$(CC) $< $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -rf *.o fac minimal branch select

.PHONY: clean
