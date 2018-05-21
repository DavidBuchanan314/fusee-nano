CC= gcc
INTERMEZZO= ./files/intermezzo.bin
CFLAGS= -Wall -Wpedantic -Wextra -std=c99 -Os -DINTERMEZZO_PATH=\"$(INTERMEZZO)\"
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
