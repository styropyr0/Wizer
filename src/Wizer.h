#ifndef WIZER_H
#define WIZER_H

#if defined(ESP32)
#include <WiFi.h>
#include <HTTPClient.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#else
#error "Wizer is only supported on ESP32 and ESP8266 platforms."
#endif

#include <Arduino.h>
#include <vector>
#include <functional>

#define WIZER_DEFAULT_HOST "google.com"
#define MAX_HISTORY_ENTRIES 20

/**
 * @brief Struct to hold information about a scanned WiFi network.
 */
struct NetworkInfo
{
    String ssid;       ///< SSID of the network
    int rssi;          ///< Signal strength
    int channel;       ///< WiFi channel
    String encryption; ///< Encryption type
};

/**
 * @brief Struct to represent details of an HTTP connection attempt.
 */
struct ConnectionInfo
{
    bool success;            ///< Whether the connection was successful
    int statusCode;          ///< HTTP status code
    int responseTime;        ///< Time taken to get response (ms)
    String url;              ///< URL tested
    bool redirected;         ///< Whether it was redirected
    String redirectLocation; ///< New location if redirected
    String errorMessage;     ///< Any error messages captured
};

/**
 * @brief Struct to store historical signal samples.
 */
struct SignalSample
{
    int rssi;                ///< RSSI value
    int signalScore;         ///< Calculated signal score
    unsigned long timestamp; ///< Time the sample was recorded
};

/**
 * @brief Wizer is a library for monitoring and analyzing WiFi connections on ESP32 and ESP8266 devices.
 *
 * It provides functionalities for network quality assessment, HTTP diagnostics, bandwidth testing,
 * WiFi scanning, connection monitoring, and signal history management.
 *
 * @author Saurav Sajeev
 */
class Wizer
{
public:
    /**
     * @brief Initializes the Wizer library.
     * @param host The host to ping for network quality tests. Defaults to WIZER_DEFAULT_HOST.
     */
    void begin(const char *host = WIZER_DEFAULT_HOST);

    /**
     * @brief Updates network status, performs ping test, and logs signal.
     */
    void update();

    /**
     * @brief Sets an external WiFiClient to be used instead of internal instance.
     * @param externalClient A pointer to an already configured WiFiClient.
     */
    void setClient(WiFiClient *externalClient);

    /**
     * @brief Returns average latency in milliseconds.
     * @return Average latency.
     */
    int getLatency() const;

    /**
     * @brief Returns calculated jitter.
     * @return Jitter value.
     */
    float getJitter() const;

    /**
     * @brief Returns the percentage of packet loss.
     * @return Packet loss percentage.
     */
    int getPacketLoss() const;

    /**
     * @brief Returns signal quality score (0-100).
     * @return Signal score.
     */
    int getSignalScore() const;

    /**
     * @brief Returns the last known RSSI value.
     * @return RSSI value.
     */
    int getRSSI() const;

    /**
     * @brief Tests HTTP connection and returns detailed information.
     * @param url URL to test.
     * @param timeout Timeout in milliseconds. Default is 3000.
     * @return Struct containing connection result.
     */
    ConnectionInfo testConnection(const char *url, int timeout = 3000);

    /**
     * @brief Returns time of last HTTP response.
     * @return Response time in milliseconds.
     */
    int getHttpResponseTime() const;

    /**
     * @brief Returns last HTTP status code received.
     * @return HTTP status code.
     */
    int getHttpStatusCode() const;

    /**
     * @brief Measures download speed.
     * @param testUrl URL of the file to download.
     * @return Download speed in KBps.
     */
    float testDownloadSpeed(const char *testUrl);

    /**
     * @brief Measures upload speed.
     * @param serverUrl URL to upload dummy data to.
     * @return Upload speed in KBps.
     */
    float testUploadSpeed(const char *serverUrl);

    /**
     * @brief Scans for nearby WiFi networks.
     * @return Vector containing information about nearby networks.
     */
    std::vector<NetworkInfo> scanNetworks();

    /**
     * @brief Returns the number of disconnects detected.
     * @return Disconnect count.
     */
    int getDisconnectCount() const;

    /**
     * @brief Returns timestamp of the last disconnect event.
     * @return Time in milliseconds since boot.
     */
    unsigned long getLastDisconnectTime() const;

    /**
     * @brief Returns the local IP address.
     * @return Local IP address.
     */
    IPAddress getLocalIP() const;

    /**
     * @brief Detects presence of a captive portal.
     * @param url URL to test for captive portal. Default is Google endpoint.
     * @return True if captive portal is active, otherwise false.
     */
    bool isCaptivePortalActive(const String url = "http://clients3.google.com/generate_204") const;

    /**
     * @brief Returns history of recorded signal samples.
     * @return Vector of SignalSample.
     */
    std::vector<SignalSample> getSignalHistory() const;

    /**
     * @brief Clears the stored signal history.
     */
    void clearSignalHistory();

    /**
     * @brief Records the current signal sample to history.
     */
    void recordSignalSample();

    /**
     * @brief Returns signal history in JSON format.
     * @return JSON string of signal samples.
     */
    String getSignalHistoryJson() const;

    /**
     * @brief Returns summary of current metrics in JSON format.
     * @return JSON string.
     */
    String toJson() const;

    /**
     * @brief Converts a ConnectionInfo structure to JSON.
     * @param info The connection info to convert.
     * @return JSON string.
     */
    String connectionToJson(const ConnectionInfo &info) const;

    std::function<void()> onDisconnect; ///< Called when WiFi disconnect is detected
    std::function<void()> onReconnect;  ///< Called when WiFi reconnect is detected

private:
    const char *pingHost;
    int latency;
    float jitter;
    int packetLoss;
    int signalScore;
    int lastRSSI;
    int disconnectCount;
    unsigned long lastDisconnectTime;
    int lastHttpCode;
    int lastHttpResponseTime;
    std::vector<SignalSample> signalHistory;
    WiFiClient *client = nullptr;

    static const int sampleSize = 10;

    void calculateSignalScore();
    void pingTest();
    void updateWiFiStatus();
};

#endif