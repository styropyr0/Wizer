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

    Serial.println("\nConnected to WiFi");
    wizer.begin();

    // Event hooks
    wizer.onDisconnect = []()
    {
        Serial.println("WiFi disconnected!");
    };
    wizer.onReconnect = []()
    {
        Serial.println("WiFi reconnected!");
    };
}

void loop()
{
    wizer.update();

    Serial.println("== Summary Metrics ==");
    Serial.println(wizer.toJson());

    // HTTP Test
    ConnectionInfo conn = wizer.testConnection("http://blender.org");
    Serial.println("== HTTP Connection ==");
    Serial.println(wizer.connectionToJson(conn));

    // Download Test (small test file)
    float downloadSpeed = wizer.testDownloadSpeed("http://speedtest.tele2.net/100KB.zip");
    Serial.print("Download Speed: ");
    Serial.print(downloadSpeed);
    Serial.println(" KB/s");

    // Upload Test (echo endpoint)
    float uploadSpeed = wizer.testUploadSpeed("http://httpbin.org/post");
    Serial.print("Upload Speed: ");
    Serial.print(uploadSpeed);
    Serial.println(" KB/s");

    // Captive Portal Check
    bool captive = wizer.isCaptivePortalActive();
    Serial.print("Captive Portal: ");
    Serial.println(captive ? "Yes" : "No");

    delay(10000);
}
