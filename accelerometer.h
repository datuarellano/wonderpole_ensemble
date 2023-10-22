void accelerometerGo() {
  int x_threshold = 20;
  int x_max = 90;
  int y_threshold = 20;
  int y_max = 90;
  
  accx = constrain(accx, -x_max, x_max);
  accy = constrain(accy, -y_max, y_max);
  
  // X-Axis FORWARD
  if (accx <= -x_threshold)
  {
    new_accx = -(map(accx, -x_threshold, -x_max, 0, -255));
    accx_fore_vol = new_accx;
  }
  // X-Axis BACK
  else if (accx >= x_threshold)
  {
    new_accx = map(accx, x_threshold, x_max, 0, 255);
    accx_back_vol = new_accx;
  }
  // Center QUIET
  else {
    accx_back_vol = 0;
    accx_fore_vol = 0;
  }

  // Y-Axis LEFT
  if (accy <= -y_threshold) 
  {
    new_accy = -(map(accy, -y_threshold, -y_max, 0, -255));
    accy_left_vol = new_accy;
  }
  // Y-Axis RIGHT
  else if (accy >= y_threshold) 
  {
    new_accy = map(accy, y_threshold, y_max, 0, 255);
    accy_right_vol = new_accy;
  }
  // Center QUIET
  else {
    accy_left_vol = 0;
    accy_right_vol = 0;
  }

  //////////////////////////////////////////////////////////////////////
  ///// DRONE SOUNDS /// X axis forward/backward ///////////////////////
  //////////////////////////////////////////////////////////////////////
  // saw0.setFreq(freqs[0] * octave);
  // saw1.setFreq(freqs[2] * octave);
  // saw0.setFreq(freqs[1] * octave);
  // saw1.setFreq(freqs[3] * octave);
  saw0.setFreq(freqs[4] * octave);
  saw1.setFreq(freqs[5] * octave);

  int rib_val = map(ribbonRead(), 200, 1023, 5, 1500);

  //////////////////////////////////////////////////////////////////////
  ///// CICADA SOUNDS /// Y axis --> RIGHT /////////////////////////////
  //////////////////////////////////////////////////////////////////////
  if (accy >= y_threshold) 
  {
    // 1:
    envelope1.setADLevels(accy_right_vol, accy_right_vol);
    acc_preset1.freq = 3000 + knob2 + rib_val;
    // 2:
    envelope2.setADLevels(accy_right_vol, accy_right_vol);
    acc_preset2.freq = 1070 + knob2 + rib_val; 

    if(gate == false) {
      sine2.setFreq(acc_preset2.freq);
      envelope2.setTimes(10,50,50,10);
      envelope2.noteOn();
      
      sine1.setFreq(acc_preset1.freq);
      envelope1.setTimes(10,50,50,10);
      envelope1.noteOn();
    }
  }

  //////////////////////////////////////////////////////////////////////
  ///// OCEAN SOUND /// Y axis <-- LEFT ////////////////////////////////
  //////////////////////////////////////////////////////////////////////
  if (accy <= -y_threshold) 
  {
    noise.setFreq(1);
  }

  // Set all (except stab sound) to zero volume when gate button is pressed
  if (gate == true) {
    accx_fore_vol = 0;
    accx_back_vol = 0;
    accy_left_vol = 0;
    accy_right_vol = 0;
  }

  // LED2 ON/OFF when button is pressed
  if (gate == true) digitalWrite(LED2_PIN, HIGH);
  else digitalWrite(LED2_PIN, LOW);
}