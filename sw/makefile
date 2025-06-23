TARGET ?= hello_uart.c
TARGET_C = $(basename $(TARGET)).c
TARGET_ELF = $(basename $(TARGET)).elf

TOOLCHAIN_PREFIX ?= riscv64-unknown-elf-
CC = $(TOOLCHAIN_PREFIX)gcc
AS = $(CC) $(CFLAGS) -c

CFLAGS = -march=rv32im_zicsr -mabi=ilp32 -nostartfiles -nostdlib -Os -Wall -g
LDFLAGS = -Tcrt/link.ld

OBJS =	crt/crt.o \
		lib/uartio.o

.PHONY: all clean

all: $(TARGET_ELF)

$(TARGET_ELF): $(TARGET_C) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@ $<

clean:
	rm -f **/*.o **/*.elf
