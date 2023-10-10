#include "zaluzie.h"


void ZALUZ::btnStateChanged(const BUTTON *btn, void *userData)
{
    static_cast<ZALUZ *>(userData)->btnStateChanged(btn);
}

void ZALUZ::btnStateChanged(const BUTTON *btn){
    printf("Btn %d change state to: %d\n",btn->btnIndex,btn->isPressed);
}


void ZALUZ::setState(ZALUZ_STATE state){
}


void ZALUZ::runUp()
{
}

void ZALUZ::runDown(){
}

void ZALUZ::stop(){
}


void ZALUZ::resetAllStates(){
    doubleUpRequest=false;
    doubleDownRequest=false;
    request.request_valid=false;
}

void ZALUZ::process()
{
    bool btnUp=false;
    bool btnDown=false;
    bool doubleUp=false;
    bool doubleDown=false;
    
    for(auto it:btnsUp){
        if(gpio->getBtn(it)->isPressed)
            btnUp=true;
        if(gpio->getBtn(it)->doublePressed)
            doubleUp=true;
    }
    for(auto it:btnsDown){
        if (gpio->getBtn(it)->isPressed)
            btnDown=true;
        if(gpio->getBtn(it)->doublePressed)
            doubleDown=true;
    }

    if(btnUp && btnDown){
        printf("BTN UP+DOWN together!!!\n");
        doubleDownRequest=false;
        doubleUpRequest=false;
        stop();
        return;
    }

    if(doubleUp && doubleDown){
        printf("BTN DOUBLE UP+DOWN together!!!\n");
        doubleDownRequest=false;
        doubleUpRequest=false;
        stop();
        return;
    }
    ////////////////////////////////////////////////////////////////

    if(btnUp){
        resetAllStates();
        runUp();
    }else if(btnDown){
        resetAllStates();
        runDown();
    }else if (doubleDownRequest){
        if(state!=CLOSE)
            runDown();
        else{
            resetAllStates();
            stop();
        }
    }else if(doubleUpRequest){
        if(state!=OPEN)
            runUp();
        else{
            resetAllStates();
            stop();
        }
    }else if(request.request_valid){
        if(position+hystereze<request.position){
            runUp();
        }else if(position>request.position+hystereze){
            runDown();
        }else{
            request.request_valid=false;
        }

    }else{
        stop();
    }
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////





void ZALUZIE::btnStateChanged(const BUTTON* btn,void * userData) {
    // Your callback logic here
    // Example: Print the address of the button
    printf("Btn %d change state to: %d\n",btn->btnIndex,btn->isPressed);
}





ZALUZIE::ZALUZIE(GPIO_BASE * gpioInterface):BASE_MODUL("zaluzie"),gpio(gpioInterface){
    //gpio->setBtnCallback(btnStateChanged,this);

    for(int i=0;i<ZALUZ_CNT;i++){
        zaluzie.push_back(ZALUZ(gpio,i));
    }
}



void ZALUZIE::process(){
    for(int i=0;i<ZALUZ_CNT;i++){
        zaluzie[i].process();
    }
}

