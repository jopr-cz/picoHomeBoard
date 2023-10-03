#include <iostream>

#include "jolibMainHelper.h"

#include "serial_linux.h"
#include "modbus.h"


int main()
{
    std::cout << "Hello World" << std::endl;
    MAIN_HELPER modul_helper;
    
    SerialLinux ser("/dev/ttyS0");
    modul_helper.addModul(&ser);
    MODBUS modbus(&ser);
    modul_helper.addModul(&modbus);



    time_t timestamp;
    time_t bootTime=time(NULL);
    uint64_t timestampUs;
    printf("Simulator started\n");


    while(true){
        timestamp = time(NULL);
        timestampUs=timestamp-bootTime;

        modul_helper.loop(timestampUs);
    }


}