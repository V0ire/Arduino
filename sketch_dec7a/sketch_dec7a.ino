#include <LiquidCrystal_I2C.h>

// Created by Simple Circuits 

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);   
#include <Servo.h> 
//membrane key 4x1 key define
#define key1 3 
#define key2 2 
#define key3 5 
#define key4 4 

Servo myservo;
int IR1 = 2;
int IR2 = 3;
bool slot[4] = {0, 0, 0, 0};          //Total number of parking Slots
int flag1 = 0;
int flag2 = 0;



void setup() {
//initializing lcd
  Serial.begin(9600); 

    lcd.begin(); //initialize the lcd

    lcd.backlight(); //open the backlight
//declare IR Sensor pinMode
pinMode(IR1, INPUT);

pinMode(IR2, INPUT);

//declare servo
myservo.attach(4);

myservo.write(100);

//Print welcoming text
lcd.setCursor (0,0);

lcd.print("     ARDUINO    ");

lcd.setCursor (0,1);

lcd.print(" PARKING SYSTEM ");

delay (2000);

lcd.clear();  

}
void arraycheck();
void entrycar(){ //Detect entry car
if(digitalRead (IR1) == LOW && flag1==0){
  if(slot>0){
    flag1=1; //Show parking slot
    lcd.setCursor (0,0);
    lcd.print("Parking Spot");
    lcd.setCursor (0,1);
    for (int i = 0 ; i < 4; i++) {
        lcd.print(slot[i]);
        lcd.print(" "); // Print space between values  
    }
    if(flag2==0){myservo.write(0); Slot = Slot-1;}
}
else{
lcd.setCursor (0,0);
lcd.print("    SORRY :(    ");  
lcd.setCursor (0,1);
lcd.print("  Parking Full  "); 
delay (3000);
lcd.clear(); 
}
}
}
void exitcar(){
  if(digitalRead (IR2) == LOW && flag2==0){flag2=1;

if(flag1==0){myservo.write(0); Slot = Slot+1;}
}
if(flag1==1 && flag2==1){

delay (1000);

myservo.write(100);

flag1=0, flag2=0;

}

lcd.setCursor (0,0);

lcd.print("    WELCOME!    ");

lcd.setCursor (0,1);

lcd.print("Slot Left: ");

lcd.print(Slot);

}


void loop(){ 
entrycar();
exitcar();
}

