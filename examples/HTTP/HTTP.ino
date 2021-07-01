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
    M5.begin(true, true, true);
    //SIM7020
    DTU.Init(&Serial2, 19, 22);
    // DTU.Init();
    //Reset Module
    DTU.sendMsg("AT+CRESET\r\n");
    delay(5000);
    M5.dis.drawpix(0, 0xff0000);
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
        if(readstr.indexOf("0,0") ==-1 || readstr.indexOf("99") ==-1 ){
            break;
        }
    }

    DTU.sendMsg("AT+CREG?\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);

    DTU.sendMsg("AT+COPS?\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);

    //Create HTTP host instance
    DTU.sendMsg("AT+CHTTPCREATE=\"http://api.m5stack.com/\"\r\n");
    readstr = DTU.waitMsg(5000);
    Serial.print(readstr);

    //Connect server
    DTU.sendMsg("AT+CHTTPCON=0\r\n");
    readstr = DTU.waitMsg(5000);
    Serial.print(readstr);

    //HTTP GET
    DTU.sendMsg("AT+CHTTPSEND=0,0,\"/v1\"\r\n");
    readstr = DTU.waitMsg(5000);
    Serial.print(readstr);

    //HTTP POST
    // DTU.sendMsg("AT+CHTTPSEND=0,1,\"/v1\",48656c6c6f204d352055736572\r\n");
    // readstr = DTU.waitMsg(5000);
    // Serial.print(readstr);


    if(readstr.indexOf("OK") !=-1){
      M5.dis.drawpix(0, 0x0000ff);
      while(Serial2.available()){
          Serial.print(Serial2.readString());
      }
    }else{
      M5.dis.drawpix(0, 0x00ff00);
    }

    DTU.sendMsg("AT+CHTTPDISCON=0\r\n");
    DTU.sendMsg("AT+CHTTPDESTROY=0\r\n");
    readstr = DTU.waitMsg(1000);
    Serial.print(readstr);

}
