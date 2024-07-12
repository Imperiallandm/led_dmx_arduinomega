//Steve note - this works


// dmx values arch 1 501 - 505 incl arch 2 506 - 510
// THIS IS THE REAR ARCH

// DMX Receiver/Fixture Example
// Target Hardware:  Arduino Mega
//
// Receives DMX data for red, green, and blue color levels as
// well as a master brightness level, and controls an entire strip
// of 60 WS2812B RGB LEDs as a single light fixture all with the
// same color and brightness settings.
//
// Required library:
//    DMXSerial        install from library manager or https://github.com/mathertel/DMXSerial
//    ws2812.h         custom LED controller for better compatibility with DMX library
//
// Gadget Reboot
// https://www.youtube.com/gadgetreboot

#include <DMXSerial.h>

#include <FastLED.h>

FASTLED_USING_NAMESPACE

#include "ws2812.h"             // custom LED controller for compatibility
#define dmxModePin        2     // configures RS485 for receive mode
#define dmxBaseCh        506 //10     // DMX channel 10 is the start channel. red = 10, grn = 11, blu = 12, bright = 14
#define NUM_LEDS        524    // 60pm LEDs numbered [0..59]


#define DATA_PIN    10
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

#define BRIGHTNESS         250
#define FRAMES_PER_SECOND  120 //120

byte maxBrightness     = 250;    // brightness range [off..on] = [0..255], keep dim for less current draw
byte redLevel,                  // store the received channel level control data
     grnLevel,
     bluLevel,
     brightness,
     glitterLevel,
     glitterLevelIn;
//   cylonLevel;
byte redCh             =   0;   // DMX channel offsets from base channel
byte grnCh             =   1;
byte bluCh             =   2;
byte brightnessCh      =   3;
byte glitterCh         =   4;
//byte cylonCh           =   5;

uint16_t ledRgbData[NUM_LEDS * 3];  // array to store RGB data for all 60 LEDs (180 bytes total)



void setup () {
  DMXSerial.init(DMXProbe, dmxModePin);   // initialize DMX receiver in manual mode
  DMXSerial.maxChannel(dmxBaseCh + 6);    // limit the number of DMX channels to read in

  setupNeopixel();                        // configure LED strip for pin 12 (hard coded in ws2812.h)
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

}

uint8_t gHue = 0; // rotating "base color" used by many of the patterns



void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(250);
  }
}

void loop() {

  EVERY_N_MILLISECONDS( 10 ) {
    gHue += 5; // slowly cycle the "base color" through the rainbow
  }





  if (DMXSerial.receive()) {
    redLevel = DMXSerial.read(dmxBaseCh + redCh);
    grnLevel = DMXSerial.read(dmxBaseCh + grnCh);
    bluLevel = DMXSerial.read(dmxBaseCh + bluCh);
    glitterLevel = DMXSerial.read(dmxBaseCh + glitterCh);
    //    cylonLevel = DMXSerial.read(dmxBaseCh + cylonCh);
    brightness = DMXSerial.read(dmxBaseCh + brightnessCh);
    brightness = map(brightness, 1, 255, 0, maxBrightness);

    // scale the RGB color levels based on the brightness level requested
    redLevel = float(redLevel) * (float(brightness) / float(maxBrightness));
    grnLevel = float(grnLevel) * (float(brightness) / float(maxBrightness));
    bluLevel = float(bluLevel) * (float(brightness) / float(maxBrightness));


    glitterLevelIn = int(glitterLevel);


    if (glitterLevelIn >= 50 && glitterLevelIn <= 99 ) {



      pride();
      //rainbow();
      FastLED.setBrightness(brightness);
      FastLED.show();
      FastLED.delay(1000 / FRAMES_PER_SECOND);



    }

    if (glitterLevelIn >= 100 && glitterLevelIn <= 149 ) {



      rainbow();
      addGlitter(80);
      FastLED.setBrightness(brightness);
      FastLED.show();
      FastLED.delay(1000 / FRAMES_PER_SECOND);



    }
    if (glitterLevelIn >= 150 && glitterLevelIn <= 199 ) {



      confetti();
      FastLED.setBrightness(brightness);
      FastLED.show();
      FastLED.delay(1000 / FRAMES_PER_SECOND);



    }


    if (glitterLevelIn >= 200 && glitterLevelIn <= 249 ) {


      //pride();
      cylon();
      FastLED.setBrightness(brightness);
      FastLED.show();
      FastLED.delay(1000 / FRAMES_PER_SECOND);


    }

    if (glitterLevelIn >= 250 && glitterLevelIn <= 255 ) {


      arch();
      FastLED.setBrightness(brightness);
      FastLED.show();
      FastLED.delay(1000 / FRAMES_PER_SECOND);

    }

    if (glitterLevel <= 49) {
      digitalWrite(11, LOW);
      for (int i = 0; i < (NUM_LEDS * 3); i += 3) {
        ledRgbData[i] = redLevel;
      }

      for (int i = 1; i < (NUM_LEDS * 3); i += 3) {
        ledRgbData[i] = grnLevel;
      }

      for (int i = 2; i < (NUM_LEDS * 3); i += 3) {
        ledRgbData[i] = bluLevel;
      }
    }

    // update the led strip with the RGB data from the array
    if (glitterLevel <= 49) {
      updateNeopixel(ledRgbData, NUM_LEDS);
    }
  }

}
void rainbow()
{
  // FastLED's built-in rainbow generator
  //fill_rainbow( leds, NUM_LEDS, 50, 7);
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void pride()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV( hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend( leds[pixelnumber], newcolor, 64);
  }
}


void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}



void rainbow_beat() {

  uint8_t beatA = beatsin8(17, 0, 255);                        // Starting hue
  uint8_t beatB = beatsin8(13, 0, 255);
  fill_rainbow(leds, NUM_LEDS, (beatA + beatB) / 2, 8);        // Use FastLED's fill_rainbow routine.

}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  //leds[pos] += CHSV( gHue + random8(64), 200, 255);
  leds[pos] += CRGB::White;
}

void cylon() {
  static uint8_t hue = 0;

  // First slide the led in one direction
  for (int i = 0; i < NUM_LEDS; i += 3) {
    // Set the i'th led to red
    leds[i] = CHSV(hue += 2, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(1);
  }


  // Now go in the other direction.
  for (int i = (NUM_LEDS) - 1; i >= 0; i -= 3) {
    // Set the i'th led to red
    leds[i] = CHSV(hue += 2, 255, 255);
    // Show the leds
    FastLED.show();
    // now that we've shown the leds, reset the i'th led to black
    // leds[i] = CRGB::Black;
    fadeall();
    // Wait a little bit before we loop around and do it again
    delay(1);
  }
}



void arch() {

  int middle = NUM_LEDS / 2;
  int groupSize = 16; // Increase the group size to 6 LEDs
  int animationDelay = 1; // Increase the animation delay to 15 (for a slower animation)

  // Clear all LEDs
  FastLED.clear();

  // Calculate the animation duration based on the speed
  unsigned long animationDuration = (middle - groupSize + 1) * animationDelay;

  // Loop from both ends towards the middle
  for (int i = 0; i < (middle - groupSize + 1); i++) {
    // Turn on LEDs at the current position
    for (int j = 0; j < groupSize; j++) {
      leds[i + j] = CRGB::White;
      leds[NUM_LEDS - 1 - i - j] = CRGB::White;
    }

    FastLED.show();
    delay(animationDelay);

    // Clear the LEDs for the next step
    for (int j = 0; j < groupSize; j++) {
      leds[i + j] = CRGB::Black;
      leds[NUM_LEDS - 1 - i - j] = CRGB::Black;
    }
  }

  // Calculate the remaining animation time to fill the middle
  unsigned long remainingTime = animationDuration - (middle - groupSize + 1) * animationDelay;
  delay(remainingTime);
}
