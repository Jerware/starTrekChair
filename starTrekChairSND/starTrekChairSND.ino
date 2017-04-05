// starTrekChairSND release 20160319
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

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Bounce2.h>
#include <Bounce2mcp.h>
#include <Entropy.h>
#include <FastLED.h>
#include "Adafruit_MCP23017.h"

#define HWSERIAL Serial1

Adafruit_MCP23017 mcp;

byte lastButton;
boolean switchClosed;
int slotMode;
boolean moviePlaying;

// GUItool: begin automatically generated code
AudioPlaySdWav           soundButtons;       //xy=223,156
AudioPlaySdWav           soundRocker;       //xy=225,228
AudioPlaySdWav           soundBackground;       //xy=225,395
AudioPlaySdWav           soundIntercom;       //xy=230,489
AudioMixer4              mixer3;         //xy=528,500
AudioMixer4              mixer1;         //xy=529,307
AudioMixer4              mixer2;         //xy=724,398
AudioFilterBiquad        intercomFilter;        //xy=714,554
AudioOutputI2S           i2s1;           //xy=892,494
AudioConnection          patchCord1(soundButtons, 0, mixer1, 0);
AudioConnection          patchCord2(soundButtons, 1, mixer1, 1);
AudioConnection          patchCord3(soundRocker, 0, mixer1, 2);
AudioConnection          patchCord4(soundRocker, 1, mixer1, 3);
AudioConnection          patchCord5(soundBackground, 0, mixer2, 1);
AudioConnection          patchCord6(soundBackground, 1, mixer2, 2);
AudioConnection          patchCord7(soundIntercom, 0, mixer3, 0);
AudioConnection          patchCord8(soundIntercom, 1, mixer3, 1);
AudioConnection          patchCord9(mixer3, intercomFilter);
AudioConnection          patchCord10(mixer1, 0, mixer2, 0);
AudioConnection          patchCord11(intercomFilter, 0, i2s1, 1);
AudioConnection          patchCord12(mixer2, 0, i2s1, 0);
// GUItool: end automatically generated code

// Create an object to control the audio shield.
AudioControlSGTL5000 audioShield;

// Bounce Teensy Pins
Bounce button1 = Bounce();
Bounce button2 = Bounce();
Bounce button3 = Bounce();
Bounce button4 = Bounce();
Bounce button5 = Bounce();
Bounce button6 = Bounce();
Bounce button7 = Bounce();

// Debounce MCP23017 Pins
BounceMcp buttonIntercom = BounceMcp();
BounceMcp rocker1 = BounceMcp();
BounceMcp rocker2 = BounceMcp();
BounceMcp rocker3 = BounceMcp();
BounceMcp rocker4 = BounceMcp();
BounceMcp rocker5 = BounceMcp();
BounceMcp rocker6 = BounceMcp();
BounceMcp rocker7 = BounceMcp();
BounceMcp slot1 = BounceMcp();
BounceMcp slot2 = BounceMcp();
BounceMcp slot3 = BounceMcp();
BounceMcp other1 = BounceMcp();


void setup() {
  Serial.begin(57600);
  HWSERIAL.begin(38400);
  delay(1000);
  Serial.println("Hello there!");

  // Start I²C bus as master
  Wire.begin();

  mcp.begin();      // use default address 0

  Entropy.Initialize();

  // Configure the pushbutton pins for pullups.
  // Each button should connect from the pin to GND.
  // Teensy Pins
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);
  button1.attach(2);
  button2.attach(3);
  button3.attach(4);
  button4.attach(5);
  button5.attach(16);
  button6.attach(20);
  button7.attach(21);
  button1.interval(35);
  button2.interval(35);
  button3.interval(35);
  button4.interval(35);
  button5.interval(35);
  button6.interval(35);
  button7.interval(35);

  // MCP23017 Pins
  for (int p=0; p<16; p++)
  {
    mcp.pinMode(p, INPUT);
    mcp.pullUp(p, HIGH);  // turn on a 100K pullup internally
  }
  buttonIntercom.attach(mcp, 0, 35);
  rocker1.attach(mcp, 1, 35);
  rocker2.attach(mcp, 2, 35);
  rocker3.attach(mcp, 3, 35);
  rocker4.attach(mcp, 4, 35);
  rocker5.attach(mcp, 5, 35);
  rocker6.attach(mcp, 6, 35);
  rocker7.attach(mcp, 7, 35);
  slot1.attach(mcp, 8, 35);
  slot2.attach(mcp, 9, 35);
  slot3.attach(mcp, 10, 35);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(64);

  // turn on the output
  audioShield.enable();
  audioShield.volume(.8);
  delay(1000);
  intercomFilter.setHighpass(0, 800, 0.707);

  // by default the Teensy 3.1 DAC uses 3.3Vp-p output
  // if your 3.3V power has noise, switching to the
  // internal 1.2V reference can give you a clean signal
  //dac.analogReference(INTERNAL);

  // reduce the gain on mixer channels, so more than 1
  // sound can play simultaneously without clipping
  float gainLevel = .4;
  mixer2.gain(0, gainLevel);
  mixer2.gain(1, gainLevel);
  mixer2.gain(2, gainLevel);
  mixer2.gain(3, gainLevel);
  mixer3.gain(0, gainLevel);
  mixer3.gain(1, gainLevel);
  mixer3.gain(2, gainLevel);
  mixer3.gain(3, gainLevel);

  SPI.setMOSI(7);
  SPI.setSCK(14);
  if (!(SD.begin(10))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

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

  // When the buttons are pressed, just start a sound playing.
  // The audio library will play each sound through the mixers
  // so combinations of banks can play simultaneously.

  // main buttons
  if (button1.fell()) {
    Serial.println("PRESSED: Button 1");
    if (lastButton == 1 && soundButtons.isPlaying()) {
      soundButtons.stop();
      lastButton = 0;
      lightShow(0);
    }
    else
    {
      lastButton = 1;
      soundButtons.play("01.wav");
      Serial.println("Playing 01.wav");
      lightShow(1);
    }
  }

  if (button2.fell()) {
    Serial.println("PRESSED: Button 2");
    if (lastButton == 2 && soundButtons.isPlaying()) {
      soundButtons.stop();
      lastButton = 0;
      lightShow(0);
    }
    else
    {
      lastButton = 2;
      soundButtons.play("02.wav");
      Serial.println("Playing 02.wav");
      lightShow(2);
    }
  }

  if (button3.fell()) {
    Serial.println("PRESSED: Button 3");
    if (lastButton == 3 && soundButtons.isPlaying()) {
      soundButtons.stop();
      lastButton = 0;
      lightShow(0);
    }
    else
    {
      lastButton = 3;
      soundButtons.play("03.wav");
      Serial.println("Playing 03.wav");
      lightShow(3);
    }
  }

  if (button4.fell()) {
    Serial.println("PRESSED: Button 4");
    if (lastButton == 4 && soundButtons.isPlaying()) {
      soundButtons.stop();
      lastButton = 0;
      lightShow(0);
    }
    else
    {
      if (!moviePlaying)
      {
        lastButton = 4;
        soundButtons.play("04.wav");
        Serial.println("Playing 04.wav");
        lightShow(4);
      }
    }
  }

  if (button5.fell()) {
    Serial.println("PRESSED: Button 5");
    if (!moviePlaying)
    {
      lastButton = 5;
      byte random_byte = random8(1,8); // returns a value from 1 to 7
      char fileNumber[2];
      char fileName[13];
      strcpy_P(fileName, PSTR("05_0"));
      itoa(random_byte, fileNumber, 10);
      strcat(fileName, fileNumber);
      strcat(fileName, ".wav");
      soundButtons.play(fileName);
      Serial.print("Playing ");
      Serial.println(fileName);
      lightShow(5);
    }
  }

  if (button6.fell()) {
    Serial.println("PRESSED: Button 6");
    lightShow(6);
    if (lastButton == 6 && soundBackground.isPlaying()) {
      soundBackground.stop();
      lastButton = 0;
    }
    else
    {
      if (!moviePlaying)
      {
        lastButton = 6;
        soundBackground.play("06.wav");
        Serial.println("Playing 06.wav");
      }
    }
  }
  
  else if (button6.rose())
  {
    lightShow(0);
  }

  if (button7.fell()) {
    Serial.println("PRESSED: Button 7");
    lightShow(7);
    if (lastButton == 7 && soundBackground.isPlaying()) {
      soundBackground.stop();
      lastButton = 0;
    }
    else
    {
      if (!moviePlaying)
      {
        lastButton = 7;
        soundBackground.play("07.wav");
        Serial.println("Playing 07.wav");
      }
    }
  }
  
  else if (button7.rose())
  {
    lightShow(0);
  }

  // intercom
  if (buttonIntercom.fell()) {
    Serial.println("PRESSED: Intercom");
    soundIntercom.play("08_01.wav");
    Serial.println("Playing 08_01.wav");
    lightShow(8);
  }
  if (buttonIntercom.rose()) {
    lightShow(0);
    byte random_byte = Entropy.random(1,20); // returns a value from 1 to 19
    char fileNumber[3];
    char fileName[13];
    if (random_byte < 10) strcpy_P(fileName, PSTR("08_up_0"));
    else strcpy_P(fileName, PSTR("08_up_"));
    itoa(random_byte, fileNumber, 10);
    strcat(fileName, fileNumber);
    strcat(fileName, ".wav");
    soundIntercom.play(fileName);
    Serial.print("Playing ");
    Serial.println(fileName);
  }

  // rocker switches
  if (rocker1.fell()) {
    Serial.println("PRESSED: Rocker 1");
    lightShow(111);
    soundRocker.play("09_01.wav");
    Serial.println("Playing 09_01.wav");
  }
  if (rocker1.rose()) {
    lightShow(110);
    soundRocker.play("09_02.wav");
    Serial.println("Playing 09_02.wav");
  }
  if (rocker2.fell()) {
    Serial.println("PRESSED: Rocker 2");
    lightShow(121);
    soundRocker.play("10_01.wav");
    Serial.println("Playing 10_01.wav");
  }
  if (rocker2.rose()) {
    lightShow(120);
    soundRocker.play("10_02.wav");
    Serial.println("Playing 10_02.wav");
  }
  if (rocker3.fell()) {
    Serial.println("PRESSED: Rocker 3");
    lightShow(131);
    soundRocker.play("11_01.wav");
    Serial.println("Playing 11_01.wav");
  }
  if (rocker3.rose()) {
    lightShow(130);
    soundRocker.play("11_02.wav");
    Serial.println("Playing 11_02.wav");
  }
  if (rocker4.fell()) {
    Serial.println("PRESSED: Rocker 4");
    lightShow(141);
    soundRocker.play("12_01.wav");
    Serial.println("Playing 12_01.wav");
  }
  if (rocker4.rose()) {
    lightShow(140);
    soundRocker.play("12_02.wav");
    Serial.println("Playing 12_02.wav");
  }
  if (rocker5.fell()) {
    Serial.println("PRESSED: Rocker 5");
    lightShow(151);
    soundRocker.play("13_01.wav");
    Serial.println("Playing 13_01.wav");
  }
  if (rocker5.rose()) {
    lightShow(150);
    soundRocker.play("13_02.wav");
    Serial.println("Playing 13_02.wav");
  }
  if (rocker6.fell()) {
    Serial.println("PRESSED: Rocker 6");
    lightShow(161);
    soundRocker.play("14_01.wav");
    Serial.println("Playing 14_01.wav");
  }
  if (rocker6.rose()) {
    lightShow(160);
    soundRocker.play("14_02.wav");
    Serial.println("Playing 14_02.wav");
  }
  if (rocker7.fell()) {
    Serial.println("PRESSED: Rocker 7");
    lightShow(171);
    soundBackground.play("15.wav");
    Serial.println("Playing 15.wav");
  }
  if (rocker7.rose()) {
    lightShow(170);
    soundBackground.stop();
    Serial.println("Stopping soundBackground");
  }

  // card slot

  if (slot1.fell()) Serial.println("PRESSED: Slot 1");
  if (slot2.fell()) Serial.println("PRESSED: Slot 2");
  if (slot3.fell()) Serial.println("PRESSED: Slot 3");


  if (slot1.fell() || slot2.fell() || slot3.fell()) 
  {
    Serial.println("Card inserted...");
    soundButtons.stop();
    soundBackground.stop();
    delay(250);
    slot1.update();
    slot2.update();
    slot3.update();
    if (!slot1.read() && slot2.read() && slot3.read())
    {
      soundButtons.play("key1.wav");
      delay(1000);
      if (!allRockersOn()) soundBackground.play("song1.wav");
      else
      {
        moviePlaying = true;
        soundBackground.stop();
      }
      Serial.println("Slot mode 100");
      slotMode = 100;
    }
    else if (slot1.read() && !slot2.read() && slot3.read())
    {
      soundButtons.play("key2.wav");
      delay(1000);
      if (!allRockersOn()) soundBackground.play("song2.wav");
      else
      {
        moviePlaying = true;
        soundBackground.stop();
      }
      Serial.println("Slot mode 010");
      slotMode = 10;
    }
    else if (slot1.read() && slot2.read() && !slot3.read())
    {
      soundButtons.play("key3.wav");
      delay(1000);
      if (!allRockersOn()) soundBackground.play("song3.wav");
      else
      {
        moviePlaying = true;
        soundBackground.stop();
      }
      Serial.println("Slot mode 001");
      slotMode = 1;
    }
    else if (!slot1.read() && !slot2.read() && slot3.read())
    {
      soundButtons.play("key4.wav");
      delay(1000);
      if (!allRockersOn()) soundBackground.play("song4.wav");
      else
      {
        moviePlaying = true;
        soundBackground.stop();
      }
      Serial.println("Slot mode 110");
      slotMode = 110;
    }
    else if (!slot1.read() && slot2.read() && !slot3.read())
    {
      soundButtons.play("key5.wav");
      delay(1000);
      if (!allRockersOn()) soundBackground.play("song5.wav");
      else moviePlaying = true;
      Serial.println("Slot mode 101");
      slotMode = 101;
    }
    else if (slot1.read() && !slot2.read() && !slot3.read())
    {
      soundButtons.play("key6.wav");
      delay(1000);
      if (!allRockersOn()) soundBackground.play("song6.wav");
      else
      {
        moviePlaying = true;
        soundBackground.stop();
      }
      Serial.println("Slot mode 011");
      slotMode = 11;
    }
    else if (!slot1.read() && !slot2.read() && !slot3.read())
    {
      soundButtons.play("key7.wav");
      delay(1000);
      if (!allRockersOn()) soundBackground.play("song7.wav");
      else
      {
        moviePlaying = true;
        soundBackground.stop();
      }
      Serial.println("Slot mode 111");
      slotMode = 111;
    }
  }

  // card removed
  else if (slot1.rose() || slot2.rose() || slot3.rose()) 
  {
    Serial.println("Card removed...");
    if (moviePlaying) moviePlaying = false;
    soundBackground.stop();
    soundButtons.play("key0.wav");
    slotMode = 0;
    delay(250);
    slot1.update();
    slot2.update();
    slot3.update();
  }


  // volume control
  //  float vol = analogRead(15);
  //  vol = vol / 1024;
  //  audioShield.volume(vol);
}

boolean allRockersOn()
{
  if (!rocker1.read() && !rocker2.read() && !rocker3.read() && !rocker4.read() && !rocker5.read() && !rocker6.read() && !rocker7.read())
  {
    return true;
  }
  else return false;
}

void lightShow(int seq)
{
  Serial.print("Sending: ");
  Serial.println(seq);
  HWSERIAL.println(seq, DEC);
}


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


