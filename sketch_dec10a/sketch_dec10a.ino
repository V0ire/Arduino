#define key1 3 
#define key2 2 
#define key3 5 
#define key4 4 

//make membrane keys 4x1 array
int keys[] = {key1, key2, key3, key4};
const int numKeys = sizeof(keys) / sizeof(keys[0]); 

void setup() {
  Serial.begin(9600);
  // Set up keys to Input Pullup
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