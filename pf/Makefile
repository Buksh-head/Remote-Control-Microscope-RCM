# Add additional project sources like this (ONLY for local files NOT mylib files:
# SRCS += X.c 
#
# all the files will be generated with this name (main.elf, main.bin, main.hex, etc)

#Name of project
PROJ_NAME=main

#Your local source files
SRCS = $(PROJ_NAME).c
CFLAGS += -DENABLE_DEBUG_UART #enable Debug Com Port
###################################################
ROOT=$(shell pwd)

OBJS=$(SRCS:.c=.o)

###################################################

.PHONY: $(shell mkdir -p obj) lib proj all depend
all: proj

#Include freertos makefile
include $(SOURCELIB_ROOT)/components/os/FreeRTOS/filelist.mk

#include nucelo-xxx makefile - NOTE MUST BE LAST TO INCLUDE
include $(SOURCELIB_ROOT)/../repo/pf/filelist.mk			#replace with your stages folder path (e.g. repo/s1)
include $(SOURCELIB_ROOT)/components/boards/nucleo-f429zi/board.mk
include $(SOURCELIB_ROOT)/components/mk/m_launch.mk
include $(SOURCELIB_ROOT)/components/mk/m_common.mk


lib:
	$(MAKE) -C lib

proj: 	$(PROJ_NAME).elf

include .depend
