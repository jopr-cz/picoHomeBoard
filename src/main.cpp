#include <iostream>

#include "jolibMainHelper.h"


int main()
{
    std::cout << "Hello World" << std::endl;
    MAIN_HELPER modul_helper;
    

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