int knob1;
int knob2;
int knob3;
int knob4;
bool switch4;

/* ======================================================================
  Read potentiometers  
*/
void readPots() {
  knob1 = mozziAnalogRead(POT1_PIN);
  knob2 = mozziAnalogRead(POT2_PIN);
  knob3 = mozziAnalogRead(POT3_PIN);
  knob4 = mozziAnalogRead(POT4_PIN);
}

/* ======================================================================
  Update buttons and switches
*/
void updateButtonsSwitches() {
  playmode_switch.update();
  ribmode_switch.update();
  custom_switch.update();
  presetA_switch.update();
  presetB_switch.update();
  button.update();
}

/* ======================================================================
  Gate button (S5; tactile switch with red LED)
  Returns true or false
*/
bool gateButton() 
{
  if (button.isPressed()) 
  {
    return true;
  } 
  else 
  { 
    return false;
  }
}

/* ======================================================================
  Tempo control knob (POT1; label: TIME)
  Remapped to min and max_tempo
*/
unsigned int tempoKnob() {
  return map(knob1, 0, 1023, min_tempo, max_tempo);
}

/* ======================================================================
  Octave control knob (POT2; label: PITCH)
  Returns 0.5, 1, or 2
*/
float pitchKnob() {
  if (knob2 >= 0 && knob2 < 250) return 0.5;
  else if (knob2 >= 251 && knob2 < 450) return 1;
  else return 2;
}

/* ======================================================================
  Custom knob (POT3; label: CUSTOM)
  Customize to your liking!
*/
unsigned int customKnob() {
  return knob3;
}

/* ======================================================================
  Play mode switch (S1; label: PLAY MODE)
  Returns true or false
*/
bool playmodeSwitch() {
  if (playmode_switch.isPressed()) return true;
  else return false;
}

/* ======================================================================
  Ribbon mode switch (S2; label: RIBBON MODE)
  Returns true or false
*/
bool ribbonmodeSwitch() {
  if (ribmode_switch.isPressed()) return true;
  else return false;
}

/* ======================================================================
  Custom switch (S3; label: CUSTOM)
  Returns true or false
*/
bool customSwitch() {
  if (custom_switch.isPressed()) return true;
  else return false; 
}

/* ======================================================================
  Preset switch (S4; label: PRESET)
  Returns a value of 1, 2, or 3
*/
byte presetSwitch() 
{
  if (presetA_switch.isPressed()) 
  {
    return 1;
  }
  else if (presetB_switch.isPressed()) 
  {
    return 3;
  }
  else 
  {
    return 2;
  }
}