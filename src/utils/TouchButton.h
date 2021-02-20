#pragma once
#include "utils/Timer.h"

class SimpleTouchButton{
    private:
        // On which pin the button should listen
        int pin;
        // What the previous state was (When tested)
        bool preState = false;

        // Event handler for when the button gets released
        void (*onRelease)(long millis);
        // If the release event has already been fired
        bool hasFiredReleaseEvent;

        // Event handler for when the button has been held a given amount of time
       void (*onHeldFor)();
        // The time the button need to be down to fire the held for event
        long onHeldForTime;
        // If the held for event has already been fired
        bool hasFiredHeldForEvent;

        // Timer for the button held
        Timer timer;

        // Returns if the touchread button is been touched
        bool isTouched();
    public:
        SimpleTouchButton(int pin);
        void setOnRelease(void (*onRelease)(long millis));
        void setOnHeldFor(void (*onHeldFor)(),long time);
        void onTick();
};