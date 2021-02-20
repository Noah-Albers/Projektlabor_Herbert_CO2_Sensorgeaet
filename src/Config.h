#pragma once

/*
    Lora settings
*/
// Frequency the lora board listens and sends on
const int LORA_FREQUENCY = 868E6; //Old 433E6

/*
    Sensor settings
*/
// Min and max values of the photo-transistor
// Used to calculate the percentage value
const int LIGHT_MAX = 3700;
const int LIGHT_MIN = 3400;
// Min and max values of the co2
// sensor value. If value higher than max,
// warnings should go off.
const int CO2_MAX = 1500; //ppm
const int CO2_MIN = 500;

/*
    Button settings
*/
// How offten the button should be checked at once
const int BUTTON_CHECK_COUNT = 10;
// At least x % of the checks must be right to count the button as pressed
const float BUTTON_SUCCESS_RATE = .6; //%
// The returned touchRead value must be lower than x to
// count as pressed
const int BUTTON_DOWN_VALUE = 25;
// How long the user must hold the button at least to count as a pressed
const int BUTTON_PRESS_TIME_TO_COUNT = 50; //ms

/*
    Pin settings
*/
// The pin where the touchread button is connected
const int PIN_TOUCH = 13;

// The pin where the touchread button is connected
const int PIN_NEOPIXEL = 17;

/*
    Other settings
*/
// Amount of neopixel connected to the board
const int NEOPIXEL_COUNT = 5;

// Time that the button needs to be held down to open a debug view
const int DEBUG_MODE_BUTTON_TIME = 5000; //ms

// Percentage of how bright the
// neopixel leds can be at max and min
const float LIGHT_DISPLAY_MAX = 1;  //%
const float LIGHT_DISPLAY_MIN = .05; // %