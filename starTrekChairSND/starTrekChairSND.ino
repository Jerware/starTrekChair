// starTrekChairSND release 20150525
//
// This code adds sound to Adam Savage's Star Trek captain's chair
// featured in the following Tested video:
//
// https://youtu.be/jaVi06DaTk0
//
// It compiles on Teensy 3.1, requires a Teensy Audio Board, and is designed to 
// work in parallel with the LED lamp code. Both sources are available in this
// GITHUB repository:
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

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Bounce.h>
#include <Entropy.h>
#include <FastLED.h>

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
  Entropy.Initialize();

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
  if (button1.fallingEdge()) {
    if (lastButton == 1 && soundButtons.isPlaying()) {
      soundButtons.stop();
      lastButton = 0;
    }
    else
    {
      lastButton = 1;
      soundButtons.play("01.wav");
      Serial.print("Playing 01.wav");
    }
  }

  if (button2.fallingEdge()) {
    if (lastButton == 2 && soundButtons.isPlaying()) {
      soundButtons.stop();
      lastButton = 0;
    }
    else
    {
      lastButton = 2;
      soundButtons.play("02.wav");
      Serial.print("Playing 02.wav");
    }
  }

  if (button3.fallingEdge()) {
    if (lastButton == 3 && soundButtons.isPlaying()) {
      soundButtons.stop();
      lastButton = 0;
    }
    else
    {
      lastButton = 3;
      soundButtons.play("03.wav");
      Serial.print("Playing 03.wav");
    }
  }

  if (button4.fallingEdge()) {
    if (lastButton == 4 && soundButtons.isPlaying()) {
      soundButtons.stop();
      lastButton = 0;
    }
    else
    {
      if (!moviePlaying)
      {
        lastButton = 4;
        soundButtons.play("04.wav");
        Serial.print("Playing 04.wav");
      }
    }
  }

  if (button5.fallingEdge()) {
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
    }
  }

  if (button6.fallingEdge()) {
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
        Serial.print("Playing 06.wav");
      }
    }
  }

  if (button7.fallingEdge()) {
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
        Serial.print("Playing 07.wav");
      }
    }
  }

  // intercom
  if (buttonIntercom.fallingEdge()) {
    soundIntercom.play("08_01.wav");
    Serial.println("Playing 08_01.wav");
  }
  if (buttonIntercom.risingEdge()) {
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
  if (rocker1.fallingEdge()) {
    soundRocker.play("09_01.wav");
    Serial.println("Playing 09_01.wav");
  }
  if (rocker1.risingEdge()) {
    soundRocker.play("09_02.wav");
    Serial.println("Playing 09_02.wav");
  }
  if (rocker2.fallingEdge()) {
    soundRocker.play("10_01.wav");
    Serial.println("Playing 10_01.wav");
  }
  if (rocker2.risingEdge()) {
    soundRocker.play("10_02.wav");
    Serial.println("Playing 10_02.wav");
  }
  if (rocker3.fallingEdge()) {
    soundRocker.play("11_01.wav");
    Serial.println("Playing 11_01.wav");
  }
  if (rocker3.risingEdge()) {
    soundRocker.play("11_02.wav");
    Serial.println("Playing 11_02.wav");
  }
  if (rocker4.fallingEdge()) {
    soundRocker.play("12_01.wav");
    Serial.println("Playing 12_01.wav");
  }
  if (rocker4.risingEdge()) {
    soundRocker.play("12_02.wav");
    Serial.println("Playing 12_02.wav");
  }
  if (rocker5.fallingEdge()) {
    soundRocker.play("13_01.wav");
    Serial.println("Playing 13_01.wav");
  }
  if (rocker5.risingEdge()) {
    soundRocker.play("13_02.wav");
    Serial.println("Playing 13_02.wav");
  }
  if (rocker6.fallingEdge()) {
    soundRocker.play("14_01.wav");
    Serial.println("Playing 14_01.wav");
  }
  if (rocker6.risingEdge()) {
    soundRocker.play("14_02.wav");
    Serial.println("Playing 14_02.wav");
  }
  if (rocker7.fallingEdge()) {
    soundBackground.play("15.wav");
    Serial.println("Playing 15.wav");
  }
  if (rocker7.risingEdge()) {
    soundBackground.stop();
    Serial.println("Stopping soundBackground");
  }

  // card slot
  if (slot1.fallingEdge() || slot2.fallingEdge() || slot3.fallingEdge()) 
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
  else if (slot1.risingEdge() || slot2.risingEdge() || slot3.risingEdge()) 
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


