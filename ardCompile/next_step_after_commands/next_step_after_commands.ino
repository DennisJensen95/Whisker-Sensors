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

///////////////////////////////////////////////////////////////////////////////////////////
//
// Move a white dot along the strip of leds. This program simply shows how to configure the leds,
// and then how to turn a single pixel white and then off, moving down the line of pixels.
//

// How many leds are in the strip?
// #define NUM_LEDS 5
#define NUM_LEDS 1
#define NUM_SENSORS 2
#define MAX_STRING_SIZE 40
#define NUM_COMMANDS 5

struct Settings
{
  bool states[NUM_COMMANDS] = {false, false, false, false, false};
  char commands[NUM_COMMANDS][MAX_STRING_SIZE] = {"ledon", "stream", "sample", "debug", "log"};
  
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
}

////////////////////////////////////////////////////
//////////////////////main//////////////////////////

const int MAX_PIN_NUM = 34;
int noTouch[MAX_PIN_NUM] = {0};

int getTouch(unsigned int pin, int num_samples, int last_value, bool stream)
{
  char id_string[] = "Pin";
  int v = 0;
  double avg = 0;
  double step_value = 0;
  const int FAC = 10; // max adapt rate, 100 = 1 value in 100ms
  if (pin < MAX_PIN_NUM)
  {
    for (int k = 0; k < num_samples; k++)
    {
      v += touchRead(pin);
    }
    num_samples = (double)(num_samples);
    avg = v/num_samples;
    avg = floor(avg);

    if (avg + 1 == last_value || avg == 1)
    {
      step_value = avg;
      avg = last_value;
      last_value = step_value;
      
    } else 
    {
      last_value = avg;
    }

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
         noTouch[pin] = avg * FAC;
    }
    if (stream)
    {
      char id_string2[] = "Offset";
      WriteToSerial(pin, avg - noTouch[pin]/FAC, id_string2);  
    }
  }
  return avg - noTouch[pin] / FAC;
}


int determineTouch(unsigned int pin, int num_samples, bool data_stream, int *last_value)
{
  int offset_value = 0;
  offset_value = getTouch(pin, num_samples, *last_value, data_stream);
  
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
  // Command set
  
  Settings data;
  char str[60];
  snprintf(str, 60, "command 0:%d", data.states[0]);
  WriteToSerial(str);

  int last_value = 0;
  int *last_value_ptr;
  last_value_ptr = &last_value;
  int index[NUM_SENSORS] = {19};
  // int index[NUM_SENSORS] = {15, 16};
  uint32_t t0;
  uint32_t t1 = 0;
  int n = 0;
  int num_samples = 300;
  int num_bytes_received = 0;
  unsigned int period = pow(10,6)*5;
  t0 = micros();
  while (true)
  {
    bool object_detection = determineTouch(index[0], num_samples, data.states[1], last_value_ptr); 
    n++;
    t1 = micros();
    
    if (object_detection && !data.states[3]) {
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

    if (data.states[4]) {
      
    }
    
    if ((t1 - t0) >= period && data.states[2]) 
    {
      char str[60];
      int sample_time = period/pow(10,6);
      snprintf(str, 60, "Number of samples: %d over a time of %d seconds", n, sample_time);
      WriteToSerial(str);
      n = 0;
      t0 = micros();
    } else if (!data.states[2]) {
      t0 = micros();
      n = 0;
    }
    
    num_bytes_received = usb_serial_available();

    if (num_bytes_received == 1) {
      char welcome[30] = "Command Interface:";
      WriteToSerial(welcome);
      int num_states = sizeof(data.states)/sizeof(data.states[0]);
      for (int n = 0; n < num_states; n++)
      {
        char help[30];
        snprintf(help, 30, " - %s", data.commands[n]);
        WriteToSerial(help);
        delay(10);
      }
    }
    
    if (num_bytes_received > 0) 
    { 
      
      char buffer_recieved[num_bytes_received + 1];
      for (int j = 0; j < num_bytes_received; j++)
      {
        buffer_recieved[j] = Serial.read();
        buffer_recieved[j + 1] = '\0';
      }
      
      char test_string[] = "test\n";
      if (strcmp(buffer_recieved, test_string) == 0)
      {
        char test_string2[] = "The string compared was true";
        WriteToSerial(test_string2); 
      }
      //WriteToSerial(buffer_recieved);
      if (num_bytes_received > 2) {
        commandLibrary(num_bytes_received, buffer_recieved, &data);

        char check_1[50];
        
        snprintf(check_1, 50, "ledon:%d, stream:%d, sample:%d debug:%d", data.states[0], data.states[1], data.states[2], data.states[3]);
        WriteToSerial(check_1);
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
