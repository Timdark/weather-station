#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

class Weatherbit
{
private:
    // Internet
    EthernetClient client;

    byte mac_[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

    // Weatherbit
    char server_[18] = "api.weatherbit.io";

    String apiKey_;
    String lat_;
    String lon_;

    // Weather data
    long epoch_now_;
    float app_temp_now_;

    // Forecast data

    // This day
    float day_0_app_temp_max_;
    float day_0_app_temp_min_;
    String day_0_icon_;

    // This day + 1
    long day_1_epoch_;
    float day_1_app_temp_max_;
    float day_1_app_temp_min_;
    String day_1_icon_;

    // This day + 2
    long day_2_epoch_;
    float day_2_app_temp_max_;
    float day_2_app_temp_min_;
    String day_2_icon_;

    void updateWeather();
    void updateForecast();
public:
    Weatherbit() = delete;
    Weatherbit(String apiKey, String lat, String lon);
    ~Weatherbit();

    boolean init();

    void update();

    float getAppTempNow() const;
    long getEpochNow() const;

    float getDay0AppTempMax() const;
    float getDay0AppTempMin() const;
    String getDay0Icon() const;

    long getDay1Epoch() const;
    float getDay1AppTempMax() const;
    float getDay1AppTempMin() const;
    String getDay1Icon() const;

    long getDay2Epoch() const;
    float getDay2AppTempMax() const;
    float getDay2AppTempMin() const;
    String getDay2Icon() const;
};
