#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

DHT dht14(D4, DHT11);

const char* ssid = "MAX"; // Change to your Wi-Fi SSID
const char* password = "fullmax1"; // Change to your Wi-Fi password

ESP8266WebServer server(80);

const int ledPin = D6;  // Define the GPIO pin connected to your LED

void init_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup(void) {
  Serial.begin(115200);
  init_wifi();
  Serial.begin(9600);
  dht14.begin();
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  server.on("/", HTTP_GET, []() {
    String html = "<html><body><center><h1>Hello from ESP8266!</h1>";
    html += "<button id='turnOnButton'>Turn LED On</button>";
    html += "<button id='turnOffButton'>Turn LED Off</button>";
    html += "<p><h1 id='tempDisplay'>Temperature: -- C</h1></p>";
    html += "<p><h1 id='humidDisplay'>Humidity: -- %</h1></p></center>";
    html += "<script>";
    html += "document.getElementById('turnOnButton').onclick = function() { turnOn(); };";
    html += "document.getElementById('turnOffButton').onclick = function() { turnOff(); };";
    html += "function turnOn() {";
    html += "var xhr = new XMLHttpRequest();";
    html += "xhr.open('GET', '/led/on', true);";
    html += "xhr.send();";
    html += "}";
    html += "function turnOff() {";
    html += "var xhr = new XMLHttpRequest();";
    html += "xhr.open('GET', '/led/off', true);";
    html += "xhr.send();";
    html += "}";
    html += "function updateTempHumid() {";
    html += "var xhr = new XMLHttpRequest();";
    html += "xhr.open('GET', '/getTempHumid', true);";
    html += "xhr.onload = function() {";
    html += "var data = JSON.parse(xhr.responseText);";
    html += "document.getElementById('tempDisplay').innerText = 'Temperature: ' + data.temp + ' C';";
    html += "document.getElementById('humidDisplay').innerText = 'Humidity: ' + data.humid + ' %';";
    html += "};";
    html += "xhr.send();";
    html += "}";
    html += "setInterval(updateTempHumid, 5000);"; // Update every 5 seconds
    html += "updateTempHumid();"; // Initial update
    html += "</script>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/led/on", HTTP_GET, []() {
    digitalWrite(ledPin, HIGH);  // Turn LED on
    server.send(200, "text/plain", "LED is ON");
  });

  server.on("/led/off", HTTP_GET, []() {
    digitalWrite(ledPin, LOW);  // Turn LED off
    server.send(200, "text/plain", "LED is OFF");
  });

  server.on("/getTempHumid", HTTP_GET, []() {
    float temperature = dht14.readTemperature();
    float humidity = dht14.readHumidity();
    String json = "{\"temp\":" + String(temperature) + ",\"humid\":" + String(humidity) + "}";
    server.send(200, "application/json", json);
  });

  server.begin();
  Serial.println("HTTP server started");
}


void loop() {
  server.handleClient();
}
