TARGET ?= hello_uart.c

ifeq ($(notdir $(TARGET)), simplex.elf)
TARGET_OBJS = simplex/$(notdir $(basename $(TARGET)).o) simplex/util.o
else
TARGET_OBJS = $(basename $(TARGET)).o
endif

TARGET_ELF = $(basename $(firstword $(TARGET))).elf

TOOLCHAIN_PREFIX ?= riscv64-unknown-elf-
CC = $(TOOLCHAIN_PREFIX)gcc
AS = $(CC) $(CFLAGS) -c

CFLAGS = \
	-march=rv32im_zicsr \
	-mabi=ilp32 \
	-nostartfiles \
	-nostdlib \
	-std=c18 \
	-Wall \
	-g \
	$(DEFINES)

LDFLAGS = \
	-Tcrt/link.ld \
	-static-libgcc -lgcc

OBJS = \
	crt/crt.o \
	lib/fmath.o \
	lib/mem.o \
	lib/uartio.o \
	ext/printf/printf.o \
	ext/mini-scanf/c_scan.o

DEFINES = \
	-DPRINTF_DISABLE_SUPPORT_LONG_LONG \
	-DPRINTF_DISABLE_SUPPORT_PTRDIFF_T \
	-DPRINTF_DISABLE_SUPPORT_EXPONENTIAL


.PHONY: all
all: $(TARGET_ELF)

$(TARGET_ELF): $(TARGET_OBJS) $(OBJS)
	$(CC) $(CFLAGS) $(TARGET_OBJS) $(OBJS) $(LDFLAGS) -o $@

.PHONY: clean
clean:
	find . -name '*.o' -delete
	find . -name '*.elf' -delete
	find . -name '*.ub' -delete
