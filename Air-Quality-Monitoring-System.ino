// Define Blynk template ID, name, authentication token, and print settings
#define BLYNK_TEMPLATE_ID "TMPL66U_DFhpO"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitoring"
#define BLYNK_AUTH_TOKEN "saFCrN51Tnv1l0g3xZk7lr1mlXfclMhA"
#define BLYNK_PRINT Serial

// Include necessary libraries
#include <ESP8266WiFi.h>         // Library for ESP8266 Wi-Fi module
#include <BlynkSimpleEsp8266.h>  // Library for Blynk communication with ESP8266
#include <DHT.h>                 // Library for DHT temperature and humidity sensor
#include <LiquidCrystal_I2C.h>   // Library for LCD display

// Initialize LCD display with I2C address and dimensions
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define custom degree symbol for LCD display
byte degree_symbol[8] = {
  0b00111,
  0b00101,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

char auth[] = BLYNK_AUTH_TOKEN;
// Define Wi-Fi credentials
char ssid[] = "OPPO Reno4 Pro";  // Your WiFi name
char pass[] = "12345678";        // Your WiFi password

BlynkTimer timer;

// Define pin for gas sensor
int gas = A0;

// Define pin and type for DHT sensor
#define DHTPIN D5  // Connect Out pin to D5 on NODE MCU
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Function to send sensor data to Blynk server
void sendSensor() {
  // Read humidity and temperature from DHT sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if readings are valid
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Read gas sensor value and send to Blynk server
  int analogSensor = analogRead(gas);
  Blynk.virtualWrite(V2, analogSensor);  // Send gas sensor value to virtual pin V2
  Blynk.virtualWrite(V0, t);             // Send temperature to virtual pin V0
  Blynk.virtualWrite(V1, h);             // Send humidity to virtual pin V1
}

// Setup function - runs once at startup
void setup() {
  Serial.begin(115200);                   // Initialize serial communication
  Blynk.begin(auth, ssid, pass);          // Connect to Blynk server
  dht.begin();                            // Initialize DHT sensor
  timer.setInterval(30000L, sendSensor);  // Set timer interval to send sensor data every 30 seconds

  // Initialize LCD display
  lcd.init();
  lcd.backlight();  // Turn on LCD backlight
  lcd.setCursor(3, 0);
  lcd.print("Air Quality");
  lcd.setCursor(3, 1);
  lcd.print("Monitoring");
  delay(2000);
  lcd.clear();
}

// Loop function - runs repeatedly
void loop() {
  Blynk.run();        // Run Blynk communication
  timer.run();        // Run timer for sending sensor data
  checkConnection();  // Check connection status with Blynk server

  // Read and display temperature on LCD
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int gasValue = analogRead(gas);

  // Display temperature on LCD
  lcd.setCursor(0, 0);
  lcd.print("Temperature ");
  lcd.setCursor(0, 1);
  lcd.print(t);
  lcd.setCursor(6, 1);
  lcd.write(1);  // Display degree symbol
  lcd.setCursor(7, 1);
  lcd.print("C");
  delay(4000);
  lcd.clear();

  // Display humidity on LCD
  lcd.setCursor(0, 0);
  lcd.print("Humidity ");
  lcd.print(h);
  lcd.print("%");
  delay(4000);
  lcd.clear();

  // Display gas value and air quality on LCD
  lcd.setCursor(0, 0);
  lcd.print("Gas Value: ");
  lcd.print(gasValue);
  lcd.setCursor(0, 1);

  // Determine air quality based on gas value
  if (gasValue < 600) {
    lcd.print("Fresh Air");
  } else {
    lcd.print("Bad Air");
  }

  delay(4000);
  lcd.clear();

  // Log pollution alert to Blynk if gas value is high
  if (gasValue > 600) {
    Blynk.logEvent("pollution_alert", "Bad Air");
  }
}

// Function to check connection status with Blynk server
void checkConnection() {
  if (Blynk.connected()) {
    Serial.println("Blynk is connected");
  } else {
    Serial.println("Blynk is disconnected");
    // Attempt to reconnect to Blynk server
    if (Blynk.connect()) {
      Serial.println("Reconnected to Blynk server");
    } else {
      Serial.println("Reconnection failed");
    }
  }
}