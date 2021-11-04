/*
    Description: 
    Use ATOM DTU NB Create HTTP request GET / POST.
    Check the status through Serial.
    Please install library before compiling:  
    FastLED: https://github.com/FastLED/FastLED
    M5Atom: https://github.com/m5stack/M5Atom
*/

#include "ATOM_DTU_NB.h"
#include "M5Atom.h"

ATOM_DTU_NB dtu;

void setup()
{
    M5.begin(true, true, true);
    //SIM7020
    dtu.init(&Serial2, 19, 22);
    dtu.reset_device();
    M5.dis.drawpix(0, 0xff0000);
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

    //Create HTTP host instance
    show_query("AT+CHTTPCREATE=\"http://api.m5stack.com/\"", timeout=5000);

    //Connect server
    show_query("AT+CHTTPCON=0", timeout=5000);

    //HTTP GET
    response = dtu.send_query("AT+CHTTPSEND=0,0,\"/v1\"", timeout=5000);
    Serial.println(response.to_string());

    //HTTP POST
    //response = show_query("AT+CHTTPSEND=0,1,\"/v1\",48656c6c6f204d352055736572", timeout=5000);
    //Serial.println(response.to_string());


    if(response.status().indexOf("OK") !=-1){
      M5.dis.drawpix(0, 0x0000ff);
      while(Serial2.available()){
          Serial.print(Serial2.readString());
      }
    }else{
      M5.dis.drawpix(0, 0x00ff00);
    }

    dtu.write("AT+CHTTPDISCON=0");
    show_query("AT+CHTTPDESTROY=0", timeout=5000);

}
