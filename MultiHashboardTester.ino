#include <Wire.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <TFT_eSPI.h>
#include <SPI.h>

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Initialize TFT display
TFT_eSPI tft = TFT_eSPI();

// Create an instance of the server
AsyncWebServer server(80);

// Define the I2C pins
#define SDA_PIN 21
#define SCL_PIN 22

// Define the selector knob pin
#define SELECTOR_PIN 36

// Define hash board models
enum HashboardModel {
  ANTMINER_L3_PLUS,
  ANTMINER_S9,
  ANTMINER_T15,
  ANTMINER_S15,
  ANTMINER_T17,
  ANTMINER_S17,
  ANTMINER_T17_PLUS,
  ANTMINER_S17_PLUS,
  ANTMINER_T19,
  ANTMINER_S19,
  NUM_MODELS
};

// Current selected model
HashboardModel currentModel = ANTMINER_L3_PLUS;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize TFT display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);

  // Initialize selector knob
  pinMode(SELECTOR_PIN, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Configure server routes
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "Welcome to the Multi-hashboard Tester!");
  });

  // Start server
  server.begin();
}

void loop() {
  // Read selector knob value
  int selectorValue = analogRead(SELECTOR_PIN);

  // Determine the selected model based on the selector value
  HashboardModel selectedModel = static_cast<HashboardModel>(map(selectorValue, 0, 4095, 0, NUM_MODELS));

  // If the selected model has changed, update the display and start the test
  if (selectedModel != currentModel) {
    currentModel = selectedModel;
    updateDisplay();
    startTest();
  }

}

void updateDisplay() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);

  switch (currentModel) {
    case ANTMINER_L3_PLUS:
      tft.print("Antminer L3+");
      break;
    case ANTMINER_S9:
      tft.print("Antminer S9");
      break;
  }
}

void startTest() {
  if (currentModel == ANTMINER_L3_PLUS) {
    test_antminer_l3_plus();
  } else if (currentModel == ANTMINER_S9) {
    test_antminer_s9();
  }
}

void test_antminer_l3_plus() {
}

void test_antminer_s9() {
}

void send_write_command(uint8_t address, uint8_t register, uint8_t data) {
  Wire.beginTransmission(address);
  Wire.write(register);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t read_data(uint8_t address, uint8_t register) {
  Wire.beginTransmission(address);
  Wire.write(register);
  Wire.endTransmission(false);

  Wire.requestFrom(address, 1);
  if (Wire.available()) {
    return Wire.read();
  } else {
    return 0;
  }
}
