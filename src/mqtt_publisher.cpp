#include "mqtt_publisher.h"





MQTT_publish::MQTT_publish(MQTT_POU * mqtt_client,ZALUZIE * zaluzie_, int offset_):
    BASE_MODUL("MQ_pub"),
    mqtt(mqtt_client),
    zaluzie(zaluzie_),
    current_public_index(-1),
    offset(offset_)
{

    std::string topic="modbus/set/zaluzie";
    for(int i=0;i<zaluzie->getZaluzCnt();i++){
        mqtt->subscribe_msg(topic+num2str(i+offset));
    }

}

void MQTT_publish::procesMS(){
    if(mqtt==nullptr)
        return;
    
    if(mqtt->isRecMsg()){
        MQTT_POU::MQTT_MSG_T msg=mqtt->getRecMsg();
        printf("Rec: %s ->  %s\n",msg.topic.c_str(),msg.msg.c_str());

        size_t pos = msg.topic.find("zaluzie");
        if (pos != std::string::npos) {
            pos += 7; // Length of "zaluzie"
            std::string number_str = msg.topic.substr(pos); // Extract the substring after "zaluzie"
            int index = std::stoi(number_str);         // Convert it to an integer
            index=index-offset;
            if(msg.msg.find("STOP")!=std::string::npos){
                printf("Settting zaluz %d STOP\n",index);
                zaluzie->stop(index);
            }else{
                bool ok;
                int value = str2num(msg.msg, &ok);
                if(ok){
                    printf("Settting zaluz %d to %d\n",index,value);
                    zaluzie->setPosition(value,index);
                }else{
                    printf("Cannot convert val to int!!\n");        
                }
            }
        } else {
            printf("Invalid format of mqqt msg!\n");
        }
    }
}

void MQTT_publish::procesS()
{
    if(current_public_index==-1)
        return;

    if(mqtt==nullptr || zaluzie==nullptr){
        return;
    }



    std::string topic="modbus/zaluzie";
    mqtt->public_msg(num2str(zaluzie->getZaluzPosition(current_public_index)),topic+num2str(current_public_index+offset) );

    current_public_index++;
    if(current_public_index>=zaluzie->getZaluzCnt())
        current_public_index=-1;
}

void MQTT_publish::proces10S(){
    if(current_public_index<0){//neprobíha žadný publish
        current_public_index=0;
    }
}


void MQTT_publish::proces60S(){

}