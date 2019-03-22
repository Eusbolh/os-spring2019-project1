INC_DIR=src/headers
SRC_DIR=src
OBJ_DIR=out
CC=gcc
CFLAGS=-I$(INC_DIR)

all: prepare shelldon

prepare:
	mkdir -p out

shelldon: $(SRC_DIR)/shelldon.c $(SRC_DIR)/commands/codesearch.c 
	$(CC) -o $(OBJ_DIR)/shelldon $(SRC_DIR)/shelldon.c $(SRC_DIR)/commands/codesearch.c $(CFLAGS)
