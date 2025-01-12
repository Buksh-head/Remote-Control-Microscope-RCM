########################################################################
# LIBRARY SOURCES - MUST BE IN THE SAME FOLDER as main.c (DO NOT CHANGE)
########################################################################

# Set mylib folder path.
# Do not change the MYLIB_PATH variable name.
# ONLY use relative file paths that start with $(SOURCELIB_ROOT)../
# DO NOT USE absolute file paths (e.g. /home/users/myuser/mydir)
# e.g. MYLIBPATH=$(SOURCELIB_ROOT)/../mylib
MYLIB_PATH=$(SOURCELIB_ROOT)/../repo/mylib

# Set folder path with header files to include.
# ONLY use relative file paths that start with $(SOURCELIB_ROOT)../
# DO NOT USE absolute file paths (e.g. /home/users/myuser/mydir)
CFLAGS += -I$(MYLIB_PATH) -I$(SOURCELIB_ROOT)/components/peripherals/nrf24l01plus/nucleo-f429 -DMYCONFIG 

# List all c files locations that must be included (use space as separate 
#e.g. LIBSRCS += path_to/file1.c path_to/file2.c)
# ONLY use relative file paths that start with $(SOURCELIB_ROOT)../
# DO NOT USE absolute file paths (e.g. /home/users/myuser/mydir)
LIBSRCS += $(MYLIB_PATH)/s4743556_rgb.c $(MYLIB_PATH)/s4743556_mfs_led.c $(MYLIB_PATH)/s4743556_hamming.c $(MYLIB_PATH)/s4743556_console.c $(MYLIB_PATH)/s4743556_rcmcont.c $(MYLIB_PATH)/s4743556_txradio.c $(MYLIB_PATH)/s4743556_brd_pb.c $(MYLIB_PATH)/s4743556_rcmdisplay.c $(MYLIB_PATH)/s4743556_ssd.c $(MYLIB_PATH)/s4743556_lta1000g.c $(SOURCELIB_ROOT)/components/os/FreeRTOS/portable/MemMang/heap_2.c $(SOURCELIB_ROOT)/components/peripherals/nrf24l01plus/nucleo-f429/nrf24l01plus.c 
