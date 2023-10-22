/* ======================================================================
  ribbonread() returns a reading of the ribbon.
  On the Wonderpole, it's the strips of copper tape that are 
  connected to RIBBON_PIN.
*/
int ribbonRead() {
  return mozziAnalogRead(RIBBON_PIN);
}

/* ======================================================================
  ribbonCalibrate() sets up the ribbon
*/
void ribbonCalibrate() {
  /* 
   Read the ribbon
  */
  rib_val = ribbonRead();
  /* 
   Set lowest and highest estimate readings of your ribbon
   Take note that these are "magic numbers". 
  */
  rib_lowest = 50;
  rib_highest = 500;
  /*
   Remap the raw ribbon readings
  */
  rib_val = map(rib_val, rib_lowest, rib_highest, 0, 800);
  /* 
   Constrain rib_val to numbers between 0 and 800,
   to make sure we do not get numbers less than 0 nor numbers greater than 800.
   Why 800? Purely for convenience. Since our ribbon will be subdivided into
   8 segments, it is easier to calculate the subdivisions this way.
  */
  rib_val = constrain(rib_val, 0, 800);
}

/* ======================================================================
  ribbonMode() function contains the call to calibrate the ribbon,
  the array of ribbon segments,
  the logic to toggle between segmented and non-segmented modes,
  and the logic to assign frequencies to segments.
*/
void ribbonMode() {
  // Run ribbon calibration
  ribbonCalibrate();
  
  // Create array of ribbon segments
  int seg[] = {0, 100, 200, 300, 400, 500, 600, 700};

  // Mode 1 subdivides the ribbon into 8 segments
  if (rib_mode == 1) 
  {
    if (rib_val < seg[0]) {
      freq = freqs[0] * octave;
    }
    else if (rib_val >= seg[0] && rib_val < seg[1]) {
      freq = freqs[1] * octave;
    }
    else if (rib_val >= seg[1] && rib_val < seg[2]) {
      freq = freqs[2] * octave;
    }
    else if (rib_val >= seg[2] && rib_val < seg[3]) {
      freq = freqs[3] * octave;
    }
    else if (rib_val >= seg[3] && rib_val < seg[4]) {
      freq = freqs[4] * octave;
    }
    else if (rib_val >= seg[4] && rib_val < seg[5]) {
      freq = freqs[5] * octave;
    }
    else if (rib_val >= seg[5] && rib_val < seg[6]) {
      freq = freqs[6] * octave;
    }
    else {
      freq = freqs[7] * octave;
    }
  }
  // Mode 0 is linear and has no subdivisions
  else if (rib_mode == 0) 
  {
    freq = map(rib_val, rib_lowest, 1023, freqs[0] * octave, freqs[7] * octave * 4);
  }
}

/* ======================================================================
  ribbon() contains part of the sound design (the other parts are in the
  updateAudio() block in the main .ino file).
  This is where you set:
  1. oscillator frequencies
  2. ADSR settings (attack, decay, sustain, and release values)
  3. envelope times and levels
  4. note on and note off
*/
void ribbon() {
  // Run ribbon mode function
  ribbonMode();
  /* --------------
    PRESET 1
    Default is a regular, monophonic sound generated by a single oscillator.
    The Custom switch allows you to toggle between a sine and a 
    sawtooth wave (code is found in updateAudio() block in main .ino file).
  */
  if (preset == 1)
  {
    sine0.setFreq(freq);
    saw0.setFreq(freq);
    rib_preset1.attack = 500;
    rib_preset1.decay = 100;
    rib_preset1.sustain = 100;
    rib_preset1.release = 400;
    envelope4.setTimes(rib_preset1.attack, rib_preset1.decay, rib_preset1.sustain, rib_preset1.release);
    envelope4.setLevels(255, 100, 100, 0);
    if (gate == true)
    {
      envelope4.noteOn();
      digitalWrite(LED2_PIN, HIGH);
    }
    else
    {
      envelope4.noteOff();
      digitalWrite(LED2_PIN, LOW);
    }
  }

  /* --------------
    PRESET 2
    Default sound is same as Preset 1 except notes 
    are played staccato using an EventDelay timer.
  */
  if (preset == 2) 
  {
    sine0.setFreq(freq);
    saw0.setFreq(freq);
    rib_preset2.attack = 1;
    rib_preset2.decay = 20;
    rib_preset2.sustain = 20;
    rib_preset2.release = 100;
    envelope4.setTimes(rib_preset2.attack, rib_preset2.decay, rib_preset2.sustain, rib_preset2.release);
    envelope4.setLevels(255, 255, 255, 0);
    rib_preset2.duration = rib_preset2.attack + rib_preset2.decay + rib_preset2.sustain + rib_preset2.release + tempo; 
    noteDelay.set(rib_preset2.duration);
    if (noteDelay.ready() && gate == true) 
    {
      noteDelay.start(rib_preset2.duration);   
      envelope4.noteOn();
      digitalWrite(LED_PIN, LOW);
      digitalWrite(LED2_PIN, HIGH);
    }
    else 
    {
      envelope4.noteOff();
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(LED2_PIN, LOW);
    }
  }

  /* --------------
    PRESET 3
    Default sound is a crazy burst of noise.
    The knobs labelled Time, Pitch, and Custom;
    the switch labelled Custom; and the ribbon,
    all have an effect on the sound.
  */
  if (preset == 3) 
  {
    noise.setFreq(10);
    sine1.setFreq(ribbonRead() * tempo);
    sine2.setFreq(ribbonRead() * 4);
    if (customKnob() > 512)
    {
      int r = rand(1,3);
      saw0.setFreq(freq*r);
      saw1.setFreq(freq*r);
      sine0.setFreq(freq*r);
    }
    else {
      saw0.setFreq(freq/16);
      saw1.setFreq(freq/2);
      sine0.setFreq(freq/16);
    }
    rib_preset3.attack = tempoKnob();
    rib_preset3.decay = 10;
    rib_preset3.sustain = customKnob();
    rib_preset3.release = customKnob();
    envelope4.setTimes(rib_preset3.attack, rib_preset3.decay, rib_preset3.sustain, rib_preset3.release);
    if (gate == true) 
    {
      envelope4.noteOn();
      digitalWrite(LED2_PIN, HIGH);
      if (switch4 == false)
      {
      /* Below is a blocking delay and will cause a nasty glitch effect
      The value of the delay is taken from the ribbon reading */
        delay(ribbonRead()); 
      }
    }
    else 
    {
      envelope4.noteOff();
      digitalWrite(LED2_PIN, LOW);
    }
  }
}