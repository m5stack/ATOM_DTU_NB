#include "ATOM_DTU_NB.h"


void ATOM_DTU_NB::init(HardwareSerial *serial, uint8_t RX, uint8_t TX) {
    _serial = serial;
    _serial->begin(115200, SERIAL_8N1, RX, TX);
    set_command_echo();
}


dtu_nb_response_t ATOM_DTU_NB::wait_message(unsigned long time) {
    dtu_nb_response_t restr;
    unsigned long start = millis();
    while(1){
        if(Serial2.available() || (millis() - start) < time) {
            String str = Serial2.readString();
                restr += str;
        }else{
            break;
        }
    }
    return restr;
}

void ATOM_DTU_NB::write(const dtu_nb_command_t& command) {
    _serial->printf("%s\r\n", command);
}

AtCommandResponse ATOM_DTU_NB::send_query(const dtu_nb_command_t& command, unsigned long timeout)
{
    write(command);
    dtu_nb_response_t rx_buff;
    unsigned long start = millis();
    AtCommandResponse retval;
    while(1) {
        if(Serial2.available() || (millis() - start) < timeout) {
            String frame = Serial2.readString();
            rx_buff += frame;
            retval = AtCommandResponse(rx_buff);
            if (retval.is_valid())
            {
                return retval;
            }
        } else {
            return AtCommandResponse(dtu_nb_response_t());
        }
    }
}

AtCommandResponse ATOM_DTU_NB::get_ccid() {
    return send_query("AT+CCID");
}

AtCommandResponse ATOM_DTU_NB::get_cimi() {
    return send_query("AT+CIMI");
}

AtCommandResponse ATOM_DTU_NB::get_device_model() {
    return send_query("AT+CGMM");
}

AtCommandResponse ATOM_DTU_NB::get_device_mfg() {
    return send_query("AT+CGMI");
}

bool ATOM_DTU_NB::reset_device(unsigned long timeout) {
    unsigned long start = millis();
    write("AT+CRESET");
    wait_message(100);
    while((send_query("AT").is_valid() == false) && ((millis() - start) < timeout))
    {
        delay(1000);
    }
    return (millis() - start) < timeout;
}

AtCommandResponse ATOM_DTU_NB::set_command_echo(bool enable) {
    return send_query(enable?"ATE1":"ATE0");
}

AtCommandResponse::AtCommandResponse(const dtu_nb_response_t& response_str)
{
    m_valid = parse(response_str, m_echo, m_payload, m_status);
}

bool AtCommandResponse::parse(const dtu_nb_response_t& buffer, dtu_nb_response_t& echo, dtu_nb_response_t& payload, dtu_nb_response_t& status)
{
    char* as_char = (char*)buffer.c_str();

    // initialize first part (string, delimiter)
    char* ptr = strtok(as_char, delimiter);
    bool have_command = false;
    dtu_nb_response_t last_part = dtu_nb_response_t("");

    while(ptr != NULL) {
        if (!have_command)
        {
            echo = dtu_nb_response_t(ptr);
            payload = dtu_nb_response_t("");
            status = dtu_nb_response_t("");
            have_command = true;
        }
        else
        {
            payload += last_part;
            last_part = dtu_nb_response_t(ptr);
        }
        // create next part
        ptr = strtok(NULL, delimiter);
    }
    status = last_part;

    return have_command;
}

String AtCommandResponse::to_string()
{
    return String("echo(") + m_echo + 
           String(") payload(") + m_payload + 
           String(") status(") + m_status + 
           String(") valid(") + String(m_valid?"true":"false") + String(")"); 
}
