#include "Weatherbit.h"

// PRIVATE

void Weatherbit::updateWeather()
{
        // close any connection before send a new request.
    client.stop();

    // if there's a successful connection:
    if (client.connect(server_, 80)) {
        //Serial.println("connecting...");
        // send the HTTP GET request:;
        client.println("GET /v2.0/current?lat=" + lat_ + "&lon=" + lon_ + "&key=" + apiKey_ + " HTTP/1.1");
        client.println("Host: api.weatherbit.io");
        //client.println("User-Agent: Mozilla/5.0");
        client.println("Connection: close");
        client.println("Content-Type: application/json");
        client.println();

    } else {
        // if you couldn't make a connection:
        Serial.println(F("connection failed"));
        init();
    }

    delay(2000); // Waitig data

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        return;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
        Serial.println(F("Invalid response"));
        return;
    }

    // Allocate the JSON document
    // Use arduinojson.org/v6/assistant to compute the capacity.
    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(37) + 490;
    DynamicJsonDocument doc(capacity);

    // Parse JSON object
    DeserializationError error = deserializeJson(doc, client);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    app_temp_now_ = doc["data"][0]["app_temp"].as<float>();
    epoch_now_ = doc["data"][0]["ts"].as<long>();
}

void Weatherbit::updateForecast()
{
    // close any connection before send a new request.
    client.stop();

    // if there's a successful connection:
    if (client.connect(server_, 80)) {
        //Serial.println("connecting...");
        // send the HTTP GET request:;
        client.println("GET /v2.0/forecast/daily?lat=" + lat_ + "&lon=" + lon_ + "&days=3&key=" + apiKey_ + " HTTP/1.1");
        client.println("Host: api.weatherbit.io");
        //client.println("User-Agent: Mozilla/5.0");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();

    } else {
        // if you couldn't make a connection:
        Serial.println(F("connection failed"));
        init();
    }

    delay(2000); // Waitig data

    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
        Serial.print(F("Unexpected response: "));
        Serial.println(status);
        return;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
        Serial.println(F("Invalid response"));
        return;
    }

    // Allocate the JSON document
    // Use arduinojson.org/v6/assistant to compute the capacity.
    const size_t capacity = JSON_ARRAY_SIZE(3) + 3*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(7) + 3*JSON_OBJECT_SIZE(37) + 1410;
    DynamicJsonDocument doc(capacity);

    // Parse JSON object
    DeserializationError error = deserializeJson(doc, client);
    if (error) {
        Serial.print(F("deserializeJson() failed!"));
        //Serial.println(error.c_str());
        return;
    }

    day_0_app_temp_max_ = doc["data"][0]["app_max_temp"].as<float>();
    day_0_app_temp_min_ = doc["data"][0]["app_min_temp"].as<float>();
    day_0_icon_ = doc["data"][0]["weather"]["icon"].as<String>();

    day_1_epoch_ = doc["data"][1]["ts"].as<long>();
    day_1_app_temp_max_ = doc["data"][1]["app_max_temp"].as<float>();
    day_1_app_temp_min_ = doc["data"][1]["app_min_temp"].as<float>();
    day_1_icon_ = doc["data"][1]["weather"]["icon"].as<String>();

    day_2_epoch_ = doc["data"][2]["ts"].as<long>();
    day_2_app_temp_max_ = doc["data"][2]["app_max_temp"].as<float>();
    day_2_app_temp_min_ = doc["data"][2]["app_min_temp"].as<float>();
    day_2_icon_ = doc["data"][2]["weather"]["icon"].as<String>();
}

// PUBLIC

Weatherbit::Weatherbit(String apiKey, String lat, String lon): apiKey_(apiKey), lat_(lat), lon_(lon)
{
}

Weatherbit::~Weatherbit()
{
}

boolean Weatherbit::init()
{
    // Ethernet SS pin
    Ethernet.init(53);

    //Serial.println("Initialize Ethernet with DHCP:");
    if (Ethernet.begin(mac_) == 0) {
        Serial.println(F("Failed to configure Ethernet using DHCP"));
        // Check for Ethernet hardware present
        if (Ethernet.hardwareStatus() == EthernetNoHardware) {
            Serial.println(F("Ethernet shield was not found.  Sorry, can't run without hardware. :("));
            while (true) {
                delay(1); // do nothing, no point running without Ethernet hardware
            }
        }
        return false;
    } else {
        Serial.print(F("DHCP assigned IP "));
        Serial.println(Ethernet.localIP());
    }

    // give the Ethernet shield a second to initialize:
    delay(1000);

    client.setTimeout(10000);

    return true;
}

void Weatherbit::update()
{
    updateWeather();
    updateForecast();
}

long Weatherbit::getEpochNow() const
{
    return epoch_now_;
}

float Weatherbit::getAppTempNow() const
{
    return app_temp_now_;
}

float Weatherbit::getDay0AppTempMax() const
{
    return day_0_app_temp_max_;
}

float Weatherbit::getDay0AppTempMin() const
{
    return day_0_app_temp_min_;
}

String Weatherbit::getDay0Icon() const
{
    return day_0_icon_;
}

long Weatherbit::getDay1Epoch() const
{
    return day_1_epoch_;
}

float Weatherbit::getDay1AppTempMax() const
{
    return day_1_app_temp_max_;
}

float Weatherbit::getDay1AppTempMin() const
{
    return day_1_app_temp_min_;
}

String Weatherbit::getDay1Icon() const
{
    return day_1_icon_;
}

long Weatherbit::getDay2Epoch() const
{
    return day_2_epoch_;
}

float Weatherbit::getDay2AppTempMax() const
{
    return day_2_app_temp_max_;
}

float Weatherbit::getDay2AppTempMin() const
{
    return day_2_app_temp_min_;
}

String Weatherbit::getDay2Icon() const
{
    return day_2_icon_;
}