#include "zaluzie.h"


void ZALUZ::btnStateChanged(const BUTTON *btn, void *userData)
{
    static_cast<ZALUZ *>(userData)->btnStateChanged(btn);
}

void ZALUZ::btnStateChanged(const BUTTON *btn){
    printf("Btn %d change state to: %d\n",btn->btnIndex,btn->isPressed);

    bool btnTypeDown=false;
    bool btnTypeUp=false;
    bool btnMatch=false;
    
    for(auto it:btnsUp){
        if(it==btn->btnIndex){
            btnTypeDown=false;
            btnMatch=true;
        }
    }

    for(auto it:btnsDown){
        if(it==btn->btnIndex){
            btnTypeUp=true;
            btnMatch=true;
        }
    }

    if(btnMatch==false){
        printf("ERROR GPIO callback but gpio is not registered\n");
        return;
    }

    if(btnTypeUp && btnTypeDown){
        printf("ERROR btn %d is up and down together!\n",btn->btnIndex);
        return;
    }


    if(btnTypeUp){
        if(btn->isPressed){
            resetAllStates();
            request.request_valid=true;
            request.position=0;
        }else if(btn->doublePressed){
            resetAllStates();
            request.request_valid=true;
            request.position=0;
        }else{
            resetAllStates();
            request.request_valid=false;
            request.position=0;
        }
        
    }else if(btnTypeDown){
        if(btn->isPressed){
            resetAllStates();
            request.request_valid=true;
            request.position=maxDownTime;
        }else if(btn->doublePressed){
            resetAllStates();
            request.request_valid=true;
            request.position=maxDownTime;
        }else{
            resetAllStates();
            request.request_valid=false;
            request.position=0;
        }
    }
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
    if(request.request_valid){
        if(position+hystereze<request.position){
            runUp();
        }else if(position>request.position+hystereze){
            runDown();
        }else{
            request.request_valid=false;
        }

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

