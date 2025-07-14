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
    diagnose HTTP connection status, including response time,
    status code, and redirection handling.

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

    Serial.println("\nWiFi Connected");
    wizer.begin();

    ConnectionInfo info = wizer.testConnection("http://google.com");

    Serial.println("== HTTP Test ==");
    Serial.println(wizer.connectionToJson(info));
}

void loop() {}
