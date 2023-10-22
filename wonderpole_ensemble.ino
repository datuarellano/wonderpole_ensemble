
//////// Include MOZZI Library and related components
#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin2048_int8.h>
#include <tables/saw_analogue512_int8.h>
#include <tables/whitenoise8192_int8.h>
#include <mozzi_midi.h>
#include <mozzi_rand.h>
#include <twi_nonblock.h>

//////// Include other libraries
#include <Bounce2.h>

/////// Define ARDUINO I/O pins
#define RIBBON_PIN A0
#define POT_PIN A1
#define POT2_PIN A2
#define POT3_PIN A3
// NOTE: PINs A4 and A5 are connected to the accelerometer module
#define POT4_PIN A6
#define LED_PIN 13
#define LED2_PIN 12
#define BUTTON_PIN 2
#define SWITCH_PIN 3
#define SWITCH2_PIN 4
#define SWITCH3A_PIN 5
#define SWITCH3B_PIN 6
#define SWITCH4_PIN 7
// NOTE: PIN D9 is being used by Mozzi as PWM sound out

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
Oscil<WHITENOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(WHITENOISE8192_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSine(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSine1(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSine2(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSine3(SIN2048_DATA);
Oscil<SAW_ANALOGUE512_NUM_CELLS, AUDIO_RATE> aSaw0(SAW_ANALOGUE512_DATA);
Oscil<SAW_ANALOGUE512_NUM_CELLS, AUDIO_RATE> aSaw1(SAW_ANALOGUE512_DATA);

////////// Create event delays for timing/sequencing
EventDelay noteDelay;
EventDelay ledDelay;

//////////////// Create envelopes
ADSR<AUDIO_RATE, AUDIO_RATE> envelope1;
ADSR<AUDIO_RATE, AUDIO_RATE> envelope2;
ADSR<AUDIO_RATE, AUDIO_RATE> envelope3;
ADSR<AUDIO_RATE, AUDIO_RATE> envelope4;
ADSR<AUDIO_RATE, AUDIO_RATE> envelope5;

struct Preset {
  unsigned int attack;
  unsigned int decay;
  unsigned int sustain;
  unsigned int release;
  unsigned int time_pad;
  unsigned int duration;
  int freq;
};

Preset rib_preset1;
Preset rib_preset2;
Preset rib_preset3;
Preset acc_preset1;
Preset acc_preset2;
Preset acc_preset3;
Preset acc_preset4;
Preset acc_preset5;

bool gate = false;
byte preset;
int freq;
int tempo;
float octave;
bool rib_mode = 0;
bool play_mode = 0;
float gain;
byte accy_left_vol;
byte accy_right_vol;
byte accx_fore_vol;
byte accx_back_vol;
bool accz_trigger = false;
long previousTime = millis();
int rib_val;
int rib_lowest;

// Declare accelerometer xyz variables
int accx; 
int accy; 
int accz;

// Declare new remapped X and Y variables
int new_accx;
int new_accy;

int knob1;
int knob2;
int knob3;
bool switch4;

unsigned long ms = millis();
unsigned long readTime = ms;

/// List of MIDI notes converted to frequencies using the mtof() function
int freqs[8] = {mtof(57), mtof(59), mtof(61), mtof(64), mtof(66), mtof(69), mtof(71), mtof(73)};

//////// Include external code
#include "controls.h"
#include "ribbon.h"
#include "accelerometer_advanced_setup.h"
#include "accelerometer.h"


//////////////////////////////////////////////////////////////////////
//// ARDUINO Setup Block /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(9600);
  #include "setup.h"
}

//////////////////////////////////////////////////////////////////////
//// MOZZI Update Control Block //////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void updateControl()
{
  gate = gateButton();
  preset = presetSwitch();
  tempo = tempoKnob();
  octave = octaveKnob();
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

//////////////////////////////////////////////////////////////////////
//// MOZZI Update Audio Block ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
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
  
  // Ribbon mode submixes
  if (play_mode == 1) 
  {
    if (preset == 1 && switch4 == true) rib = envelope4.next() * aSine.next() >> 2;
    else if (preset == 1 && switch4 == false) rib = envelope4.next() * aSaw0.next() >> 2;

    if (preset == 2 && switch4 == true) rib = envelope4.next() * aSine.next() >> 2;
    else if (preset == 2 && switch4 == false) rib = envelope4.next() * aSaw0.next() >> 2;

    if (preset == 3 && switch4 == true) {
      rib = envelope4.next() *
      ( 
        aSaw1.next() +
        aSine1.next() +
        aSine2.next() +
        (aSine.next() * aSaw0.next())
      ) >> 3;
    }
    else if (preset == 3 && switch4 == false) {
      rib = envelope4.next() *
      ( 
        (aSine.next() * rand(0, 5)) + 
        (aSine1.next() * aSaw1.next()) 
      ) >> 3;
    }
  }
  // Accelerometer mode submixes
  else
  {
    acc1 = envelope1.next() * aSine1.next() >> 3;
    acc2 = envelope2.next() * aSine2.next() >> 3;
    acc3 = accy_left_vol * aNoise.next() >> 3;
    acc4 = envelope5.next() * aSine3.next();
    acc5 = accx_fore_vol * aSaw0.next() >> 2;
    acc6 = accx_back_vol * aSaw1.next() >> 2;
    xyz = acc1 + acc2 + acc3 + acc4 + acc5 + acc6;
  } 

  // Master output
  if (play_mode == 1) return MonoOutput::from16Bit(rib);
  else return MonoOutput::from16Bit(xyz);
}


//////////////////////////////////////////////////////////////////////
//// ARDUINO Loop Block //////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void loop() {
  audioHook(); // REQUIRED! And there's no need to do anything in here.
}