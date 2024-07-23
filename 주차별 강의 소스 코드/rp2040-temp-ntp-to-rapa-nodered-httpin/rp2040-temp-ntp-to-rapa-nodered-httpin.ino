#include <WiFiNINA.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

// Replace with your network credentials
char ssid[] = "iptime_shkim_24";
char pass[] = "ksh89377";

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 20000); // Update every 20 seconds

// HTTP Client setup
const char* serverAddress = "192.168.0.58"; // Replace with your Raspberry Pi's IP address
int port = 1880;
WiFiClient wifiClient;
HttpClient httpClient = HttpClient(wifiClient, serverAddress, port);

// Dummy function to get temperature. Replace with actual sensor reading code.
float getTemperature() {
  // Simulated temperature value
  return random(0,100);
}

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  randomSeed(analogRead(0));

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Initialize NTP Client
  timeClient.begin();
}

void loop() {
  // Update the NTP client
  timeClient.update();

  // Get the current date and time
  String formattedDate = timeClient.getFormattedTime();
  Serial.print("Current time: ");
  Serial.println(formattedDate);

  // Get the temperature
  float temperature = getTemperature();
  Serial.print("Temperature: ");
  Serial.println(temperature);

  // Create JSON object
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["datetime"] = formattedDate;
  jsonDoc["temperature"] = temperature;

  // Serialize JSON to string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Send HTTP POST request
  Serial.println("Sending HTTP POST request...");
  httpClient.beginRequest();
  httpClient.post("/temperature");
  httpClient.sendHeader("Content-Type", "application/json");
  httpClient.sendHeader("Content-Length", jsonString.length());
  httpClient.beginBody();
  httpClient.print(jsonString);
  httpClient.endRequest();

  // Get HTTP response
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  // Print HTTP response
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  // Wait for a minute before sending the next request
  delay(20000);
}

