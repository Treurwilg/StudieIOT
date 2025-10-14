/* 
 *  Dit is de eigen uitwerking van de het eindproject van de Udemy cursus "RaspberryPi en 
 *  Arduino", namelijk om het systeem "intercom" te maken. Componenten zijn RaspberryPi, 
 *  Arduino UNO, en Telegram. Ik doe dit aan de hand van al eerder uitgevoerde projecten
 *  voor de elementen ten behoeve van de aansturing LED's, LCD, BUZZER, SERVO, de verwerking
 *  van BUTTON, de communicatie tussen Arduino en RaspberryPI en die tussen de RaspberryPi
 *  en Telegram. Op de RaspberryPi komt daarnaast de mogelijkheid om foto's te nemen.
 *  
*/
#include <LiquidCrystal.h>
#include <Servo.h>
// Voor de button
const byte BUTTON_PIN = 7;
byte previousButtonState;
bool buttonPressed;
bool messageSent;
unsigned long lastTimeStateChanged = millis();
unsigned long buttonDebounceDuration = 20;
//Voor de servo van de deur
const int SERVO_PIN = 12;
String door;
Servo servo;
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
 * Format command is: "frequency:duration"
 */
void setBuzzer(String command) {
  int location = command.indexOf(":"); 
  String frequency = command.substring(0, location);
  String duration = command.substring(location + 1, command.length());     
  tone(BUZZER_PIN, frequency.toInt(), duration.toInt());
}

// Uitgangspunt bij start van het programma is dat de 
// deur gesloten is. 
void setDoor(String command) {
  if (command == "open" && door == "closed") {
    servo.write(90);
    door = "opened";
  } else if (command == "close" && door == "opened") {
    servo.write(0);
    door = "closed";
  } else {
    // do nothing
  }
}

void checkButton() {
  unsigned long timeNow = millis();
  if(timeNow - lastTimeStateChanged >= buttonDebounceDuration) {
    byte buttonState = digitalRead(BUTTON_PIN);
    if(buttonState != previousButtonState) {
      lastTimeStateChanged = timeNow;
      previousButtonState = buttonState;
      if (buttonState == HIGH) {
        buttonPressed = true;
      }
    }
  }
}

void respond(String command) { //reactie op de call
  unsigned long timeNow = millis();
  if (command == "open") {
    setDoor("open");
    setLed("GREEN");
    setBuzzer("1000:1000");
    setLCD("0:0:Door is opened");
    while ((millis() - timeNow) < 10000) {}
    setDoor("close");
    setLed("YELLOW");
    setBuzzer("3000:1000");
    setLCD("0:0:Press button");
    setLCD("7:1:to call.");
    Serial.println("closed");
  } else if ( command = "deny") {
    unsigned long timeNow = millis();
    setLed("RED");
    setBuzzer("2000:2000");
    setLCD("0:0:Access denied");
    while ((millis() - timeNow) < 5000) {}
    setLed("YELLOW");
    setLCD("0:0:Press button");
    setLCD("7:1:to call.");
    Serial.println("waiting");   
  } else {
    // do nothing
  }
}

void setDoorWaiting() {
  setDoor("close");
  setLed("YELLOW");
  setLCD("0:0:Press button");
  setLCD("7:1:to call.");
  Serial.println("waiting");
}

void setup() {
  // voor de communicatie
  Serial.begin(115200); 
  // voor de button
  pinMode(BUTTON_PIN,INPUT);
  previousButtonState = digitalRead(BUTTON_PIN);
  buttonPressed = false;
  messageSent = false;
  // voor de servo
  servo.attach(SERVO_PIN);  
  servo.write(0);
  door = "closed";
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
  // voor de initial state van de deur
  setDoorWaiting(); 
  
  

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
    if (cmd.startsWith("led:")) {
      int colonIndex = cmd.indexOf(":");
      cmd.remove(0, colonIndex + 1);
      setLed(cmd);
    } else if (cmd.startsWith("lcd:")) {
      int colonIndex = cmd.indexOf(":");
      cmd.remove(0, colonIndex + 1);
      setLCD(cmd);
    } else if (cmd.startsWith("buzz:")) {
      int colonIndex = cmd.indexOf(":");
      cmd.remove(0, colonIndex + 1);
      setBuzzer(cmd);
    } else if (cmd.startsWith("door:")) {
      int colonIndex = cmd.indexOf(":");
      cmd.remove(0, colonIndex + 1);
      setDoor(cmd);
    } else if (cmd.startsWith("respond:")) {
      int colonIndex = cmd.indexOf(":");
      cmd.remove(0, colonIndex + 1);
      respond(cmd);
      buttonPressed = false;
      messageSent = false;          
    } else {
      // do nothing
    }
  }
  checkButton();
  if (buttonPressed == true ) {
    if ( !messageSent )  {
    Serial.println("caller"); // wordt gelezen door de pi
    messageSent = true;
    // buttonPressed wordt na de deurprocedure weer op false gezet
    // zodat niet tijdens de procedure opnieuw gebeld wordt.
    // idem voor messageSent.Dit zorgt ervoor dat eenmaal de boodschap wordt
    // verstuurd en niet in elke loop.
    }
  } 
}
