CFLAGS = -pedantic -Wall -std=c++17
CPP = g++
RM = rm
CP = cp
MKDIR = mkdir
EXE = snrub-editor
SRC = src
OBJ = obj
BIN = bin
RES = res
PREFIX = /usr/local
EXEC_PREFIX = $(PREFIX)
BINDIR = $(EXEC_PREFIX)/bin
DATADIR = $(PREFIX)/share
OBJS = $(OBJ)/main.o \
	$(OBJ)/Document.o \
	$(OBJ)/FontSheet.o \
	$(OBJ)/SpriteSheet.o \
	$(OBJ)/Texture.o \
	$(OBJ)/Common.o

.PHONY: all clean install uninstall

all: $(BIN)/$(EXE)

clean:
	$(RM) -f $(BIN)/$(EXE) $(OBJS)

install: $(BIN)/$(EXE)
	$(CP) $(BIN)/$(EXE) $(BINDIR)/$(EXE)
	$(MKDIR) -p $(DATADIR)/$(EXE)/$(RES)
	$(CP) -r $(RES) $(DATADIR)/$(EXE)

uninstall:
	$(RM) -f $(BINDIR)/$(EXE)
	$(RM) -fr $(DATADIR)/$(EXE)

$(BIN)/$(EXE): $(OBJS)
	$(MKDIR) -p $(BIN)
	$(CPP) $(CFLAGS) -o $@ $^ -lSDL2

$(OBJ)/%.o: $(SRC)/%.cpp
	$(MKDIR) -p $(OBJ)
	$(CPP) $(CFLAGS) -c $< -o $@
