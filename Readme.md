# Wizer

**Wizer** is a comprehensive WiFi network diagnostics library for ESP32 and ESP8266 boards. It offers advanced tools to monitor signal strength, measure latency and jitter, detect captive portals, run HTTP diagnostics, scan nearby WiFi networks, and perform bandwidth testing.

---

## Features

- **Latency and Jitter Measurement** – Monitors real-time delay and signal stability.
- **Signal Strength Analysis** – Provides RSSI and a derived signal quality score.
- **HTTP Diagnostics** – Tests endpoints and provides response time, status code, and redirection info.
- **Bandwidth Testing** – Performs basic upload and download speed estimation.
- **WiFi Scanning** – Lists nearby WiFi networks with encryption type and signal info.
- **Connection Monitoring** – Logs disconnect events and timestamps.
- **Captive Portal Detection** – Useful for identifying login pages in public networks.
- **Signal History** – Tracks signal variations over time with optional JSON export.

---

## Installation

1. Copy the `Wizer.h`, `Wizer.cpp`, and `WizerConst.h` files into your Arduino/PlatformIO project.
2. Include Wizer in your sketch:

```cpp
#include "Wizer.h"
```

3. Ensure you have WiFi credentials set and connected before using Wizer methods.

---

## Basic Usage

```cpp
WiFi.begin("YourSSID", "YourPassword");
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}

Wizer wizer;
wizer.begin();

void loop() {
  wizer.update();
  delay(5000);
}
```

---

## Method Reference (Detailed)

### `void begin(const char* host = WIZER_DEFAULT_HOST);`
Initializes the Wizer engine. You must call this once in `setup()`.

- **@param** `host` – The hostname used for ping diagnostics (default: `google.com`).

### `void update();`
Runs the diagnostics cycle:
- Checks WiFi status (disconnect/reconnect events).
- Performs ping tests.
- Computes signal score.
- Records signal sample to history.

### `void setClient(WiFiClient *externalClient);`
Injects an external `WiFiClient` object if you want Wizer to reuse an existing client.

- **@param** `externalClient` – A pointer to an externally managed `WiFiClient`.

### `int getLatency() const;`
Returns the average ping round-trip time (RTT) in milliseconds.

- **@return** Latency in ms, or -1 if all pings failed.

### `float getJitter() const;`
Calculates jitter based on variance between ping RTTs.

- **@return** Jitter value in ms.

### `int getPacketLoss() const;`
Measures percentage of failed pings.

- **@return** Packet loss in %.

### `int getSignalScore() const;`
Returns a normalized signal quality score (0–100) derived from RSSI.

- **@return** Signal quality score.

### `int getRSSI() const;`
Returns the last known RSSI value from the WiFi interface.

- **@return** Signal strength in dBm.

### `ConnectionInfo testConnection(const char* url, int timeout = 3000);`
Tests connectivity to an HTTP server.

- **@param** `url` – Full URL to test (e.g., `http://example.com`).
- **@param** `timeout` – Timeout for HTTP request (default 3000 ms).
- **@return** Struct containing connection success, status code, response time, and redirection info.

### `int getHttpResponseTime() const;`
Returns response time (ms) from last `testConnection()` call.

- **@return** Time in ms.

### `int getHttpStatusCode() const;`
Returns last received HTTP status code.

- **@return** Status code (e.g., 200, 301, 404).

### `float testDownloadSpeed(const char* testUrl);`
Downloads a test file and estimates speed.

- **@param** `testUrl` – URL of the file to download.
- **@return** Speed in KB/s. Returns 0.0 if failed.

### `float testUploadSpeed(const char* serverUrl);`
Uploads dummy data to test server.

- **@param** `serverUrl` – Target URL to upload to (must accept POST).
- **@return** Upload speed in KB/s. Returns 0.0 if failed.

### `std::vector<NetworkInfo> scanNetworks();`
Scans and returns a list of visible WiFi networks.

- **@return** Vector of `NetworkInfo` structs.

### `int getDisconnectCount() const;`
Returns the number of detected disconnections since boot.

- **@return** Count of disconnects.

### `unsigned long getLastDisconnectTime() const;`
Timestamp of the most recent disconnect (in ms).

- **@return** Millis since boot.

### `IPAddress getLocalIP() const;`
Returns the current IP address of the device.

- **@return** IPAddress object.

### `bool isCaptivePortalActive(const String url = "http://clients3.google.com/generate_204") const;`
Detects presence of captive portals by checking for redirection or content mismatch.

- **@param** `url` – URL that normally returns 204 with no content.
- **@return** True if captive portal is likely active.

### `std::vector<SignalSample> getSignalHistory() const;`
Returns a history of recent signal samples.

- **@return** Vector of `SignalSample`.

### `void clearSignalHistory();`
Clears stored signal samples.

### `void recordSignalSample();`
Manually records current signal values into history.

### `String getSignalHistoryJson() const;`
Exports the signal history as a JSON string.

- **@return** JSON-formatted array of signal samples.

### `String toJson() const;`
Exports current diagnostic values as a JSON object.

- **@return** JSON string of latency, jitter, RSSI, etc.

### `String connectionToJson(const ConnectionInfo &info) const;`
Converts a `ConnectionInfo` struct into a JSON string.

- **@param** `info` – Connection struct.
- **@return** JSON string with full connection detail.

---

## Event Hooks

### `std::function<void()> onDisconnect;`
Called automatically when WiFi drops.

### `std::function<void()> onReconnect;`
Called when WiFi reconnects after disconnect.

---

## License

MIT License – use, modify, and distribute freely.

## Author

**Saurav Sajeev**  
GitHub: [https://github.com/sauravsajeew/wizer](https://github.com/sauravsajeew/wizer) *(placeholder)*

