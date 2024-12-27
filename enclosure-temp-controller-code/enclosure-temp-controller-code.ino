// Include the libraries:
#include <Adafruit_Sensor.h>
#include <LiquidCrystal.h>
#include <DHT.h>

//LCD initialization
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);
int lcdBacklightPin = A0;

//DHT initialization
#define DHTPIN 2 // Set DHT pin
#define DHTTYPE DHT11   // set DHT type 11 
DHT dht = DHT(DHTPIN, DHTTYPE); // Initialize DHT sensor for normal 16mhz Arduino:
int DHTResult = 0;
float h = 0;
float hic = 0;

//Potentiometer Initialization
int analogPin = A0; // Pin A0 
int potValue = 0;
int potScaledValue = 0;

// Relay Initialization
const int relayPin = 7; // Pin 7 for the relay
unsigned long previousMillis = 0; // Store the last time the relay changed state
const unsigned long interval = 10000; // Interval to change state (10 seconds)

void setup() {
  Serial.begin(9600); // Begin serial communication at a baud rate of 9600:
  lcd.begin(16, 2); //set the type of LCD display
  pinMode(lcdBacklightPin, OUTPUT);
  lcd.clear();
  lcd.home();
  lcd.print("Starting Up...");

  dht.begin(); // Setup DHT sensor

  pinMode(relayPin, OUTPUT); // Set relay pin as an output
  digitalWrite(relayPin, LOW); // Initialize the relay to be off

}

void loop() {

  delay(200); //makes this routine run at 50ms

  h = getHumidity();
  hic = getHeatIndex();

  //Serial.println(hif);
  //Serial.println(h);

  // Read the analog input on pin A0 
  potValue = analogRead(analogPin);
  // Scale the value to the range 30-60
  potScaledValue = map(potValue, 0, 1023, 15, 60);
  Serial.println(potScaledValue);

  // Print the setpoint
  lcd.home();
  lcd.print("Setpoint: ");
  lcd.print(round(potScaledValue));
  lcd.print(" \xDF");
  lcd.print("C");

  // Print the current temp
  lcd.setCursor(0, 1);
  lcd.print("T: ");
  lcd.print(round(hic));
  lcd.print("\xDF");
  lcd.print("C");

  //print the current humidity
  lcd.print("  H: ");
  lcd.print(round(h));
  lcd.print("%");

  //control the relay
  unsigned long currentMillis = millis(); // Get the current time in milliseconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Update the last time the relay changed state
    
    // Control logic to turn the relay on or off
    if (hic > potScaledValue) {
      digitalWrite(relayPin, HIGH); // Turn the relay on
    } else {
      digitalWrite(relayPin, LOW); // Turn the relay off
    }

  }
}

float getHumidity() {
  
  float h = dht.readHumidity(); // Read the humidity in %

  if (isnan(h)) { // Check if any reads failed and exit early (to try again):
    Serial.println("Failed to read from Humidity sensor!");
    return 0;
  }
  else {
    return h;
  }
}

float getHeatIndex() {
  
  float f = dht.readTemperature(true); // Read the temperature as Fahrenheit:

  if (isnan(f)) { // Check if any reads failed and exit early (to try again):
    Serial.println("Failed to read from temperature sensor!");
    return 0;
  }
  else {
    float hif = dht.computeHeatIndex(f, h); // Compute heat index in Fahrenheit (default):
    float hic = convertFtoC(hif);
    return hic;
  }
}

float convertFtoC(float fahrenheit) {
  float celsius = (fahrenheit - 32) * 5.0 / 9.0;
  return celsius;
}
