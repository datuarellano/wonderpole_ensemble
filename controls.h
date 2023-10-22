int knob1;
int knob2;
int knob3;
int knob4;
bool switch4;

// Read potentiometers
void readPots() {
  knob1 = mozziAnalogRead(POT1_PIN);
  knob2 = mozziAnalogRead(POT2_PIN);
  knob3 = mozziAnalogRead(POT3_PIN);
  knob4 = mozziAnalogRead(POT4_PIN);
}

// Update buttons and switches
void updateButtonsSwitches() {
  playmode_switch.update();
  ribmode_switch.update();
  custom_switch.update();
  presetA_switch.update();
  presetB_switch.update();
  button.update();
}

// Preset switch returns a value of 1, 2, or 3
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

// Gate button returns true or false.
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

// Tempo control knob remapped
unsigned int tempoKnob() {
  return map(knob1, 0, 1023, 0, 1000); // basic mapping can be overriden
}

// Octave control knob returns 0.5, 1, or 2
float pitchKnob() {
  if (knob2 >= 0 && knob2 < 250) return 0.5;
  else if (knob2 >= 251 && knob2 < 450) return 1;
  else return 2;
}

// Play mode switch returns true or false
bool playmodeSwitch() {
  if (playmode_switch.isPressed()) return true;
  else return false;
}

// Ribbon mode switch returns true or false
bool ribbonmodeSwitch() {
  if (ribmode_switch.isPressed()) return true;
  else return false;
}

// Custom knob. Customize to your liking!
unsigned int customKnob() {
  return knob3;
}

// Custom switch returns true or false
bool customSwitch() {
  if (custom_switch.isPressed()) return true;
  else return false; 
}