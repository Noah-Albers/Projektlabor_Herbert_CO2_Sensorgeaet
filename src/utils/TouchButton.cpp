#include "TouchButton.h"
#include <Arduino.h>
#include "utils/Timer.h"
#include "Config.h"

SimpleTouchButton::SimpleTouchButton(int pin){
    this->pin=pin;
}

bool SimpleTouchButton::isTouched(){
    // Will hold the amount of times the touchread gave a positive value
  int succ=0;

  // Checks x times if the touchread gave a positive value back
  for(int i=0;i<BUTTON_CHECK_COUNT;i++)
    // Checks if the button is touched
    if(touchRead(this->pin)<BUTTON_DOWN_VALUE)
      succ++;

  // Returns if at least x% of the tested times the button was touched 
  return (float)succ/(float)BUTTON_CHECK_COUNT >= BUTTON_SUCCESS_RATE;
}

void SimpleTouchButton::setOnRelease(void (*onRelease)(long millis)){
    // Updates the event handler
    this->onRelease=onRelease;
}

void SimpleTouchButton::setOnHeldFor(void (*onHeldFor)(),long time){
    // Updates the event handler
    this->onHeldFor=onHeldFor;
    this->onHeldForTime=time;
}

void SimpleTouchButton::onTick(){
    // Gets the current state
    bool touched = this->isTouched();

    // Checks if the state changes
    if(touched != this->preState){
        // Updates the state
        this->preState=touched;

        // Checks if the button gets pressed
        if(touched){
            // Resets all states
            this->hasFiredReleaseEvent=this->hasFiredHeldForEvent=false;
            this->timer.reset();
        }else{
            // Checks if the button has been down for at lease x ms
            if(this->timer.hasReached(BUTTON_PRESS_TIME_TO_COUNT)){
                // Fires the release event
                if(this->onRelease != NULL && !this->hasFiredReleaseEvent){
                    this->hasFiredReleaseEvent=true;
                    this->onRelease(this->timer.getRunTime());
                }
            }
        }
        
    }

    // Checks if the button is down,
    // the held for event is set,
    // and the button has been down for as long as needed
    if(this->preState && !this->hasFiredHeldForEvent && this->onHeldFor != NULL && this->timer.hasReached(this->onHeldForTime)){
        this->hasFiredHeldForEvent=true;
        this->onHeldFor();
    }
}