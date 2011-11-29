/*

Rainbowduino Plasma

based on Pladma for the Meggy Jr. by Ken Corey

*/

/*
  Rainbowduino_Plasma.pde
 
  based on MeggyJr_Plasma.pde 0.3

 Color cycling plasma   
    
 Version 0.1 - 8 July 2009
 
 Copyright (c) 2009 Ben Combee.  All right reserved.
 Copyright (c) 2009 Ken Corey.  All right reserved.
 Copyright (c) 2008 Windell H. Oskay.  All right reserved.
 http://www.evilmadscientist.com/
 
 This library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this library.  If not, see <http://www.gnu.org/licenses/>.
 	  
 */
#include <math.h>
#include "Rainbow.h"
#include <avr/pgmspace.h>


#define screenWidth 8
#define screenHeight 8
#define paletteSize 64

typedef struct
{
  int r;
  int g;
  int b;
} ColorRGB;

//a color with 3 components: h, s and v
typedef struct 
{
  int h;
  int s;
  int v;
} ColorHSV;

int plasma[screenWidth][screenHeight];
long paletteShift;
byte state;

void SetPixel(byte x, byte y, byte r, byte g, byte b);

//Converts an HSV color to RGB color
void HSVtoRGB(void *vRGB, void *vHSV) 
{
  float r, g, b, h, s, v; //this function works with floats between 0 and 1
  float f, p, q, t;
  int i;
  ColorRGB *colorRGB=(ColorRGB *)vRGB;
  ColorHSV *colorHSV=(ColorHSV *)vHSV;

  h = (float)(colorHSV->h / 256.0);
  s = (float)(colorHSV->s / 256.0);
  v = (float)(colorHSV->v / 256.0);

  //if saturation is 0, the color is a shade of grey
  if(s == 0.0) {
    b = v;
    g = b;
    r = g;
  }
  //if saturation > 0, more complex calculations are needed
  else
  {
    h *= 6.0; //to bring hue to a number between 0 and 6, better for the calculations
    i = (int)(floor(h)); //e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
    f = h - i;//the fractional part of h

    p = (float)(v * (1.0 - s));
    q = (float)(v * (1.0 - (s * f)));
    t = (float)(v * (1.0 - (s * (1.0 - f))));

    switch(i)
    {
      case 0: r=v; g=t; b=p; break;
      case 1: r=q; g=v; b=p; break;
      case 2: r=p; g=v; b=t; break;
      case 3: r=p; g=q; b=v; break;
      case 4: r=t; g=p; b=v; break;
      case 5: r=v; g=p; b=q; break;
      default: r = g = b = 0; break;
    }
  }
  colorRGB->r = (int)(r * 255.0);
  colorRGB->g = (int)(g * 255.0);
  colorRGB->b = (int)(b * 255.0);
}

int RGBtoINT(void *vRGB)
{
  ColorRGB *colorRGB=(ColorRGB *)vRGB;

  return (colorRGB->r<<16) + (colorRGB->g<<8) + colorRGB->b;
}


void setup()                    // run once, when the sketch starts
{
  int x,y;

  _init();

  Serial.begin(9600);

  //generate the plasma once
  for(x = 0; x < screenWidth; x++)
    for(y = 0; y < screenHeight; y++)
    {
      x &= 7;
      y &= 7;
      SetPixel(x, y, 0xFF, 0xFF, 0xFF);
    }
}

void loop()                     // run over and over again
{  
  if (Serial.available() >= 12) {
      ColorRGB north;
      north.r = Serial.read();
      north.g = Serial.read();
      north.b = Serial.read();
  
      ColorRGB south;
      south.r = Serial.read();
      south.g = Serial.read();
      south.b = Serial.read();

      ColorRGB east;
      east.r = Serial.read();
      east.g = Serial.read();
      east.b = Serial.read();

      ColorRGB west;
      west.r = Serial.read();
      west.g = Serial.read();
      west.b = Serial.read();

      SetPixel(3,0,north.r,north.g,north.b);
      SetPixel(4,0,north.r,north.g,north.b);
      SetPixel(3,1,north.r,north.g,north.b);
      SetPixel(4,1,north.r,north.g,north.b);
      SetPixel(3,6,south.r,south.g,south.b);
      SetPixel(4,6,south.r,south.g,south.b);
      SetPixel(3,7,south.r,south.g,south.b);
      SetPixel(4,7,south.r,south.g,south.b);
      SetPixel(0,3,east.r,east.g,east.b);
      SetPixel(0,4,east.r,east.g,east.b);
      SetPixel(1,3,east.r,east.g,east.b);
      SetPixel(1,4,east.r,east.g,east.b);
      SetPixel(6,3,west.r,west.g,west.b);
      SetPixel(6,4,west.r,west.g,west.b);
      SetPixel(7,3,west.r,west.g,west.b);
      SetPixel(7,4,west.r,west.g,west.b);
  }
}


//=============================================================
extern unsigned char dots_color[2][3][8][4];  //define Two Buffs (one for Display ,the other for receive data)
extern unsigned char GamaTab[16];             //define the Gamma value for correct the different LED matrix
//=============================================================
unsigned char line,level;
unsigned char Buffprt=0;
unsigned char State=0;

void SetPixel(byte x, byte y, byte r, byte g, byte b)
{
  x &= 7;
  y &= 7;

  r = (r >> 4);
  g = (g >> 4);
  b = (b >> 4);

  if ((x & 1) == 1) {
      dots_color[Buffprt][1][y][x >> 1] = r |
        (dots_color[Buffprt][1][y][x >> 1] & 0xF0);
      dots_color[Buffprt][0][y][x >> 1] = g |
        (dots_color[Buffprt][0][y][x >> 1] & 0xF0);
      dots_color[Buffprt][2][y][x >> 1] = b |
        (dots_color[Buffprt][2][y][x >> 1] & 0xF0);
  }
  else {
      dots_color[Buffprt][1][y][x >> 1] = (r << 4) |
        (dots_color[Buffprt][1][y][x >> 1] & 0x0F);
      dots_color[Buffprt][0][y][x >> 1] = (g << 4) |
        (dots_color[Buffprt][0][y][x >> 1] & 0x0F);
      dots_color[Buffprt][2][y][x >> 1] = (b << 4) |
        (dots_color[Buffprt][2][y][x >> 1] & 0x0F);
  }
}

ISR(TIMER2_OVF_vect)          //Timer2  Service 
{ 
  TCNT2 = GamaTab[level];    // Reset a  scanning time by gamma value table
  flash_next_line(line,level);  // sacan the next line in LED matrix level by level.
  line++;
  if(line>7)        // when have scaned all LEC the back to line 0 and add the level
  {
    line=0;
    level++;
    if(level>15)       level=0;
  }
}

void init_timer2(void)               
{
  TCCR2A |= (1 << WGM21) | (1 << WGM20);   
  TCCR2B |= (1<<CS22);   // by clk/64
  TCCR2B &= ~((1<<CS21) | (1<<CS20));   // by clk/64
  TCCR2B &= ~((1<<WGM21) | (1<<WGM20));   // Use normal mode
  ASSR |= (0<<AS2);       // Use internal clock - external clock not used in Arduino
  TIMSK2 |= (1<<TOIE2) | (0<<OCIE2B);   //Timer2 Overflow Interrupt Enable
  TCNT2 = GamaTab[0];
  sei();   
}

void _init(void)    // define the pin mode
{
  DDRD=DDRD | 0xff;
  DDRC=0xff;
  DDRB=0xff;
  PORTD=0;
  PORTB=0;
  init_timer2();  // initial the timer for scanning the LED matrix
}

//==============================================================
void shift_1_bit(unsigned char LS)  //shift 1 bit of  1 Byte color data into Shift register by clock
{
  if(LS)
  {
    shift_data_1;
  }
  else
  {
    shift_data_0;
  }
  clk_rising;
}
//==============================================================
void flash_next_line(unsigned char line,unsigned char level) // scan one line
{
  disable_oe;
  close_all_line;
  open_line(line);
  shift_24_bit(line,level);
  enable_oe;
}

//==============================================================
void shift_24_bit(unsigned char line,unsigned char level)   // display one line by the color level in buff
{
  unsigned char color=0,row=0;
  unsigned char data0=0,data1=0;
  le_high;
  for(color=0;color<3;color++)//GBR
  {
    for(row=0;row<4;row++)
    {
      data1=dots_color[Buffprt][color][line][row]&0x0f;
      data0=dots_color[Buffprt][color][line][row]>>4;

      if(data0>level)   //gray scale,0x0f aways light
      {
        shift_1_bit(1);
      }
      else
      {
        shift_1_bit(0);
      }

      if(data1>level)
      {
        shift_1_bit(1);
      }
      else
      {
        shift_1_bit(0);
      }
    }
  }
  le_low;
}



//==============================================================
void open_line(unsigned char line)     // open the scaning line 
{
  switch(line)
  {
  case 0: open_line0; break;
  case 1: open_line1; break;
  case 2: open_line2; break;
  case 3: open_line3; break;
  case 4: open_line4; break;
  case 5: open_line5; break;
  case 6: open_line6; break;
  case 7: open_line7; break;
  }
}
