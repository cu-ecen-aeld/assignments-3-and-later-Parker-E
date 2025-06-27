CC = gcc
CROSS_CC = aarch64-none-linux-gnu-gcc
CFLAGS = -Wall -g
TARGET = aesdsocket
SRC = aesdsocket.c

ifeq ($(CROSS-COMPILE), aarch64-none-linux-gnu-)
	CC := $(CROSS_CC)
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean
