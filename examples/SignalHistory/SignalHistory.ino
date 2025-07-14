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
    analyze WiFi signal quality, Signal History.

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
    WiFi.begin("your-ssid", "your-password");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected!");
    wizer.begin();
}

void loop()
{
    wizer.update();
    Serial.println("== Current Signal ==");
    Serial.print("RSSI: ");
    Serial.println(wizer.getRSSI());

    if (millis() % 30000 < 100)
    {
        Serial.println("== Signal History (JSON) ==");
        Serial.println(wizer.getSignalHistoryJson());
    }

    delay(5000);
}
