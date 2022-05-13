/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with ATOM DTU NB HTTP Client sample source code
* Visit the website for more
information：https://docs.m5stack.com/en/atom/atom_dtu_nb
* describe: ATOM DTU NB HTTP Clien Example.
* Libraries:
    - [TinyGSM - modify](https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/arduino/lib/TinyGSM.zip)
    - [ArduinoHttpClient](https://github.com/arduino-libraries/ArduinoHttpClient)
* date：2022/4/14
*******************************************************************************
*/

#include <M5Atom.h>
#include "ATOM_DTU_NB.h"
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <time.h>
#include <sys/time.h>

const char server[] = "api.m5stack.com";
const char resource[] = "/v1";
const int  port = 80;

TinyGsm modem(SerialAT, ATOM_DTU_SIM7020_RESET);
TinyGsmClient tcpClient(modem);
HttpClient http(tcpClient, server, port);

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
    Serial.println(">>ATOM DTU NB HTTP TEST");
    SerialAT.begin(SIM7020_BAUDRATE, SERIAL_8N1, ATOM_DTU_SIM7020_RX,
                   ATOM_DTU_SIM7020_TX);
    M5.dis.fillpix(0x0000ff);
    nbConnect();
}

void loop() {

  log("Performing HTTP GET request... ");
  int err = http.get(resource);
  if (err != 0) {
    log("failed to connect");
    delay(1000);
    return;
  }

  int status = http.responseStatusCode();
  log("Response status code: ");
  log(status);
  if (!status) {
    delay(10000);
    return;
  }

  log("Response Headers:");
  while (http.headerAvailable()) {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    log("    " + headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    log("Content length is: ");
    log(length);
  }
  if (http.isResponseChunked()) {
    log("The response is chunked");
  }
  String body = http.responseBody();
  log("Response:");
  log(body);
  // Shutdown
  http.stop();
  delay(5000);
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
