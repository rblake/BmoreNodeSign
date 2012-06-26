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

void setup() 
{

  LW.begin(32, 8);
}

char stuff[] =
  "\xcc0\xcc0\xcc0\xcc0\xaaV\xaaV\xaaV\xaaV\x99e\x99e\x99e\x99e\xcc0\xcc0\xcc0\xcc0\xaaV\xaaV\xaaV\xaaV\x99e\x99e\x99e\x99e\xcc0\xcc0\xcc0\xcc0\xaaV\xaaV\xaaV\xaaV\x99e\x99e\x99e\x99e\xcc0\xcc0\xcc0\xcc0\xaaV\xaaV\xaaV\xaaV\x99e\x99e\x99e\x99e\xce\xc4\xce\xc4\xce\xc4FD\xfd\xf7\xfd\xf7\xfd\xf7 \"\xbb\xb3\xbb\xb3\xbb\xb3\x13\x11B\xf4B\xf4B\xf4Btu\xd7u\xd7u\xd7$\x127\xa37\xa37\xa3\x17!B\xf4B\xf4B\xf4Btu\xd7u\xd7u\xd7$\x127\xa37\xa37\xa3\x17!~\x07~\x07~\x07v\x07]u]u]u\x10a+r+r+r#R\xcc0\xcc0\xcc0\xcc0\xaaV\xaaV\xaaV\xaaV\x99e\x99e\x99e\x99e33333333UUUUUUUUffffffff33333333UUUUUUUUffffffff\xcc0\xcc0\xcc0\xcc0\xaaV\xaaV\xaaV\xaaV\x99e\x99e\x99e\x99e\x9e\x84\x9e\x84\x9e\x84\x16\x04\xfd\xf7\xfd\xf7\xfd\xf7pb\xeb\xf3\xeb\xf3\xeb\xf3CQ{\x13{\x13{\x13s\x13]u]u]u\x15u.f.f.f&F\x07\xe0\x07\xe0\x07\xe0\x07`u\xd7u\xd7u\xd7a\x06r\xb7r\xb7r\xb7R5^\x86^\x86^\x86V\x06}\xf5}\xf5}\xf50`+\xf3+\xf3+\xf3\x03S\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\x99\x99\x99\x99\x99\x99\x99\x993\xcf3\xcf3\xcf3\xcfU\xa9U\xa9U\xa9U\xa9f\x9af\x9af\x9af\x9a3\xcf3\xcf3\xcf3\xcfU\xa9U\xa9U\xa9U\xa9f\x9af\x9af\x9af\x9a\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\x99\x99\x99\x99\x99\x99\x99\x99\x9c\x8c\x9c\x8c\x9c\x8c\x14\x04\xff\xff\xff\xff\xff\xffrb\xeb\xfb\xeb\xfb\xeb\xfbAQ}\n}\n}\nu\x02_}_}_}\x13d*\x7f*\x7f*\x7f W\x01\xf9\x01\xf9\x01\xf9\x01qw\xdfw\xdfw\xdfg\x17v\xaev\xaev\xaeT$\\\x8e\\\x8e\\\x8eT\x06\x7f\xfd\x7f\xfd\x7f\xfd2`+\xfb+\xfb+\xfb\x01S\xcc0\xcc0\xcc0\xcc0\xaaV\xaaV\xaaV\xaaV\x99e\x99e\x99e\x99e\xcc0\xcc0\xcc0\xcc0\xaaV\xaaV\xaaV\xaaV\x99e\x99e\x99e\x99e\xcc0\xcc0\xcc0\xcc0\xaaV\xaaV\xaaV\xaaV\x99e\x99e\x99e\x99e\xcc0\xcc0\xcc0\xcc0\xaaV\xaaV\xaaV\xaaV\x99e\x99e\x99e\x99e\x8e\xc5\x8e\xc5\x8e\xc5\x06E\xfd\xf7\xfd\xf7\xfd\xf7`#\xfb\xb2\xfb\xb2\xfb\xb2S\x10\xeaT\xeaT\xeaTbT\xddw\xddw\xddw\x042\xbf#\xbf#\xbf#7\x01\xca\xd4\xca\xd4\xca\xd4BT\xfd\xf7\xfd\xf7\xfd\xf7$2\xbf\xa3\xbf\xa3\xbf\xa3\x17\x01\xce\xc4\xce\xc4\xce\xc4FD\xfd\xf7\xfd\xf7\xfd\xf7 \"\xbb\xb3\xbb\xb3\xbb\xb3\x13\x11";

void loop()
{
  /*
  // Code for measuring the timing of the output loop.
  // We measured this with an oscilliscope to be 10us.
  while (1) {
    volatile byte zero = 0xFF;
    OUT_VAR(zero,zero);
    volatile byte one = 0x00;
    OUT_VAR(one,one);
  }


  while (1) {
    SAY_ONE;  _delay_ns(9480);
    SAY_ZERO; _delay_ns(9480);
  }
  */

  int do_read = 0;
  while (1) {
    char O;
    char K;
    if (do_read) {
      O = Serial.read();
      K = Serial.read();
    }
    if (!do_read || O == 'O' && K == 'K') {
      for (int ii=0; ii<32; ii++) {
        for (int jj=0; jj<24; jj++) {
          if (do_read) {
            LW.Buffer[jj]=Serial.read();
          } else {
            LW.Buffer[jj]=stuff[ii*24+jj];
          }
        }
        LW.send_frame(ii);
        _delay_us(48);
      }
    }
  }
}

