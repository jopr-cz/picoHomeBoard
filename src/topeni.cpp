#include "topeni.h"




void TOP::setState(TOP_STATE newState){
    if(newState == state )
        return;
    printf("TOP %d new state: %d\n", top_index, newState);
    state=newState;
    switch (state) {
    case OPEN:
        gpio->setOutput(top_index,true);
    break;
    case CLOSE:    
        gpio->setOutput(top_index,false);
    break;
    default:
        break;
    }
}

void TOP::process(){

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



TOPENI::TOPENI(GPIO_BASE *gpioInterface):BASE_MODUL("topeni"),gpio(gpioInterface) {
    for(int i=0;i<TOPEN_CNT;i++){
        TOP * top=new TOP(gpio,i);
        topeni.push_back(top);
    }
}




TOP::TOP_STATE TOPENI::getState(int topIndex) const{
    if(topIndex<0 || topIndex>= TOPEN_CNT){
        printf("Top index out of range\n");
        return TOP::TOP_STATE();
    }
    return topeni.at(topIndex)->getState();
}



void TOPENI::setState(TOP::TOP_STATE newState, int topIndex){
    if(topIndex<0 || topIndex>= TOPEN_CNT){
        printf("Top index out of range\n");
        return;
    }
    topeni[topIndex]->setState(newState);
}




void TOPENI::process(){
    //for(int i=0;i<TOPEN_CNT;i++){
    //    topeni[i]->Process();
    //}
}



