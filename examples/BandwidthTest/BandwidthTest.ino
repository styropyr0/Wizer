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
    test download and upload bandwidth on ESP-based boards.

  NOTE:
    This library does NOT manage or initiate WiFi connections.
    You are responsible for connecting to WiFi before calling Wizer methods.

  --------------------------------------------------------
*/

#include "Wizer.h"

const char *SSID = "YourSSID";
const char *PASSWORD = "YourPassword";

Wizer wizer;

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("Connecting to WiFi...");
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    Serial.println("\n[Wizer Bandwidth Test]");

    float down = wizer.testDownloadSpeed("http://speedtest.tele2.net/100KB.zip");
    Serial.print("Download Speed  : ");
    Serial.print(down);
    Serial.println(" KB/s");

    float up = wizer.testUploadSpeed("http://httpbin.org/post");
    Serial.print("Upload Speed    : ");
    Serial.print(up);
    Serial.println(" KB/s");

    delay(5000); // Run every 5 seconds
}
