/*
 * Tap Tremelo Pedal Firmware
 * V1 by Dennis Gunia 
 * 
 * 2022/07
 * 
 */

#define lmicros() ((long)micros())

//Lookup Tables
static const uint8_t sineLUT[256] PROGMEM = {
  0x80, 0x83, 0x86, 0x89, 0x8c, 0x8f, 0x92, 0x95, 0x98, 0x9b, 0x9e, 0xa2, 0xa5, 0xa7, 0xaa, 0xad,
  0xb0, 0xb3, 0xb6, 0xb9, 0xbc, 0xbe, 0xc1, 0xc4, 0xc6, 0xc9, 0xcb, 0xce, 0xd0, 0xd3, 0xd5, 0xd7,
  0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xeb, 0xed, 0xee, 0xf0, 0xf1, 0xf3, 0xf4,
  0xf5, 0xf6, 0xf8, 0xf9, 0xfa, 0xfa, 0xfb, 0xfc, 0xfd, 0xfd, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfd, 0xfd, 0xfc, 0xfb, 0xfa, 0xfa, 0xf9, 0xf8, 0xf6,
  0xf5, 0xf4, 0xf3, 0xf1, 0xf0, 0xee, 0xed, 0xeb, 0xea, 0xe8, 0xe6, 0xe4, 0xe2, 0xe0, 0xde, 0xdc,
  0xda, 0xd7, 0xd5, 0xd3, 0xd0, 0xce, 0xcb, 0xc9, 0xc6, 0xc4, 0xc1, 0xbe, 0xbc, 0xb9, 0xb6, 0xb3,
  0xb0, 0xad, 0xaa, 0xa7, 0xa5, 0xa2, 0x9e, 0x9b, 0x98, 0x95, 0x92, 0x8f, 0x8c, 0x89, 0x86, 0x83,
  0x80, 0x7c, 0x79, 0x76, 0x73, 0x70, 0x6d, 0x6a, 0x67, 0x64, 0x61, 0x5d, 0x5a, 0x58, 0x55, 0x52,
  0x4f, 0x4c, 0x49, 0x46, 0x43, 0x41, 0x3e, 0x3b, 0x39, 0x36, 0x34, 0x31, 0x2f, 0x2c, 0x2a, 0x28,
  0x25, 0x23, 0x21, 0x1f, 0x1d, 0x1b, 0x19, 0x17, 0x15, 0x14, 0x12, 0x11, 0xf, 0xe, 0xc, 0xb,
  0xa, 0x9, 0x7, 0x6, 0x5, 0x5, 0x4, 0x3, 0x2, 0x2, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x2, 0x2, 0x3, 0x4, 0x5, 0x5, 0x6, 0x7, 0x9,
  0xa, 0xb, 0xc, 0xe, 0xf, 0x11, 0x12, 0x14, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f, 0x21, 0x23,
  0x25, 0x28, 0x2a, 0x2c, 0x2f, 0x31, 0x34, 0x36, 0x39, 0x3b, 0x3e, 0x41, 0x43, 0x46, 0x49, 0x4c,
  0x4f, 0x52, 0x55, 0x58, 0x5a, 0x5d, 0x61, 0x64, 0x67, 0x6a, 0x6d, 0x70, 0x73, 0x76, 0x79, 0x7c
};

static const uint8_t bumpLUT[256] PROGMEM = {
  0, 4, 7, 10, 13, 16, 19, 22, 25, 29, 32, 35, 38, 41, 44, 47,
  50, 53, 56, 59, 62, 65, 69, 72, 75, 78, 80, 83, 86, 89, 92, 95,
  98, 101, 104, 107, 110, 112, 115, 118, 121, 123, 126, 129, 132, 134, 137, 140,
  142, 145, 147, 150, 152, 155, 157, 160, 162, 165, 167, 169, 172, 174, 176, 179,
  181, 183, 185, 187, 189, 192, 194, 196, 198, 200, 202, 203, 205, 207, 209, 211,
  213, 214, 216, 218, 219, 221, 222, 224, 225, 227, 228, 230, 231, 232, 234, 235,
  236, 237, 238, 240, 241, 242, 243, 244, 245, 245, 246, 247, 248, 249, 249, 250,
  251, 251, 252, 252, 253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 252, 251,
  251, 250, 249, 249, 248, 247, 246, 245, 245, 244, 243, 242, 241, 240, 238, 237,
  236, 235, 234, 232, 231, 230, 228, 227, 225, 224, 222, 221, 219, 218, 216, 214,
  213, 211, 209, 207, 205, 203, 202, 200, 198, 196, 194, 192, 189, 187, 185, 183,
  181, 179, 176, 174, 172, 169, 167, 165, 162, 160, 157, 155, 152, 150, 147, 145,
  142, 140, 137, 134, 132, 129, 126, 123, 121, 118, 115, 112, 110, 107, 104, 101,
  98, 95, 92, 89, 86, 83, 80, 78, 75, 72, 69, 65, 62, 59, 56, 53,
  50, 47, 44, 41, 38, 35, 32, 29, 25, 22, 19, 16, 13, 10, 7, 4
};

//Waveform Constants
#define WF_SAW 1
#define WF_SINE 2
#define WF_SQUARE 4

int depth = 0;
unsigned long delayTime = 1;
uint8_t waveform = WF_SINE;

uint8_t symetry = 127;
uint16_t wfStep = 0;

unsigned long lastStep = millis();
unsigned long lastTest = millis();

//IO Pin Definition
#define PIN_A_RATE A1   //Pot Rate
#define PIN_A_DEPTH A2  //Pot Depth
#define PIN_A_SHAPE A0  //Pot Shape
#define PIN_A_MUL A3    //Pot Clock-Modifier

#define PIN_D_WF1 7     //Waveform Switch Pole 1
#define PIN_D_WF2 10    //Waveform Switch Pole 2
#define PIN_D_TAP 9     //Tap Tempo Push button
#define PIN_D_MOD 8     //Tempo Modulation Switch

#define PIN_PWM A5

void setup() {
  // setup Timer 1 to 31.2kHz @ 8 Bit
  TCCR1B &= 0b11100000;
  TCCR1B |= 0b00001001;
  TCCR1A &= 0b11111100;
  TCCR1A |= 0b00000001;

  //Set input pins to pullup
  pinMode(PIN_D_WF1, INPUT_PULLUP);
  pinMode(PIN_D_WF2, INPUT_PULLUP);
  pinMode(PIN_D_TAP, INPUT_PULLUP);
  pinMode(PIN_D_MOD, INPUT_PULLUP);
}

//Read pots and switch state
void getInput() {
  depth = analogRead(PIN_A_DEPTH) / 4;
  //delayTime = map(analogRead(A1),0,1023,5000,20000);
  delayTime = map(analogRead(PIN_A_RATE), 0, 1023, 0, 40000); ;
  symetry = map(analogRead(PIN_A_SHAPE), 0, 1023, 4, 251); ;

  waveform = WF_SINE;
  if (digitalRead(PIN_D_WF1) == LOW) {
    waveform = WF_SQUARE;
  }
  if (digitalRead(PIN_D_WF2) == LOW) {
    waveform = WF_SAW;
  }
}

void loop() {
  unsigned long millisNow = lmicros() / 8;  //Timestamp NOW

  if (lastTest + 100 < millisNow) {   //Get panel state every 100ms
    lastTest = millisNow;
    getInput();
  }

  if (lastStep < millisNow - delayTime) { //do waveform stuff
    wfStep ++;
    if (wfStep > 255) wfStep = 0;
    lastStep = millisNow;


    if ( waveform == WF_SAW ) { 
      // SAW/TRI
      if (wfStep < symetry) {
        analogWrite(PIN_PWM, map(wfStep, 0, symetry, depth, 255));
      } else {
        analogWrite(PIN_PWM, map(wfStep, 255, symetry, depth, 255));
      }
    } else if ( waveform == WF_SINE ) {
      // SINE/BUMP
      if ( symetry >= 63 && symetry <= 191){
        uint8_t val = pgm_read_word_near(sineLUT + (wfStep));
        analogWrite(PIN_PWM, map(val, 0, 255, depth, 255));
      }else{
        uint8_t val = pgm_read_word_near(bumpLUT + (wfStep));
        if (symetry > 127){val = 255-val;}
        analogWrite(PIN_PWM, map(val, 0, 255, depth, 255));
      }
    } else {
      // SQUARE/PWM
      if (wfStep < symetry) {
        analogWrite(PIN_PWM, depth);
      } else {
        analogWrite(PIN_PWM, 255);
      }
    }

  }
}
