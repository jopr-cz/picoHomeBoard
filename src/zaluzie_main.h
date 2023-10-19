#ifndef HEADER_H_ZALUZIE_MAIN     // equivalently, #if !defined HEADER_H_
#define HEADER_H_ZALUZIE_MAIN

#include "zaluzie.h"
#include "modbus.h"


/* ~~~MODBUS REGISTERS~~~
*
*
*   HOLDING REGISTERS
*   ^address ^ description ^
*   | 0x00   | pozice v [%] žaluzie 1 |
*   | 0x01   | pozice v [%] žaluzie 2 |
*   | 0x02   | pozice v [%] žaluzie 3 |
*   | 0x03   | pozice v [%] žaluzie 4 |
*   | 0x04   | pozice v [%] žaluzie 5 |
*   | 0x05   | pozice v [%] žaluzie 6 |
*
*
*   DISCRETE INPUTS (tlačítka)
*   ^address ^ description ^
*    | 0x00      | vstup 0 |
*    | 0x01      | vstup 1 |
*    | 0x02      | vstup 2 |
*    | 0x03      | vstup 3 |
*    | 0x04      | vstup 4 |
*    | 0x05      | vstup 5 |
*    | 0x06      | vstup 6 |
*    | 0x07      | vstup 7 |
*    | 0x08      | vstup 8 |
*    | 0x09      | vstup 9 |
*    | 0x0A      | vstup 10|
*    | 0x0B      | vstup 11|
*
*
*  COILS (relátka - zapnuto/vypnuto)
*    ^address ^ description ^
*    | 0x00   |  rele 0 |
*    | 0x01   |  rele 1 |
*    | 0x02   |  rele 2 |
*    | 0x03   |  rele 3 |
*    | 0x04   |  rele 4 |
*    | 0x05   |  rele 5 |
*    | 0x06   |  rele 6 |
*    | 0x07   |  rele 7 |
*    | 0x08   |  rele 8 |
*    | 0x09   |  rele 9 |
*    | 0x0A   |  rele 10|
*    | 0x0B   |  rele 11|
*
*
*
*
*/



class ZALUZIE_MAIN:public MODBUS{
public:
    ZALUZIE_MAIN(ZALUZIE * zaluzie_,int deviceID, SerialPort * serPort);
    virtual ~ZALUZIE_MAIN();

protected:
    ZALUZIE * zaluzie;

    virtual DATA_BUFFER readCoils(uint16_t address, uint16_t size)override;////<funkce pro čtení cívky FCE-1
    virtual DATA_BUFFER readDiscreteInput(uint16_t address, uint16_t size)override;////<funkce pro čtení vstupů FCE-2
    virtual DATA_BUFFER readHolding(uint16_t address, uint16_t size)override;////<čteni 16bitové slova - tzn size==1 -> 2B
    virtual bool writeCoil(uint16_t address, bool state)override;////<funkce pro zapis vystupů state==true -zapnuto
    virtual bool writeMultiCoil(uint16_t address, const DATA_BUFFER & states, uint16_t count)override;////<funkce pro zapis vystupů state==true -zapnuto
    virtual bool writeHolding(uint16_t address, uint16_t data)override;
    
    virtual bool readOneInput(uint16_t address) override;

};


#endif