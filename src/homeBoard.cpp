#include "homeBoard.h"

#include "srcVersions.h"

HomeBoard::HomeBoard(ZALUZIE *zaluzie_,GPIO_BASE * gpio_,SerialPort * serPort):MODBUS(serPort,gpio_->getAddress()&0x1F), zaluzie(zaluzie_),gpio(gpio_){

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
        zaluzie->setPosition(data, address & 0xf);
        return true;
    }else if(address>=0x10 && address<= 0x15 && zaluzie!=nullptr){
        zaluzie->setShutter(data, address & 0xf);
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
    int16_t result=0;
    if(addresse>=0 && addresse<= 0x05 && zaluzie!=nullptr){//pozice zaluzie
        result= zaluzie->getZaluzPosition(addresse);
    }else if (addresse>=0x10 && addresse<= 0x15 && zaluzie!=nullptr){//pozice shutteru
        result= zaluzie->getShutterPosition(addresse &0xf);
    }else if (addresse>=0x100 && addresse<=0x105 && zaluzie!=nullptr){//zaluzie maxTime
        result= zaluzie->getMaxDownTime(addresse&0xf)/1000;
    }else if (addresse>=0x110 && addresse<=0x115 && zaluzie!=nullptr){//shutter maxTime
        result= zaluzie->getMaxShutterTime(addresse&0xf)/1000;
    }else if(addresse==0x1000){//GIT verze
        result= git_version_int()>>16;
    }else if(addresse==0x1001){//GIT verze
        result= git_version_int()&0xffff;
    }else if(addresse==0x1002){//uptime
        result= timestampS>>16;
    }else if(addresse==0x1003){//uptime
        result= timestampS&0xffff;
    }

    result=swapEndian(result);
    return result;
}
