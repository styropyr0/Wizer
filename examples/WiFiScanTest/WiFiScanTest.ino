/*
  ██     ██ ██ ███████ ███████ ██████  
  ██     ██ ██      ██ ██      ██   ██ 
  ██  █  ██ ██   ███   █████   ██████  
  ██ ███ ██ ██ ██      ██      ██   ██ 
   ███ ███  ██ ███████ ███████ ██   ██ 

  WIZER - Network Diagnostics Library for ESP32 / ESP8266
  --------------------------------------------------------
  Version   : 1.0.0
  Author    : Saurav Sajeev
  GitHub    : https://github.com/styropyr0/wizer
  License   : MIT (You may reuse, modify, and distribute freely)

  Description:
    This sketch demonstrates the use of the Wizer library to
    analyze WiFi signal quality.

  NOTE:
    This library does NOT manage or initiate WiFi connections. 
    You are responsible for connecting to WiFi before calling Wizer methods.

  --------------------------------------------------------
*/

#include <Wizer.h>

Wizer wizer;

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(); // disconnect any previous connections. this is important for ESP32/ESP8266

    delay(2000);
    std::vector<NetworkInfo> networks = wizer.scanNetworks();

    Serial.println("== Nearby Networks ==");
    for (const auto &net : networks)
    {
        Serial.print(net.ssid);
        Serial.print(" (");
        Serial.print(net.rssi);
        Serial.print(" dBm), Ch ");
        Serial.print(net.channel);
        Serial.print(", ");
        Serial.println(net.encryption);
    }
}

void loop() {}
