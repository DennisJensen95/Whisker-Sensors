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
#include "string.h"

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
bool debug = false;
bool data_stream = true;
bool measure_speed = false;
bool sample_measure = false;

void setLED(int idx, int r, int g, int b, bool show = true);

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// This function sets up the ledsand tells the controller about them
void setup() 
{
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
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

int getTouch(unsigned int pin, int num_samples)
{
  int v = 0;
  const int FAC = 10; // max adapt rate, 100 = 1 value in 100ms
  int samples[num_samples];
  if (pin < MAX_PIN_NUM)
  {
    for (int k = 0; k < num_samples; k++)
    {
      v += touchRead(pin);
    }
    v = v/num_samples;
    
    if (noTouch[pin] == 0)
        noTouch[pin] = v * FAC;
    else
    {
      noTouch[pin] += 1;
      // listen to low value only
      if (v * FAC < noTouch[pin])
         noTouch[pin] = v * FAC;
    }
  }
  return v - noTouch[pin] / FAC;
}

int getTouchVerbose(unsigned int pin, int num_samples)
{
  char id_string[] = "Pin";
  int v = 0;
  const int FAC = 10; // max adapt rate, 100 = 1 value in 100ms
  int samples[num_samples];
  if (pin < MAX_PIN_NUM)
  {
    for (int k = 0; k < num_samples; k++)
    {
      v += touchRead(pin);
    }
    v = v/num_samples;
    WriteToSerial(pin, v, id_string);
    
    if (noTouch[pin] == 0)
        noTouch[pin] = v * FAC;
    else
    {
      noTouch[pin] += 1;
      // listen to low value only
      if (v * FAC < noTouch[pin])
         noTouch[pin] = v * FAC;
    }
    
    char id_string2[] = "Offset";
    WriteToSerial(pin, v - noTouch[pin]/FAC, id_string2); 
  }
  return v - noTouch[pin] / FAC;
}


int determineTouch(unsigned int pin, int num_samples)
{
  int offset_value = 0;
  if (data_stream) {
    offset_value = getTouchVerbose(pin, num_samples);
  } else {
    offset_value = getTouch(pin, num_samples);
  }
  
  if (offset_value > 1 or offset_value < 0)
  {
    return true;
  } else
  {
    return false;
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
  int index[NUM_SENSORS] = {19};
  // int index[NUM_SENSORS] = {15, 16};
  const int MSL = 100;
  char s[MSL];
  setLED(1,0,30,0);
  int values[NUM_SENSORS];
  uint32_t t0;
  uint32_t t1 = 0;
  int n = 0;
  int num_samples = 300;
  int sample_value = 0;
  int num_bytes_received = 0;
  int period = pow(10,6)*5;
  t0 = micros();
  bool led_on = false;
  while (true)
  {
    bool object_detection = determineTouch(index[0], num_samples); 
    n++;
    t1 = micros();
    
    if (object_detection && !debug) {
      if (!led_on) {
        digitalWrite(13, HIGH);
        led_on = true;
      }
    } else {
      if (led_on) {
        digitalWrite(13, LOW);
        led_on = false;
      }  
    }
    
    if ((t1 - t0) >= period && sample_measure) 
    {
      char str[60];
      int sample_time = period/pow(10,6);
      snprintf(str, 60, "Number of samples: %d over a time of %d seconds", n, sample_time);
      WriteToSerial(str);
      n = 0;
      t0 = micros();
    }
    num_bytes_received = usb_serial_available();
    if (num_bytes_received > 0) {
      char buffer_recieved[num_bytes_received + 1];
      char usb_available[15];
      snprintf(usb_available, 15, "Available:%d", num_bytes_received);
      WriteToSerial(usb_available);
      for (int n = 0; n < num_bytes_received; n++) {
        buffer_recieved[n] = Serial.read();
      }
      
      buffer_recieved[num_bytes_received + 1] = '\n';
      WriteToSerial(buffer_recieved);
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
