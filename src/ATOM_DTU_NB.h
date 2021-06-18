
#include <Arduino.h>

class ATOM_DTU_NB
{
    private:
        HardwareSerial *_serial;
    public:
        void Init(HardwareSerial *serial = &Serial2, uint8_t RX = 19, uint8_t TX = 22);
        String waitMsg(int time);
        void sendMsg(String command);
};