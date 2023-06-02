#include <LiquidCrystal_I2C.h>
#include "max6675.h"

// Defin pins
int reverseSwitch = 2;  // Push button za okrenuti smjer
int driverPUL = 7;    // PUL- pin
int driverDIR = 6;    // DIR- pin
int spd = A0;     // Potenciometar 
int brz=0;


int thermoDO = 8;
int thermoCS = 9;
int thermoCLK =10;
float temperature;

const int RELAY_PIN = 3;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

const int TEMP_THRESHOLD_UPPER = 190; 
const int TEMP_THRESHOLD_LOWER = 185;

int uvjet=0;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables 
int pd = 500;       // Pulse Delay period
int refresh = 0;
boolean setdir = LOW; // smjer motora 
// Interrupt Handler 
void revmotor (){
  setdir = !setdir; 
}
 
void setup() {
  Serial.begin(9600);
  lcd.init(); // initialize the lcd
  lcd.backlight(); 
      lcd.clear();                 
      lcd.setCursor(0, 0);         
      lcd.print("Zagrijavanje..."); 
  pinMode (driverPUL, OUTPUT);
  pinMode (driverDIR, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(reverseSwitch), revmotor, FALLING); 
  delay(500);
  
}

void loop(){
  loop_start();
  loop_radnja();
  }

void loop_start() { 
  temperature = thermocouple.readCelsius();
  while(temperature<TEMP_THRESHOLD_LOWER){ 
    temperature = thermocouple.readCelsius();
    lcd.setCursor(0, 1);
    lcd.print(temperature);
    lcd.print((char)223);
    lcd.print("C");
    delay(500);

    digitalWrite(RELAY_PIN, HIGH); //na upper 190 dojde do---->> dojde do 198,5

  }
  digitalWrite(RELAY_PIN, LOW); //gasi ga na 155 stupnjeva
  return;
}

void loop_radnja() { 
  refresh = 0;
  while(true){
    if(refresh%500==0){
    temperature = thermocouple.readCelsius();
         
      if((temperature<TEMP_THRESHOLD_UPPER)&&(uvjet==0)){                 //dok padne ispod 190 ---------->dojde do 178,5 ||||| manja od 200 
      digitalWrite(RELAY_PIN, HIGH); //pali grijanje

        if(temperature<185){
          uvjet=1;
        }
      }

      else if ((temperature>TEMP_THRESHOLD_UPPER)&&(uvjet==1)){           //dok prejde 190 ------------> dojde do 203,5 ||||| veca od 180
      digitalWrite(RELAY_PIN, LOW);

        if(temperature<195){
          uvjet=0;
        }
      }
      
    }
      pd = map(analogRead(spd)%1000,0,1023,2000,50);
      digitalWrite(driverDIR,setdir);
      digitalWrite(driverPUL,HIGH);
      delayMicroseconds(pd);
      digitalWrite(driverPUL,LOW);
      delayMicroseconds(pd);
      
   
    if(refresh == 1000) {
      brz=analogRead(spd);
      lcd.clear();                 
      lcd.setCursor(0, 0);         
      lcd.print(brz/10);
      lcd.print("% rada motora");
      refresh = 0;
      lcd.setCursor(0, 1);
      lcd.print(temperature);
      lcd.print((char)223);
      lcd.print("C");
    }
    refresh++;
    
  } 
}
