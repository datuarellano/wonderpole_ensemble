/* ======================================================================
  accelerometer() contains logic for the accelerometer mode
  as well as sound design for when the pole is tilted to the
  left, right, forward, and back
*/
void accelerometer() {
  // Update accelerometer
  updateAccelerometer();

  // Constrain x and y values  
  accx = constrain(accx, -x_max, x_max);
  accy = constrain(accy, -y_max, y_max);
  
  /*
    Check which direction the pole is tilted,
    then increase the amplitude of the sound assigned to that direction.
  */
  // X-Axis FORWARD
  if (accx <= -x_threshold)
  {
    accx_fore_vol = -(map(accx, -x_threshold, -x_max, 0, -255));
  }
  // X-Axis BACK
  else if (accx >= x_threshold)
  {
    accx_back_vol = map(accx, x_threshold, x_max, 0, 255);
  }
  // Center QUIET
  else {
    accx_back_vol = 0;
    accx_fore_vol = 0;
  }

  // Y-Axis LEFT
  if (accy <= -y_threshold) 
  {
    accy_left_vol = -(map(accy, -y_threshold, -y_max, 0, -255));
  }
  // Y-Axis RIGHT
  else if (accy >= y_threshold) 
  {
    accy_right_vol = map(accy, y_threshold, y_max, 0, 255);
  }
  // Center QUIET
  else {
    accy_left_vol = 0;
    accy_right_vol = 0;
  }

  /////////////////////////////////////////////////////
  ///// X axis FORE ///////////////////////////////////
  /////////////////////////////////////////////////////
  if (accx <= -x_threshold)
  {
    sine1.setFreq(fore_freq * octave);
  }
  /////////////////////////////////////////////////////
  ///// X axis BACK ///////////////////////////////////
  /////////////////////////////////////////////////////
  if (accx >= x_threshold)
  {
    sine1.setFreq(back_freq1 * octave);
    sine2.setFreq(back_freq2 * octave);
  }

  /////////////////////////////////////////////////////
  ///// Y axis <-- LEFT ///////////////////////////////
  /////////////////////////////////////////////////////
  if (accy <= -y_threshold) 
  {
    sine3.setFreq(left_freq1 * octave);
    sine4.setFreq(left_freq2 * octave);
  }

  /////////////////////////////////////////////////////
  ///// Y axis --> RIGHT //////////////////////////////
  /////////////////////////////////////////////////////
  if (accy >= y_threshold) 
  { 
    sine3.setFreq(right_freq1 * octave);
    sine4.setFreq(right_freq2 * octave);
  }

  // Set to zero volume when gate button is pressed
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