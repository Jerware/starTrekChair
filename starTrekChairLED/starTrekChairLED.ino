// starTrekChairLED release 20150525
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
// Jeremy Williams wrote the code and designed the sound/LED hardware. Please
// note that it is still very much in the prototype phase, and you will find room for 
// improvement. For instance, the two code bases are independent and unaware
// of each other. (Each switch is wired to both boards.) A revision might employ
// i2c and a master/slave relationship between the boards.
//
// As always, thanks to the amazing library developers for keeping Arduino coding
// from becoming a chore. You are all giants. I'll monitor the feedback on Github
// from time to time but this code should be considered unsupported. #LLAP.

#include <Bounce.h>
#include <FastLED.h>
#include <IRremote.h>

IRsend irsend;

byte lastButton;
boolean switchClosed;
int slotMode;
boolean moviePlaying;

// LED SETUP
#define LED_PIN     17
#define NUM_LEDS    318
#define BRIGHTNESS  255
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
int UPDATES_PER_SECOND = 100;
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

// Bounce objects to read pushbuttons (20 buttons)
Bounce button1 = Bounce(0, 35);  // 5 ms debounce time
Bounce button2 = Bounce(1, 35);
Bounce button3 = Bounce(2, 35);
Bounce button4 = Bounce(3, 35);
Bounce button5 = Bounce(4, 35);
Bounce button6 = Bounce(8, 35);
Bounce button7 = Bounce(16, 35);
Bounce buttonIntercom = Bounce(20, 35);
Bounce rocker1 = Bounce(21, 35);
Bounce rocker2 = Bounce(24, 35);
Bounce rocker3 = Bounce(25, 35);
Bounce rocker4 = Bounce(26, 35);
Bounce rocker5 = Bounce(27, 35);
Bounce rocker6 = Bounce(28, 35);
Bounce rocker7 = Bounce(29, 35);
Bounce slot1 = Bounce(30, 35);
Bounce slot2 = Bounce(31, 35);
Bounce slot3 = Bounce(32, 35);
Bounce other1 = Bounce(33, 35);

void setup() {
  Serial.begin(57600);
  delay( 1000 ); // power-up safety delay
  Serial.println("Hello there!");
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  currentPalette = RainbowColors_p;
  currentBlending = BLEND;
  
  resetJewels();
  leds[10] = CRGB::White;
  leds[11] = CRGB::White;
  
  // Configure the pushbutton pins for pullups.
  // Each button should connect from the pin to GND.
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  pinMode(24, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  pinMode(28, INPUT_PULLUP);
  pinMode(29, INPUT_PULLUP);
  pinMode(30, INPUT_PULLUP);
  pinMode(31, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);

  // Store the button states
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  button5.update();
  button6.update();
  button7.update();
  buttonIntercom.update();
  rocker1.update();
  rocker2.update();
  rocker3.update();
  rocker4.update();
  rocker5.update();
  rocker6.update();
  rocker7.update();
  slot1.update();
  slot2.update();
  slot3.update();
  other1.update();
}

void loop() {
  // Update all the button objects
  button1.update();
  button2.update();
  button3.update();
  button4.update();
  button5.update();
  button6.update();
  button7.update();
  buttonIntercom.update();
  rocker1.update();
  rocker2.update();
  rocker3.update();
  rocker4.update();
  rocker5.update();
  rocker6.update();
  rocker7.update();
  slot1.update();
  slot2.update();
  slot3.update();
  other1.update();

  // main buttons
  if (button1.fallingEdge()){
    ledColor = 0;
    buttonPressed(1, 57000, 90);  // button, duration in ms, updates per second
  }
  if (button2.fallingEdge()) {
    ledColor = 0;
    buttonPressed(2, 33000, 90);
  }
  if (button3.fallingEdge()) {
    ledColor = 0;
    buttonPressed(3, 64000, 15);
  }
  if (button4.fallingEdge()) {
    if (moviePlaying)
    {
      irsend.sendSony(0xA70, 12); // OK/Pause
    }
    ledColor = 0;
    buttonPressed(4, 11000, 12);
  }
  if (button5.fallingEdge()) {
    if (moviePlaying)
    {
      irsend.sendSony(0x338, 12); // left
    }
    lastButton = 0;
    ledColor = 255;
    buttonPressed(5, 10000, 100);
  }
  if (button6.fallingEdge()) {
    if (moviePlaying)
    {
      irsend.sendSony(0x1D0, 12); // TV-RETURN/Kill omxplayer
    }
    leds[8] = CRGB::Red;
    leds[9] = CRGB::Red;
  }
  if (button6.risingEdge()) {
    leds[8] = CRGB::White;
    leds[9] = CRGB::White;
  }
  if (button7.fallingEdge()) {
    if (moviePlaying)
    {
      irsend.sendSony(0x738, 12); // right
    }
    leds[4] = CRGB::Black;
    leds[5] = CRGB::Black;
  }
  if (button7.risingEdge()) {
    leds[4] = CRGB::White;
    leds[5] = CRGB::White;
  }
  
  // intercom
  if (buttonIntercom.fallingEdge()) {
    leds[10] = CRGB::Yellow;
    leds[11] = CRGB::Yellow;
  }
  if (buttonIntercom.risingEdge()) {
    leds[10] = CRGB::White;
    leds[11] = CRGB::White;
  }

  // rocker switches
  if (rocker1.fallingEdge()) {
    lightPanelLife+=5;
    lightPanelTickDuration-=20;
  }
  if (rocker1.risingEdge()) {
    lightPanelLife-=5;
    lightPanelTickDuration+=20;
  }
  if (rocker2.fallingEdge()) {
    lightPanelLife+=5;
    lightPanelTickDuration-=20;
  }
  if (rocker2.risingEdge()) {
    lightPanelLife-=5;
    lightPanelTickDuration+=20;
  }
  if (rocker3.fallingEdge()) {
    lightPanelLife+=5;
    lightPanelTickDuration-=20;
  }
  if (rocker3.risingEdge()) {
    lightPanelLife-=5;
    lightPanelTickDuration+=20;
  }
  if (rocker4.fallingEdge()) {
    lightPanelLife+=5;
    lightPanelTickDuration-=20;
  }
  if (rocker4.risingEdge()) {
    lightPanelLife-=5;
    lightPanelTickDuration+=20;
  }
  if (rocker5.fallingEdge()) {
    lightPanelLife+=5;
    lightPanelTickDuration-=20;
  }
  if (rocker5.risingEdge()) {
    lightPanelLife-=5;
    lightPanelTickDuration+=20;
  }
  if (rocker6.fallingEdge()) {
    lightPanelLife+=5;
    lightPanelTickDuration-=20;
  }
  if (rocker6.risingEdge()) {
    lightPanelLife-=5;
    lightPanelTickDuration+=20;
  }
  if (rocker7.fallingEdge()) {
    lightPanelLife+=5;
    lightPanelTickDuration-=20;
  }
  if (rocker7.risingEdge()) {
    lightPanelLife-=5;
    lightPanelTickDuration+=20;
  }

  // card slot
  if (slot1.fallingEdge() || slot2.fallingEdge() || slot3.fallingEdge()) {
    delay(250);
    slot1.update();
    slot2.update();
    slot3.update();
    if (!slot1.read() && slot2.read() && slot3.read())
    {
      if (allRockersOn() == true)
      {
        moviePlaying = true;
        irsend.sendSony(0x010, 12); // 1
      }
      lastButton = 0;
      resetJewels();
      Serial.println("Slot mode 100");
      slotMode = 100;
    }
    else if (slot1.read() && !slot2.read() && slot3.read())
    {
      if (allRockersOn() == true)
      {
        moviePlaying = true;
        irsend.sendSony(0x810, 12); // 2
      }
      lastButton = 0;
      resetJewels();
      Serial.println("Slot mode 010");
      slotMode = 10;
    }
    else if (slot1.read() && slot2.read() && !slot3.read())
    {
      if (allRockersOn() == true)
      {
        moviePlaying = true;
        irsend.sendSony(0x410, 12); // 3
      }
      lastButton = 0;
      resetJewels();
      Serial.println("Slot mode 001");
      slotMode = 1;
    }
    else if (!slot1.read() && !slot2.read() && slot3.read())
    {
      if (allRockersOn() == true)
      {
        moviePlaying = true;
        irsend.sendSony(0xC10, 12); // 4
      }
      lastButton = 0;
      resetJewels();
      Serial.println("Slot mode 110");
      slotMode = 110;
    }
    else if (!slot1.read() && slot2.read() && !slot3.read())
    {
      if (allRockersOn() == true)
      {
        moviePlaying = true;
        irsend.sendSony(0x210, 12); // 5
      }
      lastButton = 0;
      resetJewels();
      Serial.println("Slot mode 101");
      slotMode = 101;
    }
    else if (slot1.read() && !slot2.read() && !slot3.read())
    {
      if (allRockersOn() == true)
      {
        moviePlaying = true;
        irsend.sendSony(0xA10, 12); // 6
      }
      lastButton = 0;
      resetJewels();
      Serial.println("Slot mode 011");
      slotMode = 11;
    }
    else if (!slot1.read() && !slot2.read() && !slot3.read())
    {
      if (allRockersOn() == true)
      {
        moviePlaying = true;
        irsend.sendSony(0x610, 12); // 7
      }
      lastButton = 0;
      resetJewels();
      Serial.println("Slot mode 111");
      slotMode = 111;
    }
  }

  // card removed
  else if (slot1.risingEdge() || slot2.risingEdge() || slot3.risingEdge()) 
  {
    Serial.println("Card removed...");
    if (moviePlaying)
    {
      irsend.sendSony(0x070, 12); // Menu/Quit
      moviePlaying = false;
    }
    slotMode = 0;
    delay(250);
    slot1.update();
    slot2.update();
    slot3.update();
  }

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
    currentBlending = BLEND;
     
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
    currentBlending = BLEND;
     
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
    currentBlending = BLEND;
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

boolean allRockersOn()
{
  if (!rocker1.read() && !rocker2.read() && !rocker3.read() && !rocker4.read() && !rocker5.read() && !rocker6.read() && !rocker7.read())
  {
    return true;
  }
  else return false;
}

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
  for( int i = 0; i < 10; i++) {
    leds[i] = CRGB::White;
  }
  for( int i = 30; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
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

void ChangePalettePeriodically()
{
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;
  
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
    if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = BLEND; }
    if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
    if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = BLEND; }
    if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = BLEND; }
    if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = BLEND; }
    if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
    if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = BLEND; }
    if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = BLEND; }
    if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = BLEND; }
    if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
    if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = BLEND; }
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
