### board config

BOARD ?= nexys_video
FLASH_ADDR ?= 0x0
RESET_VECTOR ?= 0x0

### files

WORKSPACE ?= $(shell pwd)
VEERWOLF_ROOT ?= $(WORKSPACE)/fusesoc_libraries/veerwolf
VEERWOLF_SW = $(VEERWOLF_ROOT)/sw
VEERWOLF_DATA = $(VEERWOLF_ROOT)/data
TARGET ?= sw/hello_world.c
TARGET_ELF = $(basename $(TARGET)).elf
TARGET_HEX = $(basename $(TARGET)).hex
TARGET_SYM = $(basename $(TARGET)).sym
TEST = $(notdir $(basename $(TARGET)))
EL2_ROOT = $(WORKSPACE)/hw/Cores-VeeR-EL2
SIMPLEX_TESTCASE ?= $(WORKSPACE)/sw/simplex/test/4/8/20008/i

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
NM = $(TOOLCHAIN_PREFIX)nm

# build software for veerwolf or el2sim
PLATFORM ?= veerwolf

# build software with or without zfinx extension
ZFINX ?= 0

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
	$(MAKE) -C sw TARGET=../$@ TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX) PLATFORM=$(PLATFORM) ZFINX=$(ZFINX)

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.ub: %.bin
	mkimage -A riscv -C none -T standalone -a $(FLASH_ADDR) -e $(RESET_VECTOR) -n '$@' -d $< $@

%.hex: %.elf
	$(OBJCOPY) -O verilog $< $@

%.sym: %.elf
	$(NM) -B -n $< > $@


### simulate EL2

.PHONY: el2sim
el2sim: $(TARGET_HEX) $(TARGET_SYM)
	cp $(TARGET_HEX) $(EL2_ROOT)/program.hex
	cp $(TARGET_SYM) $(EL2_ROOT)/$(TEST).sym
	$(MAKE) -C $(EL2_ROOT) -f tools/Makefile verilator TEST=$(TEST) RV_ROOT=$(EL2_ROOT) SIMPLEX_TESTCASE=$(SIMPLEX_TESTCASE) SKIP_HEX_BUILD=1 debug=1


### tools

.PHONY: gdb
gdb: $(LAST_FLASHED_ELF)
	@$(GDB) --command=openocd.gdb $(LAST_FLASHED_ELF)

.PHONY: objdump
objdump: $(LAST_FLASHED_ELF)
	@$(OBJDUMP) -D $(LAST_FLASHED_ELF)


### misc

$(VEERWOLF_SW)/bootloader.vh:
	$(MAKE) -C $(VEERWOLF_SW) TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX) bootloader.vh

# needed by gdb
$(LAST_FLASHED_ELF):
	@echo "The device must be flashed before this tool can be used. Flash by running:"
	@echo "	make flash"
	@exit 1

.PHONY: clean
clean:
	$(MAKE) -C $(VEERWOLF_SW) TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX) clean
	find . -name '*.o'   -not -path "./hw/*" -delete
	find . -name '*.out' -not -path "./hw/*" -delete
	find . -name '*.elf' -not -path "./hw/*" -delete
	find . -name '*.ub'  -not -path "./hw/*" -delete
	find . -name '*.hex' -not -path "./hw/*" -delete
	find . -name '*.sym' -not -path "./hw/*" -delete
	rm -rf .temp/

.PHONY: clean_el2sim
clean_el2sim:
	$(MAKE) -C $(EL2_ROOT) -f tools/Makefile clean

.PHONY: clean_all
clean_all: clean clean_el2sim
	rm -rf build/ *.jou *.log .Xil/