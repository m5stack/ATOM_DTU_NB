
#include <Arduino.h>

using dtu_nb_command_t = String;
using dtu_nb_response_t = String;

class AtCommandResponse
{
public:
    AtCommandResponse() : m_valid(false) {};
    AtCommandResponse(const dtu_nb_response_t& response_str);

    bool parse(const dtu_nb_response_t& buffer, dtu_nb_response_t& echo, dtu_nb_response_t& payload, dtu_nb_response_t& status);

    String to_string();
    bool is_valid() {return m_valid;}
    dtu_nb_response_t payload() {return m_payload};
    dtu_nb_response_t status() {return m_status};

    static constexpr char* delimiter = "\r\n";
    
private:
    dtu_nb_command_t m_echo;
    dtu_nb_response_t m_payload;
    dtu_nb_response_t m_status;
    bool m_valid;
};

class ATOM_DTU_NB
{
    private:
        HardwareSerial *_serial;
    public:
        void init(HardwareSerial *serial = &Serial2, uint8_t RX = 19, uint8_t TX = 22);

        dtu_nb_response_t wait_message(unsigned long time);
        void write(const dtu_nb_command_t& command);
        AtCommandResponse send_query(const dtu_nb_command_t& command, unsigned long timeout=1000);	

	AtCommandResponse get_ccid();
	AtCommandResponse get_cimi();
	AtCommandResponse get_device_model();
	AtCommandResponse get_device_mfg();

        AtCommandResponse set_command_echo(bool enable=true);
        bool reset_device(unsigned long timeout=10000);
};


