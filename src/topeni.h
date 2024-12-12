#ifndef HEADER_H_TOPENI     // equivalently, #if !defined HEADER_H_
#define HEADER_H_TOPENI

#include "jolib.h"

#include "base_modul.h"
#include "gpio_base.h"


class TOP:public BASE_MODUL{
public:
 
    enum TOP_STATE{
        OPEN,   // otevreno 
        CLOSE,  //zatazeno
    };


    TOP(GPIO_BASE * gpioInterface,int index):
        BASE_MODUL("top"),
        top_index(index),
        state(OPEN),
        gpio(gpioInterface){
    }
   
    void setState(TOP_STATE newState);

    void process()override;
    TOP_STATE getState()const{return state;}

private:
    int top_index;
    TOP_STATE state;
    GPIO_BASE * gpio;
};



class TOPENI: public BASE_MODUL{
public:
    TOPENI(GPIO_BASE * gpioInterface);

    TOP::TOP_STATE getState(int topIndex) const;
    void setState(TOP::TOP_STATE newState, int topIndex);
protected:
    GPIO_BASE * gpio;
    std::vector<TOP*> topeni;
    const int TOPEN_CNT=12;

    virtual void process()override;
};


#endif