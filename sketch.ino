

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>


#define PIR_PIN 9
#define LDR_PIN A3
#define SERVO_PIN 3
#define TEMP_PIN A0
#define house_light 5
#define security_light 10 
#define detection_light 6
#define ONE_WIRE_BUS 7

//creating object instances.....

Servo servo;
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27,16,2);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//initialization......
int ldrThreshold=500;

//moon shape to indicate night time.....
byte moon[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01111,
  B00111,
  B00010,
  B00000
};


byte sun[8] = {
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte warning[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte degreeSymbol[8] = {
  B00011,
  B00011,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
void setup() {
  //pinModes
  pinMode(LDR_PIN, INPUT);
  pinMode(TEMP_PIN, OUTPUT);
  pinMode(house_light, OUTPUT);
  pinMode(security_light, OUTPUT);


  //initialization
  Serial.begin(115200);
  servo.attach(SERVO_PIN);
  lcd.begin(16,2);
  lcd.backlight();
  servo.write(0);
  
  if(! rtc.begin()){
    lcd.clear();
    lcd.home();
    lcd.print("RTC Failed...");
  }
  drawCurtain();
  lighting();
}

void loop() {
  lcd.createChar(0, moon);
  lcd.createChar(1,sun);
  lcd.createChar(2,warning);
  lcd.createChar(3,degreeSymbol);
  displayTime();

  sensors.requestTemperatures();
  float tempValue = sensors.getTempCByIndex(0);
  Serial.println(tempValue);
  bool detection=digitalRead(PIR_PIN);
  Serial.println(detection);
  int ldr_value=analogRead(LDR_PIN);
  Serial.println(ldr_value);
  if (tempValue){
      lcd.setCursor(12,0);
      lcd.print(25);
      lcd.write(byte(3));
      lcd.print("C");
    }
  else{
    lcd.setCursor(15,0);
    lcd.print("!");
  }
  if(detection){
    detectionAlert(detection);
  }
  else{
    digitalWrite(detection_light, LOW);
    lcd.setCursor(15,1);
    lcd.print("");
  }

  if(ldr_value>ldrThreshold){
    digitalWrite(security_light, HIGH);
    lcd.setCursor(0,0);
    lcd.write(byte(0));
    lcd.setCursor(0,1);
    lcd.print("N");
  }
  else {
    digitalWrite(security_light, LOW);
    lcd.setCursor(0,0);
    lcd.write(byte(1));
    lcd.setCursor(0,1);
    lcd.print("D");
  }

    
}

void displayTime(){
  DateTime now = rtc.now();
  lcd.setCursor(4,0);
  lcd.print("WELCOME");
  lcd.setCursor(4,1);
  if(now.hour()<10){
    lcd.print("0");
  }
  lcd.print(now.hour(),DEC);
  lcd.print(":");
  if(now.minute()<10){
    lcd.print("0");
  }
  lcd.print(now.minute(),DEC);
  lcd.print(":");
  if(now.second()<10){
    lcd.print("0");
  }
  lcd.print(now.second(),DEC);
  delay(1000);
}

void drawCurtain(){
  DateTime now=rtc.now();
  int servo_angular=0;
  int cursor_position=0;

  bool openHours=(now.hour()>=9&&now.hour()<18);
  

  //opening the curtains.....
  if(openHours){
    for(servo_angular=0;servo_angular<=180;servo_angular=servo_angular+20){
      servo.write(servo_angular);
      lcd.setCursor(0,0);
      lcd.print("Curtain");
      lcd.setCursor(0,1);
      lcd.print("Opening");
      if(servo_angular<180){
        cursor_position=servo_angular/20+8;
        Serial.println(cursor_position);
        lcd.setCursor(cursor_position,1);
        lcd.print(".");
      }
      delay(300);
    }
  }
  
  //closing the curtains....
  else{
     for(servo_angular=180;servo_angular>=0;servo_angular=servo_angular-20){
      servo.write(servo_angular);
      lcd.setCursor(0,0);
      lcd.print("Curtain");
      lcd.setCursor(0,1);
      lcd.print("Closing");
      if(servo_angular<180){
        cursor_position=servo_angular/20+8;
        Serial.println(cursor_position);
        lcd.setCursor(cursor_position,1);
        lcd.print(".");
      }
      delay(300);
    }
  }
}

void lighting(){

  DateTime now =rtc.now();
  bool lightHours=(now.hour()>=6&&now.minute()>=30&&now.hour()<21);
  Serial.println(lightHours);
  //lighting on the house lights....
  if(lightHours){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("House Lighting");
    lcd.setCursor(0,1);
    lcd.print("ON");
    delay(2000);
    digitalWrite(house_light, HIGH);
    lcd.clear();
  }
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("House Lighting");
    lcd.setCursor(0,1);
    lcd.print("OFF");
    delay(2000);
    digitalWrite(house_light, LOW);
    lcd.clear();
  }
}


void detectionAlert(bool detection){
  if(detection){
    lcd.setCursor(15,1);
    lcd.print("!");
    digitalWrite(detection_light, HIGH);
    delay(500);
    lcd.setCursor(15,1);
    lcd.write(byte(2));
    digitalWrite(detection_light, LOW);
    delay(500);
  }
 
}


