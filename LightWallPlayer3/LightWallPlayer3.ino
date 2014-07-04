/*
  Lightwall Player v3.0
  This code controls a series of GE Color-effects lights.
  It uses Digital pins 0-7 for output.
  It also makes use of the SD Card shield v2.1 from SeeedStudios (http://www.seeedstudio.com/depot/sd-card-shield-for-arduino-v21-p-492.html)

  Written by Chris Cooper (CCooper@QCCoLab.com, @CC_DKP)
  Derived from the reference implementation by Mark Kruse
  Based on the research by Robert "Darco" Quattlebaum (http://www.deepdarc.com/2010/11/27/hacking-christmas-lights/)
  
  Designed for the QC Co-Lab Hackerspace
  Http://www.qccolab.com

  Copyright 2011 Chris Cooper

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

*/
#include "lightwall.h"
#include "delay_x.h" // by Hans-Juergen Heinrichs

/*************************************************
 * Include message type modules here.            *
 * Don't forget to add them to the switch below. *
 *************************************************/
//#include "messagetype3_0.h"

LightWall LW;

void demo();

#define ARDUINO_ID 0

const int lights_per_row = 24;
const int lights_per_string = 24;

byte default_display[BYTES_PER_FRAME*lights_per_string];

void setup() 
{
  Serial.begin(500000);
  LW.begin(lights_per_string, lights_per_row);
  Serial.println(ARDUINO_ID);
}

void loop()
{
  {
    byte assert_default_display[sizeof(default_display)/sizeof(default_display[0]) == lights_per_string*BYTES_PER_FRAME];
  }
  const bool test_pin_timings=false;
  if (test_pin_timings) {
    // Code for measuring the timing of the output loop.
    // We measured this with an oscilliscope to be 10us.
    const bool should_test_var = false;
    if (should_test_var) {
      volatile byte zero[BYTES_PER_OUTPUT];
      volatile byte one[BYTES_PER_OUTPUT];
      for (int ii=0; ii<BYTES_PER_OUTPUT; ii++) {
        zero[ii] = 0xFF;
        one[ii] = 0x00;
      }
      while (1) {
        OUT_VAR(zero);
        OUT_VAR(one);
      }
    } else {
      while (1) {
        OUT_ONE;
        OUT_ZERO;
      }
    }
  } 

  const bool test_colors=true;

  byte blue[BYTES_PER_FRAME];
  byte green[BYTES_PER_FRAME];
  byte red[BYTES_PER_FRAME];

  byte* currentColor=blue;
  const int colorCounterMax=10;
  int colorCounter=0;

  if (test_colors) {      
    int icolor=0;
    for (; icolor<4; icolor++) {
      for (int ioutput=0; ioutput<BYTES_PER_OUTPUT; ioutput++) {
        blue [icolor*BYTES_PER_OUTPUT+ioutput] = 0x00;
        green[icolor*BYTES_PER_OUTPUT+ioutput] = 0xFF;
        red  [icolor*BYTES_PER_OUTPUT+ioutput] = 0xFF;
      }
    }
    for (; icolor<8; icolor++) {
      for (int ioutput=0; ioutput<BYTES_PER_OUTPUT; ioutput++) {
        blue [icolor*BYTES_PER_OUTPUT+ioutput] = 0xFF;
        green[icolor*BYTES_PER_OUTPUT+ioutput] = 0x00;
        red  [icolor*BYTES_PER_OUTPUT+ioutput] = 0xFF;
      }
    }
    for (; icolor<12; icolor++) {
      for (int ioutput=0; ioutput<BYTES_PER_OUTPUT; ioutput++) {
        blue [icolor*BYTES_PER_OUTPUT+ioutput] = 0xFF;
        green[icolor*BYTES_PER_OUTPUT+ioutput] = 0xFF;
        red  [icolor*BYTES_PER_OUTPUT+ioutput] = 0x00;
      }
    }
  }

  while (1) {
    if (test_colors) {
      int ilight=0;
      memcpy(LW.Buffer, blue , BYTES_PER_FRAME); LW.send_frame(ilight++); _delay_us(48);
      memcpy(LW.Buffer, green, BYTES_PER_FRAME); LW.send_frame(ilight++); _delay_us(48);
      memcpy(LW.Buffer, red  , BYTES_PER_FRAME); LW.send_frame(ilight++); _delay_us(48);

      memcpy(LW.Buffer, blue , BYTES_PER_FRAME); LW.send_frame(ilight++); _delay_us(48);
      memcpy(LW.Buffer, blue , BYTES_PER_FRAME); LW.send_frame(ilight++); _delay_us(48);
      memcpy(LW.Buffer, green, BYTES_PER_FRAME); LW.send_frame(ilight++); _delay_us(48);
      memcpy(LW.Buffer, green, BYTES_PER_FRAME); LW.send_frame(ilight++); _delay_us(48);
      memcpy(LW.Buffer, red  , BYTES_PER_FRAME); LW.send_frame(ilight++); _delay_us(48);
      memcpy(LW.Buffer, red  , BYTES_PER_FRAME); LW.send_frame(ilight++); _delay_us(48);

      for ( ; ilight<lights_per_string; ilight++) {
        for (int jj=0; jj<BYTES_PER_FRAME; jj++) {
          LW.Buffer[jj]=currentColor[jj];
        }
        LW.send_frame(ilight);
        _delay_us(48);
      }
      colorCounter++;
      if (colorCounter == colorCounterMax) {
        colorCounter=0;
        if (currentColor == blue) {
          currentColor = green;
        } else if (currentColor == green) {
          currentColor = red;
        } else {
          currentColor = blue;
        }
      }
    } else {
      for (int ii=0; ii<lights_per_string; ii++) {
        for (int jj=0; jj<BYTES_PER_FRAME; jj++) {
          LW.Buffer[jj]=default_display[ii*BYTES_PER_FRAME+jj];
        }
        LW.send_frame(ii);
        _delay_us(48);
      }
    }

    const bool do_read = false;
    if (do_read) {
      Serial.println("OK");
      //search for an "OK"
      bool found_ok = false;
      while (!found_ok) {
        while (Serial.available() < 2) {}
        byte O = Serial.read();
        if (O == 'O') {
          byte K = Serial.read();
          if (K == 'K') {
            found_ok = true;
          }
        }
      }
      for (int ii=0; ii<lights_per_string; ii++) {
        for (int jj=0; jj<BYTES_PER_FRAME; jj++) {
          while (Serial.available() < 1) {}
          default_display[ii*BYTES_PER_FRAME+jj]=Serial.read();
        }
      }
    }
  }
}

