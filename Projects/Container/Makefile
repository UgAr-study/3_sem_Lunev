CC = gcc
CFLAGS = -c -MD -g -Wall
INCLUDES = -I./include -I./Test
TSRC = Test/main.c Test/tester.c
LSRC = src/RBTree.c
TOBJ = $(TSRC:.c=.o)
LOBJ = $(LSRC:.c=.o)
LIBNAME = mlib.a
LIBPATH = ./
EXEC = main.out

ifeq ($(TEST), true)
	DEFINES = -DTEST
else
	DEFINES =
endif

build: $(LIBNAME)

$(LIBNAME): $(LOBJ)
	ar rcs $@ $<

u_test: $(EXEC)

$(EXEC): u_test.o
	$(CC) -o $@ $<

u_test.o: $(TOBJ) $(LIBNAME)
	ld -r -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $<

clean:
	rm -rf $(LOBJ) $(TOBJ) $(EXEC) $(LIBNAME) u_test.o u_test

-include $(TOBJ:.o=.d) $(LOBJ:.o=.d)

.PHONY: clean build u_test