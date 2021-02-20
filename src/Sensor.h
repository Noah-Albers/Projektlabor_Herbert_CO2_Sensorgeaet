#pragma once
#include <SparkFun_SCD30_Arduino_Library.h>
#include "utils/ScannerData.h"


class Sensor{
    private:
        // Sensor for humidity, co2 and temperature
        SCD30* scd30;

        // When the sensor last send data
        long timeoutTimer=0;

        // Event handlers
        void (*onReceive)(ScannerData* data);
        void (*onTimeout)();

        // If the timeout event has already fired
        bool hasTimeOutFired=false;

        // The last received sensor data
        ScannerData* lastData = NULL;

    public:
        Sensor(void (*onReceive)(ScannerData* data),void (*onTimeout)());
        // Updates the sensor
        void onTick();

        // If the sensor has timeouted
        bool hasTimeouted();

        // Returns the last received scanner data
        ScannerData* getScannerData();

};