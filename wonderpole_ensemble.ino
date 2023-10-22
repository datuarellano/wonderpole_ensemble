/* ===============================================================================
 * WONDERPOLE ENSEMBLE (From the Wonderpole Project)
 * Placed under The MIT License (MIT)
 * Copyright (c) 2023 Datu Arellano
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
================================================================================= */

//////// Include MOZZI Library and related modules
#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <mozzi_midi.h>
#include <mozzi_rand.h>
//////// Include MOZZI wavetables
#include <tables/sin2048_int8.h>
#include <tables/saw_analogue512_int8.h>
#include <tables/whitenoise8192_int8.h>

//////// Include other libraries
#include <Bounce2.h>

/////// Define ARDUINO I/O pins
#define RIBBON_PIN A0
#define POT1_PIN A1
#define POT2_PIN A2
#define POT3_PIN A3
// NOTE: PINs A4 and A5 are connected to the accelerometer module
#define POT4_PIN A6
#define BUTTON_PIN 2
#define SWITCH_PIN 3
#define SWITCH2_PIN 4
#define SWITCH3A_PIN 5
#define SWITCH3B_PIN 6
#define SWITCH4_PIN 7
// NOTE: PIN D9 is being used by Mozzi as PWM sound out
#define LED2_PIN 12
#define LED_PIN 13

/////// Define MOZZI control rate
#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

//////// Create instances of buttons and switches for debouncing
Bounce2::Button button = Bounce2::Button();
Bounce2::Button ribmode_switch = Bounce2::Button();
Bounce2::Button playmode_switch = Bounce2::Button();
Bounce2::Button presetA_switch = Bounce2::Button();
Bounce2::Button presetB_switch = Bounce2::Button();
Bounce2::Button custom_switch = Bounce2::Button();

///// Create MOZZI oscillators
Oscil<WHITENOISE8192_NUM_CELLS, AUDIO_RATE> noise(WHITENOISE8192_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> sine0(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> sine1(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> sine2(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> sine3(SIN2048_DATA);
Oscil<SAW_ANALOGUE512_NUM_CELLS, AUDIO_RATE> saw0(SAW_ANALOGUE512_DATA);
Oscil<SAW_ANALOGUE512_NUM_CELLS, AUDIO_RATE> saw1(SAW_ANALOGUE512_DATA);

////////// Create event delays for timing/sequencing
EventDelay noteDelay;

//////////////// Create envelopes
ADSR<AUDIO_RATE, AUDIO_RATE> envelope1;
ADSR<AUDIO_RATE, AUDIO_RATE> envelope2;
ADSR<AUDIO_RATE, AUDIO_RATE> envelope3;
ADSR<AUDIO_RATE, AUDIO_RATE> envelope4;
ADSR<AUDIO_RATE, AUDIO_RATE> envelope5;

// This struct is for grouping synth parameters into presets
struct Preset {
  unsigned int attack;
  unsigned int decay;
  unsigned int sustain;
  unsigned int release;
  unsigned int duration;
  int freq;
};

// Preset variables
Preset rib_preset1;
Preset rib_preset2;
Preset rib_preset3;
Preset acc_preset1;
Preset acc_preset2;
Preset acc_preset3;

// Control variables
byte preset;
int freq;
int tempo;
float octave;
bool rib_mode = 0;
bool play_mode = 0;
bool gate = false;

// Ribbon variables
int rib_val;
int rib_lowest;
int rib_highest;

// Accelerometer variables
int accx; 
int accy; 
int accz; 
int new_accx;
int new_accy;
byte accy_left_vol;
byte accy_right_vol;
byte accx_fore_vol;
byte accx_back_vol;

// Variables used for timing
unsigned long ms = millis();
unsigned long readTime = ms;

//////// Include custom code
/*
  params.h contains basic parameters that you can easily customize  
*/
#include "params.h"
/* 
  controls.h contains interface controls (pots, switches, buttons)
*/
#include "controls.h"
/* 
  ribbon.h contains code that handles the ribbon controller 
*/
#include "ribbon.h"
/*  
  accelerometer_advanced_setup.h contains important code that makes
  the accelerometer module play nicely with MOZZI
*/
#include "accelerometer_advanced_setup.h"
/*
  accelerometer.h contains custom accelerometer functions
*/ 
#include "accelerometer.h"

/* ======================================================================
  ARDUINO Setup Block
*/
void setup()
{
  Serial.begin(9600);
  #include "setup.h"
}

/* ======================================================================
  MOZZI Update Control Block
*/
void updateControl()
{
  // Run interface read and update functions
  readPots();
  updateButtonsSwitches();
  
  // Run control functions and store values in variables
  gate = gateButton();
  preset = presetSwitch();
  tempo = tempoKnob();
  octave = pitchKnob();
  play_mode = playmodeSwitch();
  rib_mode = ribbonmodeSwitch();
  switch4 = customSwitch();
  
  // Switch between ribbon and accelerometer modes
  if (play_mode == true) 
  {
    ribbon();
  }
  else 
  {
    updateAccelerometer();
    accelerometerGo();
  }
}

/* ======================================================================
  MOZZI Update Audio Block
*/
AudioOutput_t updateAudio()
{
  // Update envelopes
  envelope1.update();
  envelope2.update();
  envelope3.update();
  envelope4.update();
  envelope5.update();
  
  // Create submix variables
  unsigned int acc1, acc2, acc3, acc4, acc5, acc6, xyz, rib;
  
  // Ribbon mode submixes (PLAY MODE: 1)
  if (play_mode == 1) 
  {
    // If preset is either 1 or 2, custom switch toggles between sine and sawtooth waves
    if (preset != 3 && switch4 == true) rib = envelope4.next() * sine0.next() >> 2;
    else if (preset != 3 && switch4 == false) rib = envelope4.next() * saw0.next() >> 2;

    // If preset is 3 and custom switch is in left position
    if (preset == 3 && switch4 == true) {
      rib = envelope4.next() *
      ( 
        saw1.next() +
        sine1.next() +
        sine2.next() +
        (sine0.next() * saw0.next())
      ) >> 3;
    }
    // If preset is 3 and custom switch is in right position
    else if (preset == 3 && switch4 == false) {
      rib = envelope4.next() *
      ( 
        (sine0.next() * rand(0, 5)) + 
        (sine1.next() * saw1.next()) 
      ) >> 3;
    }
  }
  
  // Accelerometer mode submixes (PLAY MODE: 0)
  else
  {
    acc1 = envelope1.next() * sine1.next() >> 3;
    acc2 = envelope2.next() * sine2.next() >> 3;
    acc3 = accy_left_vol * noise.next() >> 3;
    acc4 = envelope5.next() * sine3.next();
    acc5 = accx_fore_vol * saw0.next() >> 2;
    acc6 = accx_back_vol * saw1.next() >> 2;
    xyz = acc1 + acc2 + acc3 + acc4 + acc5 + acc6;
  } 

  // Master output
  if (play_mode == 1) return MonoOutput::from16Bit(rib);
  else return MonoOutput::from16Bit(xyz);
}

/* ======================================================================
  ARDUINO Loop Block
  All we need inside the loop is the required MOZZI audioHook() function.
  It is best to avoid placing anything else in here.
*/
void loop() {
  audioHook();
}