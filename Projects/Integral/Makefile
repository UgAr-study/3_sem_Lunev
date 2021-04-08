CC = gcc -std=c99
CFLAGS = -c -MD -g -Wall
THREADFLAG = -pthread
SRC = main.c calc.c pthread_manager.c
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