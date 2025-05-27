TOOLCHAIN_PREFIX ?= riscv-none-elf-
BOARD ?= nexys_video

WORKSPACE ?= $(shell pwd)
VEERWOLF_ROOT ?= $(WORKSPACE)/fusesoc_libraries/veerwolf
VEERWOLF_SW = $(VEERWOLF_ROOT)/sw
VEERWOLF_DATA = $(VEERWOLF_ROOT)/data

CC = $(TOOLCHAIN_PREFIX)gcc
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump
GDB = $(TOOLCHAIN_PREFIX)gdb

CFLAGS = -g -Wall
LDFLAGS = -g

TARGET ?= $(VEERWOLF_SW)/blinky.vh
TARGET_ELF = $(basename $(TARGET)).elf
# uImage to flash
TARGET_UB = $(basename $(TARGET)).ub
# symlink to the last program that was flashed
LAST_FLASHED_ELF = temp/last_flashed.elf

RESET_VECTOR = 0x0


.PHONY: synth flash program debug gdb objdump clean

# ensures the intermediate elf file is not deleted by make as it is needed by e.g. gdb
.PRECIOUS: $(TARGET_ELF)

#all: synth program debug

# probably not needed as this is the default for --bootrom_file
$(VEERWOLF_SW)/bootloader.vh:
	make -C $(VEERWOLF_SW) TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX) bootloader.vh

synth: $(VEERWOLF_SW)/bootloader.vh
	fusesoc run --build --target=$(BOARD) --flag=cpu_el2 veerwolf --bootrom_file=$(VEERWOLF_SW)/bootloader.vh

flash: $(TARGET_UB)
	openocd -c "set BINFILE $(TARGET_UB)" -f $(VEERWOLF_DATA)/veerwolf_$(BOARD)_write_flash.cfg
	mkdir -p temp
	ln -sf $(shell realpath --relative-to=temp $(TARGET_ELF)) $(LAST_FLASHED_ELF)

program:
	openocd -f $(VEERWOLF_DATA)/veerwolf_$(BOARD)_program.cfg

debug: program
	openocd -f $(VEERWOLF_DATA)/veerwolf_$(BOARD)_debug.cfg

$(LAST_FLASHED_ELF):
	@echo "The device must be flashed before this tool can be used. Flash by running:"
	@echo "	make flash"
	@exit 1

gdb: $(LAST_FLASHED_ELF)
	$(GDB) --command=openocd.gdb $(LAST_FLASHED_ELF)


objdump: $(LAST_FLASHED_ELF)
	$(OBJDUMP) -D $(LAST_FLASHED_ELF)


%.elf: %.S
	$(CC) $(CFLAGS) -nostartfiles -march=rv32im_zicsr -mabi=ilp32 -T$(VEERWOLF_SW)/link.ld -o $@ $<
%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
%.ub: %.bin
	mkimage \
	-A riscv \
	-C none \
	-T standalone \
	-a 0x0 \
	-e $(RESET_VECTOR) \
	-n '$@' \
	-d $< \
	$@

clean:
	rm -rf build/ temp/ *.jou *.log .Xil/
	make -C $(VEERWOLF_SW) TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX) clean