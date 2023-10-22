/* ======================================================================
  This file contains global variables that you can change to 
  easily customize the Wonderpole.
====================================================================== */

/* 
  List of MIDI notes converted to frequencies using the mtof() function
  Tuning is currently set to: A Major Pentatonic scale
*/
int freqs[8] = {mtof(57), mtof(59), mtof(61), mtof(64), mtof(66), mtof(69), mtof(71), mtof(73)};

/*
  Set the min and max interval of staccato mode
  Used in controls.h --> tempoKnob()
*/ 
int min_tempo = -100;
int max_tempo = 1200;

/*
  Set the pole's tilt (accelerometer mode) thresholds
  Used in accelerometer.h --> accelerometerGo()
*/ 
int x_threshold = 20;
int x_max = 90;
int y_threshold = 20;
int y_max = 90;

/*
  Set the frequencies of the forward and back (accelerometer mode) oscillators
  Used in accelerometer.h --> DRONE SOUNDS
*/ 
int drone_fore_freq = freqs[0];
int drone_back_freq = freqs[2];