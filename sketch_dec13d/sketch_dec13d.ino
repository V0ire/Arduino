#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

// Initialize the LCD with the I2C address (e.g., x27) and dimensions (16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

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
car* myCar;
// Create a boolean array of size 4, initialized to  (false)
bool myArray[4] = {0, 0, 0, 0};

//Membrane Keys setup
#define key1 3 
#define key2 2 
#define key3 5 
#define key4 4 

//make membrane keys 4x1 array
int keys[] = {key1, key2, key3, key4};
const int numKeys = sizeof(keys) / sizeof(keys[0]); 


void setup() {
    //Serial connect for monitoring
    Serial.begin(9600);
    // Initializing all   
    lcd.begin();
    lcd.backlight();
    //RTC
    rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Uncomment the following line to set the RTC manually:
    // setRTC(2024, 12, 13, 22, 21, 0); // Example: 2024-12-13 10:30:00

    //loop pinmode for keypad
    for (int i = 0; i < numKeys; i++) {
    pinMode(keys[i], INPUT_PULLUP);
}
}
void loop() {
    displayArray();
    carEntry();
    

}

    
      

void carEntry(){
  if (spotCheck()) {
    Serial.println("Sorry, parking is full.");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sorry, parking");
    lcd.setCursor(0, 1);
    lcd.print("is full!");
    delay(3000); // Delay to let the user read the message
    return; // Exit function
  }

    for (int choice = 0; choice < numKeys; choice++) {
      int keyState = digitalRead(keys[choice]);
      if (!keyState) {
        if (myArray[choice] == 0) { // Check if the spot is available
          Serial.print("Key ");
          Serial.print(choice + 1);
          Serial.println(" is pressed. Spot assigned.");
          myArray[choice] = 1; //Fill the spot with 1
          displayArray();
          delay(2000);
        } else { // When spot choosen is not available
          lcd.clear();
          Serial.println("Spot not available!");
          lcd.setCursor(0, 0);
          lcd.print("Spot not");
          lcd.setCursor(0, 1);
          lcd.print("available!");
          delay(2000);
        }
        return; // Exit function
      }
    }   
    }
void carExit(){
 if (!spotCheck()) {
    Serial.println("Error! Parking is empty");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error! Parking");
    lcd.setCursor(0, 1);
    lcd.print("is empty!");
    delay(3000); // Delay to let the user read the message
    return; // Exit function
  }
    for (int choice = 0; choice < numKeys; choice++) {
      int keyState = digitalRead(keys[choice]);
      if (!keyState) {
        if (myArray[choice] == 1) { // Check if the spot is available
          Serial.print("Key ");
          Serial.print(choice + 1);
          Serial.println(" is pressed. Spot assigned.");
          myArray[choice] = 0; //Fill the spot with 1
          displayArray();
          delay(2000);
        } else { // When spot choosen is not available
          lcd.clear();
          Serial.println("Spot is empty!");
          lcd.setCursor(0, 0);
          lcd.print("Spot Is Empty!");
          delay(2000);
        }
        return; // Exit the function after processing input
      }
    }  
    }

void displayArray() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Choose Spot:");

    lcd.setCursor(0, 1); // Set cursor to the second row  
    for (int i = 0; i < 4; i++) {
        char spot = i+65;
        lcd.print(spot);
        lcd.print("=");
        lcd.print(myArray[i]);
        lcd.print(" "); // Print space between values  
    }
    delay(1000);
}

bool spotCheck() {
    for (int i = 0; i < 4; i++) {
        if (myArray[i] == 0 ) {
            return false; // Return false if any element is  0  
        }
    }
    return true; // Return true if all elements are 1  
}

