/***************************************************************************
 *   Copyright (C) 2019 by DTU                             *
 *   jca@elektro.dtu.dk                                                    *
 *
 *   Test of touch function on Teensy 3.5
 *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#define REV_ID "$Id: main.cpp 402 2016-09-11 11:25:36Z jcan $"

#include "FastLED.h"

///////////////////////////////////////////////////////////////////////////////////////////
//
// Move a white dot along the strip of leds. This program simply shows how to configure the leds,
// and then how to turn a single pixel white and then off, moving down the line of pixels.
//

// How many leds are in the strip?
#define NUM_LEDS 5
uint8_t max_bright = 255;
// This is an array of leds. One item for each led in your strip.
CRGB leds[NUM_LEDS];
bool silentUSB = false;

bool send_usb(const char * str, bool blocking = false); //
void setLED(int idx, int r, int g, int b, bool show = true);

////////////////////////////////////////////////////
////////////////////////////////////////////////////

// This function sets up the ledsand tells the controller about them
void setup() {
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(200);
  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 500); // FastLED 2.1 Power management set at 5V, 500mA
  FastLED.addLeds<WS2801, RGB>(leds, NUM_LEDS);
  delay(1000);
  send_usb("setup finished\n", true);
}

////////////////////////////////////////////////////
//////////////////////main//////////////////////////

const int MAX_PIN_NUM = 34;
int noTouch[MAX_PIN_NUM] = {0};

int getTouch(unsigned int pin)
{
  int v = 0;
  const int FAC = 1000; // max adapt rate, 100 = 1 value in 100ms
  if (pin < MAX_PIN_NUM)
  {
    v = touchRead(pin);
    if (noTouch[pin] == 0)
      noTouch[pin] = v * FAC;
    else
    { // flow up to value
      noTouch[pin] += 1;
      // listen to low value only
      if (v * FAC < noTouch[pin])
        noTouch[pin] = v * FAC;
    }
  }
  return v - noTouch[pin] / FAC;
}


// This function runs over and over, and is where you do the magic to light
// your leds.
void loop()
{
  const int MSL = 100;
  char s[MSL];
//   setup();
  setLED(1,0,30,0);
  const int MVC = 12;
  int values[MVC];
//  int index[MVC] = {15, 16, 17, 18, 19};
  int index[MVC] = {15, 16, 17, 18, 19, 22, 23, 0, 1, 25, 32, 33};
  uint32_t t0, t2 = 0, t4 = 0, t5 = 0;
  uint32_t t1 = 0;
  int n = 0;
  uint32_t T = 2000; // sample time in us
  snprintf(s, MSL, "%% sample time T= %luus\n", T);
  send_usb(s, true);
  t1 = micros();
  while (true)
  {
    t0 = micros();
    if ((t0 - t1) >= T)
    {
      for (int j = 0; j < MVC; j++)
      {
        values[j] = getTouch(index[j]);
        if (j < NUM_LEDS)
        {
          setLED(j, values[j]*3, values[j]*3,0, false);
        }
      }
      FastLED.show();
      if (t0 - t2 > 100000)
      { // every 100ms
        int sec = t0 / 1000000;
        int ms = (t0 - sec*1000000)/1000;
        int dt = t4 - t5; // time spent for last send
        t5 = micros();
        snprintf(s, MSL, "%d.%03d (print=%dus) :", sec, ms, dt);
        send_usb(s, true);
        for (int j = 0; j < MVC; j++)
        {
          snprintf(s, MSL, " p%d=%d", index[j], values[j]); // , noTouch[index[j]]);
          send_usb(s, true);
        }
        snprintf(s, MSL, "\n");
        send_usb(s, true);
        // add 100ms
        t2 += 100000;
        t4 = micros();
        n = 0;
      }
      t1 += T;
      n++;
    }
    // wait for next 1s tick
  }
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////


bool send_usb(const char * str, bool blocking)
{
  int n = strlen(str);
  bool okSend = true;
  // a human client, so send all to USB and other clients
  if (not silentUSB)
  { // sending may give a timeout, and then the rest is not send!
    // this happends especially often on lenovo PCs, but they are especially slow in data transfer.
    if (not blocking)
      // surplus characters will be skipped
      usb_serial_write(str, n);
    else
    { // ensure all is send (like log data)
      int k = n, m;
      const char * p1 = str;
      uint32_t t0 = micros();
      // debug
      //       usb_serial_write("#blocking\r\n", 11);
      // debug end
      // send as much as possible at a time
      while (k > 0 /*and usbWriteFullCnt < usbWriteFullLimit*/)
      {
        m = usb_serial_write_buffer_free();
        if (m > 0)
        {
          if (m > k)
            m = k;
          usb_serial_write(p1, m);
          k = k - m;
          if (k <= 0)
            break;
          p1 += m;
        }
        if (micros() - t0 > 1500)
          // wait no more
          break;
      }
    }
  }
  return okSend;
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
