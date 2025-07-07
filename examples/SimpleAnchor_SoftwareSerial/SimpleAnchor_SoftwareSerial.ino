#include <RYUW122_UWB.h>
#include <SoftwareSerial.h>

#define RYUW122_SERIAL_TX 8
#define RYUW122_SERIAL_RX 9
#define RYUW122_RESET_PIN 12

SoftwareSerial ryuwSerial(RYUW122_SERIAL_RX, RYUW122_SERIAL_TX); // RX, TX
RYUW122_UWB uwb(ryuwSerial); // Pass SoftwareSerial to the driver

void setup() {
  Serial.begin(115200); // Serial for debug output
  ryuwSerial.begin(115200); // Serial for RYUW122

  Serial.println("RYUW122 example: Simple anchor using SoftwareSerial");

  bool module = uwb.begin(RYUW122_RESET_PIN); // Hardware reset is recommended
  if (module) {
    Serial.println("Module online!");
  } else {
    while (1) {
      Serial.println("Module offline");
      delay(500);
    }
  }
}

void loop() {
  RYUW122_MessageInfo info;

  // Send message "TEST_MSG" to tag with address "DAVID123"
  uwb.sendMessage("DAVID123", "TEST_MSG");

  if (uwb.receiveMessage(info)) {
    Serial.print("Tag address: ");
    Serial.println(info.address);

    Serial.print("Payload length: ");
    Serial.println(info.payloadLength);

    Serial.print("Payload: ");
    Serial.println(info.payload);

    Serial.print("Distance: ");
    Serial.print(info.distance);
    Serial.println(" cm");

    Serial.println();
  } else {
    Serial.println("Anchor did not receive a response from tag");
  }

  delay(1000); // Wait 1 second before sending next message
}
