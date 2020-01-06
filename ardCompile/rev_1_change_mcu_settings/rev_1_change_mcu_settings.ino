/***************************************************************************
     Copyright (C) 2019 by DTU
     s155629@student.dtu.dk

     Test of touch function on Teensy 3.2

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

#define NUM_LEDS 1
#define NUM_SENSORS 2
#define MAX_STRING_SIZE 40
#define NUM_COMMANDS 6
#define NUM_LOGS 2500
#define FILTER_GAIN 1

struct Settings
{
  bool states[NUM_COMMANDS] = {false, false, false, false, false, false};
  char commands[NUM_COMMANDS][MAX_STRING_SIZE] = {"ledon", "stream", "sample", "debug", "log", "sendlog"};
  
};

struct Touch
{
  bool object_detection;
  double touch_value;
};

  
////////////////////////////////////////////////////
////////////////////////////////////////////////////

// This function sets up the ledsand tells the controller about them
void setup() 
{
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(1000); 
  Serial.begin(9600);

  char test[10];
  snprintf(test, 10, "%d", TSI_SCANC_EXTCHRG(0));
  WriteToSerial(test);
  int h = TSI_SCANC_EXTCHRG(19);
  
}

const int MAX_PIN_NUM = 34;
int noTouch[MAX_PIN_NUM] = {0};

double getTouch(unsigned int pin, int num_samples, bool stream, Touch *touch_data)
{
  char id_string[] = "Pin";
  double v = 0;
  double avg = 0;
  const int FAC = 1000; // max adapt rate, 100 = 1 value in 100ms
  if (pin < MAX_PIN_NUM)
  {
    for (int k = 0; k < num_samples; k++)
    {
      v += touchRead(pin);
    }
    num_samples = (double)(num_samples);
    avg = v/num_samples;

    if (stream)
    {
      WriteToSerial(pin, avg, id_string);
    }
    
    if (noTouch[pin] == 0)
        noTouch[pin] = avg * FAC;
    else
    {
      noTouch[pin] += 1;
      // listen to low value only
      if (avg * FAC < noTouch[pin]) 
      {
        noTouch[pin] = avg * FAC;
      }
    }
    if (stream)
    {
      char id_string2[] = "Offset";
      WriteToSerial(pin, avg - noTouch[pin]/FAC, id_string2);  
    }
  }
  return avg - noTouch[pin] / FAC;
}

void determineTouch(unsigned int pin, int num_samples, bool data_stream, Touch *touch_data)
{
  touch_data->touch_value = getTouch(pin, num_samples, data_stream, touch_data);
  
  if (touch_data->touch_value > 0.8 or touch_data->touch_value < 0)
  {
    touch_data->object_detection =  true;
  } else
  {
    touch_data->object_detection = false;
  }
}

void WriteToSerial(unsigned int pin, double touchValue, char id_string[])
{
  int m = usb_serial_write_buffer_free();
  if (m > 25 && Serial) {
    char str[60];
    snprintf(str, 60, "%s%d:%f", id_string, pin, touchValue);
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

////////////////////////////////////////////////////
//////////////////////main//////////////////////////

void loop()
{
  // Command set
  Settings data;
  Touch touch_data;
  bool logged_saved = false;
  int data_log[NUM_LOGS];
  int index[NUM_SENSORS] = {19};
  char message[100];
  char buffer_recieved[100];
  uint32_t t0;
  uint32_t t1;
  uint32_t t2;
  int n = 0;
  int i = 0;
  int num_samples = 150;
  int num_bytes_received = 0;
  unsigned int period = pow(10,6)*5;
  t0 = micros();
  int sample_time = 0;
  while (true)
  {
    determineTouch(index[0], num_samples, data.states[1], &touch_data); 
    n++;
    t1 = micros();
    
    if (touch_data.object_detection && !data.states[3]) {
      if (!data.states[0]) 
      {
        data.states[0] = true;
      }
    } else if (!data.states[3]) 
    {
      if (data.states[0]) {
        data.states[0] = false;
      }  
    }

    switch_led(&data.states[0]);
    
    if ((t1 - t0) >= period && data.states[2]) 
    {
      sample_time = period/pow(10,6);
      snprintf(message, 100, "Number of samples: %d over a time of %d seconds", n, sample_time);
      WriteToSerial(message);
      n = 0;
      t0 = micros();
    } else if (!data.states[2]) {
      t0 = micros();
      n = 0;
    }

    if (data.states[4])
    {
      if (i == 0) 
      {
        snprintf(message, 100, "Started logging");
        WriteToSerial(message);
        t2 = micros();
      } 
      else if (i == NUM_LOGS - 1)
      {
        sample_time = (micros() - t2)/pow(10,6);
        snprintf(message, 100, "Log time: %d seconds", sample_time);
        WriteToSerial(message);
        data.states[4] = false;
        logged_saved = true;

        digitalWrite(13, HIGH);
        delay(200);
        digitalWrite(13, LOW);
        
      }
      data_log[i] = touch_data.touch_value;
      i++;
    } else 
    {
      i = 0;
    }

    if (data.states[5] && logged_saved)
    {
      for (i = 0; i < NUM_LOGS; i++) {
        snprintf(message, 100, "log:%d", data_log[i]);
        WriteToSerial(message);
        delay(5);
      }
      i = 0;
      data.states[5] = false;
      snprintf(message, 100, "Sample time: %d\nLogged send done", sample_time);
      WriteToSerial(message);
    } else if (data.states[5] && !logged_saved)
    {
      snprintf(message, 100, "No log saved");
      WriteToSerial(message);
      data.states[5] = false;
    }
    
    num_bytes_received = usb_serial_available();

    if (num_bytes_received == 1) {
      char welcome[30] = "Command Interface:";
      snprintf(message, 100, "Command Interface:");
      WriteToSerial(welcome);
      int num_states = sizeof(data.states)/sizeof(data.states[0]);
      for (int n = 0; n < num_states; n++)
      {
        snprintf(message, 100, " - %s", data.commands[n]);
        WriteToSerial(message);
        delay(10);
      }
    }
    
    if (num_bytes_received > 0) 
    { 
      
      for (int j = 0; j < num_bytes_received; j++)
      {
        buffer_recieved[j] = Serial.read();
        buffer_recieved[j + 1] = '\0';
      }
      
      //WriteToSerial(buffer_recieved);
      if (num_bytes_received > 2) {
        commandLibrary(num_bytes_received, buffer_recieved, &data);
        if (data.states[3]) {
           snprintf(message, 100, "ledon:%d, stream:%d, sample:%d debug:%d, log:%d, send_log:%d", data.states[0], data.states[1], data.states[2], data.states[3], data.states[4], data.states[5]);
           WriteToSerial(message);
        }
       
      }
    }
  }
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

void commandLibrary(int num_bytes_received, char string_check[], Settings *data) 
{ 
  for (int n = 0; n < NUM_COMMANDS; n++)
  { 
    if (strncmp(string_check, data->commands[n], num_bytes_received - 1) == 0)
    {
      switch_state(&data->states[n]);
    }
    
  }
}

void switch_led(bool *led_on) {
  if (*led_on) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
}

void switch_state(bool *state) {
  if (*state) {
    *state = false;
  } else {
    *state = true;
  }
}

char strContains(char *str, char *sfind)
{
    unsigned int found = 0;
    unsigned int index = 0;
    char len;
    len = strlen(str);
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }
    return 0;
}
