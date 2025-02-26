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
            request.shutter=maxShutterTime;
        }else if(btn->doublePressed){
            resetAllStates();
            request.request_valid=true;
            request.position=0;
            request.shutter=maxShutterTime;
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
            request.shutter=0;
        }else if(btn->doublePressed){
            resetAllStates();
            request.request_valid=true;
            request.position=maxDownTime;
            request.shutter=0;
        }else{
            resetAllStates();
            request.request_valid=false;
            request.position=0;
        }
    }
}

void ZALUZ::setPosition(uint16_t newPositionPercent){
    if(request.request_valid==false){//pokud by byl už nějaký request, tak přepíšu jen hodnotu Position
        resetAllStates();
    }
    request.request_valid=true;
    request.position=(maxDownTime/100)*newPositionPercent;
    //if(request.position==0)//pokud zavřu, tak nechám shutter na 0(otevřený) -> žaluzije je zajetá
    //    request.shutter=0;
    printf("Zaluzie %d seting request position:%u shutter:%u act:%u shut:%u\n",zaluzie_index,request.position,request.shutter,position,shutter_position);
}


void ZALUZ::setShutter(uint16_t newShuttePositionPerent){
    if(request.request_valid==false){//pokud by byl už nějaký request, tak přepíšu jen hodnotu Shutter
        resetAllStates();
    }
    request.request_valid=true;
    request.shutter=(maxShutterTime*newShuttePositionPerent)/100;
    printf("Zaluzie %d seting request position:%u shutter:%u\n",zaluzie_index,request.position,request.shutter);
}

void ZALUZ::setState(ZALUZ_STATE state)
{
    resetAllStates();
    request.request_valid=true;
    switch (state) {
    case OPEN:
        request.position=0;
        request.shutter=maxShutterTime;
        break;
    case CLOSE:    
        request.position=maxDownTime;
        request.shutter=0;
        break;
    case CLOSE_LIGHT:
        //request.position=maxDownTime*0.8;
        request.position=maxDownTime;
        request.shutter=maxShutterTime;
        break;
    default:
        break;
    }
    printf("Zaluzie %d seting state to %d (position:%u)\n",zaluzie_index,state,request.position);
}

void ZALUZ::runUp(){
    if(position<=0){
        if(position<0)
            position=0;
        state=OPEN;
        printf("ZALUZE %d je už nahoře %u\n",zaluzie_index,position);
        request.request_valid=false;
        return;
    }

    if(shutter_position<maxShutterTime){
        shutterOpen();
        return;
    }

    setMoveState(MOVE_UP);
    motor_up();
}

void ZALUZ::runDown(){
    if(position>=maxDownTime){
        if(position>maxDownTime)
            position=maxDownTime;
        printf("ZALUZE %d je už dole %u\n",zaluzie_index,position);
        state=CLOSE;
        request.request_valid=false;
        return;
    }
    
    if(shutter_position>0){
        shutterClose();
        return;
    }

    setMoveState(MOVE_DOWN);
    motor_down();
}


void ZALUZ::stop(){
    if(isPrinted==false)
        printf("ZALUZ %d STOP %u\n", zaluzie_index,position);
    isPrinted=true;

    request.request_valid=false;
    motor_stop();
    setMoveState(MOVE_NONE);
}


void ZALUZ::shutterOpen(){
    if(shutter_position>=maxShutterTime){
        shutter_position=maxShutterTime;
        printf("ZALUZE %d je ma maximualni naklon OTEVRENO %u\n",zaluzie_index,shutter_position);
        return;
    }
    
    setMoveState(SHUTTER_OPEN);
    motor_up();
}


void ZALUZ::shutterClose(){
    if(shutter_position<=0){
        if(shutter_position<0)
            shutter_position=0;
        printf("ZALUZE %d je ma minimalni naklon ZAVRENO %u\n",zaluzie_index,shutter_position);
        return;
    }
    
    setMoveState(SHUTTER_CLOSE);
    motor_down();
}


void ZALUZ::countMovePosition(){
    uint32_t diff=(timestamp-lastProcessedTime);
    switch (moveState)
    {
    case MOVE_DOWN:
        if(position > maxDownTime - diff){//pretečení
            position=maxDownTime;
        }else{
            position += diff;
        }
    break;
    case MOVE_UP:
        if(position<diff)
            position=0;
        else
            position -= diff;
    break;
    case SHUTTER_CLOSE:
        if(shutter_position<diff)  ///<!Co je asi pod 0 by mělo jit do pozice zaluzie ne?
            shutter_position =0;
        else
            shutter_position -= diff;
    break;
    case SHUTTER_OPEN:
        if(shutter_position > maxShutterTime - diff){//pretečení
            shutter_position=maxShutterTime;
        }else{
            shutter_position += diff;
        }
    break;
    default:
        break;
    }

    if(timestamp%100000==0)
        printf("ZALUZ %d state:%d pos: %u -> %u \t shut:%u -> %u \n", zaluzie_index,moveState,position/1000,request.position/1000,shutter_position/1000,request.shutter/1000);

}

void ZALUZ::setMoveState(ZALUZ_MOVE newState){
    countMovePosition();
    moveState=newState;
}

void ZALUZ::motor_up(){
    gpio->setOutput(false,zaluzie_index*2);
    gpio->setOutput(true,(zaluzie_index*2)+1);
}

void ZALUZ::motor_down(){
    gpio->setOutput(false,(zaluzie_index*2)+1);
    gpio->setOutput(true,zaluzie_index*2);
}

void ZALUZ::motor_stop(){
    gpio->setOutput(false,zaluzie_index*2);
    gpio->setOutput(false,(zaluzie_index*2)+1);
}


void ZALUZ::resetAllStates(){
    doubleUpRequest=false;
    doubleDownRequest=false;
    request.request_valid=false;
    request.position=0;
    request.shutter=0;
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
            //žaluzie je na správné pozici, nyní naklopím na daný úhel
            //to znamena že motorem trošku ještě zakroutím
            if(shutter_position+hystereze<request.shutter){
                shutterOpen();
            }else if(shutter_position>request.shutter+hystereze){
                shutterClose();
            }else{
                request.request_valid=false;
                stop();
                resetAllStates();
            }
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
    uint32_t mx=maxDownTime/100;
    if(position+hystereze >=maxDownTime)
        return 100;
    else if(position-hystereze<=0)
        return 0;
    return (position/mx);
}


uint8_t ZALUZ::getShutterPercent() const{
    if(shutter_position+hystereze >=maxShutterTime)
        return 100;
    else if(shutter_position-hystereze<=0)
        return 0;
    return (shutter_position*100/maxShutterTime);
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

bool ZALUZIE::chectZaluzIndex(int index) const{
    if(index<0 || index>= ZALUZ_CNT){
        printf("Zaluz index out of range %d\n",index);
        return false;
    }
    return true;
}

ZALUZIE::ZALUZIE(GPIO_BASE * gpioInterface,const ZALUZ_SETTING * setting, int ZALUZIE_COUNT):
    BASE_MODUL("zaluzie"),
    gpio(gpioInterface),
    ZALUZ_CNT(ZALUZIE_COUNT)
{

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
    if(!chectZaluzIndex(zaluzIndex)){
        return ZALUZ::ZALUZ_STATE();
    }
    return zaluzie.at(zaluzIndex)->getState();
}

int ZALUZIE::getZaluzPosition(int zaluzIndex) const{
    if(!chectZaluzIndex(zaluzIndex)){
        return 0;
    }
    return zaluzie.at(zaluzIndex)->getPositionPercent();
}

int ZALUZIE::getShutterPosition(int zaluzIndex) const{
    if(!chectZaluzIndex(zaluzIndex)){
        return 0;
    }
    return zaluzie.at(zaluzIndex)->getShutterPercent();
}

uint32_t ZALUZIE::getMaxDownTime(int zaluzIndex) const{
    if(!chectZaluzIndex(zaluzIndex)){
        return 0;
    }
    printf("Getting maxDown of: %d -> %u\n", zaluzIndex,zaluzie.at(zaluzIndex)->getMaxDownTime());
    return zaluzie.at(zaluzIndex)->getMaxDownTime();
}


uint32_t ZALUZIE::getMaxShutterTime(int zaluzIndex) const{
    if(!chectZaluzIndex(zaluzIndex)){
        return 0;
    }
    printf("Getting maxShutter of: %d -> %u\n", zaluzIndex,zaluzie.at(zaluzIndex)->getMaxShutterTime());
    return zaluzie.at(zaluzIndex)->getMaxShutterTime();
}

void ZALUZIE::setState(ZALUZ::ZALUZ_STATE newState, int zaluzIndex){
    if(!chectZaluzIndex(zaluzIndex)){
        return;
    }
    zaluzie[zaluzIndex]->setState(newState);
}

void ZALUZIE::setShutter(uint16_t newShutterPercentage, int zaluzIndex){
    if(!chectZaluzIndex(zaluzIndex)){
        return;
    }
    zaluzie[zaluzIndex]->setShutter(newShutterPercentage);
}



void ZALUZIE::stop(int zaluzIndex){
    if(!chectZaluzIndex(zaluzIndex)){
        return;
    }
    zaluzie[zaluzIndex]->stop();
}

void ZALUZIE::setPosition(uint16_t newPositionPercentage, int zaluzIndex){
    if(!chectZaluzIndex(zaluzIndex)){
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
