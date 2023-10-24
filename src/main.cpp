#include <iostream>
#include <sys/time.h>

#include "jolibMainHelper.h"

//#include "serial_linux.h"
//#include "serial_linuxUDP.h"
#include "serial_linuxTCP.h"
#include "modbus.h"

#include "zaluzie.h"
#include "gpio.h"
#include "homeBoard.h"

int main()
{
    std::cout << "Hello World" << std::endl;
    MAIN_HELPER modul_helper;
    
    SerialLinuxTCP ser;
    GPIO_BASE gpio;
    ZALUZIE zaluzie(&gpio);
    HomeBoard homeBoard(&zaluzie,0x05,&ser);


    modul_helper.addModul(&ser);
    modul_helper.addModul(&gpio);
    modul_helper.addModul(&zaluzie);
    modul_helper.addModul(&homeBoard);



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