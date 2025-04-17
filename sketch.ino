
#include <LiquidCrystal_I2C.h.h>
#include <Servo.h>
#include <Wire.h>
#include <RTClib.h>

//declaration of pins........................
#define LDR_PIN A1
#define PIR_PIN 6
#define SECURITY_LIGHT 3
#define MOTION_LIGHT 4
#define HOME_LIGHT 5
#define LM35_PIN A0
#define SERVO_PIN 7
#define FAN_PIN 8

//creating object instances...........................
//////////////////////////////////////////////////////

Servo servo;
LiquidCrystal_I2C lcd(0x27, 20, 4);
RTC_DS3107 rtc;

//Initial Declaration.................................
......................................................

float temperature = 0.0;
int servoSpeed = 60;
DateTime now = rtc.now();

bool pirState = false;
int ldrThreshold = 500;
int ldrValue=0;

bool openHours = (now.hours() > 18 && now.minutes() >= 30 && now.hours() < 21);
bool openCurtain = (now.hours() >= 8 && now.hours() < 18);
bool fanOn = (temperature >= 30);


void setup() {

/*pinMode.............................................
......................................................*/

  pinMode(LDR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(LM35_PIN, INPUT);
  pinMode(SERVO_PIN, OUTPUT);
  pinMode(SECURITY_LIGHT, OUTPUT);
  pinMode(MOTION_LIGHT, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(FAN_PIN,OUTPUT);

  //initialisation.........................................
  /////////////////////////////////////////////////////////

  lcd.init();
  lcd.backlight();
  servo.attach(SERVO_PIN);
  servo.write(0) // initially the curtain should be closed...........................................
//////////////////////////////////
////display the temperature value initially on the screen//////////
  lcd.setCursor(0,2);
  lcd.print("TEMP");
  lcd.print(temperature);
}

void loop(){
  pirState=digitalRead(PIR_PIN);
  ldrValue=digitalRead(LDR_PIN);
  temperature=analogRead(LM35_PIN)*0.4888;
  
  displayTime();
  activateFan(bool fanOn);
  DrawCurtain(bool OpenCurtain);
  homeLight(bool OpenHours);
  motionLight(bool pirState);
  securityLight(int ldrValue);
  delay(200);
}
void displayTemperature(){
  temperature=analogRead(LM35_PIN)*0.4888;
  if(isnan(temperature)){
    lcd.setCursor(0,2);
    lcd.print("TEMP:");
    lcd.print("!");
  }
  else{
    lcd.setCursor(0,2);
    lcd.print("TEMP");
    lcd.print(temperature);
  }
}

void displayTime(){
  DateTime now = rtc.now();
  lcd.setCursor(2,0);
  lcd.print("HOME AUTOMATION");
  lcd.setCursor(5,1);
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
 
void activateFan() {
 if(fanOn)){
  digitalWrite(FAN_PIN,HIGH);
  lcd.setCursor(12,2);
  lcd.print("FAN:ON.");
  
 }
 else{
  digitalWrite(FAN_PIN,LOW);
  lcd.setCursor(12,2);
  lcd.print("FAN:OFF");
  
 }
}


void DrawCurtain() {
   if(openCurtain){
     int i = 0;
    for (i = 0; i <= 180; i++) {
       servo.write(i);
       delay(servoSpeed);
    }
    else{
      servo.write(0);
    }
   }
}


void homeLight(bool openHours) {
 if(openHours){
  digitalWrite(HOME_LIGHT,HIGH);
 }
 else{
  digitalWrite(HOME_LIGHT,LOW);
 }
}


void securtiyLight(int ldrVallue){
 if(ldrValue>ldrThreshold){
  digitalWrite(LDR_PIN,LOW);
 }
 else{
  digitalWrite(LDR_PIN,HIGH);
 }
}


void motionLight(bool pirState){
  if(pirState){
    digitalWrite(MOTION_LIGHT,HIGH);
    lcd.setCursor(3,2);
    lcd.print("MOTION DECTECTED")
  }
  else{
    digitalWrite(MOTION_LIGHT,LOW);
    lcd.SetCursor(3,3);
    lcd.print("..............");
  }
}