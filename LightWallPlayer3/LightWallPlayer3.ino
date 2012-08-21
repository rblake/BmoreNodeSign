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

const int lights_per_row = 8;
const int lights_per_string = 32;

byte default_display[] =
"\x44\xb8\x44\xb8\x44\xb8\x44\xb8\x22\xde\x22\xde\x22\xde\x22\xde\x11\xed\x11\xed\x11\xed\x11\xed"
"\x44\xb8\x44\xb8\x44\xb8\x44\xb8\x22\xde\x22\xde\x22\xde\x22\xde\x11\xed\x11\xed\x11\xed\x11\xed"
"\x44\xb8\x44\xb8\x44\xb8\x44\xb8\x22\xde\x22\xde\x22\xde\x22\xde\x11\xed\x11\xed\x11\xed\x11\xed"
"\x44\xb8\x44\xb8\x44\xb8\x44\xb8\x22\xde\x22\xde\x22\xde\x22\xde\x11\xed\x11\xed\x11\xed\x11\xed"
"\xff\xff\xff\xff\xff\xff\x46\x4d\xff\xff\xff\xff\xff\xff\x20\x2b\xff\xff\xff\xff\xff\xff\x13\x18"
"\xff\xff\xff\xff\xff\xff\xca\x7c\xff\xff\xff\xff\xff\xff\xac\x1a\xff\xff\xff\xff\xff\xff\x9f\x29"
"\xff\xff\xff\xff\xff\xff\xca\x7c\xff\xff\xff\xff\xff\xff\xac\x1a\xff\xff\xff\xff\xff\xff\x9f\x29"
"\xff\xff\xff\xff\xff\xff\xf6\x8e\xff\xff\xff\xff\xff\xff\x90\xe8\xff\xff\xff\xff\xff\xff\xa3\xdb"
"\x44\xb8\x44\xb8\x44\xb8\x44\xb8\x22\xde\x22\xde\x22\xde\x22\xde\x11\xed\x11\xed\x11\xed\x11\xed"
"\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xee\xee\xee\xee\xee\xee\xee\xee"
"\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xbb\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xdd\xee\xee\xee\xee\xee\xee\xee\xee"
"\x44\xb8\x44\xb8\x44\xb8\x44\xb8\x22\xde\x22\xde\x22\xde\x22\xde\x11\xed\x11\xed\x11\xed\x11\xed"
"\xff\xff\xff\xff\xff\xff\x16\x0c\xff\xff\xff\xff\xff\xff\x70\x6a\xff\xff\xff\xff\xff\xff\x43\x59"
"\xff\xff\xff\xff\xff\xff\xf3\x9a\xff\xff\xff\xff\xff\xff\x95\xfc\xff\xff\xff\xff\xff\xff\xa6\xcf"
"\xff\xff\xff\xff\xff\xff\x8f\x69\xff\xff\xff\xff\xff\xff\xe9\x0f\xff\xff\xff\xff\xff\xff\xda\x3c"
"\xff\xff\xff\xff\xff\xff\xd6\x0e\xff\xff\xff\xff\xff\xff\xb0\x68\xff\xff\xff\xff\xff\xff\x83\x5b"
"\x44\x44\x44\x44\x44\x44\x44\x44\x22\x22\x22\x22\x22\x22\x22\x22\x11\x11\x11\x11\x11\x11\x11\x11"
"\xbb\x47\xbb\x47\xbb\x47\xbb\x47\xdd\x21\xdd\x21\xdd\x21\xdd\x21\xee\x12\xee\x12\xee\x12\xee\x12"
"\xbb\x47\xbb\x47\xbb\x47\xbb\x47\xdd\x21\xdd\x21\xdd\x21\xdd\x21\xee\x12\xee\x12\xee\x12\xee\x12"
"\x44\x44\x44\x44\x44\x44\x44\x44\x22\x22\x22\x22\x22\x22\x22\x22\x11\x11\x11\x11\x11\x11\x11\x11"
"\xff\xff\xff\xff\xff\xff\x14\x04\xff\xff\xff\xff\xff\xff\x72\x62\xff\xff\xff\xff\xff\xff\x41\x51"
"\xff\xff\xff\xff\xff\xff\xf5\x83\xff\xff\xff\xff\xff\xff\x93\xe5\xff\xff\xff\xff\xff\xff\xa0\xd6"
"\xff\xff\xff\xff\xff\xff\x89\x70\xff\xff\xff\xff\xff\xff\xef\x16\xff\xff\xff\xff\xff\xff\xdc\x25"
"\xff\xff\xff\xff\xff\xff\xd4\x06\xff\xff\xff\xff\xff\xff\xb2\x60\xff\xff\xff\xff\xff\xff\x81\x53"
"\x44\xb8\x44\xb8\x44\xb8\x44\xb8\x22\xde\x22\xde\x22\xde\x22\xde\x11\xed\x11\xed\x11\xed\x11\xed"
"\x44\xb8\x44\xb8\x44\xb8\x44\xb8\x22\xde\x22\xde\x22\xde\x22\xde\x11\xed\x11\xed\x11\xed\x11\xed"
"\x44\xb8\x44\xb8\x44\xb8\x44\xb8\x22\xde\x22\xde\x22\xde\x22\xde\x11\xed\x11\xed\x11\xed\x11\xed"
"\x44\xb8\x44\xb8\x44\xb8\x44\xb8\x22\xde\x22\xde\x22\xde\x22\xde\x11\xed\x11\xed\x11\xed\x11\xed"
"\xff\xff\xff\xff\xff\xff\x06\x4c\xff\xff\xff\xff\xff\xff\x60\x2a\xff\xff\xff\xff\xff\xff\x53\x19"
"\xff\xff\xff\xff\xff\xff\x62\xdc\xff\xff\xff\xff\xff\xff\x04\xba\xff\xff\xff\xff\xff\xff\x37\x89"
"\xff\xff\xff\xff\xff\xff\x42\x5c\xff\xff\xff\xff\xff\xff\x24\x3a\xff\xff\xff\xff\xff\xff\x17\x09"
"\xff\xff\xff\xff\xff\xff\x46\x4d\xff\xff\xff\xff\xff\xff\x20\x2b\xff\xff\xff\xff\xff\xff\x13\x18"
;

void setup() 
{
  Serial.begin(115200);
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
    const bool should_test_const = false;
    if (should_test_const) {
      while (1) {
        volatile byte zero = 0xFF;
        OUT_VAR(zero,zero);
        volatile byte one = 0x00;
        OUT_VAR(one,one);
      }
    } else {
      while (1) {
        SAY_ONE;  _delay_ns(9480);
        SAY_ZERO; _delay_ns(9480);
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

    const bool do_read = true;
    if (do_read) {
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

