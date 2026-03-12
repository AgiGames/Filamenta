# Makefile for Filamenta

CC = gcc
CFLAGS = -D_POSIX_C_SOURCE=199309L -Wall -std=c99
SRC = main.c grid/grid.c helper/helper.c
OBJ = $(SRC:.c=.o)
TARGET = filamenta

# Detect OS
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

ifeq ($(UNAME_S), Darwin)
	# macOS paths for Homebrew
	BREW_PATH = $(shell brew --prefix)
	CFLAGS += -I$(BREW_PATH)/include
	LDFLAGS = -L$(BREW_PATH)/lib
	LIBS = -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
endif

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run
