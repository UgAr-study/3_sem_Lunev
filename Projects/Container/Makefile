CC = gcc
CFLAGS = -c -g -Wall
DEFINES = -DTEST
SOURCES = Test/main.c Test/tester.c src/RBTree.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = main.out


all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC)  $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ $<

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
