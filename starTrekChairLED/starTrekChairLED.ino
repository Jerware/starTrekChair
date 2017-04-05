// starTrekChairLED release 20160319
//
// This code controls the LEDs on Adam Savage's Star Trek captain's chair
// featured in the following Tested video:
//
// https://youtu.be/jaVi06DaTk0
//
// It compiles on Teensy 3.1 and is designed to work in parallel with the 
// sound code. Both sources are available in this GITHUB repository:
//
// https://github.com/Jerware/
//
// Jeremy Williams wrote the code and designed the sound/LED hardware.
// This is the second revision of the code & hardware. The sound board now
// acts as a master and the LED board is a slave. All buttons are routed
// solely to the sound board, and it sends signals to the LED board
// via hardware serial. There is also a pin expander IC incorporated 
// instead of the pins on the underside of the Teensy 3.1.
//
// As always, thanks to the amazing library developers for keeping Arduino coding
// from becoming a chore. You are all giants. I'll monitor the feedback on Github
// from time to time but this code should be considered unsupported. #LLAP.

#include "FastLED.h"
#include <IRremote.h>

#define HWSERIAL Serial1

IRsend irsend;

byte lastButton;
boolean switchClosed;
int slotMode;
boolean moviePlaying;
byte lightShow; // received i2c command

// LED SETUP
#define LED_PIN     17
#define NUM_LEDS    318
#define BRIGHTNESS  255
#define LED_TYPE    NEOPIXEL
#define COLOR_ORDER GRB
int UPDATES_PER_SECOND = 100;
long showRate = 100;
CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
extern const TProgmemPalette16 alarm1_p PROGMEM;
extern const TProgmemPalette16 alarm2_p PROGMEM;
long ledUpdate;
long effectDuration;
long effectStopTime;
byte ledColor;
int ledPhase;
long lightPanelTick;
long lightPanelTickDuration = 500; // time between ticks in ms
byte lightPanelLife = 10; // 0-255 likelihood of panel blinks

void setup() {
  Serial.begin(57600);
  HWSERIAL.begin(38400);
  delay(1000);
  Serial.println("Hello there!");
  
  FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(  BRIGHTNESS );
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  
  resetJewels();
  leds[10] = CRGB::White;
  leds[11] = CRGB::White;
  Serial.println("All systems online, Captain!");
}

void checkHWSerial()
{
  int incomingByte;
  if (HWSERIAL.available() > 0) {
    incomingByte = HWSERIAL.parseInt();
    Serial.print("Message from Starfleet: ");
    Serial.println(incomingByte);
    lightShow = incomingByte;
    HWSERIAL.clear();
  }
}

void loop() {
  checkHWSerial();
  if (lightShow == 0)
  {
    lightShow = 255;
    resetJewels();
  }
  // main buttons
  else if (lightShow == 1) {
    Serial.println("Lightshow 1");
    lightShow = 255;
    ledColor = 0;
    buttonPressed(1, 57000, 90);  // button, duration in ms, updates per second
  }
  else if (lightShow == 2) {
    Serial.println("Lightshow 2");
    lightShow = 255;
    ledColor = 0;
    buttonPressed(2, 33000, 90);
  }
  else if (lightShow == 3) {
    Serial.println("Lightshow 3");
    lightShow = 255;
    ledColor = 0;
    buttonPressed(3, 64000, 15);
  }
  else if (lightShow == 4) {
    Serial.println("Lightshow 4");
    lightShow = 255;
    if (moviePlaying)
    {
      irsend.sendSony(0xA70, 12); // OK/Pause
    }
    ledColor = 0;
    buttonPressed(4, 11000, 12);
  }
  else if (lightShow == 5) {
    Serial.println("Lightshow 5");
    lightShow = 255;
    if (moviePlaying)
    {
      irsend.sendSony(0x338, 12); // left
    }
    lastButton = 0;
    ledColor = 255;
    buttonPressed(5, 10000, 100);
  }
  else if (lightShow == 6) {
    Serial.println("Lightshow 5");
    lightShow = 255;
    if (moviePlaying)
    {
      irsend.sendSony(0x1D0, 12); // TV-RETURN/Kill omxplayer
    }
    leds[8] = CRGB::Red;
    leds[9] = CRGB::Red;
  }
//  if (button6.risingEdge()) {
//    leds[8] = CRGB::White;
//    leds[9] = CRGB::White;
//  }
  else if (lightShow == 7) {
    lightShow = 255;
    if (moviePlaying)
    {
      irsend.sendSony(0x738, 12); // right
    }
    leds[4] = CRGB::Black;
    leds[5] = CRGB::Black;
  }
//  if (button7.risingEdge()) {
//    leds[4] = CRGB::White;
//    leds[5] = CRGB::White;
//  }
  
  // intercom
  else if (lightShow == 8) {
    lightShow = 255;
    leds[10] = CRGB::Yellow;
    leds[11] = CRGB::Yellow;
  }
//  if (buttonIntercom.risingEdge()) {
//    leds[10] = CRGB::White;
//    leds[11] = CRGB::White;
//  }

//  // rocker switches
//  if (rocker1.fallingEdge()) {
//    lightPanelLife+=5;
//    lightPanelTickDuration-=20;
//  }
//  if (rocker1.risingEdge()) {
//    lightPanelLife-=5;
//    lightPanelTickDuration+=20;
//  }
//  if (rocker2.fallingEdge()) {
//    lightPanelLife+=5;
//    lightPanelTickDuration-=20;
//  }
//  if (rocker2.risingEdge()) {
//    lightPanelLife-=5;
//    lightPanelTickDuration+=20;
//  }
//  if (rocker3.fallingEdge()) {
//    lightPanelLife+=5;
//    lightPanelTickDuration-=20;
//  }
//  if (rocker3.risingEdge()) {
//    lightPanelLife-=5;
//    lightPanelTickDuration+=20;
//  }
//  if (rocker4.fallingEdge()) {
//    lightPanelLife+=5;
//    lightPanelTickDuration-=20;
//  }
//  if (rocker4.risingEdge()) {
//    lightPanelLife-=5;
//    lightPanelTickDuration+=20;
//  }
//  if (rocker5.fallingEdge()) {
//    lightPanelLife+=5;
//    lightPanelTickDuration-=20;
//  }
//  if (rocker5.risingEdge()) {
//    lightPanelLife-=5;
//    lightPanelTickDuration+=20;
//  }
//  if (rocker6.fallingEdge()) {
//    lightPanelLife+=5;
//    lightPanelTickDuration-=20;
//  }
//  if (rocker6.risingEdge()) {
//    lightPanelLife-=5;
//    lightPanelTickDuration+=20;
//  }
//  if (rocker7.fallingEdge()) {
//    lightPanelLife+=5;
//    lightPanelTickDuration-=20;
//  }
//  if (rocker7.risingEdge()) {
//    lightPanelLife-=5;
//    lightPanelTickDuration+=20;
//  }
//
//  // card slot
//  if (slot1.fallingEdge() || slot2.fallingEdge() || slot3.fallingEdge()) {
//    delay(250);
//    slot1.update();
//    slot2.update();
//    slot3.update();
//    if (!slot1.read() && slot2.read() && slot3.read())
//    {
//      if (allRockersOn() == true)
//      {
//        moviePlaying = true;
//        irsend.sendSony(0x010, 12); // 1
//      }
//      lastButton = 0;
//      resetJewels();
//      Serial.println("Slot mode 100");
//      slotMode = 100;
//    }
//    else if (slot1.read() && !slot2.read() && slot3.read())
//    {
//      if (allRockersOn() == true)
//      {
//        moviePlaying = true;
//        irsend.sendSony(0x810, 12); // 2
//      }
//      lastButton = 0;
//      resetJewels();
//      Serial.println("Slot mode 010");
//      slotMode = 10;
//    }
//    else if (slot1.read() && slot2.read() && !slot3.read())
//    {
//      if (allRockersOn() == true)
//      {
//        moviePlaying = true;
//        irsend.sendSony(0x410, 12); // 3
//      }
//      lastButton = 0;
//      resetJewels();
//      Serial.println("Slot mode 001");
//      slotMode = 1;
//    }
//    else if (!slot1.read() && !slot2.read() && slot3.read())
//    {
//      if (allRockersOn() == true)
//      {
//        moviePlaying = true;
//        irsend.sendSony(0xC10, 12); // 4
//      }
//      lastButton = 0;
//      resetJewels();
//      Serial.println("Slot mode 110");
//      slotMode = 110;
//    }
//    else if (!slot1.read() && slot2.read() && !slot3.read())
//    {
//      if (allRockersOn() == true)
//      {
//        moviePlaying = true;
//        irsend.sendSony(0x210, 12); // 5
//      }
//      lastButton = 0;
//      resetJewels();
//      Serial.println("Slot mode 101");
//      slotMode = 101;
//    }
//    else if (slot1.read() && !slot2.read() && !slot3.read())
//    {
//      if (allRockersOn() == true)
//      {
//        moviePlaying = true;
//        irsend.sendSony(0xA10, 12); // 6
//      }
//      lastButton = 0;
//      resetJewels();
//      Serial.println("Slot mode 011");
//      slotMode = 11;
//    }
//    else if (!slot1.read() && !slot2.read() && !slot3.read())
//    {
//      if (allRockersOn() == true)
//      {
//        moviePlaying = true;
//        irsend.sendSony(0x610, 12); // 7
//      }
//      lastButton = 0;
//      resetJewels();
//      Serial.println("Slot mode 111");
//      slotMode = 111;
//    }
//  }
//
//  // card removed
//  else if (slot1.risingEdge() || slot2.risingEdge() || slot3.risingEdge()) 
//  {
//    Serial.println("Card removed...");
//    if (moviePlaying)
//    {
//      irsend.sendSony(0x070, 12); // Menu/Quit
//      moviePlaying = false;
//    }
//    slotMode = 0;
//    delay(250);
//    slot1.update();
//    slot2.update();
//    slot3.update();
//  }

  // LED FX
  // alarm 1
  if (lastButton == 1)
  {
    if (millis() > ledUpdate)
    {
      ledUpdate = millis() + (1000 / UPDATES_PER_SECOND);
      for (int i = 0; i < 2; i++)
      {
        leds[i].setRGB( ledColor, ledColor, ledColor);
        ledColor++;
      }
    }
      
    currentPalette = alarm1_p; 
    currentBlending = LINEARBLEND;
     
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */

    FillLEDsFromPaletteColors( startIndex);
  }
  
  // alarm 2
  else if (lastButton == 2)
  {
    if (millis() > ledUpdate)
    {
      ledUpdate = millis() + (1000 / UPDATES_PER_SECOND);
      for (int i = 2; i < 4; i++)
      {
        leds[i].setRGB( ledColor, ledColor, ledColor);
        ledColor++;
      }
    }
      
    currentPalette = alarm2_p; 
    currentBlending = LINEARBLEND;
     
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */

    FillLEDsFromPaletteColors( startIndex);
  }
  
  // space sound
  else if (lastButton == 3)
  {
    if (millis() > ledUpdate)
    {
      ledUpdate = millis() + (1000 / UPDATES_PER_SECOND);
      byte ledColor = random8();
      leds[4].setRGB( 0, ledColor, ledColor);
      ledColor = random8();
      leds[5].setRGB( ledColor, ledColor, 0);
    }

    SetupSpaceyPalette();             
    currentBlending = LINEARBLEND;
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    FillLEDsFromPaletteColors( startIndex);
  }
  
  // transporter
  else if (lastButton == 4)
  {
    if (millis() > ledUpdate)
    {
      ledUpdate = millis() + (1000 / UPDATES_PER_SECOND);
      if (ledPhase == 0)
      {
        for (int i = 6; i < 8; i++)
        {
          leds[i].setRGB( ledColor, ledColor, 0);
        }
        for (int i = 30; i < NUM_LEDS; i++)
        {
          leds[i].setRGB( ledColor, ledColor, 0);
        }
        ledColor+=4;
        if (ledColor > 250) ledPhase = 1;
      }
      else if (ledPhase == 1)
      {
        for (int i = 6; i < 8; i++)
        {
          leds[i].setRGB( ledColor, ledColor, 0);
        }
        for (int i = 30; i < NUM_LEDS; i++)
        {
          leds[i].setRGB( ledColor, ledColor, 0);
        }
        ledColor-=4;
        if (ledColor < 5) ledPhase = 0; 
      }
    }
  }
  
  // phaser
  else if (lastButton == 5)
  {
    if (millis() > ledUpdate)
    {
      ledUpdate = millis() + (1000 / UPDATES_PER_SECOND);
      for (int i = 8; i < 10; i++)
      {
        leds[i].setRGB( ledColor, 0, 0);
      }
      for (int i = 30; i < NUM_LEDS; i++)
      {
        leds[i].setRGB( ledColor, 0, 0);
      }
      ledColor-=4;
      if (ledColor < 5) 
      {
        lastButton = 0;
        resetJewels();
      }
    }
  }

  // light panel
  if (millis() > lightPanelTick)
  {
    lightPanelTick = millis() + lightPanelTickDuration;
    for (int i = 12; i < 30; i++)
    {
      leds[i] = CRGB::White;
    }
    randomlyBlink(12, 13);
    randomlyBlink(14, 15);
    randomlyBlink(16, 17);
    randomlyBlink(18, 19);
    randomlyBlink(20, 21);
    randomlyBlink(22, 23);
    randomlyBlink(24, 25);
    randomlyBlink(26, 27);
    randomlyBlink(28, 29);
  }
  
  // stop LED effect at end of sound effect
  if (millis() > effectStopTime && lastButton != 0)
  {
    lastButton = 0;
    resetJewels();
  }
  
  // showtime!
  FastLED.show();
}

//boolean allRockersOn()
//{
//  if (!rocker1.read() && !rocker2.read() && !rocker3.read() && !rocker4.read() && !rocker5.read() && !rocker6.read() && !rocker7.read())
//  {
//    return true;
//  }
//  else return false;
//}

void randomlyBlink(int led1, int led2)
{
  if (random8() < lightPanelLife)
  {
    leds[led1].setRGB( 0, 0, 0);
    leds[led2].setRGB( 0, 0, 0);
  }
}

void buttonPressed(uint8_t buttonNumber, uint32_t duration, uint16_t ups)
{
  if (lastButton > 0) resetJewels();
  if (lastButton == buttonNumber) lastButton = 0;
  else
  {
    lastButton = buttonNumber;
    effectDuration = duration;
    UPDATES_PER_SECOND = ups;
    ledPhase = 0;
    effectStopTime = millis() + effectDuration;
  }
}

void resetJewels()
{
  for( int i = 0; i < 12; i++) {
    leds[i] = CRGB::White;
  }
  for( int i = 30; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
  effectStopTime = 0;
  FastLED.show();
  Serial.print("Jewels reset: ");
  Serial.println(lightShow);
}

// LED FX CODE
void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  
  for( int i = 30; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}


// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
  for( int i = 0; i < 16; i++) {
    currentPalette[i] = CHSV( random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;

}

void SetupBlackAndRedStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::Red;
  currentPalette[4] = CRGB::Red;
  currentPalette[8] = CRGB::Red;
  currentPalette[12] = CRGB::Red;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV( HUE_PURPLE, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;
  
  currentPalette = CRGBPalette16( 
    green,  green,  black,  black,
    purple, purple, black,  black,
    green,  green,  black,  black,
    purple, purple, black,  black );
}

void SetupSpaceyPalette()
{
  CRGB cyan = CRGB::Cyan;
  CRGB yellow  = CRGB::Yellow;
  CRGB black  = CRGB::Black;
  
  currentPalette = CRGBPalette16( 
    yellow,  yellow,  black,  black,
    cyan, cyan, black,  black,
    yellow,  yellow,  black,  black,
    cyan, cyan, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more 
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
  CRGB::Red,
  CRGB::Gray, // 'white' is too bright compared to red and blue
  CRGB::Blue,
  CRGB::Black,

  CRGB::Red,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Black,

  CRGB::Red,
  CRGB::Red,
  CRGB::Gray,
  CRGB::Gray,
  CRGB::Blue,
  CRGB::Blue,
  CRGB::Black,
  CRGB::Black
};

// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more 
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 alarm1_p PROGMEM =
{
  CRGB::Yellow,
  CRGB::Gray, // 'white' is too bright compared to red and blue
  CRGB::Yellow,
  CRGB::Black,

  CRGB::Yellow,
  CRGB::Gray,
  CRGB::Yellow,
  CRGB::Black,

  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Gray,
  CRGB::Gray,
  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Black,
  CRGB::Black
};

const TProgmemPalette16 alarm2_p PROGMEM =
{
  CRGB::Red,
  CRGB::Gray, // 'white' is too bright compared to red and blue
  CRGB::Red,
  CRGB::Black,

  CRGB::Red,
  CRGB::Gray,
  CRGB::Red,
  CRGB::Black,

  CRGB::Red,
  CRGB::Red,
  CRGB::Gray,
  CRGB::Gray,
  CRGB::Red,
  CRGB::Red,
  CRGB::Black,
  CRGB::Black
};

// Additionl notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes. 
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette 
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact 
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved 
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.

//      irsend.sendSony(0x070, 12); // Menu/Quit
//      irsend.sendSony(0xA70, 12); // OK/Pause
//      irsend.sendSony(0x1D0, 12); // TV-RETURN/Kill omxplayer
//      irsend.sendSony(0x010, 12); // 1
//      irsend.sendSony(0x810, 12); // 2
//      irsend.sendSony(0x410, 12); // 3
//      irsend.sendSony(0xC10, 12); // 4
//      irsend.sendSony(0x210, 12); // 5
//      irsend.sendSony(0xA10, 12); // 6
//      irsend.sendSony(0x610, 12); // 7
//      irsend.sendSony(0x738, 12); // right
//      irsend.sendSony(0x338, 12); // left
