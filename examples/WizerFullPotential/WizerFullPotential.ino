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

  // Initialize Wizer
  wizer.begin("google.com");

  // Setup event callbacks
  wizer.onDisconnect = []() {
    Serial.println("[Wizer] Detected WiFi disconnection.");
  };

  wizer.onReconnect = []() {
    Serial.println("[Wizer] WiFi connection restored.");
  };
}

void loop()
{
  Serial.println("\n====== Wizer Diagnostic Report ======");

  wizer.update();

  // --- Core Metrics ---
  Serial.print("RSSI            : ");
  Serial.print(wizer.getRSSI());
  Serial.println(" dBm");

  Serial.print("Signal Score    : ");
  Serial.print(wizer.getSignalScore());
  Serial.println(" / 100");

  Serial.print("Latency         : ");
  Serial.print(wizer.getLatency());
  Serial.println(" ms");

  Serial.print("Jitter          : ");
  Serial.print(wizer.getJitter(), 2);
  Serial.println(" ms");

  Serial.print("Packet Loss     : ");
  Serial.print(wizer.getPacketLoss());
  Serial.println(" %");

  Serial.print("Disconnect Count: ");
  Serial.println(wizer.getDisconnectCount());

  Serial.print("Last Disconnect : ");
  Serial.print(wizer.getLastDisconnectTime());
  Serial.println(" ms since boot");

  Serial.print("Local IP        : ");
  Serial.println(wizer.getLocalIP());

  // --- Captive Portal Detection ---
  bool captive = wizer.isCaptivePortalActive();
  Serial.print("Captive Portal? : ");
  Serial.println(captive ? "YES" : "NO");

  // --- HTTP Diagnostics ---
  Serial.println("\n[HTTP Diagnostic Test]");
  ConnectionInfo info = wizer.testConnection("http://example.com");

  Serial.print("Success         : ");
  Serial.println(info.success ? "YES" : "NO");

  Serial.print("Status Code     : ");
  Serial.println(info.statusCode);

  Serial.print("Response Time   : ");
  Serial.print(info.responseTime);
  Serial.println(" ms");

  if (info.redirected)
  {
    Serial.print("Redirected To   : ");
    Serial.println(info.redirectLocation);
  }

  if (!info.errorMessage.isEmpty())
  {
    Serial.print("Error Message   : ");
    Serial.println(info.errorMessage);
  }

  // --- Speed Test ---
  Serial.println("\n[Bandwidth Tests]");

  float down = wizer.testDownloadSpeed("http://speedtest.tele2.net/1MB.zip");
  Serial.print("Download Speed  : ");
  Serial.print(down);
  Serial.println(" KB/s");

  float up = wizer.testUploadSpeed("http://httpbin.org/post");
  Serial.print("Upload Speed    : ");
  Serial.print(up);
  Serial.println(" KB/s");

  // --- WiFi Scan ---
  Serial.println("\n[Nearby WiFi Networks]");
  std::vector<NetworkInfo> nets = wizer.scanNetworks();
  for (auto &net : nets)
  {
    Serial.print("SSID: ");
    Serial.print(net.ssid);
    Serial.print(" | RSSI: ");
    Serial.print(net.rssi);
    Serial.print(" | Channel: ");
    Serial.print(net.channel);
    Serial.print(" | Encryption: ");
    Serial.println(net.encryption);
  }

  delay(15000); // Run every 15 seconds
}
