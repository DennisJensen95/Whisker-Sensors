/***************************************************************************
     Copyright (C) 2019 by DTU
     s155629@student.dtu.dk

     A program for a tactile whisker sensor with the Teensy 3.2

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
#define NUM_COMMANDS 8
#define NUM_LOGS 12000*2
#define COUNT_LOGS 0
#define NUM_MEAS 50
#define TRANSMIT_COUNT 50
#define NUM_DATA 450
#define NUM_SAMPLES 1

struct Settings
{
  bool states[NUM_COMMANDS] = {false, false, false, false, false, false, false, false};
  char commands[NUM_COMMANDS][MAX_STRING_SIZE] = {"ledon", "stream", "sample", "debug", "log", "sendlog", "meas", "signal"};
  
};

struct Touch
{
  bool apply_cusum = false;
  bool glr = false;
  bool object_detection;
  float touch_value;
  float real_touch;
  int counter = 0;
  float touch_detection = 0;
};

struct Cusum
{
  int index = 0;
  int num_data_present = 0;
  float mean_data = 0;
  float sd_data = 0;
  float h = 0;
  float S[2] = {0};
  float g[2] = {0};
  float sequence[NUM_DATA] = {0};
  float change_time = 0;
  float k = 0;
  float j = 0;
  float loss;
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

const int MAX_PIN_NUM = 34;
int noTouch[MAX_PIN_NUM] = {0};

double getTouch(unsigned int pin, int num_sampels, bool stream, Touch *touch_data, bool stream_signal)
{
  char id_string[] = "Pin";
  float v = 0;
  float x = 0;
  float result = 0;
  
  // Raw data 
  int FAC = 100000;

  //Average data
  //int FAC = 4000;

  // dont FAC
  //int FAC = 1;
  if (pin < MAX_PIN_NUM)
  {
    if (stream_signal || stream) 
    {
      touch_data->counter = touch_data->counter + 1;
    }

    if (num_sampels > 1)
    {
      for (int k = 0; k < num_sampels; k++)
      { 
        v += touchRead(pin);
        if (k == 0){
          touch_data->real_touch = v;
        }
      }
      num_sampels = (float)(num_sampels);
      x = v/num_sampels * FAC;
      
    } else 
    {
      x = touchRead(pin);
      touch_data->real_touch = x;
      x = x*FAC;
    }
    

    if (stream && touch_data->counter == TRANSMIT_COUNT)
    {
      WriteToSerial(pin, touch_data->real_touch, id_string);
    }

    if (noTouch[pin] == 0)
    {
      noTouch[pin] = x;
    }
    else if (x < noTouch[pin] and FAC != 1)
    {
      noTouch[pin] -= 1;
      result = filter(noTouch[pin], x, touch_data, FAC);
    }
    else if (x > noTouch[pin] and FAC != 1)
    {
      noTouch[pin] += 1; 
      result = filter(noTouch[pin], x, touch_data, FAC);
    }
    else {
      result = filter(noTouch[pin], x, touch_data, FAC);
    }
    
    if (stream && touch_data->counter == TRANSMIT_COUNT)
    {
      char id_string2[] = "Offset";
      WriteToSerial(pin, result/FAC, id_string2);  
      
      char id_string3[] = "thresh";
      WriteToSerial(pin, touch_data->touch_detection, id_string3);

      if (touch_data->glr == false) {
        touch_data->counter = 0;
      }
    }
    
    if (stream_signal && touch_data->counter == TRANSMIT_COUNT)
    {
      //WriteToSerial(touch_data->counter);
      WriteToSerial(touch_data->touch_detection);
      //touch_data->counter = 0;
    }

    
  }
  return result/FAC;
}

float e[7] = {0, 0, 0};
float u[7] = {0, 0, 0};

float filter(float y, float x, Touch *touch_data, int FAC)
{
  e[0] = x - y;
  // One pole transfer and one zero transfer function time constant = 5
  //u[0] = 0.99999988*u[1] + 0.49999997*e[0] - 0.49999997*e[1];

  // No filter
  u[0] = e[0];

  //u[0] = 0.99999878*u[1] + 6.10500238*e[0] - 6.10500238*e[1];

  // Bandpass [100 1000]
  //u[0] = 6.0013061*u[1] - 15.0065312 *u[2] + 20.01306382*u[3] - 15.01306525*u[4] + 6.00653334*u[5] - 1.00130681*u[6] + 0.00170111*e[0] - 0.01020666*e[1] + 0.02551666*e[2] - 0.03402221*e[3] + 0.02551666*e[4] - 0.01020666*e[5] + 0.00170111*e[6];
  
  // Bandpass [10 600]
  //u[0] = 6.0013061*u[1] - 15.0065312 *u[2] + 20.01306382*u[3] - 15.01306525*u[4] + 6.00653334*u[5] - 1.00130681*u[6] + 0.00050034*e[0] - 0.00300203*e[1] + 0.00750508*e[2] - 0.01000677*e[3] + 0.00750508*e[4] - 0.00300203*e[5] + 0.00050034*e[6];

  // Bandpass [1000 10000]
  //u[0] = 5.99998406*u[1] - 14.99992018 *u[2] + 19.99984013*u[3] - 14.99983991*u[4] + 5.99991985*u[5] - 0.99998395*u[6] + 0.57649536*e[0] - 3.45897228*e[1] + 8.64743086*e[2] - 11.52990787*e[3] + 8.64743086*e[4] - 3.45897228*e[5] + 0.57649536*e[6];
  
  // Bandpass [
  //u[0] = 5.99942324*u[1] - 14.99711632*u[2] + 19.99423291*u[3] - 14.99423318*u[4] + 5.99711673*u[5] - 0.99942337*u[6] + 0.13646063*e[0] - 0.81876383*e[1] + 2.04690961*e[2] - 2.72921283*e[3] + 2.04690961*e[4] - 0.81876383*e[5] + 0.13646063*e[6];
  
  // Bandpass [10 10000]
  //u[0] = 6.00135384*u[1] - 15.00676995*u[2] + 20.01354142*u[3] - 15.01354295*u[4] + 6.00677224*u[5] - 1.0013546*u[6] + 0.77204647*e[0] - 4.63227897*e[1] + 11.58069762*e[2] - 15.44093025*e[3] + 11.58069762*e[4] - 4.63227897*e[5] + 0.77204647*e[6];

  // Bandpass [10 10000]
  //u[0] = 5.99998406*u[1] - 14.99992018*u[2] + 19.99984013*u[3] - 14.99983991*u[4] + 5.99991985*u[5] - 0.99998395*u[6] + 0.57649536*e[0] - 3.45897228*e[1] + 8.64743086*e[2] - 11.52990787*e[3] + 8.64743086*e[4] - 3.45897228*e[5] + 0.57649536*e[6];

  // Bandpass [5 5000]
  //u[0] = 6.00077339*u[1] - 15.0038672 *u[2] + 20.00773485*u[3] - 15.00773532  *u[4] + 6.00386789*u[5] - 1.00077362*u[6] + 0.13625267*e[0] - 0.81751604*e[1] +  2.04379014*e[2] - 2.72505354*e[3] + 2.04379014*e[4] - 0.81751604*e[5] + 0.13625267*e[6];
  
  // Bandpass [10 1000]
  //u[0] = 6.0012987*u[1] - 15.00649422*u[2] + 20.01298986*u[3] - 15.01299126*u[4] + 6.00649634*u[5] - 1.00129941*u[6] + 0.00220143*e[0] - 0.01320857*e[1] + 0.03302143*e[2] - 0.04402858*e[3] + 0.03302143*e[4] - 0.01320857*e[5] + 0.00220143*e[6];

  // Highpass
  //u[0] = 3.00071765*u[1] - 3.00143546*u[2] + 1.00071782*u[3] + 0.99969993*e[0] - 2.99981736*e[1] +   3.0005351*e[2] - 1.00041767 *e[3];

  // Highpass
  //u[0] = 1.99987291*u[1] - 0.99987293*u[2] + 0.17264772*e[0] - 0.34537805*e[1] + 0.17273034*e[2];

  // Highpass 9000
  //u[0] = 1.99970023*u[1] - 0.99970026*u[2] + 0.05277946*e[0] - 0.10558417*e[1] + 0.05280472*e[2];

  // Highpass 6000
  //u[0] = 1.9999548*u[1] - 0.99995481*u[2] + 0.24725357*e[0] - 0.49438882*e[1] + 0.24713527*e[2];

  // Highpass 9500
  //u[0] = 1.99965431*u[1] - 0.99965434*u[2] + 0.03088727*e[0] - 0.06178932*e[1] + 0.03090205*e[2];

  // Highpass 9500
  //u[0] = 0.6633*u[1] + 0.1684*e[0] - 0.1684*e[1];

  // Lowpass
  //u[0] = 2.00038185*u[1] - 1.00038189*u[2] + 0.0172074*e[0] -0.03440657*e[2] + 0.01719917*e[3];

  // Reject [0.1 100]
  //u[0] = 4.00094729*u[1] - 6.0028422*u[2] + 4.00284254*u[3] - 1.00094763*u[4] + 0.97919522*e[0] - 3.91771802*e[1] + 5.87798308*e[2] - 3.91959298*e[3] + 0.9801327*e[4];

  // Reject [0.1 1000]
  //u[0] = 4.00086038*u[1] - 6.00258142*u[2] + 4.00258169*u[3] - 1.00086066*u[4] + 0.81516058*e[0] - 3.26142251*e[1] + 4.89330434*e[2] - 3.26298347*e[3] + 0.81594106*e[4];

  // Bandpass [0.1 1000]
  //u[0] = 4.00086038*u[1] - 6.00258142*u[1] + 4.00258169*u[3] - 1.00086066*u[4] + 0.0172074*e[0] - 0.0688296*e[1] + 0.1032444*e[2] - 0.0688296*e[3] + 0.0172074*e[4];
  
  if (abs(u[0]) > 2.0*FAC)
  {
    touch_data->touch_detection = 1;
    touch_data->object_detection = true;  
  } else 
  {
    touch_data->object_detection = false;
    touch_data->touch_detection = 0;
  }

  e[1] = e[0];
  u[1] = u[0];
  e[2] = e[1];
  u[2] = u[1];
  e[3] = e[2];
  u[3] = u[2];
  e[4] = e[3];
  u[4] = u[3];
  e[5] = e[4];
  u[5] = u[4];
  e[6] = e[5];
  u[6] = u[5];
  

  return u[0];
}

void MeanCusum(Cusum *change_det_data)
{
  float mean = 0;
  for (int j = 0; j < change_det_data->num_data_present; j++) {
      mean += change_det_data->sequence[j];
      //WriteToSerial(change_det_data->sequence[j], true);
      //delay(5);
    }

  change_det_data->mean_data = mean/change_det_data->num_data_present;
}

void SumLoss(Cusum *change_det_data, float z)
{
  float sum = 0;
  for (int j = 0; j < change_det_data->num_data_present; j++) {
      sum += z - change_det_data->sequence[j];
    }

  change_det_data->loss = sum;
}

void SDCusum(Cusum *change_det_data)
{
  float sd = 0;
  for (int j = 0; j < change_det_data->num_data_present; j++) {
    sd += pow(change_det_data->sequence[j] - change_det_data->mean_data, 2);
  }
  change_det_data->sd_data = sqrt(sd/change_det_data->num_data_present);
}

void GLR(Touch *touch, Cusum *change_det_data)
{
  char mes[40];
  
  float z = touch->real_touch;
  float g = 0;
  change_det_data->sequence[change_det_data->index] = z;
  if (change_det_data->index == NUM_DATA-1) {
    change_det_data->index = 0;
    change_det_data->num_data_present = NUM_DATA;
  }
  change_det_data->index++;

  if (change_det_data->change_time < NUM_DATA-1) 
  {
    change_det_data->change_time++;
  }
  
  if (change_det_data->num_data_present < change_det_data->index) {
    change_det_data->num_data_present = change_det_data->index;
  }

  if (change_det_data->change_time == NUM_DATA-1)
  {
    SDCusum(change_det_data);
    SumLoss(change_det_data, z);
    g = 1/(2*pow(change_det_data->sd_data, 2)) * 1/(change_det_data->k - change_det_data->change_time + 1)*pow(change_det_data->loss,2);
  }
  g = 1/(2*pow(change_det_data->sd_data, 2)) * 1/(change_det_data->k - change_det_data->change_time + 1)*pow(change_det_data->loss,2);

  if (change_det_data->k < NUM_DATA-1)
  {
    change_det_data->k++;
  }
  

  if (touch->counter == TRANSMIT_COUNT)
  {
    delay(10);
    snprintf(mes, 40, "cusum19:%f", g);
    WriteToSerial(mes);
    touch->counter = 0;
    delay(10);
  }

}

void CuSum(Touch *touch, Cusum *change_det_data)
{
  char mes[40];
  
  float z = touch->real_touch;
  change_det_data->sequence[change_det_data->index] = z;
  if (change_det_data->index == NUM_DATA-1) {
    change_det_data->index = 0;
    change_det_data->num_data_present = NUM_DATA;
  }
  change_det_data->index++;
  
  if (change_det_data->num_data_present < change_det_data->index) {
    change_det_data->num_data_present = change_det_data->index;
  }

  MeanCusum(change_det_data);
  SDCusum(change_det_data);
  float mu_old = change_det_data->mean_data;
  float sd = change_det_data->sd_data;
  float mu_new = z;
  float s = (mu_new - mu_old)/pow(sd, 2)*(z-(mu_new + mu_old)/2);

  change_det_data->S[0] = change_det_data->S[1] + s;
  
  change_det_data->g[0] = fmax(0, change_det_data->g[1] + s);
  
 
  

  if (touch->counter == TRANSMIT_COUNT)
  {
    delay(10);
    snprintf(mes, 40, "cusum19:%f", change_det_data->g[0]);
    WriteToSerial(mes);
    touch->counter = 0;
  }
}

void determineTouch(unsigned int pin, int num_sampels, bool data_stream, Touch *touch_data, bool stream_signal, Cusum *change_det_data)
{
  touch_data->touch_value = getTouch(pin, num_sampels, data_stream, touch_data, stream_signal);
  if (touch_data->apply_cusum == true)
  {
    CuSum(touch_data, change_det_data);
  }

  if (touch_data->glr == true)
  {
    GLR(touch_data, change_det_data);
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

void WriteToSerial(float value)
{
  int m = usb_serial_write_buffer_free();
  if (m > 25 && Serial) {
    Serial.println(value);
  }
}

void WriteToSerial(float value, bool arr)
{
  int m = usb_serial_write_buffer_free();
  if (m > 25 && Serial) {
    char str[30];
    snprintf(str, 30, "%f,", value);
    Serial.print(str);
  }
}

void WriteToSerial(int value)
{
  int m = usb_serial_write_buffer_free();
  if (m > 25 && Serial) {
    Serial.println(value);
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
  Cusum change_det_data;
  bool logged_saved = false;
  //int *data_log = (int*)malloc(NUM_LOGS * sizeof(int));
  uint16_t data_log[NUM_LOGS];
  int index[NUM_SENSORS] = {19};
  char message[100];
  char buffer_recieved[100];
  uint32_t t0;
  uint32_t t1;
  uint32_t t2;
  int t1_log;
  float time_test;
  int period_log = pow(10,6)*7.5;
  int num_saved_logs = 0;
  int meas[NUM_MEAS];
  double measurement_val = 0;
  int l = 0;
  int n = 0;
  int i = 0;
  int num_sampels = NUM_SAMPLES;
  int num_bytes_received = 0;
  unsigned int period = pow(10,6)*2;
  t0 = micros();
  int sample_time = 0;
  while (true)
  {

    //sprintf(message, 100, "%d", NSCAN);
    //writeToSerial(message);
    determineTouch(index[0], num_sampels, data.states[1], &touch_data, data.states[7], &change_det_data); 
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

    // 
    if (data.states[6])
    {
      meas[l] = touch_data.real_touch;

      if (l == NUM_MEAS - 1)
      {
        for (l = 0; l < NUM_MEAS; l++)
        {
          measurement_val += meas[l];
        }
        measurement_val = measurement_val/NUM_MEAS;

        l = 0;
        snprintf(message, 100, "Measurement average:%f", measurement_val);
        WriteToSerial(message);
      }
      else 
      {
        l++;
      }
      
    }

    // Counting samples taking over a sample time
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


    // Offline logging
    if (data.states[4])
    {
      if (i == 0) 
      {
        
        snprintf(message, 100, "Started logging");
        WriteToSerial(message);
        t1_log = micros();
        t2 = micros();
      } 
      
      time_test = (micros() - t1_log);
      if (i == NUM_LOGS - 1 || time_test > period_log)
      {
        sample_time = (micros() - t2)/pow(10,6);
        //snprintf(message, 100, "Ended log, log time: %d seconds", sample_time);
        time_test = time_test/pow(10,6);
        snprintf(message, 100, "Time: %.2f, period: %.2f, saved: %d", time_test, period_log/pow(10,6), i);
        WriteToSerial(message);
        data.states[4] = false;
        logged_saved = true;
        num_saved_logs = i;
        

        digitalWrite(13, LOW);
        delay(200);
        digitalWrite(13, HIGH);
        
      }
      data_log[i] = (uint16_t)touch_data.real_touch;
      i++;
    } else 
    {
      i = 0;
    }
    
    if (data.states[5] && logged_saved)
    {
      for (i = 0; i < num_saved_logs; i++) {
        snprintf(message, 100, "log:%d", data_log[i]);
        WriteToSerial(message);
        delay(5);
      }
      i = 0;
      data.states[5] = false;
      snprintf(message, 100, "Sample time: %.2f\nLogged send done", time_test);
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
      for (int k = 0; k < num_states; k++)
      {
        snprintf(message, 100, " - %s", data.commands[k]);
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
