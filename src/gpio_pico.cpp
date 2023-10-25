#include "gpio_pico.h"

#include "hardware/gpio.h"



GPIO_PICO::GPIO_PICO():GPIO_BASE(){

    configOutput(LED_PIN);

    //INPUTS
    configInput(3);
    configInput(4);
    configInput(5);
    configInput(6);
    configInput(7);
    configInput(8);
    configInput(9);
    configInput(10);
    configInput(11);
    configInput(12);
    configInput(13);
    configInput(16);
    configInput(17);
    configInput(18);

    //ADDR:
    configInput(14,true);
    configInput(15,true);
    configInput(19,true);
    configInput(20,true);
    configInput(21,true);
    configInput(22,true);

}



uint16_t GPIO_PICO::getAddress() const{
    uint32_t addr=0;
    addr |= (gpio_get(14));
    addr |= (gpio_get(15) << 1);
    addr |= (gpio_get(19) << 2);
    addr |= (gpio_get(20) << 3);
    addr |= (gpio_get(21) << 4);
    addr |= (gpio_get(22) << 5);
    return addr;
}

void GPIO_PICO::setStatusLed(bool on){
    gpio_put(LED_PIN, on);
}

uint16_t GPIO_PICO::getBtnInputState(){
    uint16_t g=0;
    g |= (gpio_get(3)  << 6);
    g |= (gpio_get(4)  << 7);
    g |= (gpio_get(5)  << 8);
    g |= (gpio_get(6)  << 9);
    g |= (gpio_get(7)  << 10);
    g |= (gpio_get(8)  << 11);
    g |= (gpio_get(9)  << 5);
    g |= (gpio_get(10) << 4);
    g |= (gpio_get(11) << 3);
    g |= (gpio_get(12) << 2);
    g |= (gpio_get(13) << 1);
    g |= (gpio_get(16) << 12);
    g |= (gpio_get(17) << 13);
    g |= (gpio_get(18) << 0);
    return g;
}



bool GPIO_PICO::setGPIO(int number, bool status){
    return mcp.setOutput(outputState);
}




void GPIO_PICO::configInput(const uint pin, bool pullUP){
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_set_pulls(pin, pullUP,false);
}




void GPIO_PICO::configOutput(const uint pin){
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
}
