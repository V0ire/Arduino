#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define car struct
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

// Membrane Keys setup
#define key1 3 
#define key2 2 
#define key3 5 
#define key4 4 

int keys[] = {key1, key2, key3, key4};
const int numKeys = sizeof(keys) / sizeof(keys[0]); 
bool myArray[4] = {0, 0, 0, 0};
int test = 0;
int* t = test;

void setup() {
    lcd.begin();
    lcd.backlight();
    Serial.begin(9600);
    rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    


    for (int i = 0; i < numKeys; i++) {
        pinMode(keys[i], INPUT_PULLUP);
    }
}

void loop() {
  displayArray();
  carEntry();
    

}
void carEntry() {
    if (spotCheck()) {
        Serial.println("Sorry, parking is full.");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sorry, parking");
        lcd.setCursor(0, 1);
        lcd.print("is full!");
        delay(3000);
        return;
    }

    for (int choice = 0; choice < numKeys; choice++) {
        if (!digitalRead(keys[choice])) {
            if (myArray[choice] == 0) {
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
                return;
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
}
void carExit() {
    if (!spotCheck()) {
        Serial.println("Error! Parking is empty");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Error! Parking");
        lcd.setCursor(0, 1);
        lcd.print("is empty!");
        delay(3000);
        return;
    }

    for (int choice = 0; choice < numKeys; choice++) {
        if (!digitalRead(keys[choice])) {
            if (myArray[choice] == 1) {
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
                return;
            } else {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Spot Is Empty!");
                delay(2000);
            }
        }
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
