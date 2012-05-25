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

#define INTER_FRAME_GAP 500 // Default delay between frames, in mS


uint16_t ifg = INTER_FRAME_GAP;

uint16_t dirIndex = 0;
uint8_t header[2];
LightWall LW;

void demo();

void setup() 
{

  LW.begin(32, 8);
}

char stuff[] = 
  /*
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  //*/  
  /*
  "\x1e\xb4\xd8\xe0"
  "\x16\xbc\xd8\xe0"
  "\x16\xbc\xd8\xe0"
  "\x0e\xa4\xc8\xf0"
  "\x0e\xa4\xc8\xf0"
  "\x06\xac\xc8\xf0"
  "\x06\xac\xc8\xf0"
  "\x02\xa8\xcc\xf0"
  "\x00\xaa\xcc\xf0"
  "\x00\xaa\xcc\xf0"
  "\x00\xaa\xcc\xf0"
  "\x00\xaa\xcc\xf0"
  "\x00\xaa\xcc\xf0"
  "\x00\xaa\xcc\xf0"
  "\x00\xaa\xcc\xf0"
  "\x00\xaa\xcc\xf0"
  "\xf8\xaa\xcc\xf0"
  "\xf0\xaa\xcc\xf0"
  "\xf8\xaa\xcc\xf0"
  "\xf8\xaa\xcc\xf0"
  "\xfc\xaa\xcc\xf0"
  "\xf4\xaa\xcc\xf0"
  "\xf8\xaa\xcc\xf0"
  "\xf4\xaa\xcc\xf0"
  "\xff\xaa\xcc\xf0"
  "\xff\xaa\xcc\xf0"
  "\xff\xaa\xcc\xf0"
  "\xff\xaa\xcc\xf0"
  "\xff\xaa\xcc\xf0"
  "\xff\xaa\xcc\xf0"
  "\xff\xaa\xcc\xf0"
  "\xff\xaa\xcc\xf0"
  //*/
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\0\0\0\0"
  "\x0f\x0f\x0f\0f"
  "\x0f\x0f\x0f\0f"
  "\x0f\x0f\x0f\0f"
  "\x0f\x0f\x0f\0f"
  "\x0f\x0f\x0f\0f"
  "\x0f\x0f\x0f\0f"
  "\x0f\x0f\x0f\0f"
  "\x0f\x0f\x0f\0f"
  "\xf0\xf0\xf0\f0"
  "\xf0\xf0\xf0\f0"
  "\xf0\xf0\xf0\f0"
  "\xf0\xf0\xf0\f0"
  "\xf0\xf0\xf0\f0"
  "\xf0\xf0\xf0\f0"
  "\xf0\xf0\xf0\f0"
  "\xf0\xf0\xf0\f0"
  "\xff\xff\xff\ff"
  "\xff\xff\xff\ff"
  "\xff\xff\xff\ff"
  "\xff\xff\xff\ff"
  "\xff\xff\xff\ff"
  "\xff\xff\xff\ff"
  "\xff\xff\xff\ff"
  "\xff\xff\xff\ff"
  ;

void loop()
{
  
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
        for (int icolor=0; icolor<3; icolor++) {
          for (int ibyte=0; ibyte<4; ibyte++) {
            if (do_read) {
              LW.Buffer[ibyte+4*icolor]=Serial.read();
            } else {
              if (icolor == 2) {
                LW.Buffer[ibyte+4*icolor]=stuff[ibyte+4*ii];
              } else {
                LW.Buffer[ibyte+4*icolor]='\xff';
              }
            }
          }
        }
        LW.send_frame(ii);
        _delay_us(48);
      }
    }
  }
  demo();
}


//Test array, 1 bit color resolution, used for debug testing and demo() output.
//                        Don't forget to invert the data!
//               RGBPYTWB  GBPYTWBR  BPYTWBRG  PYTWBRGB  YTWBRGBP  TWBRGBPY  WBRGBPTY  BRGBPYTW
byte rbits[] = {B01100101,B11001010,B10010101,B00101011,B01010110,B10101100,B01011001,B10110010}; 
byte gbits[] = {B10110001,B01100011,B11000110,B10001101,B00011011,B00110110,B01101100,B11011000};   
byte bbits[] = {B11001001,B10010011,B00100111,B01001110,B10011100,B00111001,B01110010,B11100100};  
int demoBytes=8; //length of the demo array

void set_single(int i, int j)
{
  int index;
  if (j == i) {
    index = 0;
  } else {
    index = 6;
  }
 
  for (int i=0;i<4;i++)
  {
    LW.Buffer[i]=bbits[index];
    LW.Buffer[i+4]=gbits[index];
    LW.Buffer[i+8]=rbits[index];
  }                            
}


// Demo Routine plays 
void demo()
{
  for (int i=0;i<50; i = (i + 1) % 50)
  { 
    for (int j=0;j<50;j++)
    {
      set_single(i, j);      
      LW.send_frame(j);
      _delay_us(48);

    }
    delay(1000);
  }
  LW.fadeout(3000);
  for (int i=0;i<30;i++)
  {
    LW.send_frame(i);
    _delay_us(48);
  }
  delay(1000);
  LW.fadein(3000);
  delay(1000);
}

// Soft Error
// Displays error in binary then continue
void error_code(int error)
{
  LW.blank_screen(); //clear the screen & buffer
  LW.Buffer[8]=LW.Buffer[9]=LW.Buffer[10]=LW.Buffer[11]=0x00; //Display in RED

  for (int i=0;i<16;i++)
  {
    if ((error >> i) & 1) 
      {
        LW.send_frame(i);
        _delay_us(50);
      }
  }
  // Blink error
  for(int i=0;i<10;i++)
  {
   delay(500); 
   LW.fadeout(3000);
   delay(500); 
   LW.fadein(3000);
  }
}

// Hard Error
// Alternate between error code and Demo() 
void error_die(int error)
{
  while (1)
  {
    error_code(error);
    for(int i=0;i<5;i++)
    {
      demo();
    }     
  }
}

