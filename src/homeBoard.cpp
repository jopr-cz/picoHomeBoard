#include "homeBoard.h"


HomeBoard::HomeBoard(ZALUZIE *zaluzie_,int deviceID,SerialPort * serPort):MODBUS(serPort,deviceID), zaluzie(zaluzie_){

}



HomeBoard::~HomeBoard(){
}



bool HomeBoard::writeCoil(uint16_t address, bool state){
    if(address>=0 && address<= 0x0B){
        zaluzie->getGpio()->setOutput(state,address);
        return true;
    }
    return false;
}



bool HomeBoard::writeHolding(uint16_t address, uint16_t data){
    if(address>=0 && address<= 0x05){
        zaluzie->setPosition(data,address);
        return true;
    }
    return false;
}



bool HomeBoard::readCoil(uint16_t address){
    if(address>=0 && address<= 0x0B){
        return zaluzie->getGpio()->getOutput(address);
    }
    return 0;
}


bool HomeBoard::readDiscreteInput(uint16_t address){
    const BUTTON * btn=zaluzie->getGpio()->getBtn(address);
    if(btn==NULL)
        return false;
    return btn->isPressed;
}


int16_t HomeBoard::readHolding(uint16_t addresse){
    if(addresse>=0 && addresse<= 0x05){
        return zaluzie->getZaluzPosition(addresse);
    }
    return 0;
}
