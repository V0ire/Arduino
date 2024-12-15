// Created by Simple Circuits 

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h> 
#include <RTClib.h>

LiquidCrystal_I2C lcd(0x27,16,2);  
RTC_DS3231 rtc;
// Membrane Keys setup
#define key1 3 
#define key2 2 
#define key3 5 
#define key4 4 
int keys[] = {key1, key2, key3, key4};
const int numKeys = sizeof(keys) / sizeof(keys[0]); 

struct car {
    int entryTime;   // Entry time in minutes since midnight
    int exitTime;    // Exit time in minutes since midnight
    float totalTime; // Total time in hours
    float totalPrice; // Total price in currency

    void calculate() {
        totalTime = (exitTime - entryTime) / 60.0; // Convert minutes to hours
        totalPrice = totalTime * 1000;            // Price per hour is 1000
    }
};

// Dynamic car slots and tracking
car* carSlots[4] = {nullptr, nullptr, nullptr, nullptr};
int carCounter[4] = {0, 0, 0, 0};


bool myArray[4] = {0, 0, 0, 0};

Servo myservo;

int IR1 = 6;
int IR2 = 8;
int flag1 = 0;
int flag2 = 0;



void setup() {

  Serial.begin(9600); 

    lcd.begin(); //initialize the lcd

    lcd.backlight(); //open the backlight

    rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    for (int i = 0; i < numKeys; i++) {
        pinMode(keys[i], INPUT_PULLUP);
    }
 
pinMode(IR1, INPUT);
pinMode(IR2, INPUT);
//servo
myservo.attach(7);
myservo.write(100);
//lcd
lcd.setCursor (0,0);
lcd.print("     ARDUINO    ");
lcd.setCursor (0,1);
lcd.print(" PARKING SYSTEM ");
delay (2000);
lcd.clear();  

}

void loop() {
    // IR1 detection for car entry
    if (digitalRead(IR1) == LOW && flag1 == 0 && flag2 == 0) {
        if (!spotCheck()) {
            flag1 = 1; // Set flag to indicate IR1 detected
            Serial.println("!spotCheck");
            if (flag2 == 0) { // Ensure IR2 is not triggered
                Serial.println("CarEntry");
                displayArray();
                carEntry();

            }
        } 
        else {
            Serial.println("Sorry, parking is full.");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Sorry, parking");
            lcd.setCursor(0, 1);
            lcd.print("is full!");
            delay(3000);
            return;
        }
    }
    else if (digitalRead(IR1) == LOW && flag1 == 0 && flag2 == 1) {
      flag1 = 1;
    }

    // IR2 detection for car exit or closing servo after entry
    if (digitalRead(IR2) == LOW && flag2 == 0 && flag1 == 0) {
        if (!spotCheck()) {
            // Normal exit process
            flag2 = 1;
            Serial.println("CarExit");
            displayArray();
            carExit();

        } else if (spotCheck()) {
            Serial.println("Error! Parking is empty");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Error! Parking");
            lcd.setCursor(0, 1);
            lcd.print("is empty!");
            delay(3000);
            return;
        }
    }
    if (digitalRead(IR2) == LOW && flag2 == 0 && flag1 == 1){
            // If flag1 is set (entry process) and IR2 is detected
            flag2 = 1; // Set flag for IR2 detection
            delay(500); // Small delay for stability
    }

    // Reset flags after both IR1 and IR2 have been triggered
    if (flag1 == 1 && flag2 == 1) {
        myservo.write(100); // Close the servo
        Serial.println("Servo closed");
        delay(1000);
        flag1 = 0;
        flag2 = 0;
        Serial.println("Flags reset");
    }
    lcd.clear();

    Serial.println("main");
    delay(1000);
}

void carEntry() {
    while (true) { // Loop indefinitely until a valid key is pressed
        for (int choice = 0; choice < numKeys; choice++) {
            if (!digitalRead(keys[choice])) { // Check if a key is pressed
                if (myArray[choice] == 0) { // Check if the spot is available
                    myArray[choice] = 1;

                    // Create new car entry
                    carCounter[choice]++;
                    String carName = String((char)(choice + 'A')) + carCounter[choice];
                    Serial.print("Spot assigned to car: ");
                    Serial.println(carName);

                    // Allocate memory for the car and store entry time
                    carSlots[choice] = new car;
                    DateTime now = rtc.now();
                    carSlots[choice]->entryTime = now.hour() * 60 + now.minute();

                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Car Entered: ");
                    lcd.print(carName);
                    delay(3000);
                    myservo.write(0); // Open the servo gate
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Welcome!");
                    delay(2000);
                    lcd.clear();
                    return; // Exit the function once the spot is assigned
                } else {
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Spot not");
                    lcd.setCursor(0, 1);
                    lcd.print("available!");
                    delay(2000);
                }
            }
        }
        delay(100); // Add a small delay to prevent excessive CPU usage
    }
}

void carExit() {
    while (true) { // Loop indefinitely until a valid key is pressed
        for (int choice = 0; choice < numKeys; choice++) {
            if (!digitalRead(keys[choice])) { // Check if a key is pressed
                if (myArray[choice] == 1) { // Check if the spot is occupied
                    myArray[choice] = 0;

                    // Access the car and calculate exit details
                    car* currentCar = carSlots[choice];
                    DateTime now = rtc.now();
                    currentCar->exitTime = now.hour() * 60 + now.minute();
                    currentCar->calculate();

                    Serial.print("Car exited from spot ");
                    Serial.println((char)(choice + 'A'));
                    Serial.print("Total time: ");
                    Serial.print(currentCar->totalTime);
                    Serial.println(" hours");
                    Serial.print("Total price: ");
                    Serial.print(currentCar->totalPrice);
                    Serial.println(" units");

                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Spot ");
                    lcd.print((char)(choice + 'A'));
                    lcd.print(" Freed");
                    lcd.setCursor(0, 1);
                    lcd.print("Price: ");
                    lcd.print(currentCar->totalPrice);

                    // Free memory and reset slot
                    delete carSlots[choice];
                    carSlots[choice] = nullptr;
                    delay(3000);
                    myservo.write(0);
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Goodbye!");
                    delay(2000);
                    lcd.clear(); // Open the gate for exit
                    return; // Exit the function once the spot is freed
                } else {
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Spot is empty!");
                    delay(2000);
                }
            }
        }
        delay(100); // Add a small delay to prevent excessive CPU usage
    }
}


void displayArray() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Choose Spot:");

    lcd.setCursor(0, 1);
    for (int i = 0; i < 4; i++) {
        char spot = i + 'A';
        lcd.print(spot);
        lcd.print("=");
        lcd.print(myArray[i]);
        lcd.print(" ");
    }
    delay(1000);
}

bool spotCheck() {
    for (int i = 0; i < 4; i++) {
        if (myArray[i] == 0) {
            return false;
        }
    }
    return true;
}
