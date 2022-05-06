.POSIX:
.SUFFIXES:
.PHONY: all clean install uninstall

CFLAGS = -std=c++17 -pedantic -Wall
LDFLAGS =
CPP = g++
RM = rm
CP = cp
MKDIR = mkdir
PREFIX = /usr/local

all: bin/snrub-editor

clean:
	$(RM) -f bin/snrub-editor obj/main.o obj/FontSheet.o obj/SpriteSheet.o obj/Texture.o obj/Document.o obj/Common.o

install: bin/snrub-editor
	$(CP) bin/snrub-editor $(DESTDIR)$(PREFIX)/bin/snrub-editor
	$(MKDIR) -p $(DESTDIR)$(PREFIX)/share/snrub-editor
	$(CP) -r res $(DESTDIR)$(PREFIX)/share/snrub-editor/res

uninstall:
	$(RM) -f $(DESTDIR)$(PREFIX)/bin/snrub-editor
	$(RM) -fr $(DESTDIR)$(PREFIX)/share/snrub-editor

bin/snrub-editor: bin obj obj/main.o obj/FontSheet.o obj/SpriteSheet.o obj/Texture.o obj/Document.o obj/Common.o
	$(CPP) $(LDFLAGS) -o bin/snrub-editor obj/main.o obj/FontSheet.o obj/SpriteSheet.o obj/Texture.o obj/Document.o obj/Common.o -lSDL2

obj/main.o: src/main.cpp obj/FontSheet.o obj/SpriteSheet.o obj/Texture.o obj/Document.o obj/Common.o
	$(CPP) $(CFLAGS) -c src/main.cpp -o obj/main.o

obj/FontSheet.o: src/FontSheet.hpp src/FontSheet.cpp obj/SpriteSheet.o
	$(CPP) $(CFLAGS) -c src/FontSheet.cpp -o obj/FontSheet.o

obj/SpriteSheet.o: src/SpriteSheet.hpp src/SpriteSheet.cpp obj/Texture.o
	$(CPP) $(CFLAGS) -c src/SpriteSheet.cpp -o obj/SpriteSheet.o

obj/Texture.o: src/Texture.hpp src/Texture.cpp obj/Common.o
	$(CPP) $(CFLAGS) -c src/Texture.cpp -o obj/Texture.o

obj/Document.o: src/Document.hpp src/Document.cpp
	$(CPP) $(CFLAGS) -c src/Document.cpp -o obj/Document.o

obj/Common.o: src/Common.hpp src/Common.cpp
	$(CPP) $(CFLAGS) -c src/Common.cpp -o obj/Common.o

bin:
	$(MKDIR) bin

obj:
	$(MKDIR) obj
