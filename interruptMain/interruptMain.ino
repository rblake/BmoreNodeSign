
#define ARDUINO_ID 0

#define LIGHTS_PER_STRING 24

#define BYTES_PER_OUTPUT 6
#define BITS_PER_COLOR 12
#define BYTES_PER_FRAME (BITS_PER_COLOR*BYTES_PER_OUTPUT)

#define BMASK (0x063FF000)
#define CMASK (0x37EFF3FE)
#define DMASK (0x000003FF)

#define OUTPUT_MASKS() do {                     \
REG_PIOB_PER = REG_PIOB_OER = BMASK;            \
REG_PIOC_PER = REG_PIOC_OER = CMASK;            \
REG_PIOD_PER = REG_PIOD_OER = DMASK;            \
} while (0);

//FIXME!  Write zero to all the registers
#define SAY_LOW() do {                          \
REG_PIOB_ODSR = 0;                              \
REG_PIOC_ODSR = 0;                              \
REG_PIOD_ODSR = 0;                              \
} while(0)

//FIXME! Write one to all the registers
#define SAY_HIGH() do {                         \
REG_PIOB_ODSR = 0xFFFFFFFF;                     \
REG_PIOC_ODSR = 0xFFFFFFFF;                     \
REG_PIOD_ODSR = 0xFFFFFFFF;                     \
} while(0)

//FIXME: write out 6 bytes to all the registers, based off of the pin
//mapping.
inline SAY_VAR(byte out[6]) {
  uint32 AAA=out[0];
  uint32 BBB=out[1];
  uint32 CCC=out[2];
  uint32 DDD=out[3];
  uint32 EEE=out[4];
  uint32 FFF=out[5];
  uint32 midFFF = FFF&0x1C;

  REG_PIOB_ODSR = ((AAA << 14) | (BBB << 6) | (FFF << 25)) & BMASK;
  REG_PIOC_ODSR = ( ((BBB&0x3F) << 21) 
                   | (CCC << 12) 
                   | (DDD << 2) 
                   | (midFFF >> 3)
                   | (midFFF << 26)
                   ) & CMASK;
  REG_PIOD_ODSR = ((EEE << 3) | (FFF >> 5)) & DMASK;
}

byte displayA[BYTES_PER_FRAME*LIGHTS_PER_STRING+1] =
"\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff"
"\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff"
"\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff"
"\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff"
"\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff"
"\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff"
"\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff"
"\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff"
"\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff"
"\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff"
"\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff"
"\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff"
"\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff"
"\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff"
"\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff"
"\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff"
"\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff"
"\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff"
"\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff"
"\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff"
"\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff"
"\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff"
"\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff"
"\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff"
"\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff"
"\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff"
"\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff"
"\xff\xfd\xff\x00\x00\x00\xff\xfd\xff\x00\x00\x00\xff\xfd\xff\x00\x00\x00\xff\xfd\xff\x00\x00\x00"
"\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff"
"\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff"
"\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff"
"\xff\xfb\xff\x00\x00\x00\xff\xfb\xff\x00\x00\x00\xff\xfb\xff\x00\x00\x00\xff\xfb\xff\x00\x00\x00"
"\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff"
"\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff"
"\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff"
"\xff\xf7\xff\x00\x00\x00\xff\xf7\xff\x00\x00\x00\xff\xf7\xff\x00\x00\x00\xff\xf7\xff\x00\x00\x00"
"\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff"
"\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff"
"\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff"
"\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff"
"\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff"
"\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff\xff\xdf\xff"
"\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff"
"\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff"
"\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff\xff\xbf\xff"
"\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff"
"\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff"
"\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff\xff\x7f\xff"
"\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe"
"\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe"
"\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe\xff\xff\xfe"
"\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd"
"\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd"
"\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd\xff\xff\xfd"
"\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb"
"\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb"
"\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb\xff\xff\xfb"
"\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7"
"\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7"
"\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7\xff\xff\xf7"
"\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef"
"\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef"
"\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef\xff\xff\xef"
"\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f"
"\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f"
"\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f\xff\xff\x5f"
"\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f"
"\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f"
"\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f\xff\xff\x3f"
"\xff\xff\x1f\xff\xff\xff\xff\xff\x1f\xff\xff\xff\xff\xff\x1f\xff\xff\xff\xff\xff\x1f\xff\xff\xff"
"\xff\xff\x1f\xff\xff\xff\xff\xff\x1f\xff\xff\xff\xff\xff\x1f\xff\xff\xff\xff\xff\x1f\xff\xff\xff"
"\xff\xff\x1f\xff\xff\xff\xff\xff\x1f\xff\xff\xff\xff\xff\x1f\xff\xff\xff\xff\xff\x1f\xff\xff\xff"
;

byte displayB[BYTES_PER_FRAME*LIGHTS_PER_STRING+1];

byte* currentDisplay = displayA;
byte* nextDisplay = displayB;


#define STATIC_ASSERT(test, name) byte assert##name[!(test)]

/* Three ticks per bite, plus one tick to start, plus a rest of at least 3 ticks)
 */
#define NUMBITS (6+8+12)
#define INTERRUPT_MAX (1+NUMBITS*3+3)
#define WAIT_FOR_FRAME (INTERRUPT_MAX+1)

volatile byte addr;
byte intensity=0xCC;
volatile byte* outputBuffer;
volatile int iterCount=WAIT_FOR_FRAME;
#if 0
//#define allLightsInterrupt TC0_Handler 
void allLightsInterrupt() {
  
  //Trigger that we're sending now.
  if (iterCount == 0) {
    SAY_HIGH();
  } else if (iterCount <= INTERRUPT_MAX) {
    switch (iterCount % 3) {
    case 1:
      SAY_LOW();
      break;
    default:
      int bit = (iterCount-2)/3;
      switch (bit) {
      case  0: if (addr & B100000) {SAY_LOW();} else {SAY_HIGH();} break;
      case  1: if (addr & B010000) {SAY_LOW();} else {SAY_HIGH();} break;
      case  2: if (addr & B001000) {SAY_LOW();} else {SAY_HIGH();} break;
      case  3: if (addr & B000100) {SAY_LOW();} else {SAY_HIGH();} break;
      case  4: if (addr & B000010) {SAY_LOW();} else {SAY_HIGH();} break;
      case  5: if (addr & B000001) {SAY_LOW();} else {SAY_HIGH();} break;
      case  6: if (intensity & B10000000) {SAY_LOW();} else {SAY_HIGH();} break;
      case  7: if (intensity & B01000000) {SAY_LOW();} else {SAY_HIGH();} break;
      case  8: if (intensity & B00100000) {SAY_LOW();} else {SAY_HIGH();} break;
      case  9: if (intensity & B00010000) {SAY_LOW();} else {SAY_HIGH();} break;
      case 10: if (intensity & B00001000) {SAY_LOW();} else {SAY_HIGH();} break;
      case 11: if (intensity & B00000100) {SAY_LOW();} else {SAY_HIGH();} break;
      case 12: if (intensity & B00000010) {SAY_LOW();} else {SAY_HIGH();} break;
      case 13: if (intensity & B00000001) {SAY_LOW();} else {SAY_HIGH();} break;
      default:
        SAY_VAR(outBuffer);
        outBuffer += BYTES_PER_OUTPUT;
        break;
      }
    case 0:
      SAY_HIGH();
      break;
    }
  } else {
    SAY_LOW();
  }

  if (iterCount == INTERRUPT_MAX) {
    if (addr < LIGHTS_PER_STRING) {
      addr++;
      iterCount=0;
    } else {
      iterCount=WAIT_FOR_FRAME;
    }
  } else {
    iterCount++;
  }
}
#endif

void renderFrame() {
  while (iterCount != WAIT_FOR_FRAME) {}
  outputBuffer = currentDisplay;
  addr=0;
  iterCount=0;
}

volatile boolean l;
#define blink TC0_Handler 
void blink()
{
    long dummy=REG_TC0_SR0; // vital - reading this clears some flag
                            // otherwise you get infinite interrupts
    l= !l;
    digitalWrite(13,l);
}

const bool blink = true;

void setup() 
{
  if (blink) {
    pinMode(13,OUTPUT);
  } else {
    Serial.println(ARDUINO_ID);
  }
  
  //setup interrupts
  pmc_enable_periph_clk(ID_TC0);
  REG_TC0_WPMR=0x54494D00; // enable write to registers
 
  // REG_TC0_CMR0=0b00000000000010011100010000000000; // set channel mode register (see datasheet)
  REG_TC0_CMR0 =
      TC_CMR_WAVE
    | TC_CMR_EEVTEDG_NONE
    | TC_CMR_EEVT_XC0
    | TC_CMR_WAVSEL_UP_RC
    // | TC_CMR_ACPA_SET
    // | TC_CMR_ACPC_CLEAR
   
    | TC_CMR_TCCLKS_TIMER_CLOCK4;  // mclk/128 = 656.250 KHz
   
  REG_TC0_RC0=656250;    // counter period
  REG_TC0_RA0=656250/5;  // PWM value
  REG_TC0_CCR0=          // start counter
      TC_CCR_SWTRG
    | TC_CCR_CLKEN;
  REG_TC0_IER0= TC_IER_CPCS; // enable interrupt on counter=rc
  REG_TC0_IDR0=~TC_IDR_CPCS; // disable other interrupts
 
  NVIC_EnableIRQ(TC0_IRQn); // enable TC0 interrupts
 
}

void loop()
{
  if (blink) {
    return;
  }

  renderFrame();
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
    for (int ii=0; ii<LIGHTS_PER_STRING; ii++) {
      for (int jj=0; jj<BYTES_PER_FRAME; jj++) {
        while (Serial.available() < 1) {}
        nextDisplay[ii*BYTES_PER_FRAME+jj]=Serial.read();
      }
    }
    //swap the displays
    //swap(currentDisplay,nextDisplay);
    byte* temp = currentDisplay;
    currentDisplay = nextDisplay;
    nextDisplay = temp;
  }
}
