#ifndef HEADER_H_ZALUZIE     // equivalently, #if !defined HEADER_H_
#define HEADER_H_ZALUZIE

#include "jolib.h"

#include "base_modul.h"
#include "gpio.h"


class ZALUZ:public BASE_MODUL{
public:
 
    enum ZALUZ_STATE{
        OPEN,   // otevreno 
        CLOSE,  //zatazeno
        CLOSE_LIGHT,//zataženo ale průchod svetla
    };


    ZALUZ(GPIO_BASE * gpioInterface,int index):
        BASE_MODUL("zaluz"),
        position(0),
        maxDownTime(5000000),
        hystereze(20),
        zaluzie_index(index),
        lastProcessedTime(0),
        state(OPEN),
        gpio(gpioInterface),
        doubleUpRequest(false),
        doubleDownRequest(false),
        isPrinted(false){
            
    }
   
    void setState(ZALUZ_STATE state); // jedeme do daneho stavu - např. modbusem

    void runUp();//jedeme nahoru
    void runDown();//jedeme dolu
    void stop();//zastavujeme vsechny pohyby

    void process()override;
    void procesMS()override;
    ZALUZ_STATE getState()const{return state;}
    uint8_t getPositionPercent()const;//vrati pozici v pracentech

    void setBtnUp(int btnIndex){
        btnsUp.push_back(btnIndex);
        gpio->setBtnCallback(btnStateChanged,0x01<<btnIndex,this);
    }
    void setBtnDown(int btnIndex){
        btnsDown.push_back(btnIndex);
        gpio->setBtnCallback(btnStateChanged,0x01<<btnIndex,this);
    }

private:
    uint32_t position;///<aktualni stav zaluzie (cas [us] ktery jela dolu)
    uint32_t maxDownTime;///<počet [ms] pro plné zavření - ze stavu OPEN -> CLOSE
    const u_int8_t hystereze;///< počet [ms] pri srovnavani pozice (position)
    int zaluzie_index;
    uint64_t lastProcessedTime;///< timestam posledniho volani process()funkce

    ZALUZ_STATE state;

    GPIO_BASE * gpio;

    bool doubleUpRequest;
    bool doubleDownRequest;

    bool isPrinted;//zabraneni neustalemu vypisu do printf

    std::vector<int> btnsDown;///<index tlačítka dolů
    std::vector<int> btnsUp;    ///<index tlačítka nahoru

    struct {///< externí request - z modbusu, mqtt atd - nikoliv z tlačítka
        bool request_valid=false;
        uint32_t position=0;///< pozice v [ms] který má sjet žaluzka dolu
    } request;
    

    void resetAllStates();
    
    static void btnStateChanged(const BUTTON* btn,void * userData);
    void btnStateChanged(const BUTTON* btn);
};



class ZALUZIE: public BASE_MODUL{
public:
    ZALUZIE(GPIO_BASE * gpioInterface);

    ZALUZ::ZALUZ_STATE getZaluzState(int zaluzIndex) const;
    int getZaluzPosition(int zaluzIndex) const;

    void setState(ZALUZ::ZALUZ_STATE newState, int zaluzIndex);
protected:
    GPIO_BASE * gpio;
    std::vector<ZALUZ*> zaluzie;
    const int ZALUZ_CNT=6;


    virtual void process()override;
    virtual void procesMS()override;
    static void btnStateChanged(const BUTTON* btn,void * userData);
    
    void btnStateChanged(const BUTTON* btn);
};


#endif