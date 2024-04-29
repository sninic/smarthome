#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <driver/adc.h>

#define SERVICE_UUID        "c5669482-ac2a-40f9-8a50-8ad39f48c44d"
#define CHARACTERISTIC_UUID "ea3e3bd6-2f3f-4600-a2fc-9053895469ca"
#define ADC_CHANNEL         ADC1_CHANNEL_3  // GPIO 4 connected to ADC1 channel 3

// RGB LED Pins - Adjust these to the pins you're using
const int pinR = 16; // Example GPIO pin for Red
const int pinG = 17; // Example GPIO pin for Green
const int pinB = 18; // Example GPIO pin for Blue

// PWM Channels
const int channelR = 0;
const int channelG = 1;
const int channelB = 2;

// PWM Frequency and Resolution
const int freq = 5000;
const int resolution = 8; // 8-bit resolution gives us 0-255 range

// Current color values
int currentRed = 0;
int currentGreen = 0;
int currentBlue = 0;
bool pr_relation = false; 

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;

void turnLEDOff() {
    // Turn off the LED by setting PWM to max due to common anode configuration
    Serial.println("Turning LED off");
    ledcWrite(channelR, 0);
    ledcWrite(channelG, 0);
    ledcWrite(channelB, 0);
}

void turnLEDOn(long color) {
    Serial.println("Turning LED on");
    // Invert values for common anode LED
    currentRed = ((color >> 16) & 0xFF);
    currentGreen = ((color >> 8) & 0xFF);
    currentBlue = ((color) & 0xFF);

    ledcWrite(channelR, currentRed);
    ledcWrite(channelG, currentGreen);
    ledcWrite(channelB, currentBlue);
}

void adjustBrightness(int scale) {

    // Calculate new PWM values while preserving color
    int pwmRed = (currentRed * scale) / 100;
    int pwmGreen = (currentGreen * scale) / 100;
    int pwmBlue = (currentBlue * scale) / 100;

    // Write new PWM values
    ledcWrite(channelR, pwmRed);
    ledcWrite(channelG, pwmGreen);
    ledcWrite(channelB, pwmBlue);
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Client Connected");
    }

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Client Disconnected");
      pServer->startAdvertising(); // Restart advertising
      turnLEDOff();
    }
};

class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.find("Lighting:") != std::string::npos) {
            size_t actionStart = value.find(":") + 1;
            size_t actionEnd = value.find(":", actionStart);
            std::string action = value.substr(actionStart, actionEnd - actionStart);
            
            if (action == "slider") {
                size_t brightnessStart = actionEnd + 1; // Start of brightness value
                std::string brightnessStr = value.substr(brightnessStart, value.length() - brightnessStart);
                int brightness = strtol(brightnessStr.c_str(), NULL, 10); // Convert string to integer
                Serial.print("Slider brightness value received: ");
                Serial.println(brightness);
                // Now you might want to adjust the LED brightness according to this value
                adjustBrightness(brightness);
            } else if(action == "toggle") {
                size_t tog = actionEnd + 1; // Start of brightness value
                std::string togstr = value.substr(tog, value.length() - tog);
                Serial.print("Toggle PhotoResistor Relationship: ");
                Serial.println(togstr.c_str());
                if (togstr == "yes") {
                  pr_relation = true;
                } else {
                  pr_relation = false;
                }

            }
            else {
                size_t colorStart = value.find("#");
                if (colorStart != std::string::npos) {
                    std::string colorStr = value.substr(colorStart + 1); // Exclude '#'
                    long color = strtol(colorStr.c_str(), NULL, 16); // Convert hex string to long
                    if (action == "on") {
                        turnLEDOn(color);
                    } else if (action == "off") {
                        turnLEDOff();
                    }
                }
            }
        }
    }
    
};


void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32_BLE_Server_lighting");

  // Configure ADC
  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_11);

  ledcSetup(channelR, freq, resolution);
  ledcAttachPin(pinR, channelR);
  ledcSetup(channelG, freq, resolution);
  ledcAttachPin(pinG, channelG);
  ledcSetup(channelB, freq, resolution);
  ledcAttachPin(pinB, channelB);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE service is running. Waiting for clients...");
}

void loop() {
  int adcValue = adc1_get_raw(ADC_CHANNEL);
  int brightness = map(adcValue, 0, 2300, 0, 100);
  if (pr_relation) {
    adjustBrightness(100-brightness);
  }
  // Serial.print("ADC Value: ");
  // Serial.println(brightness);
  String strValue = String(brightness);
  pCharacteristic->setValue(strValue.c_str());
  pCharacteristic->notify();
  delay(1000);  // Update every second
}
