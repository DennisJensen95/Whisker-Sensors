/***************************************************************************
     Copyright (C) 2019 by DTU
     jca@elektro.dtu.dk

     Test of touch function on Teensy 3.5

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.
 *                                                                         *
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 *                                                                         *
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the
     Free Software Foundation, Inc.,
     59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#define REV_ID "$Id: main.cpp 402 2016-09-11 11:25:36Z jcan $"

#include "FastLED.h"

///////////////////////////////////////////////////////////////////////////////////////////
//
// Move a white dot along the strip of leds. This program simply shows how to configure the leds,
// and then how to turn a single pixel white and then off, moving down the line of pixels.
//

// How many leds are in the strip?
// #define NUM_LEDS 5
#define NUM_LEDS 1
#define NUM_SENSORS 2
uint8_t max_bright = 255;
// This is an array of leds. One item for each led in your strip.
CRGB leds[NUM_LEDS];
bool silentUSB = false;
int initializeTouchValue[NUM_SENSORS];

void setLED(int idx, int r, int g, int b, bool show = true);
void initializeTouch(int pins[]);



////////////////////////////////////////////////////
////////////////////////////////////////////////////

// This function sets up the ledsand tells the controller about them
void setup() 
{
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(200);
  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 500); // FastLED 2.1 Power management set at 5V, 500mA
  FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  delay(1000); 
  Serial.begin(9600);
}

////////////////////////////////////////////////////
//////////////////////main//////////////////////////

const int MAX_PIN_NUM = 34;
int noTouch[MAX_PIN_NUM] = {0};

int getTouch(unsigned int pin)
{
  char id_string[] = "Pin";
  int v = 0;
  const int FAC = 1000; // max adapt rate, 100 = 1 value in 100ms
  if (pin < MAX_PIN_NUM)
  {
    v = touchRead(pin);
    //WriteToSerial(pin, v, id_string);
    
    if (noTouch[pin] == 0)
        noTouch[pin] = v * FAC;
    else
    {
      noTouch[pin] += 1;
      // listen to low value only
      if (v * FAC < noTouch[pin])
         noTouch[pin] = v * FAC;
    }
    char id_string[] = "Offset";
    //WriteToSerial(pin, noTouch[pin], id_string);
  }
  return v - noTouch[pin] / FAC;
}

void getTouch2(unsigned int pin) 
{
  char id_string[] = "Pin";
  int v = 0;
  if (pin < MAX_PIN_NUM)
  {
    v = touchRead(pin);
    float diff = (initializeTouchValue[pin]/100 - v/100)/initializeTouchValue[pin]/100;
    if (diff > 0.005)
    {
      
    }
  }
}

void initializeTouch(int pins[])
{
  int samples = 100;
  int meanSample = 0;
  for (int j = 0; j < NUM_SENSORS; j++) {
    if (pins[j] < MAX_PIN_NUM) {
      
      for (int i = 0; i < samples; i++) {
        meanSample += touchRead(pins[j]);
      }
      meanSample = meanSample / samples;
    }
    initializeTouchValue[j] = meanSample;
  }
}

void WriteToSerial(unsigned int pin, int touchValue, char id_string[])
{
  int m = usb_serial_write_buffer_free();
  if (m > 25 && Serial) {
    char str[60];
    snprintf(str, 60, "%s%d:%d", id_string, pin, touchValue);
    Serial.println(str);
  }
}

void WriteToSerial(char message[]) 
{
  int m = usb_serial_write_buffer_free();
  if (m > 25 && Serial) 
  {
    char str[60];
    snprintf(str, 60, "%s", message);
    Serial.println(str);
  }
}

// This function runs over and over, and is where you do the magic to light
// your leds.

void loop()
{
  int index[NUM_SENSORS] = {15};
  // int index[NUM_SENSORS] = {15, 16};
  initializeTouch(index);
  const int MSL = 100;
  char s[MSL];
  setLED(1,0,30,0);
  int values[NUM_SENSORS];
  uint32_t t0, t2 = 0, t4 = 0, t5 = 0;
  uint32_t t1 = 0;
  int n = 0;
  uint32_t T = 1; // sampling frequency
  int average_samples = 400;
  int num_samples = 0;
  t1 = micros();
  t2 = micros();
  while (true)
  { 
    t0 = micros();
    if ((t0 - t1) >= T)
    {
      for (int j = 0; j < NUM_SENSORS; j++)
      {
        values[j] = getTouch(index[j]);
        /*
        if (j < NUM_LEDS)
        {
          setLED(j, values[j]*1, values[j]*1,0, false);
        }
        */    
      }
  
      //FastLED.show();
      t1 += T;
      n++;
    }
    int period = pow(10,6)*5;
    if ((t0 - t2) >= period) 
    {
      char str[60];
      int sample_time = period/pow(10,6);
      snprintf(str, 60, "Number of samples: %d over a time of %d seconds", n, sample_time);
      WriteToSerial(str);
      n = 0;
      t2 = micros();
    }
  }
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void setLED(int idx, int r, int b, int g, bool show)
{
  if (idx < NUM_LEDS and idx >= 0)
  {
    leds[idx] = (r << 16) + (g << 8) + b;
    if (show)
      FastLED.show();
    // make sure there is a pause between updates
    //delay(3);
  }
}
