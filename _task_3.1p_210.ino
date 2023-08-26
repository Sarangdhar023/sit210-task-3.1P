#include <WiFiNINA.h>
#include <DHT.h>
//  WiFi name and wifi password
char wifiname[] = "Sarang";
char wifipass[] = "12345678";

// ThingSpeak settings
char server[] = "api.thingspeak.com";
String apiKey = "VP7RYTX8YAINQR65";

// Configuration of  pin and sensor
#define PIN 2
#define sensortype DHT11
DHT dht11(PIN, sensortype);

// Initializing the WiFi consumer
WiFiClient consumer;

void setup() {
  // communication with serial monitor at baud rate 
  Serial.begin(9600);

  // WiFi connection steps
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("... Searching WiFi to connect ...");
    WiFi.begin(wifiname, wifipass);
    delay(11000); // Adjust delay as needed
  }

  Serial.println("Connection is established to WIFI");
 dht11.begin();
}

void loop() {
  // Read Temp from DHT11 sensor
  float Temp = dht11.readTemperature();

  // Check if any reads failed and exit early (to try again)
  if (isnan(Temp)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Preparing data to send to ThingSpeak
  String data = "Api=" + apiKey;
  data += "&field1=" + String(Temp);

  // Make an HTTP POST request to ThingSpeak
  if (consumer.connect(server, 80)) {
    consumer.print("POST /update HTTP/1.1\r\n");
    consumer.print("Host: " + String(server) + "\r\n");
    consumer.print("Content-Type: application/x-www-form-urlencoded\r\n");
    consumer.print("Content-Length: " + String(data.length()) + "\r\n");
    consumer.print("\r\n");
    consumer.print(data);
    
    Serial.println("Data sent to ThingSpeak!");
  } else {
    Serial.println("Connection to ThingSpeak failed!");
  }

  // Wait for response and close the connection
  while (consumer.connected()) {
    if (consumer.available()) {
      char c = consumer.read();
      Serial.print(c);
    }
  }
  consumer.stop();

  // Wait before sending the next set of data
  delay(15000); // Adjust delay as needed
}