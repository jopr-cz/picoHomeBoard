#include "mqtt_publisher.h"


#include "wifi.h"


MQTT_publish::MQTT_publish(MQTT_POU * mqtt_client,ZALUZIE * zaluzie_, int offset_, WIFI * _wifi):
    BASE_MODUL("MQ_pub"),
    mqtt(mqtt_client),
    zaluzie(zaluzie_),
    offset(offset_),
    wifi(_wifi)
{

    std::string topic="modbus/set/zaluzie";
    for(int i=0;i<zaluzie->getZaluzCnt();i++){
        mqtt->subscribe_msg(topic+num2str(i+offset));
    }

    topic="modbus/set/zaluzie_tilt";
    for(int i=0;i<zaluzie->getZaluzCnt();i++){
        mqtt->subscribe_msg(topic+num2str(i+offset));
    }


    for (size_t i = 0; i < maxZaluzCnt_LastMove; i++)    {
        zaluzLastMove[i]=false;
    }
    
    if (zaluzie->getZaluzCnt() > maxZaluzCnt_LastMove) {
        printf("WARNING: MQTT_publish maxZaluzCnt_LastMove too small %d < %d\n", maxZaluzCnt_LastMove, zaluzie->getZaluzCnt());
    }
}


void MQTT_publish::procesMS(){
    if(mqtt==nullptr)
        return;
    
    if(mqtt->isRecMsg()){
        MQTT_POU::MQTT_MSG_T msg=mqtt->getRecMsg();
        printf("Rec: %s ->  %s\n",msg.topic.c_str(),msg.msg.c_str());

        int index=getNumber(msg.topic);
        bool ok;
        int value = str2num(msg.msg, &ok);
        if(index<0)
            return;

        index=index-offset;


        if(msg.msg.find("STOP")!=std::string::npos){
            printf("Settting zaluz %d STOP\n",index);
            zaluzie->stop(index);
            return;
        }

        if(ok==false){
            //
            return;
        }
        if (msg.topic.find("zaluzie_tilt") != std::string::npos) {
            printf("Settting zaluz %d tilt to %d\n",index,value);
            zaluzie->setShutter(value,index);
        }else if (msg.topic.find("zaluzie") != std::string::npos) {
            printf("Settting zaluz %d to %d\n",index,value);
            zaluzie->setPosition(value,index);
        }
    }
}

void MQTT_publish::send_zaluz_state(int zaluzID){
    MQTT_POU::MQTT_MSG_T msg;
    msg.topic="modbus/zaluzie"+num2str(zaluzID+offset)+"/state";
    msg.msg+="{ \"pos\": ";
    msg.msg+=num2str(zaluzie->getZaluzPosition(zaluzID));
    msg.msg+=" , \"shut\": ";
    msg.msg+=num2str(zaluzie->getShutterPosition(zaluzID));
    msg.msg+=" }";
    mqtt->public_buffer_msg(msg);
}



int MQTT_publish::getNumber(const std::string &str) const{
    size_t pos = str.find_first_of("0123456789");
    if (pos != std::string::npos) {
        return std::stoi(str.substr(pos));
    } else {
        //"No number found"
        return -1;
    }
}



void MQTT_publish::procesS(){
    for(int i=0;i<zaluzie->getZaluzCnt();i++){
        if(zaluzie->getZaluzMove(i)!=ZALUZ::MOVE_NONE){
            send_zaluz_state(i);
            zaluzLastMove[i]=true;
        }else if (zaluzLastMove[i]==true ){//abych poslal i posledni stav kdy se přestala hýbat
            send_zaluz_state(i);
            zaluzLastMove[i]=false;
        }
    }
}

void MQTT_publish::proces60S(){
    MQTT_POU::MQTT_MSG_T msg;
    for (int i=0;i<zaluzie->getZaluzCnt();i++) {
        send_zaluz_state(i);
    }

    msg.topic="modbus/zaluzie"+num2str(offset)+"/linkQuality/state";
    msg.msg=num2str(wifi->getSignal());
    mqtt->public_buffer_msg(msg);
}


void MQTT_publish::proces10S(){

}
