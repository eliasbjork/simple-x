### board config

BOARD ?= nexys_video
FLASH_ADDR ?= 0x0
RESET_VECTOR ?= 0x0

### files

WORKSPACE ?= $(shell pwd)
VEERWOLF_ROOT ?= $(WORKSPACE)/fusesoc_libraries/veerwolf
VEERWOLF_SW = $(VEERWOLF_ROOT)/sw
VEERWOLF_DATA = $(VEERWOLF_ROOT)/data
TARGET ?= sw/hello_uart.c
TARGET_ELF = $(basename $(TARGET)).elf

# ensures the intermediate elf file is not deleted by make as it is needed by e.g. gdb
.PRECIOUS: $(TARGET_ELF)

# uImage to flash
TARGET_UB = $(basename $(TARGET)).ub

# symlink to the last program that was flashed
LAST_FLASHED_ELF = .temp/last_flashed.elf

### toolchain

TOOLCHAIN_PREFIX ?= riscv64-unknown-elf-
OBJCOPY = $(TOOLCHAIN_PREFIX)objcopy
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump
GDB = gdb-multiarch


#all: synth program debug


### build hardware

.PHONY: synth
synth: $(VEERWOLF_SW)/bootloader.vh
	fusesoc run --build --target=$(BOARD) --flag=cpu_el2 veerwolf --bootrom_file=$(VEERWOLF_SW)/bootloader.vh

.PHONY: flash
flash: $(TARGET_UB)
	openocd -c "set BINFILE $(TARGET_UB)" -f $(VEERWOLF_DATA)/veerwolf_$(BOARD)_write_flash.cfg
	mkdir -p .temp
	ln -sf $(shell realpath --relative-to=.temp $(TARGET_ELF)) $(LAST_FLASHED_ELF)

.PHONY: program
program:
	openocd -f $(VEERWOLF_DATA)/veerwolf_$(BOARD)_program.cfg

.PHONY: debug
debug: program
	openocd -f $(VEERWOLF_DATA)/veerwolf_$(BOARD)_debug.cfg


### build software

%.elf:
	make -C sw TARGET=../$@ TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.ub: %.bin
	mkimage -A riscv -C none -T standalone -a $(FLASH_ADDR) -e $(RESET_VECTOR) -n '$@' -d $< $@


### tools

.PHONY: gdb
gdb: $(LAST_FLASHED_ELF)
	@$(GDB) --command=openocd.gdb $(LAST_FLASHED_ELF)

.PHONY: objdump
objdump: $(LAST_FLASHED_ELF)
	@$(OBJDUMP) -D $(LAST_FLASHED_ELF)


### misc

$(VEERWOLF_SW)/bootloader.vh:
	make -C $(VEERWOLF_SW) TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX) bootloader.vh

# needed by gdb
$(LAST_FLASHED_ELF):
	@echo "The device must be flashed before this tool can be used. Flash by running:"
	@echo "	make flash"
	@exit 1

.PHONY: clean
clean:
	make -C $(VEERWOLF_SW) TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX) clean
	find . -name '*.o' -delete
	find . -name '*.elf' -delete
	find . -name '*.ub' -delete
	rm -rf .temp/

.PHONY: clean_all
clean_all: clean
	rm -rf build/ *.jou *.log .Xil/