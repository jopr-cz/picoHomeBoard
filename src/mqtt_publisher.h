#ifndef HEADER_H_MQTT_PUBLISHER_POU     // equivalently, #if !defined HEADER_H_
#define HEADER_H_MQTT_PUBLISHER_POU


#include "base_modul.h"
#include "mqtt_pou.h"
#include "zaluzie.h"


class MQTT_publish: public BASE_MODUL{
public:
    MQTT_publish(MQTT_POU * mqtt_client,ZALUZIE * zaluzie_, int offset_);

protected:
    MQTT_POU * mqtt;
    ZALUZIE *  zaluzie;
    int offset;//žaluziovy ofset žaluzie č. = offset + žaluz číslo


    virtual void procesMS()override;
    virtual void procesS()override;
    virtual void proces10S()override;
    virtual void proces60S()override;


};




#endif
