#include <RYUW122_UWB.h>

#define RYUW122_SERIAL_TX 8
#define RYUW122_SERIAL_RX 15
#define RYUW122_RESET_PIN 12

RYUW122_UWB uwb(Serial1); // You can also pass SoftwareSerial

void setup() {
  Serial.begin(115200); // Serial for debug output
  Serial1.begin(115200, SERIAL_8N1, RYUW122_SERIAL_RX, RYUW122_SERIAL_TX); // Serial for RYUW122

  Serial.println("RYUW122 example: Simple tag");

  bool module = uwb.begin(RYUW122_RESET_PIN); // Hardware reset is recommended
  if (module) {
    Serial.println("Module online!");
  } else {
    while (1) {
      Serial.println("Module offline");
      delay(500);
    }
  }

  uwb.setTagResponseMessage("HELLO"); // Set response message to "HELLO"
}

void loop() {
  RYUW122_MessageInfo info;

  // Check if message was received from anchor
  if (uwb.receiveMessage(info)) {
    Serial.print("Anchor address: ");
    Serial.println(info.address);

    Serial.print("Payload length: ");
    Serial.println(info.payloadLength);

    Serial.print("Payload content: ");
    Serial.println(info.payload);

    Serial.print("Estimated distance: ");
    Serial.print(info.distance);
    Serial.println(" cm");

    Serial.println();
  }
}