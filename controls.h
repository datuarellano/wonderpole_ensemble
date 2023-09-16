// Preset switch
byte presetSwitch() 
{
  presetA_switch.update();
  presetB_switch.update();
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

// Gate button
bool gateButton() 
{
  button.update();
  if (button.isPressed()) 
  {
    return true;
  } 
  else 
  { 
    return false;
  }
}

// Tempo control knob
unsigned int tempoKnob() {
  knob1 = mozziAnalogRead(POT_PIN);
  return map(knob1, 0, 1023, 0, 1000); // basic mapping can be overriden
}

// Octave control knob
float octaveKnob() {
  knob2 = mozziAnalogRead(POT2_PIN);
  if (knob2 >= 0 && knob2 < 250) return 0.5;
  else if (knob2 >= 251 && knob2 < 450) return 1;
  else return 2;
}

// Play mode switch
bool playmodeSwitch() {
  playmode_switch.update();
  if (playmode_switch.isPressed()) return true;
  else return false;
}

// Ribbon mode switch
bool ribbonmodeSwitch() {
  ribmode_switch.update();
  if (ribmode_switch.isPressed()) return true;
  else return false;
}

// Custom knob
unsigned int customKnob() {
  knob3 = mozziAnalogRead(POT3_PIN);
  return knob3;
}

// Custom switch
bool customSwitch() {
  custom_switch.update();
  if (custom_switch.isPressed()) return true;
  else return false; 
}