#define BLYNK_TEMPLATE_ID "TMPL66U_DFhpO"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitoring"
#define BLYNK_AUTH_TOKEN "saFCrN51Tnv1l0g3xZk7lr1mlXfclMhA"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <DHT.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

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
char ssid[] = "OPPO Reno4 Pro";  // Your WiFi name
char pass[] = "12345678";           // Your WiFi password

BlynkTimer timer;

int gas = A0;

#define DHTPIN D5  // Connect Out pin to D5 on NODE MCU
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  int analogSensor = analogRead(gas);
  Blynk.virtualWrite(V2, analogSensor);
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(30000L, sendSensor);

  lcd.init();
  lcd.setCursor(3, 0);
  lcd.print("Air Quality");
  lcd.setCursor(3, 1);
  lcd.print("Monitoring");
  delay(2000);
  lcd.clear();
}

void loop() {
  Blynk.run();
  timer.run();
  checkConnection();

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