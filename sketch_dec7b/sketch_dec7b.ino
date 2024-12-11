#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD with the I2C address (e.g., x27) and dimensions (16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Create a boolean array of size 4, initialized to  (false)
bool myArray[4] = {0, 0, 0, 0};

void setup() {
    // Initialize the LCD  
    lcd.begin();
    lcd.backlight(); // Turn on the backlight  
    Serial.begin(9600); // Initialize serial communication for user input  
    displayArray();
}

void loop() {
    // Check if all elements are 1  
    /*if (allOnes()) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Error");
        return; // Exit the loop if all are 1  
    }*/

    // Prompt user for input  
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Choose 1-4:");

    while (true) {
        if (Serial.available() > 0 ) {
            int choice = Serial.parseInt(); // Read user input

            // Validate input  
            if (choice < 1 || choice > 4) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Invalid! 1-4");
                delay(2000);
                displayArray();
                continue; // Go back to display array  
            }

            // Check if the chosen index is already 1  
            /*if (myArray[choice - 1] == 1) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Try again");
                delay(200);
                displayArray();
                continue; // Go back to display array  
            }*/
            if (myArray[*ptr - 1] == 1) {
                myArray[choice - 1] = 0;
                displayArray();
                delay(2000);
                continue; // Go back to display array  
            }

            // Set the chosen index to 1  
            myArray[choice - 1] = 1;
            displayArray();
            delay(2000);
            break; // Exit the input loop  
        }
    }
}

void displayArray() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("A B C D");

    lcd.setCursor(0, 1); // Set cursor to the second row  
    for (int i = 0; i < 4; i++) {
        char spot = i+65;
        lcd.print(spot);
        lcd.print("=");
        lcd.print(myArray[i]);
        lcd.print(" "); // Print space between values  
    }
}

bool spotCheck() {
    for (int i = 0; i < 4; i++) {
        if (myArray[i] == 0 ) {
            return false; // Return false if any element is  0  
        }
    }
    return true; // Return true if all elements are 1  
}