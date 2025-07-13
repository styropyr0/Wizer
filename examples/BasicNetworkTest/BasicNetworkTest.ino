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
    analyze WiFi signal quality, diagnose HTTP connection health,
    and test download/upload bandwidth on ESP-based boards.

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

    Serial.println("== Network Quality ==");
    Serial.print("Latency: ");
    Serial.print(wizer.getLatency());
    Serial.println(" ms");
    Serial.print("Jitter: ");
    Serial.print(wizer.getJitter());
    Serial.println(" ms");
    Serial.print("Packet Loss: ");
    Serial.print(wizer.getPacketLoss());
    Serial.println(" %");
    Serial.print("Signal Score: ");
    Serial.println(wizer.getSignalScore());

    delay(5000);
}
