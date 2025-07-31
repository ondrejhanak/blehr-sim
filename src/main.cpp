//
//  Created by Ondrej Hanak on 30.07.2025.
//

#include <NimBLEDevice.h>

void startAdvertising();

NimBLEServer *pServer = nullptr;
NimBLECharacteristic *pHRMChar = nullptr;
NimBLEAdvertising *pAdvertising = nullptr;
bool deviceConnected = false;

class HRMServerCallbacks : public NimBLEServerCallbacks
{
  void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override
  {
    deviceConnected = true;
    Serial.println("Client connected");
  }
  void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override
  {
    deviceConnected = false;
    Serial.println("Client disconnected");
    startAdvertising();
  }
};

void startAdvertising()
{
  pAdvertising->start();
  Serial.println("Started advertising");
}

uint16_t getNextHRValue()
{
  static float phase = 0;
  static float waveSpeed = 0.05;
  static uint16_t baseHR = 75;
  static bool inSpike = false;
  static uint16_t spikeBPM = 0;
  static uint16_t spikeDuration = 0;

  phase += waveSpeed;
  if (phase >= 2 * PI)
    phase -= 2 * PI;

  // Occasionally change wave speed
  if (random(0, 5) == 0)
  {
    waveSpeed = 0.04f + random(0, 30) / 1000.0;
  }

  // Occasionally adjust baseline HR
  if (random(0, 15) == 0)
  {
    baseHR = 70 + random(0, 6);
  }

  // Occasionally trigger a spike
  if (!inSpike && random(0, 20) == 0)
  {
    inSpike = true;
    spikeBPM = random(200, 400);
    spikeDuration = random(5, 10);
  }

  uint16_t bpm = baseHR + (uint16_t)(20 * sin(phase));
  if (inSpike)
  {
    bpm = spikeBPM + random(-5, 6);
    spikeDuration--;
    if (spikeDuration <= 0)
    {
      inSpike = false;
    }
  }

  return bpm;
}

void setup()
{
  Serial.begin(115200);
  NimBLEDevice::init("ESP32-HRM-Sim");
  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new HRMServerCallbacks());

  NimBLEService *pService = pServer->createService("180D");
  pHRMChar = pService->createCharacteristic("2A37", NIMBLE_PROPERTY::NOTIFY);

  // Configuration descriptor for notifications
  NimBLEDescriptor *cccd = new NimBLEDescriptor("2902", NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE, 2, pHRMChar);
  pHRMChar->addDescriptor(cccd);
  pService->start();

  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID("180D");
  startAdvertising();
}

void loop()
{
  if (!deviceConnected)
  {
    return;
  }
  uint16_t hr = getNextHRValue();
  uint8_t hrmData[3];
  size_t len = 0;

  if (hr <= 255)
  {
    hrmData[0] = 0x00; // 8b HR value
    hrmData[1] = hr;
    len = 2;
  }
  else
  {
    hrmData[0] = 0x01; // 16b little endian HR value
    hrmData[1] = hr & 0xFF;
    hrmData[2] = hr >> 8;
    len = 3;
  }

  pHRMChar->setValue(hrmData, len);
  pHRMChar->notify();
  Serial.printf("Sent value %d\n", hr);
  delay(1000);
}