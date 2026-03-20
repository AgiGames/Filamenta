# Makefile for Filamenta

# Makefile for Filamenta

CC = nvcc
TARGET = filamenta

SRC = main.cu grid/grid.cu helper/helper.cu
OBJ = $(SRC:.cu=.o)

CFLAGS = -std=c++17 -arch=sm_75
HOSTFLAGS = -Wall -D_POSIX_C_SOURCE=199309L

# Detect OS
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

ifeq ($(UNAME_S), Darwin)
	BREW_PATH = $(shell brew --prefix)
	CFLAGS += -I$(BREW_PATH)/include
	LDFLAGS = -L$(BREW_PATH)/lib
	LIBS = -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework OpenGL
endif

# =========================
# Build rules
# =========================

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS) $(LIBS)

%.o: %.cu
	$(CC) $(CFLAGS) -Xcompiler "$(HOSTFLAGS)" -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean run
