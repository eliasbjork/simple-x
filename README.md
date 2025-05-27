# simple-x

More details are available in the [VeeRwolf repo README](https://github.com/chipsalliance/VeeRwolf/blob/main/README.md).

## Prerequisites

- riscv toolchain
- verilator verison >= 3.918
- vivado (if using an AMD Xilinx board)
- fusesoc
- openocd
- u-boot-tools

### Verilator

Verilator i used by FuseSoC to simulate the system. A version >= 3.918 is required. On Ubuntu 24.04 this can be installed with
```sh
sudo apt install verilator
```

### Vivado (if using an AMD Xilinx board)

FuseSoC uses Vivado to synthesize for AMD Xilinx boards.


## Getting started

### Setting up the FuseSoC workspace

Add the FuseSoC base library to the workspace with
```sh
fusesoc library add fusesoc-cores https://github.com/fusesoc/fusesoc-cores
```
Add the VeeRwolf library with
```sh
fusesoc library add veerwolf https://github.com/chipsalliance/VeeRwolf
```


### Synthesize

To synthesize the design, run
```sh
make synth
```
A `BOARD` parameter can optionally be given to select the target board. Default is `BOARD=nexys_video`.

### Programming the FPGA and booting from flash

Writing to the board's flash i

We are using OpenOCD to write a program to the boards flash. First the FPGA must be programmed with a JTAG-SPI proxy bitstream to enable the OpenOCD `jtagspi` flash driver. The bitstream for Nexys Video is provided in this repo, and bitstreams for other boards can be acquired [here](https://github.com/quartiq/bscan_spi_bitstreams). OpenOCD is then used to write the program to the boards flash

This method means that the SoC doesn't need to be resynthesized in order to run a different program, however the FPGA still needs to be reprogrammed.

The program is written to memory address `0`.

Bootloader.


```sh
make TARGET=<path/to/program> flash
```
If no `TARGET` is given, a blinky program is flashed.

```sh
make program
```

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
If blinky was flashed, you should now see a blinking LED on the board.

#### Connecting GDB to OpenOCD

To connect GDB and start debugging the last program that was flashed, open a new terminal and run
```sh
make gdb
```

### UART

On Nexys Video you need to connect a second micro-USB cable to the UART port.