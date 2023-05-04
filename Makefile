CC= gcc
INTERMEZZO=./files/intermezzo.bin
CFLAGS= -Wall -Wpedantic -Wextra -std=gnu99 -Os
OBJ= exploit.o usb.o
TARGET= fusee-nano
SRCDIR= ./src
PREFIX=/usr/bin

.PHONY: clean install

all: $(TARGET)

%.o: $(SRCDIR)/%.c $(SRCDIR)/intermezzo.h
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

src/intermezzo.h: $(INTERMEZZO)
	xxd -n intermezzo -i $< > $@

clean:
	rm -f $(TARGET) $(OBJ) $(SRCDIR)/intermezzo.h

install: $(TARGET)
	install -Dm755 $(TARGET) $(PREFIX)
