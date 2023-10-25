#ifndef HEADER_H_SERIAL_PICO_POU     // equivalently, #if !defined HEADER_H_
#define HEADER_H_SERIAL_PICO_POU

#include "serial_port.h"


class SerialPico: public SerialPort {
public:
    SerialPico(bool _rs485);

    virtual void send(const DATA_BUFFER &message) override;

    friend void on_uart_rx();
protected:
    virtual void process()override;
    virtual void procesS()override;

private:
    const bool rs485;
};


#endif