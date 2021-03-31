CC = gcc
CFLAGS = -c -MD -g -Wall
THREADFLAG = -pthread
SRC = main.c calc.c cpu_info.c
OBJ = $(SRC:.c=.o)
EXEC = calc

all: $(EXEC)

run:
	bash ./test

$(EXEC): $(OBJ)
	$(CC) $(THREADFLAG) -o $@ $^

%.o: %.c
	$(CC) $(THREADFLAG) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(OBJ) $(EXEC) $(OBJ:.o=.d)

-include $(OBJ:.o=.d)

.PHONY: clean run