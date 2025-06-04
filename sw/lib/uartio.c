#define CLK_FREQ_HZ 0x8000103C
#define UART_BASE   0x80002000

#define BAUD_RATE   115200

#define REG_BRDL    (4*0x00)   // Baud rate divisor (LSB)
#define REG_IER     (4*0x01)   // Interrupt enable reg.
#define REG_FCR     (4*0x02)   // FIFO control reg.
#define REG_LCR     (4*0x03)   // Line control reg.
#define REG_LSR     (4*0x05)   // Line status reg.
#define LCR_CS8     0x03       // 8 bits data size
#define LCR_1_STB   0x00       // 1 stop bit
#define LCR_PDIS    0x00       // parity disable
#define LSR_THRE    0x20
#define FCR_FIFO    0x01       // enable XMIT and RCVR FIFO
#define FCR_RCVRCLR 0x02       // clear RCVR FIFO
#define FCR_XMITCLR 0x04       // clear XMIT FIFO
#define FCR_MODE0   0x00       // set receiver in mode 0
#define FCR_MODE1   0x08       // set receiver in mode 1
#define FCR_FIFO_8  0x80       // 8 bytes in RCVR FIFO

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

volatile uint8_t* uart_base = (volatile uint8_t*)UART_BASE;
volatile uint32_t* clk_freq_hz = (volatile uint32_t*)CLK_FREQ_HZ;


static inline void init_uart() {
    // Set DLAB bit in LCR
    *(uart_base + REG_LCR) = 0x80;

    // Set divisor regs
    uint32_t baud_rate_divisor = *clk_freq_hz/BAUD_RATE >> 4;
    *(uart_base + REG_BRDL) = (uint8_t)baud_rate_divisor;

    // 8 data bits, 1 stop bit, no parity, clear DLAB
    *(uart_base + REG_LCR) = LCR_CS8 | LCR_1_STB | LCR_PDIS;
    *(uart_base + REG_FCR) = FCR_FIFO | FCR_MODE0 | FCR_FIFO_8 | FCR_RCVRCLR | FCR_XMITCLR;

    // disable interrupts 
    *(uart_base + REG_IER) = (uint8_t)0;
}


void print(char* s) {
    volatile uint8_t line_status;

    init_uart();

    uint8_t* c = (uint8_t*)s;
    while (*c) {
        // Await space in UART FIFO
        do {
            line_status = *(uart_base + REG_LSR);
        } while (!(line_status & LSR_THRE));

        // Write byte to UART
        *uart_base = *c++;
    }
}

int main() {
    print("Hello world!\n");
}
