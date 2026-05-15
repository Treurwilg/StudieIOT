/* 
 *  Dit is de uitwerking van Edouard Renard van de het eindproject van de Udemy cursus 
 *  "RaspberryPi en 
 *  Arduino", namelijk om het systeem "intercom" te maken. Componenten zijn RaspberryPi, 
 *  Arduino UNO, en Telegram. Het verschil met min eigen uitwerking zit vooral in de taken 
 *  die aan de Arduino teogekend worden. Ik liet Arduino het hele proces voor het openen en 
 *  sluiten van de deur beheren. Edouard kent alleen atomische functies toe aan de Arduino. 
 *  Het procesbeheer komt geheel voor rekening van de Pi. Dat blijkt uiteindelijk het geheel
 *  eenvoudiger te maken. Allerlei randvoorwaarden hoeven nu alleen op de Pi in acht genomen
 *  te worden. De Arduino heeft een aantal componenten zonder relatie onderling. Er is geen  
 *  communicatie over de randvoorwaarden tussen Pi en Arduino plaats te vinden.
 *  
*/
#include <LiquidCrystal.h>
#include <Servo.h>

const int SERVO_PIN = 12;
const byte RGB_RED_PIN = 11;
const byte RGB_YELLOW_PIN = 10;
const byte RGB_GREEN_PIN = 9;
const int BUZZER_PIN = 8;
const byte BUTTON_PIN = 7;
const int LCD_RS_PIN = A4;
const int LCD_E_PIN = A5;
const int LCD_D4_PIN = 2;
const int LCD_D5_PIN = 3;
const int LCD_D6_PIN = 4;
const int LCD_D7_PIN = 5;

const int SERVO_OPEN_DOOR_POSITION = 50;
const int SERVO_CLOSE_DOOR_POSITION = 140;
;
LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN,
                  LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);
Servo servo;

unsigned long lastTimeButtonChanged = millis();
unsigned long debounceDelay = 50;
byte previousButtonState;

void setup() {
  Serial.begin(115200);
  while (!Serial) {} // voor compatibiliteit met andere Arduino boards
    while(Serial.available() > 0) {
    Serial.read(); // best practiceom zeker te zijn dat de input buffer leeg is 
  } 
  
  servo.attach(SERVO_PIN);  
  servo.write(SERVO_CLOSE_DOOR_POSITION);

  pinMode(BUTTON_PIN,INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_YELLOW_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);

  previousButtonState = digitalRead(BUTTON_PIN);

  lcd.begin(16, 2);
  lcd.print("Starting...");
  delay(1000);
  lcd.clear();
}

void loop() {

  unsigned long timeNow = millis();
  if(timeNow - lastTimeButtonChanged >= debounceDelay) {
    byte buttonState = digitalRead(BUTTON_PIN);
    if(buttonState != previousButtonState) {
      lastTimeButtonChanged = timeNow;
      previousButtonState = buttonState;
      if (buttonState == HIGH) {
        Serial.println("button_pressed");
      }
    }
  }

 if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    if (cmd == "open_door") {
      servo.write(SERVO_OPEN_DOOR_POSITION);
    }
    else if (cmd == "close_door") {
      servo.write(SERVO_CLOSE_DOOR_POSITION);
    }
    else if (cmd.startsWith("print_text")) {
      cmd.remove(0, 11);
      String line1 = cmd.substring(0, 16);
      String line2 = cmd.substring(16);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(line1);
      lcd.setCursor(0, 1);
      lcd.print(line2);
    }
    else if (cmd.startsWith("play_buzzer")) {
      cmd.remove(0, 12);
      int comaIndex = cmd.indexOf(',');
      int frequency = cmd.substring(0, comaIndex).toInt();
      int duration = cmd.substring(comaIndex + 1).toInt();
      tone(BUZZER_PIN, frequency, duration);
    }
    else if (cmd.startsWith("set_led")) {
      cmd.remove(0, 8);
      setLed(cmd);
    }
    else {
      while (Serial.available() > 0) {
        Serial.read(); // als een verkeerd bericht is ontvangen wil je buffer weer leeg
      }
    }
  }
}
         
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
