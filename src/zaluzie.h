#ifndef HEADER_H_ZALUZIE     // equivalently, #if !defined HEADER_H_
#define HEADER_H_ZALUZIE

#include "jolib.h"

#include "base_modul.h"
#include "gpio_base.h"

struct ZALUZ_SETTING{
    uint32_t maxDownTime;
    uint32_t maxShutterTime;
};



class ZALUZ:public BASE_MODUL{
public:

    static constexpr uint64_t motorTotalDelayTime = 500*1000;///<čas v [uS] mezi vypnutim motoru a zapnutim

 
    enum ZALUZ_STATE{
        OPEN,   // otevreno 
        CLOSE,  //zatazeno
        CLOSE_LIGHT,//zataženo ale průchod svetla
    };

    /// @brief Aktualni pohyb (kam se toci motor)
    enum ZALUZ_MOVE{
        MOVE_NONE,///<motor stoji
        MOVE_UP,///<namotavam
        MOVE_DOWN,///<odmotavam
        SHUTTER_OPEN,///<otviram (motor nahoru)
        SHUTTER_CLOSE,///<zaviram (motor dolu)
        MOTOR_WAITING,///<cekání na přepnutí směru motoru ↑ ↓ - motor se netočí!
        POSITION_RESET,///<reset pozice - pokud je žaluzie zavřená (nahoře) tak ještě chvilku zamotám aby se dotáhla úplně nahoru
    };


    ZALUZ(GPIO_BASE * gpioInterface,int index, const ZALUZ_SETTING setting):
        BASE_MODUL("zaluz"),
        position_reset_time(1*1000*1000),
        position_reset_act_time(0),
        position(setting.maxDownTime),
        maxDownTime(setting.maxDownTime),
        shutter_position(setting.maxShutterTime),//predpokladam ze zaluzie jou zavinute tedy museli byt shuttle otevřen ->max
        maxShutterTime(setting.maxShutterTime),
        hystereze(1000),
        zaluzie_index(index),
        lastProcessedTime(0),
        motorStopTime(0),
        state(OPEN),
        moveState(MOVE_NONE),
        lastMoveState(MOVE_NONE),
        gpio(gpioInterface),
        doubleUpRequest(false),
        doubleDownRequest(false),
        isPrinted(false){

            setPosition(0);
    }
   
    void setPosition(uint16_t newPositionPercent); ///< nova zadana pozici v [%]
    void setShutter(uint16_t newShuttePositionPerent); ///< náklon v [%]
    void setState(ZALUZ_STATE state); ///< jedeme do daneho stavu - např. modbusem

    void runUp();///<jedeme nahoru
    void runDown();///<jedeme dolu
    void stop();///<zastavujeme vsechny pohyby

    void info()const;

    /// @brief Pokud je požadavek na zavření žaluzie, tak to přejedu, abych ji úplně zavřel a vynujuji proměnné
    void resetPosition(); 

    /// @brief MOTOR NAVIJI - zaluzie jedou dolů zavřené, - chci otvirat? motor musi navíjet
    void shutterOpen(); 

    /// @brief MOTOR ODVIJI - žaluzie jednou nahoru otevřené - chci zavirat? motor musi odvijet
    void shutterClose();

    void countMovePosition();
    void setMoveState(ZALUZ_MOVE newState);
    ZALUZ_MOVE getMoveState()const {return moveState;}

    void motor_up();
    void motor_down();
    void motor_stop();

    void process()override;
    void procesMS()override;
    ZALUZ_STATE getState()const{return state;}
    uint8_t getPositionPercent()const;///<vrati pozici v pracentech
    uint8_t getShutterPercent()const;///<vrati pozici v pracentech
    uint32_t getMaxDownTime()const{return maxDownTime;}///<v [uS]
    uint32_t getMaxShutterTime()const{return maxShutterTime;}///<v [uS]

    void setBtnUp(int btnIndex){
        btnsUp.push_back(btnIndex);
        gpio->setBtnCallback(btnStateChanged,0x01<<btnIndex,this);
    }
    void setBtnDown(int btnIndex){
        btnsDown.push_back(btnIndex);
        gpio->setBtnCallback(btnStateChanged,0x01<<btnIndex,this);
    }

private:
    const uint32_t position_reset_time;///<čas v [uS] který motor jede navíc při zatažení žaluzií resetu pozice
    uint32_t position_reset_act_time;///<čas v [uS] který motor jede navíc při resetu pozice

    uint32_t position;///<aktualni stav zaluzie (cas [us] ktery jela dolu)
    uint32_t maxDownTime;///<počet [us] pro plné zavření - ze stavu OPEN -> CLOSE

    /// @brief Aktualni stav nakloneni zaluzie (cas [us] ktery je roven náklonu) 
    ///
    ///     0=zavreno (motor odviji)
    ///
    ///     maxShutterTime=otevreno (motor naviji)
    uint32_t shutter_position;///<0-zavřeno(nepruhledno) , maxShutterTime->nejvíce otevřeno
    uint32_t maxShutterTime;///<počet [us] pro úplné naklonění žaluze
    const uint16_t hystereze;///< počet [us] pri srovnavani pozice (position)
    int zaluzie_index;
    uint64_t lastProcessedTime;///< timestam posledniho volani process()funkce

    uint64_t motorStopTime;///<kdy byl motor zastaven


    ZALUZ_STATE state;
    ZALUZ_MOVE moveState;
    ZALUZ_MOVE lastMoveState;//proměnná pro počítání času mezi stavy (přiřazení času ke správné proměnné)

    GPIO_BASE * gpio;

    bool doubleUpRequest;
    bool doubleDownRequest;

    bool isPrinted;//zabraneni neustalemu vypisu do printf

    std::vector<int> btnsDown;///<index tlačítka dolů
    std::vector<int> btnsUp;    ///<index tlačítka nahoru

    struct {///< externí request - z modbusu, mqtt atd - nikoliv z tlačítka
        bool request_valid=false;
        uint32_t position=0;///< pozice v [ms] který má sjet žaluzka dolu
        uint32_t shutter=0;///< pozice v [ms] naklopeni žaluzií
    } request;
    

    void resetAllStates();
    
    
    static void btnStateChanged(const BUTTON* btn,void * userData);
    void btnStateChanged(const BUTTON* btn);
};



class ZALUZIE: public BASE_MODUL{
public:
    ZALUZIE(GPIO_BASE * gpioInterface,const ZALUZ_SETTING * setting, int ZALUZIE_COUNT=6);

    ZALUZ::ZALUZ_STATE getZaluzState(int zaluzIndex) const;
    ZALUZ::ZALUZ_MOVE getZaluzMove(int zaluzIndex) const;
    int getZaluzPosition(int zaluzIndex) const;
    int getShutterPosition(int zaluzIndex) const;
    uint32_t getMaxDownTime(int zaluzIndex) const; ///<v [uS]
    uint32_t getMaxShutterTime(int zaluzIndex) const; ///<v [uS]

    void info(int zaluzIndex)const;


    void setState(ZALUZ::ZALUZ_STATE newState, int zaluzIndex);
    void setPosition(uint16_t newPositionPercentage, int zaluzIndex); // nova zadana pozici v [%]
    void setShutter(uint16_t newShutterPercentage, int zaluzIndex); // nova zadana pozici v [%]
    void stop(int zaluzIndex);// zastavi pohyb žaluzie

    GPIO_BASE * getGpio()const {return gpio;}
    int getZaluzCnt()const {return ZALUZ_CNT;}
protected:
    GPIO_BASE * gpio;
    std::vector<ZALUZ*> zaluzie;
    const int ZALUZ_CNT;


    virtual void process()override;
    virtual void procesMS()override;
    static void btnStateChanged(const BUTTON* btn,void * userData);
    bool chectZaluzIndex(int index)const;
    
    void btnStateChanged(const BUTTON* btn);
};


#endif
