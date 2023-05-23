#include <Arduino.h>
#include <Adafruit_MCP4728.h>
#include <Adafruit_ADS1X15.h>
#include <Wire.h>

Adafruit_MCP4728 mcp;
Adafruit_ADS1115 ads1115;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  if (!mcp.begin()) {
    Serial.println("Failed to find MCP4728 chip");
    while (1) {
      delay(10);
    }
  }
  if (!ads1015.begin()) {
    Serial.println("Failed to find ASD1115 chip");
    while(1)  {
      delay(10);
    }
  }


  mcp.setChannelValue(MCP4728_CHANNEL_A, 200);

}

void loop() {
  // put your main code here, to run repeatedly:
  
}