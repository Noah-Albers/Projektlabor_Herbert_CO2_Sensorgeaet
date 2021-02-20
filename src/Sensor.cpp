#include "Sensor.h"
#include <SparkFun_SCD30_Arduino_Library.h>
#include "utils/ScannerData.h"
#include "Config.h"

#define lightPin 0 // The pin that is used to get the data from the phototransistor

 Sensor::Sensor(void (*onReceive)(ScannerData* data),void (*onTimeout)()){
     this->onReceive=onReceive;
     this->onTimeout=onTimeout;

    // Creates the scd30 sensor instance
    Wire.begin();
    this->scd30=new SCD30();

    // Starts the scd30 sensor
    this->scd30->begin(Wire,true);
}


void Sensor::onTick(){
    // Checks if the timeout has not fired and has been reached
    if(!this->hasTimeOutFired && millis()-this->timeoutTimer > 12000){
        this->hasTimeOutFired=true;
        this->onTimeout();
        return;
    }

    // Checks if new data got received
    if(!this->scd30->dataAvailable()){
        return;
    }

    // Gets the data from the phototransistor
    int lightVal = analogRead(lightPin);

    // Clamps the light value
    lightVal = max(LIGHT_MIN,min(lightVal,LIGHT_MAX));

    // Calcualtes the percentage value and multiplies it by 100 to get the human-percentage and not that between 0.00 to 1.00
    float lightPerc = ((float)(lightVal-LIGHT_MIN)/(float)(LIGHT_MAX-LIGHT_MIN))*100;

    // Checks if any previous data exists
    if(this->lastData != NULL)
        delete this->lastData;

    // Updates the data
    this->lastData = new ScannerData (
        this->scd30->getCO2(),
        this->scd30->getHumidity(),
        this->scd30->getTemperature(),
        lightPerc
    );

    // Fires the receive event
    this->onReceive(this->lastData);

    // Resets the timeout time
    this->hasTimeOutFired=false;
    this->timeoutTimer=millis();
}

bool Sensor::hasTimeouted(){
    return this->hasTimeOutFired;
}

ScannerData* Sensor::getScannerData(){
    return this->lastData;
}