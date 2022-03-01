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

ATOM_DTU_NB dtu;

void setup()
{
    M5.begin();
    //SIM7020
    dtu.init(&Serial2, 19, 22);
    dtu.reset_device();
}

String response;

void show_query(String command, timeout=1000) 
{
  Serial.println(dtu.send_query(command, timeout=timeout).to_string());
}

void loop()
{
    show_query("AT+CSMINS=?");

    while(1){
        response = dtu.send_query("AT+CSQ\r\n")
        Serial.print(response.to_string());
        if(readstr.payload().indexOf("0,0") == -1){
            break;
        }
    }

    show_query("AT+CREG?");
    show_query("AT+COPS?");

    //Create MQTT connection
    //If succeed, MQTT id will return.

    show_query("AT+CMQNEW=\"broker.emqx.io\",\"1883\",12000,1024", timeout=5000);
    response = dtu.send_query("AT+CMQCON=0,3,\"myclient\",600,1,0", timeout=5000);
    Serial.println(response.to_string());

    if(response.status().indexOf("OK") !=-1){
        show_query("AT+CMQSUB=0,\"mytopic\",1", timeout=5000);
        
        while(1){
            M5.update();
            readstr = dtu.wait_message(0);
            Serial.print(readstr);
            //Click Btn Public Topic
            if(M5.Btn.wasPressed()){
                response = dtu.send_query("AT+CMQPUB=0,\"mytopic\",1,0,0,8,\"31323334\"", timeout=5000);
                Serial.print(response.to_string());
                if(response.status().indexOf("ERR") !=-1) {
                    ESP.restart();
                }
            }
        }
    }
}
