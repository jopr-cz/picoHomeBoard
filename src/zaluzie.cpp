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
            btnTypeUp=true;
            btnMatch=true;
        }
    }

    for(auto it:btnsDown){
        if(it==btn->btnIndex){
            btnTypeDown=true;
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

void ZALUZ::setPosition(uint16_t newPositionPercent){
    resetAllStates();
    request.request_valid=true;
    request.position=(maxDownTime*newPositionPercent)/100;
    printf("Zaluzie %d seting position:%d\n",zaluzie_index,request.position);
}

void ZALUZ::setState(ZALUZ_STATE state)
{
    resetAllStates();
    request.request_valid=true;
    switch (state) {
    case OPEN:
        request.position=0;
        break;
    case CLOSE:    
        request.position=maxDownTime;
        break;
    case CLOSE_LIGHT:
        request.position=maxDownTime*0.8;
        break;
    default:
        break;
    }
    printf("Zaluzie %d seting state to %d (position:%d)\n",zaluzie_index,state,request.position);
}

void ZALUZ::runUp(){
    if(position<=0){
        if(position<0)
            position=0;
        state=OPEN;
        printf("ZALUZE %d je už nahoře %zu\n",zaluzie_index,position);
        request.request_valid=false;
        return;
    }
    
    uint32_t diff=(timestamp-lastProcessedTime);
    position -= diff;
    if(position<0)
        position=0;

    if(timestamp%100000==0)
        printf("ZALUZ %d UP %zu -> \n", zaluzie_index,position,request.position);

    gpio->setOutput(false,zaluzie_index*2);
    gpio->setOutput(true,(zaluzie_index*2)+1);
}

void ZALUZ::runDown(){
    if(position>=maxDownTime){
        if(position>maxDownTime)
            position=maxDownTime;
        printf("ZALUZE %d je už dole %zu\n",zaluzie_index,position);
        state=CLOSE;
        request.request_valid=false;
        return;
    }
    
    uint32_t diff=(timestamp-lastProcessedTime);
    position += diff;
    if(position>maxDownTime)
            position=maxDownTime;

    if(timestamp%100000==0)
        printf("ZALUZ %d DOWN %zu -> %zu\n", zaluzie_index,position,request.position);
    
    gpio->setOutput(true,zaluzie_index*2);
    gpio->setOutput(false,(zaluzie_index*2)+1);
}


void ZALUZ::stop(){
    if(isPrinted==false)
        printf("ZALUZ %d STOP %zu\n", zaluzie_index,position);
    isPrinted=true;

    gpio->setOutput(false,zaluzie_index*2);
    gpio->setOutput(false,(zaluzie_index*2)+1);
}


void ZALUZ::resetAllStates(){
    doubleUpRequest=false;
    doubleDownRequest=false;
    request.request_valid=false;
    
}

void ZALUZ::process(){

    if(request.request_valid){
        isPrinted=false;
        if(position+hystereze<request.position){
        //if(position<request.position){
            runDown();
        }else if(position>request.position+hystereze){
        //}else if(position>request.position){
            runUp();
        }else{
            request.request_valid=false;
        }
    }else{
        resetAllStates();
        stop();
    }
    lastProcessedTime=timestamp;

}


void ZALUZ::procesMS(){

}

uint8_t ZALUZ::getPositionPercent() const{
    return (position*100/maxDownTime);
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





ZALUZIE::ZALUZIE(GPIO_BASE * gpioInterface,const ZALUZ_SETTING * setting):BASE_MODUL("zaluzie"),gpio(gpioInterface){

    for(int i=0;i<ZALUZ_CNT;i++){
        ZALUZ * zaluz=new ZALUZ(gpio,i,setting[i]);
        zaluz->setBtnDown(i*2);
        zaluz->setBtnUp((i*2)+1);

        zaluz->setBtnDown(12);
        zaluz->setBtnUp(13);
        zaluzie.push_back(zaluz);
    }
}

ZALUZ::ZALUZ_STATE ZALUZIE::getZaluzState(int zaluzIndex) const{
    if(zaluzIndex<0 || zaluzIndex>= ZALUZ_CNT){
        printf("Zaluz index out of range\n");
        return ZALUZ::ZALUZ_STATE();
    }
    return zaluzie.at(zaluzIndex)->getState();
}

int ZALUZIE::getZaluzPosition(int zaluzIndex) const{
    if(zaluzIndex<0 || zaluzIndex>= ZALUZ_CNT){
        printf("Zaluz index out of range\n");
        return 0;
    }
    return zaluzie.at(zaluzIndex)->getPositionPercent();
}

uint32_t ZALUZIE::getMaxDownTime(int zaluzIndex) const{
    if(zaluzIndex<0 || zaluzIndex>= ZALUZ_CNT){
        printf("Zaluz index out of range\n");
        return 0;
    }
    printf("Getting maxDown of: %d -> %d\n", zaluzIndex,zaluzie.at(zaluzIndex)->getMaxDownTime());
    return zaluzie.at(zaluzIndex)->getMaxDownTime();
}

void ZALUZIE::setState(ZALUZ::ZALUZ_STATE newState, int zaluzIndex){
    if(zaluzIndex<0 || zaluzIndex>= ZALUZ_CNT){
        printf("Zaluz index out of range\n");
        return;
    }
    zaluzie[zaluzIndex]->setState(newState);
}


void ZALUZIE::setPosition(uint16_t newPositionPercentage, int zaluzIndex){
    if(zaluzIndex<0 || zaluzIndex>= ZALUZ_CNT){
        printf("Zaluz index out of range\n");
        return;
    }
    zaluzie[zaluzIndex]->setPosition(newPositionPercentage);
}

void ZALUZIE::process(){
    for(int i=0;i<ZALUZ_CNT;i++){
        zaluzie[i]->process();
    }
}

void ZALUZIE::procesMS(){
    for(int i=0;i<ZALUZ_CNT;i++){
        zaluzie[i]->ProcessMS();
    }
}
