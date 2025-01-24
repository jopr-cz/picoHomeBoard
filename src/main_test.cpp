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


#include "wifi.h"
#include "mqtt_pou.h"
#include "mqtt_publisher.h"


class GPIO_PICO_W: public GPIO_PICO{
public:
    GPIO_PICO_W():GPIO_PICO(),blink_modul(nullptr){}

    void setBlinkModul(BASE_MODUL * blink_modul_p){
        blink_modul=blink_modul_p;
    }

protected:
    virtual void setStatusLed(bool on) override{
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
    }

    virtual void proces10S()override{
        if(blink_modul!=nullptr){
            if(blink_modul->errorCode()==0){
                printf("TEST WIFI EOK\n");
                setBlink(1000,50);
            }else{
                printf("TEST WIFI ERR %d \n",blink_modul->errorCode());
                setBlink(100,50);
            }
        }else
            printf("TEST WIFI IS NULL PTR\n");
    }

    BASE_MODUL * blink_modul;
};


class TESTER: public BASE_MODUL{
    public:
        TESTER(GPIO_BASE * _gpio):BASE_MODUL("tester"),gpio(_gpio),stav(0){}
    protected:
        GPIO_BASE * gpio;
        int stav;

        virtual void proces10S()override{
                if(gpio==nullptr)
                    return;

                if(stav==0){
                    stav=1;
                    for(int i=0;i<16;i++){
                        gpio->setOutput(i%2,i);
                    }
                }else if(stav==1){
                    stav=2;
                    for(int i=0;i<16;i++){
                        gpio->setOutput(0,i);
                    }
                }else if(stav==2){
                    for(int i=0;i<16;i++){
                        gpio->setOutput((i+1)%2,i);
                    }
                    stav=3;
                } else {
                    for(int i=0;i<16;i++){
                        gpio->setOutput(0,i);
                    }
                    stav=0;
                }
        }
};

int main()
{
    stdio_init_all();
    cyw43_arch_init();
    int userInput;
    bool error=false;
    MAIN_HELPER modul_helper;
    GPIO_PICO_W gpio;
    TESTER test(&gpio);
    
    modul_helper.addModul(&gpio);
    modul_helper.addModul(&test);
    
    static absolute_time_t timestamp;

    if(error)
        gpio.setBlink(800,90);
    else{//pokud neni chyba, tak blink bude blikat podle wifi
        gpio.setBlink(500);
    }

	while (1) {
		timestamp = get_absolute_time();
        modul_helper.loop(timestamp._private_us_since_boot);
		userInput = getchar_timeout_us(0);//us
		switch(userInput){
            case 'a':
                printf("Address is: %d\n", gpio.getAddress());
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