/*
    Description: 
    Use ATOM DTU NB to connect to the MQTT server, and implement subscription and publishing messages.
    Check the status through Serial. When the MQTT connection is successful, Click Btn Public Topic
    Please install library before compiling:  
    FastLED: https://github.com/FastLED/FastLED
    M5Atom: https://github.com/m5stack/M5Atom
*/

#include "ATOM_DTU_NB.h"
#include "M5Atom.h"

ATOM_DTU_NB DTU;

void setup()
{
    M5.begin();
    //SIM7020
    DTU.Init(&Serial2, 19, 22);
    // DTU.Init();
    //Reset Module
    DTU.sendMsg("AT+CRESET\r\n");
    delay(5000);
}

String readstr;

void loop()
{

    DTU.sendMsg("AT+CSMINS=?\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);

    while(1){
        DTU.sendMsg("AT+CSQ\r\n\r\n");
        readstr = DTU.waitMsg(1000);
        Serial.print(readstr);
        if(readstr.indexOf("0,0") ==-1){
            break;
        }
    }

    DTU.sendMsg("AT+CREG?\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);

    DTU.sendMsg("AT+COPS?\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);

    //Create MQTT connection
    //If succeed, MQTT id will return.
    DTU.sendMsg("AT+CMQNEW=\"broker.emqx.io\",\"1883\",12000,1024\r\n");
    readstr = DTU.waitMsg(5000);
    Serial.print(readstr);

    DTU.sendMsg("AT+CMQCON=0,3,\"myclient\",600,1,0\r\n");
    readstr = DTU.waitMsg(5000);
    Serial.print(readstr);

    if(readstr.indexOf("OK") !=-1){
        DTU.sendMsg("AT+CMQSUB=0,\"mytopic\",1\r\n");
        readstr = DTU.waitMsg(5000);
        Serial.print(readstr);
        
        while(1){
            M5.update();
            readstr = DTU.waitMsg(0);
            Serial.print(readstr);
            //Click Btn Public Topic
            if(M5.Btn.wasPressed()){
                DTU.sendMsg("AT+CMQPUB=0,\"mytopic\",1,0,0,8,\"31323334\"\r\n");
                readstr = DTU.waitMsg(5000);
                Serial.print(readstr);
                if(readstr.indexOf("ERR") !=-1) {
                    ESP.restart();
                }
            }
        }
    }
}
