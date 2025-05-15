#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define BUTTON_PIN 4

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
bool oldButtonState = HIGH;

#define SERVICE_UUID        "b9e2a522-08cc-4ff0-9ff4-569377be33d8"
#define CHARACTERISTIC_UUID "2da79697-0701-4a05-bb6a-433c7424a15a"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("✅ Client connected");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("⚠️ Client disconnected. Restarting advertising…");
    BLEDevice::getAdvertising()->start();  // 🔁 광고 재시작
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  BLEDevice::init("ESP32-Button-Server");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();

  Serial.println("🚀 BLE Server ready and advertising…");
}

void loop() {
  bool buttonState = digitalRead(BUTTON_PIN);
  if (deviceConnected && buttonState != oldButtonState) {
    oldButtonState = buttonState;
    String message = buttonState == LOW ? "Pressed" : "Released";

    pCharacteristic->setValue(message.c_str());
    pCharacteristic->notify();
    Serial.println("📤 Sent: " + message);
  }
  delay(100);
}
