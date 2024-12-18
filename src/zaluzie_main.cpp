#include "zaluzie_main.h"

ZALUZIE_MAIN::ZALUZIE_MAIN(ZALUZIE *zaluzie_,int deviceID,SerialPort * serPort):MODBUS(serPort,deviceID), zaluzie(zaluzie_){

}



ZALUZIE_MAIN::~ZALUZIE_MAIN(){
}


DATA_BUFFER ZALUZIE_MAIN::readCoils(uint16_t address, uint16_t size){
    DATA_BUFFER r;
    return r;
}


DATA_BUFFER ZALUZIE_MAIN::readDiscreteInput(uint16_t address, uint16_t size){
    DATA_BUFFER r;
    return r;
}


DATA_BUFFER ZALUZIE_MAIN::readHolding(uint16_t address, uint16_t size){
    DATA_BUFFER r;
    return r;
}




bool ZALUZIE_MAIN::writeCoil(uint16_t address, bool state){
    return false;
}


bool ZALUZIE_MAIN::writeMultiCoil(uint16_t address, const DATA_BUFFER &states, uint16_t count){
    return false;
}


bool ZALUZIE_MAIN::writeHolding(uint16_t address, uint16_t data){
    return false;
}



int16_t ZALUZIE_MAIN::readOneInput(uint16_t address){
    const BUTTON * btn=zaluzie->getGpio()->getBtn(address);
    if(btn==NULL)
        return false;
    return btn->isPressed?0x01:0x00;
}
