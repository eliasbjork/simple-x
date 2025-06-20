# simple-x

This project uses FuseSoC to build a bespoke VeeRwolf SoC based on a bespoke VeeR EL2 core. The sources for these parts are found here
- [VeeRwolf SoC](https://github.com/eliasbjork/VeeRwolf) (forked from [chipsalliance/VeeRwolf](https://github.com/chipsalliance/VeeRwolf))
- [VeeR EL2 core](https://github.com/eliasbjork/Cores-VeeR-EL2) (forked from [chipsalliance/Cores-VeeR-EL2](https://github.com/chipsalliance/Cores-VeeR-EL2))

More details are available in the [VeeRwolf repo README](https://github.com/chipsalliance/VeeRwolf/blob/main/README.md).

## Prerequisites

### RISC-V toolchain

A cross-compilation toolchain is (most likely) needed. On Ubuntu 24.04, the GNU RISC-V toolchain can be installed with
```sh
sudo apt install gcc-riscv64-unknown-elf
```

If using a toolchain with a prefix different from `riscv64-unknown-elf-`, `make` can be invoked by supplying `TOOLCHAIN_PREFIX=<your-toolchain-prefix->` to the command.

### FuseSoC

```sh
pip3 install --upgrade --user fusesoc
```

May not work on Ubuntu  24.04. You can use `pip install --upgrade --user fusesoc --break-system-packages` as a (dangerous) workaround.


### Verilator

Verilator i used by FuseSoC to simulate the system. A version >= 3.918 is required. On Ubuntu 24.04 this can be installed with
```sh
sudo apt install verilator
```

### Vivado (if using an AMD Xilinx board)

FuseSoC uses Vivado to synthesize for AMD Xilinx boards.

### RISC-V OpenOCD

OpenOCD is used to write to flash and debug on the FPGA. As of writing, there is an issue with the `jimtcl` dependency (see [issue #1250](https://github.com/riscv-collab/riscv-openocd/issues/1250)). The current workaround is to use the git submodule (which is to be deprecated), and use the flag `--enable-internal-jimtcl` when configuring OpenOCD.
```shell
git clone https://github.com/riscv/riscv-openocd
cd riscv-openocd
git submodule update --init --recursive
./bootstrap
./configure --enable-jtag_vpi --enable-ftdi --enable-internal-jimtcl
make
sudo make install
```

### U-Boot

U-Boot is used to prepare uImages which can be written to the a board's flash. On Ubuntu 24.04, U-Boot can be installed with
```sh
sudo apt install u-boot-tools
```

## Getting started

### Setting up the FuseSoC workspace

Add the FuseSoC base library to the workspace with
```sh
fusesoc library add fusesoc-cores https://github.com/fusesoc/fusesoc-cores
```
Add the VeeRwolf library with
```sh
fusesoc library add veerwolf https://github.com/eliasbjork/VeeRwolf
```


### Synthesize

To synthesize the design, run
```sh
make synth
```
A `BOARD` parameter can optionally be given to select the target board. Default is `BOARD=nexys_video`.

### Programming the FPGA and booting from flash

Now, we want to program the FPGA and run an executable on the SoC! This is a two-stage rocket. First we will write an executable to the board's flash and then we will program the FPGA with the bitstream of the SoC. The SoC will then boot into a bootloader that loads the program in flash.

Some more details: we are using OpenOCD to write a program to the boards flash. First the FPGA must be programmed with a JTAG-SPI proxy bitstream to enable the OpenOCD `jtagspi` flash driver. The bitstream for Nexys Video is provided in this repo, and bitstreams for other boards can be acquired [here](https://github.com/quartiq/bscan_spi_bitstreams). OpenOCD is then used to write the program to the boards flash at memory address `0`, in the U-Boot uImage binary format. After this is done, the FPGA can be programmed with the bitstream created with `make synth` and the bootloader will boot the flashed binary. This method makes sure that the SoC doesn't need to be resynthesized in order to run a different program, however the FPGA still needs to be reprogrammed.

The entire procedure is structured into two Make rules:

Flash with
```sh
make TARGET=<path/to/program> flash
```
If no `TARGET` is given, a blinky program is flashed. The `TARGET` can be a RISC-V assembly or C source file, or an ELF (okay actually C is not supported by the Makefile yet). More example programs are available in `fusesoc_libraries/veerwolf/sw` (after setting up the FuseSoC workspace).

Program the FPGA with the SoC bitstream

```sh
make program
```
If blinky was flashed, you should now see a blinking LED on the board.

### Debugging on the board

Using OpenOCD and GDB, the SoC and program running on it can be debugged on the FPGA. To open the OpenOCD server which is used to connect to the board, run
```sh
make debug
```

Now, in a new terminal, the OpenOCD client can be connected to the session using `telnet`
```sh
telnet localhost 4444
```
To reset the program counter and resume execution, in the OpenOCD client, type
```OpenOCD
> reg pc 0
> resume
```

#### Connecting GDB to OpenOCD

To connect GDB and start debugging the last program that was flashed, open a new terminal and run
```sh
make gdb
```
To start debugging from the beginning of the program, use OpenOCD over telnet to set `reg pc 0` before running `make gdb` (can also be done while running GDB).

### UART

On Nexys Video you need to connect a second micro-USB cable to the UART port.

The serial port can be monitored e.g. with [`tio`](https://github.com/tio/tio). On Ubuntu 24.04 `tio` can be installed with
```sh
sudo apt install tio
```

The available serial ports can be shown with
```shell
tio --list
```
Look for something including UART and run
```shell
tio -b 115200 -d 8 -s 1 -p none <port>
```
to connect to it. In fact, the settings given here for baud rate, data bits, stop bits and parity are the defaults in `tio`, but are given explicitly to match VeeRwolf. It is thus sufficient to run `tio <port>`.


To test the setup, flash the `hello_uart` program which writes a string to the UART
```sh
make TARGET=fusesoc_libraries/veerwolf/sw/hello_uart.S flash
```
Start the debug server
```sh
make debug
```
and you should see something printed to `tio`.
