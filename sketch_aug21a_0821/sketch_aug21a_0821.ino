#include <ArduinoBLE.h>

#define SENSOR_PIN A0
#define SEND_INTERVAL_MS 500   // ← 전송 주기(ms). 원하면 100~1000 등으로 바꿔도 됨

BLEService sensorService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLECharacteristic sensorChar(
  "19B10002-E8F2-537E-4F6C-D104768A1214",
  BLENotify | BLERead,
  32
);

unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);

  if (!BLE.begin()) {
    while (1) {}
  }

  BLE.setLocalName("Nano33BLE-Sensor");
  BLE.setDeviceName("Nano33BLE-Sensor");
  BLE.setAdvertisedService(sensorService);

  sensorService.addCharacteristic(sensorChar);
  BLE.addService(sensorService);

  sensorChar.writeValue((const uint8_t*)"sensor: 0\n", 10); // 초기 표시값
  BLE.advertise();
}
//my name is sunny
void loop() {
  BLEDevice central = BLE.central();
  if (!central) return;

  while (central.connected()) {
    unsigned long now = millis();
    if (now - lastSend >= SEND_INTERVAL_MS) {
      lastSend = now;

      int val = analogRead(SENSOR_PIN);           // 0~1023 (10-bit ADC)
      char msg[32];
      int n = snprintf(msg, sizeof(msg), "sensor: %d\n", val);
      sensorChar.writeValue((const uint8_t*)msg, n); // ASCII Notify 전송

      // 필요하면 IDE 시리얼 모니터에서도 확인
      // Serial.print(msg);
    }
  }
}
