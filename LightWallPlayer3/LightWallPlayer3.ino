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

#define ARDUINO_ID 1

const int lights_per_row = 24;
const int lights_per_string = 24;

byte default_display[] =
"\xf3\x03\xf3\x03\xf3\x03\xf3\x03\xf5\x05\xfd\x0d\xf5\x05\xf5\x05\xfe\x0e\xfe\x0e\xfe\x0e\xfe\x0e"
"\x33\x03\x33\x03\x33\x03\x33\x03\x55\x05\xdd\x0d\x55\x05\x55\x05\xee\x0e\xee\x0e\xee\x0e\xee\x0e"
"\xf3\x0f\xf3\x0f\xf3\x0f\xf3\x0f\xf5\x0f\xfd\x0f\xf5\x0f\xf5\x0f\xfe\x0f\xfe\x0f\xfe\x0f\xfe\x0f"
"\x33\x0f\x33\x0f\x33\x0f\x33\x0f\x55\x0f\xdd\x0f\x55\x0f\x55\x0f\xee\x0f\xee\x0f\xee\x0f\xee\x0f"
"\xf3\x03\xf3\x03\xf3\x03\xf3\x03\xf5\x05\xfd\x0d\xf5\x05\xf5\x05\xfe\x0e\xfe\x0e\xfe\x0e\xfe\x0e"
"\x33\x03\x33\x03\x33\x03\x33\x03\x55\x05\xdd\x0d\x55\x05\x55\x05\xee\x0e\xee\x0e\xee\x0e\xee\x0e"
"\xf3\x0f\xf3\x0f\xf3\x0f\xf3\x0f\xf5\x0f\xfd\x0f\xf5\x0f\xf5\x0f\xfe\x0f\xfe\x0f\xfe\x0f\xfe\x0f"
"\x33\x0f\x33\x0f\x33\x0f\x33\x0f\x55\x0f\xdd\x0f\x55\x0f\x55\x0f\xee\x0f\xee\x0f\xee\x0f\xee\x0f"
"\xf3\x03\xf3\x03\xf3\x03\xf3\x03\xf5\x05\xfd\x0d\xf5\x05\xf5\x05\xfe\x0e\xfe\x0e\xfe\x0e\xfe\x0e"
"\x33\x03\x33\x03\x33\x03\x33\x03\x55\x05\xdd\x0d\x55\x05\x55\x05\xee\x0e\xee\x0e\xee\x0e\xee\x0e"
"\xf3\x0f\xf3\x0f\xf3\x0f\xf3\x0f\xf5\x0f\xfd\x0f\xf5\x0f\xf5\x0f\xfe\x0f\xfe\x0f\xfe\x0f\xfe\x0f"
"\x33\x0f\x33\x0f\x33\x0f\x33\x0f\x55\x0f\xdd\x0f\x55\x0f\x55\x0f\xee\x0f\xee\x0f\xee\x0f\xee\x0f"
"\xf3\x03\xf3\x03\xf3\x03\xf3\x03\xf5\x05\xfd\x0d\xf5\x05\xf5\x05\xfe\x0e\xfe\x0e\xfe\x0e\xfe\x0e"
"\x33\x03\x33\x03\x33\x03\x33\x03\x55\x05\xdd\x0d\x55\x05\x55\x05\xee\x0e\xee\x0e\xee\x0e\xee\x0e"
"\xf3\x0f\xf3\x0f\xf3\x0f\xf3\x0f\xf5\x0f\xfd\x0f\xf5\x0f\xf5\x0f\xfe\x0f\xfe\x0f\xfe\x0f\xfe\x0f"
"\x33\x0f\x33\x0f\x33\x0f\x33\x0f\x55\x0f\xdd\x0f\x55\x0f\x55\x0f\xee\x0f\xee\x0f\xee\x0f\xee\x0f"
"\xf3\x03\xf3\x03\xf3\x03\xf3\x03\xf5\x05\xfd\x0d\xf5\x05\xf5\x05\xfe\x0e\xfe\x0e\xfe\x0e\xfe\x0e"
"\x33\x03\x33\x03\x33\x03\x33\x03\x55\x05\xdd\x0d\x55\x05\x55\x05\xee\x0e\xee\x0e\xee\x0e\xee\x0e"
"\xf3\x0f\xf3\x0f\xf3\x0f\xf3\x0f\xf5\x0f\xfd\x0f\xf5\x0f\xf5\x0f\xfe\x0f\xfe\x0f\xfe\x0f\xfe\x0f"
"\x33\x0f\x33\x0f\x33\x0f\x33\x0f\x55\x0f\xdd\x0f\x55\x0f\x55\x0f\xee\x0f\xee\x0f\xee\x0f\xee\x0f"
"\xf3\x03\xf3\x03\xf3\x03\xf3\x03\xf5\x05\xfd\x0d\xf5\x05\xf5\x05\xfe\x0e\xfe\x0e\xfe\x0e\xfe\x0e"
"\x33\x03\x33\x03\x33\x03\x33\x03\x55\x05\xdd\x0d\x55\x05\x55\x05\xee\x0e\xee\x0e\xee\x0e\xee\x0e"
"\xff\x0f\x00\x00\xff\x0f\xff\x0f\xff\x0f\x00\x00\x00\x00\x00\x00\xff\x0f\x00\x00\x00\x00\x00\x00"
"\xff\x0f\x00\x00\xff\x0f\xff\x0f\xff\x0f\x00\x00\x00\x00\x00\x00\xff\x0f\x00\x00\x00\x00\x00\x00"
;

void setup() 
{
  Serial.begin(500000);
  LW.begin(lights_per_string, lights_per_row);
  Serial.println(ARDUINO_ID);
}

void loop()
{
  {
    byte assert_default_display[sizeof(default_display)/sizeof(default_display[0]) == lights_per_string*24];
  }
  const bool test_pin_timings=false;
  if (test_pin_timings) {
    // Code for measuring the timing of the output loop.
    // We measured this with an oscilliscope to be 10us.
    const bool should_test_var = false;
    if (should_test_var) {
      while (1) {
        volatile byte zero = 0xFF;
        OUT_VAR(zero,zero);
        volatile byte one = 0x00;
        OUT_VAR(one,one);
      }
    } else {
      while (1) {
        OUT_ONE;
        OUT_ZERO;
      }
    }
  } 


  while (1) {
    for (int ii=0; ii<lights_per_string; ii++) {
      for (int jj=0; jj<24; jj++) {
        LW.Buffer[jj]=default_display[ii*24+jj];
      }
      LW.send_frame(ii);
      _delay_us(48);
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
        for (int jj=0; jj<24; jj++) {
          while (Serial.available() < 1) {}
          default_display[ii*24+jj]=Serial.read();
        }
      }
    }
  }
}

