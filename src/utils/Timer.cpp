#include <Arduino.h>
#include "Timer.h"

Timer::Timer(){
    this->reset();
}

void Timer::reset(){
    this->since=millis();
}

bool Timer::hasReached(long time){
    return millis()-this->since >= time;
}

long Timer::getRunTime(){
    return millis()-this->since;
}