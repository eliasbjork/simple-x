TARGET ?= hello_world.c

ifeq ($(notdir $(TARGET)), simplex.elf)
TARGET_OBJS = simplex/$(notdir $(basename $(TARGET)).o) simplex/util.o
else
TARGET_OBJS = $(basename $(TARGET)).o
endif

TARGET_ELF = $(basename $(firstword $(TARGET))).elf

TOOLCHAIN_PREFIX ?= riscv64-unknown-elf-
CC = $(TOOLCHAIN_PREFIX)gcc
AS = $(CC) $(CFLAGS) -c

PLATFORM ?= veerwolf

ifeq ($(PLATFORM), veerwolf)
LINKER_SCRIPT = crt/link.ld
PLATFORM_SPECIFIC_OBJS = \
	crt/crt0.o \
	lib/uartio.o \
	ext/mini-scanf/c_scan.o
PLATFORM_DEFINE = VEERWOLF
else ifeq ($(PLATFORM), el2sim)
LINKER_SCRIPT = el2sim/link.ld
PLATFORM_SPECIFIC_OBJS = \
	el2sim/crt0.o \
	el2sim/hostio.o
PLATFORM_DEFINE = EL2SIM
endif

CFLAGS = \
	-march=rv32imac_zicsr \
	-mabi=ilp32 \
	-nostartfiles \
	-nostdlib \
	-std=c18 \
	-Wall \
	-g \
	$(DEFINES)

LDFLAGS = \
	-T$(LINKER_SCRIPT) \
	-static-libgcc -lgcc

OBJS = \
	$(PLATFORM_SPECIFIC_OBJS) \
	lib/fmath.o \
	lib/mem.o \
	ext/printf/printf.o \

DEFINES = \
	-D$(PLATFORM_DEFINE) \
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
