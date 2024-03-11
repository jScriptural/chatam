CC := cc
CFLAGS := -Wall -Werror -pedantic

SRC := main.c unsdb.c address.c routines.c 
OBJ := $(SRC:.c=.o)

EXEC := tellam

$(EXEC) : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

clean:
	rm $(OBJ)
