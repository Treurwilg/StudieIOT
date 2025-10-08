/* 
 *  Dit is de eigen uitwerking van de het eindproject van de Udemy cursus "RaspberryPi en 
 *  Arduino", namelijk om het systeem "intercom" te maken. Componenten zijn RaspberryPi, 
 *  Arduino UNO, en Telegram. Ik doe dit aan de hand van al eerder uitgevoerde projecten
 *  voor de elementen ten behoeve van de aansturing LED's, LCD, BUZZER, SERVO, de verwerking
 *  van BUTTON, de communicatie tussen Arduino en RaspberryPI en die tussen de RaspberryPi
 *  en Telegram. Op de RaspberryPi is daarnaast de mogelijkheid om foto's te nemen.
 *  
*/

const byte RGB_RED_PIN = 11;
const byte RGB_YELLOW_PIN = 10;
const byte RGB_GREEN_PIN = 9;

void switchOffLeds() {
  digitalWrite(RGB_RED_PIN, LOW);
  digitalWrite(RGB_YELLOW_PIN, LOW);
  digitalWrite(RGB_GREEN_PIN, LOW);
}

void setLed(String color) {
  Serial.println("Kleur: " + color);
  if (color == "RED") {
    switchOffLeds();
    digitalWrite(RGB_RED_PIN, HIGH);
    delay(1000);
  } else if (color == "YELLOW") {
    switchOffLeds();
    digitalWrite(RGB_YELLOW_PIN, HIGH);
  } else if (color == "GREEN") {
    switchOffLeds();
    digitalWrite(RGB_GREEN_PIN, HIGH);
  } else {
    // do nothing
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_YELLOW_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  switchOffLeds();

  /* initieer hier een initiele presentatie van het systeem door 
   *  - de naam van de sketch op de LCD weer te geven 
   *  - de andere element zich ook te laten presenteren en 
   *  - opverzoek van de BUTTON te stoppen en naar het echte 
   *  - telecom project over te gaan.
   */
}

void loop() {

  delay(10);
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('/n');
    cmd.trim();
    Serial.println("cmd: " + cmd);
    if (cmd.startsWith("led:")) {
      cmd.remove(0, 4);
      Serial.println("kort: " + cmd);
      setLed(cmd);
    }
  }
}
