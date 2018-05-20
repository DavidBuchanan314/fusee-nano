CC= gcc
CFLAGS= -Wall -Wpedantic -Wextra -std=c99 -Os
OBJ= exploit.o usb.o
TARGET= fusee-nano
SRCDIR= ./src/

.PHONY: clean

all: $(TARGET)

%.o: $(SRCDIR)%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm $(TARGET) $(OBJ)
