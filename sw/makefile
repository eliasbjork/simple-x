TARGET ?= hello_uart.c
TARGET_C = $(basename $(TARGET)).c
TARGET_ELF = $(basename $(TARGET)).elf

TOOLCHAIN_PREFIX ?= riscv64-unknown-elf-
CC = $(TOOLCHAIN_PREFIX)gcc
AS = $(CC) $(CFLAGS) -c

CFLAGS = \
	-march=rv32im_zicsr \
	-mabi=ilp32 \
	-nostartfiles \
	-nostdlib \
	-std=c18 \
	-Os \
	-Wall \
	-g \
	$(DEFINES)

LDFLAGS = \
	-Tcrt/link.ld \
	-lgcc

OBJS = \
	crt/crt.o \
	lib/uartio.o \
	ext/printf/printf.o

DEFINES = \
	-DPRINTF_DISABLE_SUPPORT_LONG_LONG \
	-DPRINTF_DISABLE_SUPPORT_PTRDIFF_T \
	-DPRINTF_DISABLE_SUPPORT_EXPONENTIAL


.PHONY: all
all: $(TARGET_ELF)

$(TARGET_ELF): $(TARGET_C) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f *.o **/*.o *.elf **/*.elf *.ub **/*.ub ext/printf/printf.o
