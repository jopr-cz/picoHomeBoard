#ifndef HEADER_H_MQTT_PUBLISHER_POU     // equivalently, #if !defined HEADER_H_
#define HEADER_H_MQTT_PUBLISHER_POU


#include "base_modul.h"
#include "mqtt_pou.h"
#include "zaluzie.h"

class WIFI;
class MQTT_publish: public BASE_MODUL{
public:
    MQTT_publish(MQTT_POU * mqtt_client,ZALUZIE * zaluzie_, int offset_, WIFI* _wifi);

protected:
    MQTT_POU * mqtt;
    ZALUZIE *  zaluzie;
    int offset;//žaluziovy ofset žaluzie č. = offset + žaluz číslo
    WIFI * wifi;

    void send_zaluz_state(int zaluzID);

    int getNumber(const std::string & str)const;


    virtual void procesMS()override;
    virtual void procesS()override;
    virtual void proces10S()override;
    virtual void proces60S()override;


};




#endif
