#include "ATOM_DTU_NB.h"


void ATOM_DTU_NB::Init(HardwareSerial *serial, uint8_t RX, uint8_t TX) {
    _serial = serial;
    _serial->begin(115200, SERIAL_8N1, RX, TX);
}

String ATOM_DTU_NB::waitMsg(int time) {
    String restr;
    delay(time);
    while(1){
        if(Serial2.available()) {
            String str = Serial2.readString();
                restr += str;
        }else{
            break;
        }
    }
    return restr;
}

void ATOM_DTU_NB::sendMsg(String command) {
    _serial->print(command);
}

