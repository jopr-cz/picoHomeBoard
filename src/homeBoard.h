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
*   | 0x10   | pozice shutter v [%] žaluzie 1 -> 0-zataženo, 100%-nejvíce průhledné|
*   | 0x11   | pozice shutter v [%] žaluzie 2 -> 0-zataženo, 100%-nejvíce průhledné|
*   | 0x12   | pozice shutter v [%] žaluzie 3 -> 0-zataženo, 100%-nejvíce průhledné|
*   | 0x13   | pozice shutter v [%] žaluzie 4 -> 0-zataženo, 100%-nejvíce průhledné|
*   | 0x14   | pozice shutter v [%] žaluzie 5 -> 0-zataženo, 100%-nejvíce průhledné|
*   | 0x15   | pozice shutter v [%] žaluzie 6 -> 0-zataženo, 100%-nejvíce průhledné|
*
*   
*   | 0x100 | zaluzie 1 maxDownTime[ms]|
*   | 0x101 | zaluzie 2 maxDownTime[ms]|
*   | 0x102 | zaluzie 3 maxDownTime[ms]|
*   | 0x103 | zaluzie 4 maxDownTime[ms]|
*   | 0x104 | zaluzie 5 maxDownTime[ms]|
*   | 0x105 | zaluzie 6 maxDownTime[ms]|
*
*   | 0x110 | zaluzie 1 maxShutterTime[ms]|
*   | 0x111 | zaluzie 2 maxShutterTime[ms]|
*   | 0x112 | zaluzie 3 maxShutterTime[ms]|
*   | 0x113 | zaluzie 4 maxShutterTime[ms]|
*   | 0x114 | zaluzie 5 maxShutterTime[ms]|
*   | 0x115 | zaluzie 6 maxShutterTime[ms]|
*
*       SW version + uptime 32b value
*   | 0x1000 | SW GIT version  hight 16b|
*   | 0x1001 | SW GIT version  down  16b|
*   | 0x1002 | uptime [seconds] hight 16b|
*   | 0x1003 | uptime [seconds] down 16b|
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
    Modbus addr:
    Topení - adresy modbus 1,2
    Zaluzie - adresy modbus 1,2,3
    Adresa 0 - nevalidni pro modbus
    
*
*/



static const ZALUZ_SETTING zaluzSettingArray[] = {
    {56000000, 1000000},        ///< zaluzie 1    modbus addr1
    {56000000, 1000000},        ///< zaluzie 2    modbus addr1
    {56000000, 1000000},        ///< zaluzie 3    modbus addr1
    {56000000, 1000000},        ///< zaluzie 4    modbus addr1
    {56000000, 1000000},        ///< zaluzie 5    modbus addr1
    {16000000, 1000000},        ///< zaluzie 6    modbus addr1

    {56000000, 1000000},        ///< zaluzie 7    modbus addr2
    {34000000, 1000000},        ///< zaluzie 8    modbus addr2
    {34000000, 1000000},        ///< zaluzie 9    modbus addr2
    {21000000, 1000000},        ///< zaluzie 10  modbus addr2
    {34000000, 1000000},        ///< zaluzie 11  modbus addr2
    {60000000, 1000000},        ///< zaluzie 12  modbus addr2

    {34000000, 1000000},        ///< zaluzie 13  modbus addr 3
    {20000000, 1000000},        ///< zaluzie 14  modbus addr 3
    {20000000, 1000000},        ///< zaluzie 15  modbus addr 3
    {34000000, 1000000},        ///< zaluzie 15  modbus addr 3
    {20000000, 1000000},        ///< zaluzie 16  modbus addr 3
    {60000000, 1000000}         ///< zaluzie 17  modbus addr 3
};


class HomeBoard:public MODBUS{
public:
    HomeBoard(ZALUZIE * zaluzie_,GPIO_BASE * gpio_, SerialPort * serPort);
    virtual ~HomeBoard();

protected:
    ZALUZIE * zaluzie;
    GPIO_BASE * gpio;

    virtual bool writeCoil(uint16_t address, bool state)override final;////<funkce pro zapis vystupů state==true -zapnuto
    virtual bool writeHolding(uint16_t address, uint16_t data)override final;
    virtual bool readCoil(uint16_t address) override final;
    virtual bool readDiscreteInput(uint16_t address) override final;////<funkce pro čtení vstupů FCE-2
    virtual int16_t readHolding(uint16_t addresse)override final;////<čteni 16bitové slova
};


#endif
