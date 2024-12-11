#define key1 2 // Connect wire 1 to pin 2
#define key2 3 // Connect wire 2 to pin 3
#define key3 5 // Connect wire 3 to pin 5
#define key4 4 // Connect wire 4 to pin 4

int keys[] = {key1, key2, key3, key4}; // Array of key pins
const int numKeys = sizeof(keys) / sizeof(keys[0]); // Number of keys

void setup() {
  Serial.begin(9600);
  // Set all keys as input with pull-up resistors
  for (int i = 0; i < numKeys; i++) {
    pinMode(keys[i], INPUT_PULLUP);
  }
}

void loop() {
  for (int i = 0; i < numKeys; i++) {
    int keyState = digitalRead(keys[i]); // Read the state of the current key
    if (!keyState) { // If the key is pressed
      Serial.print("key ");
      Serial.print(i + 1); // Key numbers start from 1
      Serial.println(" is pressed");
    }
  }
  delay(100); // Debounce delay
}