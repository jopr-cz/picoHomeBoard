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

int main()
{
    stdio_init_all();
    int userInput;
    MAIN_HELPER modul_helper;
    
    SerialPico ser(true);
    GPIO_PICO gpio;
    ZALUZIE zaluzie(&gpio);
    HomeBoard homeBoard(&zaluzie,0x05,&ser);


    modul_helper.addModul(&ser);
    modul_helper.addModul(&gpio);
    modul_helper.addModul(&zaluzie);
    modul_helper.addModul(&homeBoard);
    
    static absolute_time_t timestamp;

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
                gpio.setOutput(true,3);
            break;
            case 'f':
                gpio.setOutput(false,3);
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
}