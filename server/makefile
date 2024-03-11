CC := cc
CFLAGS := -Wall -Werror -pedantic 

SRC := main.c address.c active.c routines.c unsdb.c bindsock.c 

OBJ := $(SRC:.c=.o)

PRE := $(SRC:.c=.i)

EXEC := tserv

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

clean: 
	rm $(OBJ)
