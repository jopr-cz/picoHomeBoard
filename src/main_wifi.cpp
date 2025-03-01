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

int main()
{
    stdio_init_all();
    int userInput;
    bool error=false;
    MAIN_HELPER modul_helper;
    
    SerialPico ser(true);
    GPIO_PICO_W gpio;
    ZALUZIE *zaluzie=nullptr;

    error=!(address_checker(&gpio));
    int offset=0;
    if(!(gpio.getAddress() & 0x20)){//zaluzie modul enabled
        offset=gpio.getAddress() & 0x1F;
        offset--;//adresy zacinaj od 1 (modbus) - pole zacina od 0
        offset=6*offset;//jedna adresa -> 6 zaluzii
        //offset=6;///TEST DEBUG
        if(offset+6 > sizeof(zaluzSettingArray)/sizeof(ZALUZ_SETTING)){
            error=true;
        }else{
            zaluzie=new ZALUZIE(&gpio,zaluzSettingArray+offset);
        }
    }


    WIFI wifi("jopr6","ytits1234");
    //WIFI wifi("jopr5","ytits1234");
    MQTT_POU mqtt("192.168.3.30",1885,"PicoW"+num2str(offset));

    //MQTT_POU mqtt("192.168.2.42",1883);

    modul_helper.addModul(&wifi);
    modul_helper.addModul(&mqtt);


    HomeBoard homeBoard(zaluzie,&gpio,&ser);
    MQTT_publish mqtt_publisher(&mqtt,zaluzie,offset);


    modul_helper.addModul(&ser);
    modul_helper.addModul(&gpio);
    modul_helper.addModul(zaluzie);
    modul_helper.addModul(&homeBoard);
    modul_helper.addModul(&mqtt_publisher);
    
    static absolute_time_t timestamp;

    if(error)
        gpio.setBlink(800,90);
    else{//pokud neni chyba, tak blink bude blikat podle wifi
        gpio.setBlink(500);
        gpio.setBlinkModul(&mqtt);
    }

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
            case 'o':
                printf("Ofset is : %d\n", offset);
            break;
            case 'f':
                printf("Test: off   %d\n",(zaluzSettingArray+1)->maxDownTime);
                printf("Test: off   %d\n",zaluzSettingArray[1].maxDownTime);
            break;
            case 's':
                ser.sendStr(std::string("AHOJ"));
            break;
            case 'w':
                wifi.print_status();
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
