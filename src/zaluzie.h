#ifndef HEADER_H_ZALUZIE     // equivalently, #if !defined HEADER_H_
#define HEADER_H_ZALUZIE

#include "jolib.h"

#include "base_modul.h"
#include "gpio.h"

struct ZALUZ_SETTING{
    uint32_t maxDownTime;
};


class ZALUZ:public BASE_MODUL{
public:
 
    enum ZALUZ_STATE{
        OPEN,   // otevreno 
        CLOSE,  //zatazeno
        CLOSE_LIGHT,//zataženo ale průchod svetla
    };


    ZALUZ(GPIO_BASE * gpioInterface,int index, const ZALUZ_SETTING setting):
        BASE_MODUL("zaluz"),
        position(0),
        maxDownTime(setting.maxDownTime),
        hystereze(1000),
        zaluzie_index(index),
        lastProcessedTime(0),
        state(OPEN),
        gpio(gpioInterface),
        doubleUpRequest(false),
        doubleDownRequest(false),
        isPrinted(false){
            
    }
   
    void setPosition(uint16_t newPositionPercent); // nova zadana pozici v [%]
    void setState(ZALUZ_STATE state); // jedeme do daneho stavu - např. modbusem

    void runUp();//jedeme nahoru
    void runDown();//jedeme dolu
    void stop();//zastavujeme vsechny pohyby

    void process()override;
    void procesMS()override;
    ZALUZ_STATE getState()const{return state;}
    uint8_t getPositionPercent()const;//vrati pozici v pracentech
    uint32_t getMaxDownTime()const{return maxDownTime;}///<v [uS]

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
    uint32_t maxDownTime;///<počet [us] pro plné zavření - ze stavu OPEN -> CLOSE
    const uint16_t hystereze;///< počet [us] pri srovnavani pozice (position)
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
    ZALUZIE(GPIO_BASE * gpioInterface,const ZALUZ_SETTING * setting);

    ZALUZ::ZALUZ_STATE getZaluzState(int zaluzIndex) const;
    int getZaluzPosition(int zaluzIndex) const;
    uint32_t getMaxDownTime(int zaluzIndex) const; ///<v [uS]

    void setState(ZALUZ::ZALUZ_STATE newState, int zaluzIndex);
    void setPosition(uint16_t newPositionPercentage,int zaluzIndex); // nova zadana pozici v [%]

    GPIO_BASE * getGpio()const {return gpio;}
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