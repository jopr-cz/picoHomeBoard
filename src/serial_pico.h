#ifndef HEADER_H_SERIAL_PICO_POU     // equivalently, #if !defined HEADER_H_
#define HEADER_H_SERIAL_PICO_POU

#include "serial_port.h"


class SerialPico: public SerialPort {
public:
    SerialPico();

    virtual void send(const DATA_BUFFER &message) override;

protected:
    virtual void process()override;
    virtual void procesS()override;

};


#endif