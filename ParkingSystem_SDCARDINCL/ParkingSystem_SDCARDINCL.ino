#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h> 
#include <RTClib.h>
#include <SD.h>

LiquidCrystal_I2C lcd(0x27,16,2);  
RTC_DS3231 rtc;
Servo myservo;

// SD card setup
#define SD_CS_PIN 10 // Adjust based on your microSD module's CS pin

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
int IR1 = 6;
int IR2 = 8;
int flag1 = 0;
int flag2 = 0;

void setup() {
    Serial.begin(9600); 
    lcd.begin(); // Initialize the LCD
    lcd.backlight(); // Enable the backlight
    rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    for (int i = 0; i < numKeys; i++) {
        pinMode(keys[i], INPUT_PULLUP);
    }
 
    pinMode(IR1, INPUT);
    pinMode(IR2, INPUT);

    // Servo setup
    myservo.attach(7);
    myservo.write(100);

    // LCD welcome screen
    lcd.setCursor (0,0);
    lcd.print("     ARDUINO    ");
    lcd.setCursor (0,1);
    lcd.print(" PARKING SYSTEM ");
    delay (2000);
    lcd.clear();  

    // Initialize SD card
    if (!SD.begin(SD_CS_PIN)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("SD init failed");
        Serial.println("SD card initialization failed!");
        while (true); // Halt if SD card is not detected
    }
    Serial.println("SD card initialized.");
    lcd.clear();
}

void loop() {
    // IR1 detection for car entry
    if (digitalRead(IR1) == LOW && flag1 == 0 && flag2 == 0) {
        if (!spotCheck()) {
            flag1 = 1; // Set flag to indicate IR1 detected
            if (flag2 == 0) { // Ensure IR2 is not triggered
                carEntry();
            }
        } 
        else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Sorry, parking");
            lcd.setCursor(0, 1);
            lcd.print("is full!");
            delay(3000);
            return;
        }
    }

    // IR2 detection for car exit or closing servo after entry
    if (digitalRead(IR2) == LOW && flag2 == 0 && flag1 == 0) {
        if (!spotCheck()) {
            flag2 = 1;
            carExit();
        } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Error! Parking");
            lcd.setCursor(0, 1);
            lcd.print("is empty!");
            delay(3000);
            return;
        }
    }

    // Reset flags after both IR1 and IR2 have been triggered
    if (flag1 == 1 && flag2 == 1) {
        myservo.write(100); // Close the servo
        delay(1000);
        flag1 = 0;
        flag2 = 0;
    }
    lcd.clear();
    delay(1000);
}

void carEntry() {
    while (true) {
        for (int choice = 0; choice < numKeys; choice++) {
            if (!digitalRead(keys[choice])) { 
                if (myArray[choice] == 0) {
                    myArray[choice] = 1;

                    carCounter[choice]++;
                    String carName = String((char)(choice + 'A')) + carCounter[choice];
                    carSlots[choice] = new car;
                    DateTime now = rtc.now();
                    carSlots[choice]->entryTime = now.hour() * 60 + now.minute();

                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Car Entered: ");
                    lcd.print(carName);
                    delay(3000);
                    myservo.write(0);

                    // Log entry to SD card
                    File dataFile = SD.open("cardata.txt", FILE_WRITE);
                    if (dataFile) {
                        dataFile.print(carName);
                        dataFile.print(",");
                        dataFile.println(carSlots[choice]->entryTime);
                        dataFile.close();
                        Serial.println("Entry logged to SD card.");
                    } else {
                        Serial.println("Error opening cardata.txt!");
                    }

                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Welcome!");
                    delay(2000);
                    lcd.clear();
                    return;
                }
            }
        }
        delay(100);
    }
}

void carExit() {
    while (true) {
        for (int choice = 0; choice < numKeys; choice++) {
            if (!digitalRead(keys[choice])) {
                if (myArray[choice] == 1) {
                    myArray[choice] = 0;

                    car* currentCar = carSlots[choice];
                    DateTime now = rtc.now();
                    currentCar->exitTime = now.hour() * 60 + now.minute();
                    currentCar->calculate();

                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Spot ");
                    lcd.print((char)(choice + 'A'));
                    lcd.print(" Freed");
                    lcd.setCursor(0, 1);
                    lcd.print("Price: ");
                    lcd.print(currentCar->totalPrice);

                    // Log exit to SD card
                    File dataFile = SD.open("cardata.txt", FILE_WRITE);
                    if (dataFile) {
                        dataFile.print((char)(choice + 'A'));
                        dataFile.print(",");
                        dataFile.print(currentCar->entryTime);
                        dataFile.print(",");
                        dataFile.print(currentCar->exitTime);
                        dataFile.print(",");
                        dataFile.print(currentCar->totalTime);
                        dataFile.print(",");
                        dataFile.println(currentCar->totalPrice);
                        dataFile.close();
                        Serial.println("Exit logged to SD card.");
                    } else {
                        Serial.println("Error opening cardata.txt!");
                    }

                    delete carSlots[choice];
                    carSlots[choice] = nullptr;
                    delay(3000);
                    myservo.write(0);
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Goodbye!");
                    delay(2000);
                    lcd.clear();
                    return;
                }
            }
        }
        delay(100);
    }
}

bool spotCheck() {
    for (int i = 0; i < 4; i++) {
        if (myArray[i] == 0) {
            return false;
        }
    }
    return true;
}
