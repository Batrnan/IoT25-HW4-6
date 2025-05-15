#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

const int LED_PIN = 2;

BLECharacteristic* pCharacteristic;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
     String rxValue = String(pCharacteristic->getValue().c_str());

      if (rxValue.length() > 0) {
        Serial.println(rxValue);
        if (rxValue == "led_on") {
          digitalWrite(LED_PIN, HIGH);
        } else if (rxValue == "led_off") {
          digitalWrite(LED_PIN, LOW);
        }
      }
    }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  BLEDevice::init("박성재-BLE-LED");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID,
                    BLECharacteristic::PROPERTY_READ |
                    BLECharacteristic::PROPERTY_WRITE
                  );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Send 'led_on' or 'led_off'");
  
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("Waiting for a BLE client to connect...");
}

void loop() {
  delay(1000);
}
