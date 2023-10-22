int ribbonRead() {
  return mozziAnalogRead(RIBBON_PIN);
}

int ribbonCalibrate() {
  return map(knob4, 0, 1023, 0, 100);
}

void ribbonMode() {
  rib_val = ribbonRead();
  rib_lowest = ribbonCalibrate();
  
  // Mode 1 subdivides the ribbon into 8 segments
  int seg[] = {rib_lowest, 130, 200, 300, 455, 610, 815, 1023};

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
    freq = map(rib_val, rib_lowest, 1023, freqs[0] * octave, freqs[7] * octave);
  }
}

void ribbon() {
  ribbonMode();

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

  if (preset == 2) 
  {
    sine0.setFreq(freq);
    saw0.setFreq(freq);
    rib_preset2.attack = 1;
    rib_preset2.decay = 20;
    rib_preset2.sustain = 20;
    rib_preset2.release = 30;
    // remap tempo knob value
    int adsr = rib_preset2.attack + rib_preset2.decay + rib_preset2.sustain + rib_preset2.release;
    tempo = map(knob1, 0, 1023, -adsr, 1000);
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