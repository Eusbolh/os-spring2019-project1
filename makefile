INC_DIR=src/headers
SRC_DIR=src
OBJ_DIR=out
CC=gcc
CFLAGS=-I$(INC_DIR)

obj-m	:= llamas.o
 
MODULEDIR = src/kernel_module
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

all: prepare shelldon kernel_module

prepare:
	mkdir -p out

shelldon: $(SRC_DIR)/shelldon.c $(SRC_DIR)/commands/codesearch.c $(SRC_DIR)/commands/birdakika.c $(SRC_DIR)/commands/pomodoro.c
	$(CC) -o $(OBJ_DIR)/shelldon $(SRC_DIR)/shelldon.c $(SRC_DIR)/commands/codesearch.c $(SRC_DIR)/commands/birdakika.c $(SRC_DIR)/commands/pomodoro.c $(CFLAGS)

kernel_module:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)/src/kernel_module modules
	rm -rf $(MODULEDIR)/*.o $(MODULEDIR)/*~ core $(MODULEDIR)/.depend $(MODULEDIR)/.*.cmd $(MODULEDIR)/*.mod.c $(MODULEDIR)/.tmp_versions $(MODULEDIR)/.cache.mk $(MODULEDIR)/*.symvers $(MODULEDIR)/*.order
	mv src/kernel_module/llamas.ko out