#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/watchdog.h"
#include "pico/binary_info.h"
#include "hardware/timer.h"

#include "pico/bootrom.h"


#include "jolibMainHelper.h"

#include "modbus.h"
#include "serial_port.h"
#include "serial_pico.h"
#include "zaluzie.h"
#include "gpio_pico.h"
#include "homeBoard.h"

bool address_checker(GPIO_PICO* gpio){
    int pin=gpio->getAddress();
    int addr=pin&0x1f;

    if(addr==0)
        return false;

    if(pin & 0x20){//TOPENI
        if(addr>2)
            return false;
    }else{// ZALUZIE
        if(addr>3)
            return false;
    }
    return true;
}


int main()
{
    stdio_init_all();
    int userInput;
    bool error=false;
    MAIN_HELPER modul_helper;
    
    SerialPico ser(true);
    GPIO_PICO gpio;
    ZALUZIE *zaluzie=nullptr;


    error=!(address_checker(&gpio));
    
    if(!(gpio.getAddress() & 0x20)){//zaluzie modul enabled
        int offset=gpio.getAddress() & 0x1F;
        offset--;//adresy zacinaj od 1 (modbus) - pole zacina od 0
        offset=6*offset;//jedna adresa -> 6 zaluzii
        if(offset+6 > sizeof(zaluzSettingArray)/sizeof(ZALUZ_SETTING)){
            error=true;
        }else{
            zaluzie=new ZALUZIE(&gpio,zaluzSettingArray+offset);
        }
    }

    HomeBoard homeBoard(zaluzie,&gpio,&ser);


    modul_helper.addModul(&ser);
    modul_helper.addModul(&gpio);
    modul_helper.addModul(zaluzie);
    modul_helper.addModul(&homeBoard);
    
    static absolute_time_t timestamp;

    if(error)
        gpio.setBlink(100,50);
    else
        gpio.setBlink(1000);

    

	while (1) {
		
		timestamp = get_absolute_time();

        modul_helper.loop(timestamp._private_us_since_boot);

		userInput = getchar_timeout_us(0);//us
		switch(userInput){
            case 'a':
                printf("Address is: %d\n", gpio.getAddress());
            break;
            case 'g':
                printf("Test: sizeof %d\n", sizeof(zaluzSettingArray)/sizeof(ZALUZ_SETTING));
            break;
            case 'f':
                printf("Test: off   %d\n",(zaluzSettingArray+1)->maxDownTime);
                printf("Test: off   %d\n",zaluzSettingArray[1].maxDownTime);
            break;
            case 's':
                ser.sendStr(std::string("AHOJ"));
            break;
			case 'r':
				puts("REBOOT\n");
				reset_usb_boot(0,0);
			break;
			case PICO_ERROR_TIMEOUT:
			break;
		}
	}
    return 0;
}