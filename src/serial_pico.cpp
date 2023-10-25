#include "serial_pico.h"


#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"


#define UART_ID uart0
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define UART_485_TX_PIN 2



SerialPico * serPico=nullptr;

void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
        // Can we send it back?
        serPico->add2RxBuffer(ch);
    }
}

SerialPico::SerialPico(bool _rs485): SerialPort(),rs485(_rs485){
    serPico=this;
 // Set up our UART with a basic baud rate.
    //uart_init(UART_ID, 2400);
    uart_init(UART_ID, 115200);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    if(rs485){
        gpio_init(UART_485_TX_PIN);
        gpio_set_dir(UART_485_TX_PIN, GPIO_OUT);
        gpio_put(UART_485_TX_PIN, false);
    }

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);


//Nejprve to zkusim bez přerušení:

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);


    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);
//*/

    // OK, all set up.
    // Lets send a basic string out, and then run a loop and wait for RX interrupts
    // The handler will count them, but also reflect the incoming data back with a slight change!
}




void SerialPico::send(const DATA_BUFFER &message){
    //uart_puts(UART_ID, message.c_str());
    if(message.size()==0) return;

    if(rs485){
        gpio_put(UART_485_TX_PIN, true);
        sleep_us(10);
    }

    for (const char &c : message) {
        uart_putc_raw(UART_ID,c);
    }

    if(rs485){
        sleep_us(200);
        gpio_put(UART_485_TX_PIN, false);
    }
}



void SerialPico::process(){
    return;
    bool newData=false;
    while (uart_is_readable(UART_ID)) {
        char ch =uart_getc(UART_ID);
        add2RxBuffer(ch);
        printf("Add new character: %c (0x%x)\n", ch,ch);
    }
}



void SerialPico::procesS(){
}
