#include <M5Atom.h>
#include "ATOM_DTU_NB.h"
#include <PubSubClient.h>
#include <TinyGsmClient.h>
#include <time.h>
#include <sys/time.h>

#define MQTT_BROKER   "mqtt.m5stack.com"
#define MQTT_PORT     1883
#define MQTT_USERNAME "DTU_NB"
#define MQTT_PASSWORD "DTU_PWD"
#define MQTT_D_TOPIC  "DTU_NB/D"
#define MQTT_U_TOPIC  "DTU_NB/U"  //  上传数据主题

#define UPLOAD_INTERVAL 10000
uint32_t lastReconnectAttempt = 0;

TinyGsm modem(SerialAT, ATOM_DTU_SIM7020_RESET);

TinyGsmClient tcpClient(modem);
PubSubClient mqttClient(MQTT_BROKER, MQTT_PORT, tcpClient);

void mqttCallback(char *topic, byte *payload, unsigned int len);
bool mqttConnect(void);
void nbConnect(void);

// Your GPRS credentials, if any
const char apn[]      = "YourAPN";
const char gprsUser[] = "";
const char gprsPass[] = "";

struct tm now;
char s_time[50];

void log(String info) {
    SerialMon.println(info);
}

void setup() {
    M5.begin(true, false, true);
    Serial.println(">>ATOM DTU NB MQTT TEST");
    SerialAT.begin(SIM7020_BAUDRATE, SERIAL_8N1, ATOM_DTU_SIM7020_RX,
                   ATOM_DTU_SIM7020_TX);
    M5.dis.fillpix(0x0000ff);
    nbConnect();
    mqttClient.setCallback(mqttCallback);
}

void loop() {
    static unsigned long timer = 0;

    if (!mqttClient.connected()) {
        log(">>MQTT NOT CONNECTED");
        // Reconnect every 10 seconds
        M5.dis.fillpix(0xff0000);
        uint32_t t = millis();
        if (t - lastReconnectAttempt > 3000L) {
            lastReconnectAttempt = t;
            if (mqttConnect()) {
                lastReconnectAttempt = 0;
            }
        }
        delay(100);
    }
    if (millis() >= timer) {
        timer = millis() + UPLOAD_INTERVAL;
        mqttClient.publish(MQTT_U_TOPIC, "hello");  // 发送数据
    }
    M5.dis.fillpix(0x00ff00);
    mqttClient.loop();
}

void mqttCallback(char *topic, byte *payload, unsigned int len) {
    char info[len];
    memcpy(info, payload, len);
    log("Message arrived [" + String(topic) + "]: ");
    log(info);
}

bool mqttConnect(void) {
    log("Connecting to ");
    log(MQTT_BROKER);
    // Connect to MQTT Broker
    String mqttid = ("MQTTID_" + String(random(65536)));
    bool status =
        mqttClient.connect(mqttid.c_str(), MQTT_USERNAME, MQTT_PASSWORD);
    if (status == false) {
        log(" fail");
        return false;
    }
    log("MQTT CONNECTED!");
    mqttClient.publish(MQTT_U_TOPIC, "NB MQTT CLIENT ONLINE");
    mqttClient.subscribe(MQTT_D_TOPIC);
    return mqttClient.connected();
}

void nbConnect(void) {
    unsigned long start = millis();
    log("Initializing modem...");
    while (!modem.init()) {
        log("waiting...." + String((millis() - start) / 1000) + "s");
    };

    start = millis();
    log("Waiting for network...");
    while (!modem.waitForNetwork()) {
        log("waiting...." + String((millis() - start) / 1000) + "s");
    }
    log("success");
}
