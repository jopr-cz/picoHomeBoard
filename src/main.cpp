#include <iostream>
#include <sys/time.h>

#include "jolibMainHelper.h"

//#include "serial_linux.h"
//#include "serial_linuxUDP.h"
#include "serial_linuxTCP.h"
#include "modbus.h"

#include "zaluzie.h"
#include "gpio.h"

int main()
{
    std::cout << "Hello World" << std::endl;
    MAIN_HELPER modul_helper;
    
    //SerialPort ser;
    //SerialLinux ser("/dev/ttyS0");
    //SerialLinuxUDP ser;
    SerialLinuxTCP ser;
    modul_helper.addModul(&ser);
    MODBUS modbus(&ser,0x05);
    modul_helper.addModul(&modbus);

    GPIO_BASE gpio;
    modul_helper.addModul(&gpio);



    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    long long timestampUs;
    printf("Simulator started\n");


    while(true){
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        timestampUs= (end_time.tv_sec - start_time.tv_sec) * 1000000LL +
               (end_time.tv_nsec - start_time.tv_nsec) / 1000LL;

        modul_helper.loop(timestampUs);
    }


}