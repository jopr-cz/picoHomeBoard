#ifndef HEADER_H_POU_GPIO_PICO     // equivalently, #if !defined HEADER_H_
#define HEADER_H_POU_GPIO_PICO


#include "gpio.h"
#include "mcp23017.h"



class GPIO_PICO: public GPIO_BASE{
public:
    GPIO_PICO();
    virtual uint16_t getAddress()const override;
protected:

    const uint LED_PIN = 25;

    virtual void setStatusLed(bool on)override;
    virtual uint16_t getBtnInputState()override;///< vráti vstupy z HW (talčítkové)
    virtual bool setGPIO(int number, bool status)override;

private:
    MCP23017 mcp;
    void configInput(const uint pin, bool pullUP=false);
    void configOutput(const uint pin);
};
#endif