# If DEBUG = 1, then the program will build with debugging symbols.
# If DEBUG = 0, then it will not.
DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -ggdb
else
	CFLAGS += -DNDEBUG -O3
endif

SRC_DIR=src
INC_DIR=include

CC=gcc
CFLAGS += -Wall -Werror -I$(INC_DIR)
NAME=forwarder.out
LINKS=-lpthread

SRC := main.c res.c io.c net.c
OBJ := $(SRC:.c=.o)

.PHONY: default clean

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LINKS)

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -f *.o *.log $(NAME) $(DEBUGNAME)
