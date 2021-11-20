CFLAGS = -pedantic -Wall -std=c++17
CPP = g++
RM = rm
CP = cp
MKDIR = mkdir
EXE = snrub-editor
SRC = src
OBJ = obj
BIN = bin
PREFIX = /usr/local
EXEC_PREFIX = $(PREFIX)
BINDIR = $(EXEC_PREFIX)/bin
OBJS = $(OBJ)/main.o \
	$(OBJ)/Document.o \
	$(OBJ)/FontSheet.o \
	$(OBJ)/SpriteSheet.o \
	$(OBJ)/Common.o

.PHONY: all clean install uninstall

all: $(BIN)/$(EXE)

clean:
	$(RM) -f $(BIN)/$(EXE) $(OBJS)

install: $(BIN)/$(EXE)
	$(CP) $(BIN)/$(EXE) $(BINDIR)/$(EXE)

uninstall:
	$(RM) -f $(BINDIR)/$(EXE)

$(BIN)/$(EXE): $(OBJS)
	$(MKDIR) -p $(BIN)
	$(CPP) $(CFLAGS) -o $@ $^ -lSDL2

$(OBJ)/%.o: $(SRC)/%.cpp
	$(MKDIR) -p $(OBJ)
	$(CPP) $(CFLAGS) -c $< -o $@
