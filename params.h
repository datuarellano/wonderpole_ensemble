/* ======================================================================
  This file contains global variables that you can change to 
  easily customize the Wonderpole.
====================================================================== */

/* 
  List of MIDI notes converted to frequencies using the mtof() function
  Tuning is currently set to: A Major Pentatonic scale
                     A4        B4        C#5       E5        F#5       A5        B5        C#6  */
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
  Set lowest and highest estimate readings of your ribbon.
*/
int rib_lowest = 50;
int rib_highest = 500;

/*
  Set the frequencies of the accelerometer mode oscillators
  Used in accelerometer.h
*/ 
int fore_freq = freqs[0];
int back_freq1 = freqs[4];
int back_freq2 = freqs[6];
int left_freq1 = freqs[5];
int left_freq2 = freqs[7];
int right_freq1 = freqs[1];
int right_freq2 = freqs[3];