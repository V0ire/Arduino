#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD with the I2C address (e.g., x27) and dimensions (16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

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
    // Initialize the LCD  
    lcd.begin();
    lcd.backlight(); // Turn on the backlight  
    Serial.begin(9600); // Initialize serial communication for user input  
    for (int i = 0; i < numKeys; i++) {
    pinMode(keys[i], INPUT_PULLUP);
}
}
void loop() {

    // Check if all elements are 1  
    /*if (allOnes()) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Error");
        return; // Exit the loop if all are 1  
    }*/
    // Get user input
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