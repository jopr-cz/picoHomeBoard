#ifndef HEADER_H_HOME_BOARD     // equivalently, #if !defined HEADER_H_
#define HEADER_H_HOME_BOARD

#include "zaluzie.h"
#include "modbus.h"


/* ~~~MODBUS REGISTERS~~~
*
*
*   HOLDING REGISTERS - FCE3 read, FCE6 write, FCE16 multi write
*   ^address ^ description ^
*   | 0x00   | pozice v [%] žaluzie 1 |
*   | 0x01   | pozice v [%] žaluzie 2 |
*   | 0x02   | pozice v [%] žaluzie 3 |
*   | 0x03   | pozice v [%] žaluzie 4 |
*   | 0x04   | pozice v [%] žaluzie 5 |
*   | 0x05   | pozice v [%] žaluzie 6 |
*
*
*   | 0x1000 | SW GIT version  hight 16b|
*   | 0x1001 | SW GIT version  down  16b|
*
*
*
*
*
*   DISCRETE INPUTS (tlačítka) FCE2
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
*    | 0x0C      | vstup 12|
*    | 0x0D      | vstup 13|
*
*
*  COILS (relátka - zapnuto/vypnuto) FCE1 Read, FCE5 Write, FCE15 multi Write
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
*  DEVICE ID
*   1-4 - device ID (modbus)
*   5- 0=zaluzie,1=topeni
*   
*
*/



class HomeBoard:public MODBUS{
public:
    HomeBoard(ZALUZIE * zaluzie_,GPIO_BASE * gpio_, SerialPort * serPort);
    virtual ~HomeBoard();

protected:
    ZALUZIE * zaluzie;
    GPIO_BASE * gpio;

    virtual bool writeCoil(uint16_t address, bool state)override;////<funkce pro zapis vystupů state==true -zapnuto
    virtual bool writeHolding(uint16_t address, uint16_t data)override;
    virtual bool readCoil(uint16_t address) override;
    virtual bool readDiscreteInput(uint16_t address) override;////<funkce pro čtení vstupů FCE-2
    virtual int16_t readHolding(uint16_t addresse)override;////<čteni 16bitové slova
};


#endif