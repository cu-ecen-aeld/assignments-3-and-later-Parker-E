CC = gcc
CROSS_CC = aarch64-none-linux-gnu-gcc
CFLAGS = -g -Wall
OBJ = writer.o
SRC = writer.c
EXE = writer

ifeq ($(CROSS-COMPILE), aarch64-none-linux-gnu-)
	CC := $(CROSS_CC)
endif

all: $(EXE)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c -o $(OBJ) $(SRC)

$(EXE): $(OBJ)
	$(CC) -I/ -o $(EXE) $(OBJ)

clean:
	rm -f *.o writer *.elf *.map
