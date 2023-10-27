#include "homeBoard.h"

#include "srcVersions.h"

HomeBoard::HomeBoard(ZALUZIE *zaluzie_,GPIO_BASE * gpio_,SerialPort * serPort):MODBUS(serPort,gpio_->getAddress()), zaluzie(zaluzie_),gpio(gpio_){

}

HomeBoard::~HomeBoard(){
}

bool HomeBoard::writeCoil(uint16_t address, bool state){
    if(address>=0 && address<= 0x0B && zaluzie==nullptr){// relatka muzu nastavit pouze kdyz nefunguje modul zaluzii
        gpio->setOutput(state,address);
        return true;
    }
    return false;
}



bool HomeBoard::writeHolding(uint16_t address, uint16_t data){
    if(address>=0 && address<= 0x05 && zaluzie!=nullptr){
        zaluzie->setPosition(data,address);
        return true;
    }
    return false;
}



bool HomeBoard::readCoil(uint16_t address){
    if(address>=0 && address<= 0x0B){
        return gpio->getOutput(address);
    }
    return false;
}


bool HomeBoard::readDiscreteInput(uint16_t address){
    const BUTTON * btn=gpio->getBtn(address);
    if(btn==NULL)
        return false;
    return btn->isPressed;
}


int16_t HomeBoard::readHolding(uint16_t addresse){
    if(addresse>=0 && addresse<= 0x05 && zaluzie!=nullptr){
        return zaluzie->getZaluzPosition(addresse);
    }else if(addresse==0x1000){
        return git_version_int()>>16;
    }else if(addresse==0x1001){
        return git_version_int()&0xffff;
    }
    return 0;
}
