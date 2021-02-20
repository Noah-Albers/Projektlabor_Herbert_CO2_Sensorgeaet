#include <Arduino.h>
#include "Sensor.h"
#include "utils/ScannerData.h"
#include "networking/Netutils.h"
#include <LoRa.h>
#include "Debugging.h"
#include <SPI.h>
#include "Config.h"
#include <Adafruit_NeoPixel.h>
#include "utils/Utils.h"
#include <U8x8lib.h>
#include "utils/Timer.h"
#include "utils/TouchButton.h"
#include "UserConfig.h"

// The display
U8X8_SSD1306_128X64_NONAME_SW_I2C display(15, 4, 16);

// The sensor for co2, temperature and humidity
Sensor* sensor;

// The neopixel instance
Adafruit_NeoPixel lightDisplay(NEOPIXEL_COUNT,PIN_NEOPIXEL,NEO_GRB + NEO_KHZ800);

// The touch button
SimpleTouchButton touchButton(PIN_TOUCH);

// How bright the light display should be
float lightBrightness=LIGHT_DISPLAY_MIN;
// The last read co2 value
int lastCo2=0;

/**
 * Updates the light display with the co2 value that is given
 */
void setLightDisplay(int co2){
  // Clamps the co2 value between the min and max
  if(co2>CO2_MAX)
    co2=CO2_MAX;
  if(co2<CO2_MIN)
    co2=CO2_MIN;

  // Gets the percentual value
  float perc = (float)(co2-CO2_MIN)/(float)(CO2_MAX-CO2_MIN);

  // Gets the color on a hsv scale (0-120 red to green)
  int rgb[3];
  convertHSVtoRGB((1-perc)*120,100,100*lightBrightness,rgb);

  // Updates the neopixel display
  lightDisplay.fill(lightDisplay.Color(rgb[0],rgb[1],rgb[2]));
  lightDisplay.show();
}

/**
 * Displays the light sensor timeout
 */
void setLightDisplayTimeouted(){
  // Get the rgb color with the brightness for a white (timeout) color
  int rgb[3];
  convertHSVtoRGB(0,0,100*lightBrightness,rgb);

  // Updates the neopixel display
  lightDisplay.fill(lightDisplay.Color(rgb[0],rgb[1],rgb[2]));
  lightDisplay.show();
}

void onRecieveLoRaPacket(byte* packet,int pktLen){
  // Gets the id
  byte id = packet[32];

  // Gets the sender
  int sender = netutils::getFromPacket(packet,32+1+4).intValue;

  // Checks if the packet is not a request data packet
  if(id!=netutils::PACKET_REQUEST_DATA)
    return;

  // Gets the data from the sensor
  ScannerData* sd = sensor->getScannerData();

  // Checks if the sensor could not provide data
  // or has timed out
  if(sd == NULL || sensor->hasTimeouted())
    return;

  // Creates the packet
  byte pktRet[4*4];
  // Inserts the sensor-data
  netutils::appendInt(pktRet,0,sd->co2);
  netutils::appendFloat(pktRet,4,sd->humidity);
  netutils::appendFloat(pktRet,8,sd->temperature);
  netutils::appendFloat(pktRet,12,sd->light);

  debug::info("Received a request from "+String(sender,HEX)+" and sending the following data: CO2 "+String(sd->co2)+"ppm / Humidity "+String(sd->humidity)+"% Temperature "+String(sd->temperature)+"° Light: "+String(sd->light));
  
  // Sends back the packet the response packet
  netutils::sendPacket(pktRet,netutils::PACKET_SCANNER_DATA,sender);
  
  debug::info("Send back response data.");
}

void onTouchButtonHeldFor(){
  Serial.println("Fired held for");
}

void onTouchButtonRelease(long millis){
  // Checks if the other event has already fired
  if(millis>=DEBUG_MODE_BUTTON_TIME)
    return;

  // Disables the annoying leds
  lightBrightness=LIGHT_DISPLAY_MIN;

  // Checks if the sensor has timeouted
  if(sensor->hasTimeouted()){
    // Updates the lightdisplay
    setLightDisplayTimeouted();
  }else{
    // Gets the last scanned data
    ScannerData* sd = sensor->getScannerData();

    // Checks if that is set
    if(sd != NULL)
      // Update the lightdisplay
      setLightDisplay(sd->co2);
  }
}

void onSensorTimeout(){
  // Updates the display
  display.clearDisplay();
  display.setCursor(1,1);
  display.print("Sensor Timeout.");
  display.setCursor(1,3);
  display.print("Id: "+String(DEVICE_ID));

  // Displays the timeout on the light display
  lightBrightness=1;
  setLightDisplayTimeouted();
}

void onSensorReceive(ScannerData* sd){
  // Resets the display
  display.clearDisplay();
  display.setCursor(1,1);

  // Updates the display
  display.print("CO2: "+String(sd->co2)+"ppm");
  display.setCursor(1,2);
  display.print("Humi: "+String(sd->humidity)+"%");
  display.setCursor(1,3);
  display.print("Temp: "+String(sd->temperature)+"°");
  display.setCursor(1,4);
  display.print("Light: "+String(sd->light)+"%");
  display.setCursor(1,6);
  display.print("ID: "+String(DEVICE_ID));

  // Checks if the co2 value has changed into the bad value
  if(sd->co2 != lastCo2){
    if(lastCo2 < CO2_MAX && sd->co2 > CO2_MAX)
      // Updates the brightness to the brightest
      lightBrightness=LIGHT_DISPLAY_MAX;

    // Updates the last scanned value
    lastCo2=sd->co2;
  }

  // Updates the light display
  setLightDisplay(sd->co2);
}

void onFailedStartup(){
  // Goes into an infinity loop (Waits until the user restarts the device)
  while(1);
}

void setup() {
  // Starts the serial connection
  Serial.begin(115200);

  debug::info("I am an end-device. My id is: "+String(DEVICE_ID));

  // Setups the display
  SPI.begin(5, 19, 27, 18);
  // Setups lora
  LoRa.setPins(18, 14, 26);

  // Creates the neopixel
  lightDisplay.begin();
  lightDisplay.show();
  
  // Sets the touchbutton handlers
  touchButton.setOnRelease(&onTouchButtonRelease);
  touchButton.setOnHeldFor(&onTouchButtonHeldFor,DEBUG_MODE_BUTTON_TIME);

  // Starts the display
  display.begin();
  display.setFont(u8x8_font_chroma48medium8_r);
  display.clearDisplay();

  // Checks that lora can start
  if(!LoRa.begin(LORA_FREQUENCY)){
    // Displays the error to the user
    display.setCursor(1,1);
    display.print("Lora star failed.");
    display.setCursor(1,2);
    display.print("Please restart.");

    debug::error("Failed to start lora. Please try again.");
    onFailedStartup();
  }

  // Creates the sensor
  sensor=new Sensor(&onSensorReceive,&onSensorTimeout);

  debug::info("Started all stuff");
}

void loop() {
  // Updates the touch button
  touchButton.onTick();

  // Updates the sensor
  sensor->onTick();

  // Gets the next packet len
  int pktLen = LoRa.parsePacket();

  // Checks if a packet got received
  if(pktLen>0){
    debug::debug("Received a packet");

    // The packet
    byte packet[pktLen];

    // Processes the packet
    if(netutils::getNextPacket(packet,pktLen)){
      onRecieveLoRaPacket(packet,pktLen);
      return;
    }
  }
}