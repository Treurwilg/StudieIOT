/* 
 *  Dit is de eigen uitwerking van de het eindproject van de Udemy cursus "RaspberryPi en 
 *  Arduino", namelijk om het systeem "intercom" te maken. Componenten zijn RaspberryPi, 
 *  Arduino UNO, en Telegram. Ik doe dit aan de hand van al eerder uitgevoerde projecten
 *  voor de elementen ten behoeve van de aansturing LED's, LCD, BUZZER, SERVO, de verwerking
 *  van BUTTON, de communicatie tussen Arduino en RaspberryPI en die tussen de RaspberryPi
 *  en Telegram. Op de RaspberryPi is daarnaast de mogelijkheid om foto's te nemen.
 *  
*/
#include <LiquidCrystal.h>
// Voor de leds
const byte RGB_RED_PIN = 11;
const byte RGB_YELLOW_PIN = 10;
const byte RGB_GREEN_PIN = 9;
// Voor de LCD
const int LCD_RS_PIN = A4;
const int LCD_E_PIN = A5;
const int LCD_D4_PIN = 2;
const int LCD_D5_PIN = 3;
const int LCD_D6_PIN = 4;
const int LCD_D7_PIN = 5;
// voor de buzzer  
const int BUZZER_PIN = 8;


LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN,
                  LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
                  
void switchOffLeds() {
  digitalWrite(RGB_RED_PIN, LOW);
  digitalWrite(RGB_YELLOW_PIN, LOW);
  digitalWrite(RGB_GREEN_PIN, LOW);
}

void setLed(String color) {
  if (color == "RED") {
    switchOffLeds();
    digitalWrite(RGB_RED_PIN, HIGH);
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

/* format command is: "position:line:text"
 * position is de positie op de regel van 0 t/m 15
 * line is de regel: 0 of 1
 * Uitgangspunt is dat als regel 0 aangepast wordt ook regel 1 
 * aangepast zal worden, dus worden het clear() bij regel 0 toegepast. 
 */
void setLCD(String command) {
  int firstLocation = command.indexOf(":");
  int lastLocation = command.lastIndexOf(":");   
  String position = command.substring(0, firstLocation);
  String line = command.substring(firstLocation + 1, lastLocation);
  String text = command.substring(lastLocation + 1, command.length());
  if (line == "0") {
    lcd.clear();
  }
  lcd.setCursor(position.toInt(),line.toInt());
  lcd.print(text);
}

/*
 * Format command is: "frequncy:duration"
 */
void setBuzzer(String command) {
  int location = command.indexOf(":"); 
  String frequency = command.substring(0, location);
  String duration = command.substring(location + 1, command.length());    
  Serial.println(frequency + "  " + duration);  
  tone(BUZZER_PIN, frequency.toInt(), duration.toInt());
}

void setup() {
  // Algemeen
  Serial.begin(115200);
  // voor de buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  // Voor de leds
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_YELLOW_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  switchOffLeds();
  // Voor de LCD
  lcd.begin(16, 2);
  lcd.clear();
  

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
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("led:")) {
      cmd.remove(0, 4);
      setLed(cmd);
    } else if (cmd.startsWith("lcd:")) {
      cmd.remove(0, 4);
      setLCD(cmd);
    } else if (cmd.startsWith("buzz:")) {
      cmd.remove(0, 5);
      setBuzzer(cmd);
    } else {
      // do nothing
    }
  }
}
