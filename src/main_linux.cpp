#include <iostream>
#include <sys/time.h>

#include "jolibMainHelper.h"

//#include "serial_linux.h"
//#include "serial_linuxUDP.h"
#include "serial_linuxTCP.h"
#include "modbus.h"

#include "zaluzie.h"
#include "gpio_base.h"
#include "homeBoard.h"

class GPIO_LINUX: public GPIO_BASE{
    public:
    GPIO_LINUX():GPIO_BASE(){}

    uint16_t getAddress()const override{
        return 0x01;
    }

protected:
    virtual bool setGPIO(int number, bool status) override final{
        return true;
    }
};


int main()
{
    std::cout << "Hello World" << std::endl;
    MAIN_HELPER modul_helper;
    
    SerialLinuxTCP ser;
    GPIO_LINUX gpio;
    ZALUZIE zaluzie(&gpio,zaluzSettingArray);
    HomeBoard homeBoard(&zaluzie, &gpio,&ser);


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