#include "Wizer.h"

void Wizer::begin(const char *host)
{
    pingHost = host;
    latency = 0;
    jitter = 0.0f;
    packetLoss = 0;
    signalScore = 0;
    lastRSSI = WiFi.RSSI();
    disconnectCount = 0;
    lastDisconnectTime = 0;
    lastHttpCode = 0;
    lastHttpResponseTime = 0;
    signalHistory.clear();
}

void Wizer::update()
{
    updateWiFiStatus();
    pingTest();
    calculateSignalScore();
    recordSignalSample();
}

void Wizer::updateWiFiStatus()
{
    static wl_status_t lastStatus = WL_CONNECTED;
    wl_status_t currentStatus = WiFi.status();
    if (lastStatus == WL_CONNECTED && currentStatus != WL_CONNECTED)
    {
        disconnectCount++;
        lastDisconnectTime = millis();
        if (onDisconnect)
            onDisconnect();
    }
    else if (lastStatus != WL_CONNECTED && currentStatus == WL_CONNECTED)
    {
        if (onReconnect)
            onReconnect();
    }
    lastStatus = currentStatus;
    lastRSSI = WiFi.RSSI();
}

void Wizer::pingTest()
{
    int totalLatency = 0;
    int failed = 0;
    int latencies[sampleSize];

    for (int i = 0; i < sampleSize; ++i)
    {
        unsigned long start = millis();
        WiFiClient tempClient;
        WiFiClient &useClient = client ? *client : tempClient;

        if (useClient.connect(pingHost, 80))
        {
            unsigned long rtt = millis() - start;
            latencies[i] = rtt;
            totalLatency += rtt;
            useClient.stop();
        }
        else
        {
            latencies[i] = -1;
            failed++;
        }
        delay(50);
    }

    packetLoss = (100 * failed) / sampleSize;
    latency = (sampleSize - failed > 0) ? totalLatency / (sampleSize - failed) : -1;

    float totalJitter = 0;
    int lastValid = -1;
    for (int i = 0; i < sampleSize; ++i)
    {
        if (latencies[i] >= 0)
        {
            if (lastValid >= 0)
            {
                totalJitter += abs(latencies[i] - latencies[lastValid]);
            }
            lastValid = i;
        }
    }
    jitter = (sampleSize - failed > 1) ? totalJitter / (sampleSize - failed - 1) : 0.0f;
}

void Wizer::calculateSignalScore()
{
    int rssi = lastRSSI;
    if (rssi >= -50)
        signalScore = 100;
    else if (rssi <= -100)
        signalScore = 0;
    else
        signalScore = 2 * (rssi + 100);
}

int Wizer::getLatency() const { return latency; }
float Wizer::getJitter() const { return jitter; }
int Wizer::getPacketLoss() const { return packetLoss; }
int Wizer::getSignalScore() const { return signalScore; }
int Wizer::getRSSI() const { return lastRSSI; }

ConnectionInfo Wizer::testConnection(const char *url, int timeout)
{
    WiFiClient tempClient;
    WiFiClient &useClient = client ? *client : tempClient;
    HTTPClient http;
    ConnectionInfo result;
    result.url = url;
    result.redirected = false;
    result.redirectLocation = "";
    result.errorMessage = "";

    http.setTimeout(timeout);
    unsigned long start = millis();
    if (!http.begin(useClient, url))
    {
        result.success = false;
        result.statusCode = -1;
        result.responseTime = -1;
        result.errorMessage = "HTTP begin failed";
        return result;
    }

    int code = http.GET();
    unsigned long duration = millis() - start;

    result.success = code >= 200 && code < 400;
    result.statusCode = code;
    result.responseTime = duration;

    if (code >= 300 && code < 400)
    {
        result.redirected = true;
        result.redirectLocation = http.getLocation();
    }

    lastHttpCode = code;
    lastHttpResponseTime = duration;

    http.end();
    return result;
}

int Wizer::getHttpResponseTime() const { return lastHttpResponseTime; }
int Wizer::getHttpStatusCode() const { return lastHttpCode; }

float Wizer::testDownloadSpeed(const char *testUrl)
{
    WiFiClient tempClient;
    WiFiClient &useClient = client ? *client : tempClient;
    HTTPClient http;
    http.begin(useClient, testUrl);
    unsigned long start = millis();
    int code = http.GET();
    if (code > 0 && http.getSize() > 0)
    {
        WiFiClient *stream = http.getStreamPtr();
        size_t total = 0;
        while (http.connected() && total < http.getSize())
        {
            size_t available = stream->available();
            if (available)
            {
                uint8_t buff[128];
                int readBytes = stream->readBytes(buff, min(available, sizeof(buff)));
                total += readBytes;
            }
            delay(1);
        }
        unsigned long duration = millis() - start;
        http.end();
        return (total / 1024.0f) / (duration / 1000.0f);
    }
    http.end();
    return 0.0f;
}

float Wizer::testUploadSpeed(const char *serverUrl)
{
    WiFiClient tempClient;
    WiFiClient &useClient = client ? *client : tempClient;
    HTTPClient http;
    http.begin(useClient, serverUrl);
    uint8_t dummyData[512];
    memset(dummyData, 'A', sizeof(dummyData));
    unsigned long start = millis();
    int code = http.POST(dummyData, sizeof(dummyData));
    unsigned long duration = millis() - start;
    http.end();
    if (code > 0)
        return (sizeof(dummyData) / 1024.0f) / (duration / 1000.0f);
    return 0.0f;
}

std::vector<NetworkInfo> Wizer::scanNetworks()
{
    std::vector<NetworkInfo> results;
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i)
    {
        NetworkInfo info;
        info.ssid = WiFi.SSID(i);
        info.rssi = WiFi.RSSI(i);
        info.channel = WiFi.channel(i);

#if defined(ESP32)
        info.encryption = WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "Open" : "Secured";
#elif defined(ESP8266)
        info.encryption = WiFi.encryptionType(i) == ENC_TYPE_NONE ? "Open" : "Secured";
#endif

        results.push_back(info);
    }
    return results;
}

int Wizer::getDisconnectCount() const { return disconnectCount; }
unsigned long Wizer::getLastDisconnectTime() const { return lastDisconnectTime; }
IPAddress Wizer::getLocalIP() const { return WiFi.localIP(); }

bool Wizer::isCaptivePortalActive(const String url) const
{
    WiFiClient tempClient;
    WiFiClient &useClient = client ? *client : tempClient;
    HTTPClient http;
    http.begin(useClient, url);
    int code = http.GET();
    http.end();
    return code != 204;
}

void Wizer::recordSignalSample()
{
    SignalSample s;
    s.rssi = lastRSSI;
    s.signalScore = signalScore;
    s.timestamp = millis();
    if (signalHistory.size() >= MAX_HISTORY_ENTRIES)
    {
        signalHistory.erase(signalHistory.begin());
    }
    signalHistory.push_back(s);
}

std::vector<SignalSample> Wizer::getSignalHistory() const
{
    return signalHistory;
}

void Wizer::clearSignalHistory()
{
    signalHistory.clear();
}

String Wizer::getSignalHistoryJson() const
{
    String json = "[";
    for (size_t i = 0; i < signalHistory.size(); ++i)
    {
        const SignalSample &s = signalHistory[i];
        json += "{\"rssi\":" + String(s.rssi) + ",\"score\":" + String(s.signalScore) + ",\"time\":" + String(s.timestamp) + "}";
        if (i < signalHistory.size() - 1)
            json += ",";
    }
    json += "]";
    return json;
}

String Wizer::toJson() const
{
    String json = "{";
    json += "\"latency\":" + String(latency) + ",";
    json += "\"jitter\":" + String(jitter, 2) + ",";
    json += "\"packetLoss\":" + String(packetLoss) + ",";
    json += "\"signalScore\":" + String(signalScore) + ",";
    json += "\"rssi\":" + String(lastRSSI) + ",";
    json += "\"httpStatus\":" + String(lastHttpCode) + ",";
    json += "\"httpResponseTime\":" + String(lastHttpResponseTime);
    json += "}";
    return json;
}

String Wizer::connectionToJson(const ConnectionInfo &info) const
{
    String json = "{";
    json += "\"url\":\"" + info.url + "\",";
    json += "\"success\":" + String(info.success ? "true" : "false") + ",";
    json += "\"statusCode\":" + String(info.statusCode) + ",";
    json += "\"responseTime\":" + String(info.responseTime) + ",";
    json += "\"redirected\":" + String(info.redirected ? "true" : "false") + ",";
    json += "\"redirectLocation\":\"" + info.redirectLocation + "\",";
    json += "\"errorMessage\":\"" + info.errorMessage + "\"";
    json += "}";
    return json;
}