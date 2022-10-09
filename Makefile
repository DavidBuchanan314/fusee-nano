CC= gcc
INTERMEZZO= ./files/intermezzo.bin
CFLAGS= -Wall -Wpedantic -Wextra -std=c99 -Os -DINTERMEZZO_PATH=\"$(INTERMEZZO)\"
OBJ= exploit.o usb.o
TARGET= fusee-nano
SRCDIR= ./src/
APPDIR= AppDir
.PHONY: clean

all: $(TARGET)

%.o: $(SRCDIR)%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	@rm -rf  $(TARGET) $(OBJ)
appimg: $(TARGET)
	@mkdir -p "$(APPDIR)"
	@mkdir -p "$(APPDIR)/files"
	cp $(TARGET) $(APPDIR)
	cp ./files/*.bin "$(APPDIR)/files"
	appimage-builder
dist: appimg
	tar -czvf latest.tgz files/*.bin *.AppImage
dist-clean: clean
	@rm -rf $(APPDIR) latest.tgz appimage-build *.AppImage
