#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Stepper.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

const int steps_per_rev = 64;  

#define IN1 1
#define IN2 2
#define IN3 3
#define IN4 4

Stepper myStepper(steps_per_rev, IN1, IN3, IN2, IN4);

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;

void disable_motor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void goUp() {
    Serial.println("Going up");
    myStepper.step(steps_per_rev);
    delay(10);  // You might need to adjust this for timing and motor heating issues
    disable_motor();
}

void goDown() {
    Serial.println("Going down");
    myStepper.step(-steps_per_rev);
    delay(10);  // You might need to adjust this for timing and motor heating issues
    disable_motor();
}

void open() {
    Serial.println("opening");
    for (int i = 0; i < 100; i++) {
      myStepper.step(steps_per_rev);
      delay(10);  // You might need to adjust this for timing and motor heating issues
    }
    disable_motor();
}
void close() {
    Serial.println("closing");
    for (int i = 0; i < 100; i++) {
      myStepper.step(-steps_per_rev);
      delay(10);  // You might need to adjust this for timing and motor heating issues
    }
    disable_motor();
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("Client Connected");
    }

    void onDisconnect(BLEServer* pServer) {
      Serial.println("Client Disconnected");
      pServer->startAdvertising(); // Restart advertising
    }
};

class MyCharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.find("Blinds:up") != std::string::npos) {
        goUp();
      } else if (value.find("Blinds:down") != std::string::npos) {
        goDown();
      } else if (value.find("Blinds:open") != std::string::npos) {
        open();
      } else if (value.find("Blinds:close") != std::string::npos) {
        close();
      }
    }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32_BLE_Server");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  pService->start();
  pServer->getAdvertising()->start();
  Serial.println("BLE service is running. Waiting for clients...");

  myStepper.setSpeed(500); // 500 is too high for most steppers, typically RPM values are lower, like 5-15 RPM
}

void loop() {
  // This loop intentionally left empty
}
