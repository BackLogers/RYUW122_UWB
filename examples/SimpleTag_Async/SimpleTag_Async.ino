#include <RYUW122_UWB.h>

#define RYUW122_SERIAL_TX 8
#define RYUW122_SERIAL_RX 15
#define RYUW122_RESET_PIN 12

// Create UWB object using hardware Serial1
RYUW122_UWB uwb(Serial1);

void setup() {
  delay(500);
  Serial.begin(115200); // Serial for debug output
  Serial1.begin(115200, SERIAL_8N1, RYUW122_SERIAL_RX, RYUW122_SERIAL_TX); // Serial for RYUW122

  Serial.println("RYUW122 example: Simple Anchor Async");

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

  // Check for asynchronous message
  RYUW122_MessageState state = uwb.receiveMessageAsyncTag(info);

  switch (state) {
    case MESSAGE_RECEIVED:
      Serial.println("Message received!");

      Serial.print("Payload length: ");
      Serial.println(info.payloadLength);

      Serial.print("Payload: ");
      Serial.println(info.payload);

      Serial.println();
      break;

    case MESSAGE_WAITING:
      Serial.println("Awaiting message...");
      break;

    case MESSAGE_PARSE_ERROR:
      Serial.println("Failed to parse received message.");
      break;

    default:
      Serial.println("Unknown message state.");
      break;
  }

  delay(1000); // Wait before next message check
}