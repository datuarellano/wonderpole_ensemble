// Button setup
  button.attach(BUTTON_PIN, INPUT_PULLUP);
  button.interval(50);
  button.setPressedState(LOW);

  // Ribbon mode switch setup
  ribmode_switch.attach(SWITCH_PIN, INPUT_PULLUP);
  ribmode_switch.interval(50);
  ribmode_switch.setPressedState(LOW);

  // Play mode switch setup
  playmode_switch.attach(SWITCH2_PIN, INPUT_PULLUP);
  playmode_switch.interval(50);
  playmode_switch.setPressedState(LOW);
  
  // Preset switch setup
  presetA_switch.attach(SWITCH3A_PIN, INPUT_PULLUP);
  presetA_switch.interval(50);
  presetA_switch.setPressedState(LOW);
  presetB_switch.attach(SWITCH3B_PIN, INPUT_PULLUP);
  presetB_switch.interval(50);
  presetB_switch.setPressedState(LOW);

  // Custom switch setup
  custom_switch.attach(SWITCH4_PIN, INPUT_PULLUP);
  custom_switch.interval(50);
  custom_switch.setPressedState(LOW);

  startMozzi();

  pinMode(LED2_PIN, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED2_PIN, LOW);
  delay(300);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED2_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED2_PIN, LOW);
  delay(300);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED2_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED2_PIN, LOW);