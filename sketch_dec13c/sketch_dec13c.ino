#include <Arduino.h>
#include <RTClib.h>

RTC_DS3231 rtc;

struct car {
    int entryTime;   // Entry time in minutes since midnight
    int exitTime;    // Exit time in minutes since midnight
    float totalTime; // Total time in hours
    float totalPrice; // Total price in currency

    // Function to calculate total time and price
    void calculate() {
        totalTime = (exitTime - entryTime) / 60.0; // Convert minutes to hours
        totalPrice = totalTime * 1000;            // Price per hour is 1000
    }
};

// Declare a global pointer to a car struct
car* myCar;

void setup() {
    Serial.begin(9600);

    // Initialize RTC
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, setting the time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    // Allocate memory for the car struct dynamically
    myCar = new car;

    // Initialize entry time
    entryTime();
}

void loop() {
    // Exit time and calculation
    exitTime();
    delay(10000); // Wait for 10 seconds before next calculation
}

void entryTime() {
    DateTime now = rtc.now();
    myCar->entryTime = now.hour() * 60 + now.minute(); // Store entry time in minutes since midnight
    Serial.print("Entry time: ");
    Serial.print(now.hour());
    Serial.print(":");
    Serial.println(now.minute());
}

void exitTime() {
    DateTime now = rtc.now();
    myCar->exitTime = now.hour() * 60 + now.minute(); // Store exit time in minutes since midnight

    // Calculate total time and price
    myCar->calculate();

    // Print details
    Serial.print("Exit time: ");
    Serial.print(now.hour());
    Serial.print(":");
    Serial.println(now.minute());

    Serial.print("Total time: ");
    Serial.print(myCar->totalTime);
    Serial.println(" hours");

    Serial.print("Total price: ");
    Serial.print(myCar->totalPrice);
    Serial.println(" units");
}
