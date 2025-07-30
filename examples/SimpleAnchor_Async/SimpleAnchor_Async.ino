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

  // Attempt to send message asynchronously to tag with address "DAVID123"
  if (uwb.sendMessageAsync("DAVID123", "DST")) {
    Serial.println("Message sent successfully.");
  } else {
    Serial.println("Failed to send async message.");
  }

  // Check for asynchronous response
  RYUW122_MessageState state = uwb.receiveMessageAsyncAnchor(info);

  switch (state) {
    case MESSAGE_RECEIVED:
      Serial.println("Message received!");
      Serial.print("From tag address: ");
      Serial.println(info.address);

      Serial.print("Payload length: ");
      Serial.println(info.payloadLength);

      Serial.print("Payload: ");
      Serial.println(info.payload);

      Serial.print("Measured distance: ");
      Serial.print(info.distance);
      Serial.println(" cm");
      Serial.println();
      break;

    case MESSAGE_WAITING:
      Serial.println("Awaiting response...");
      break;

    case MESSAGE_TIMEOUT:
      Serial.println("Response timeout.");
      break;

    case MESSAGE_PARSE_ERROR:
      Serial.println("Failed to parse received message.");
      break;

    case MESSAGE_NOT_REQUESTED:
      Serial.println("No async message was initiated.");
      break;

    default:
      Serial.println("Unknown message state.");
      break;
  }

  delay(1000); // Wait before sending next message
}