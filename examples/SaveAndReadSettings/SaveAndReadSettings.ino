#include <RYUW122_UWB.h>

#define RYUW122_SERIAL_TX 8
#define RYUW122_SERIAL_RX 15
#define RYUW122_RESET_PIN 12

RYUW122_UWB uwb(Serial1); // You can also pass SoftwareSerial

void setup() {
  Serial.begin(115200); // Serial for debug output
  Serial1.begin(115200, SERIAL_8N1, RYUW122_SERIAL_RX, RYUW122_SERIAL_TX); // Serial for RYUW122

  Serial.println("RYUW122 example: Save and read module settings");

  bool module = uwb.begin(RYUW122_RESET_PIN); // Hardware reset is recommended
  if (module) {
    Serial.println("Module online!");
  } else {
    while (1) {
      Serial.println("Module offline");
      delay(500);
    }
  }

  if (saveSettings()) {
    Serial.println("Settings saved successfully");
  } else {
    Serial.println("Failed to save settings");
  }
}

void loop() {
  readSettings();
  delay(5000);
}

bool saveSettings() {
  bool success = true;
  success &= uwb.setMode(MODE_ANCHOR);
  success &= uwb.setBaudRate(BAUD_115200);
  success &= uwb.setChannel(CHANNEL_6489_6_MHz);
  success &= uwb.setBandwidth(BANDWIDTH_850_Kbps);
  success &= uwb.setNetworkID("REYAX123");
  success &= uwb.setAddress("REYAX003");
  success &= uwb.setPassword("FABC0002EEDCAA90FABC0002EEDCAA90");
  success &= uwb.setTagParameters(0, 0);
  success &= uwb.setCalibrationDistance(0);
  return success;
}

void readSettings() {
  RYUW122_Mode mode;
  uwb.getMode(mode);
  Serial.print("Module mode: ");
  Serial.println(toString(mode));

  RYUW122_BaudRate rate;
  uwb.getBaudRate(rate);
  Serial.print("Module baud rate: ");
  Serial.println(toString(rate));

  RYUW122_Channel channel;
  uwb.getChannel(channel);
  Serial.print("Module channel: ");
  Serial.println(toString(channel));

  RYUW122_Bandwidth bandwidth;
  uwb.getBandwidth(bandwidth);
  Serial.print("Module bandwidth: ");
  Serial.println(toString(bandwidth));

  char networkID[9]; // 8 characters + null terminator
  uwb.getNetworkID(networkID, sizeof(networkID));
  Serial.print("Module network ID: ");
  Serial.println(networkID);

  char UID[13]; // 12 characters + null terminator
  uwb.getUID(UID, sizeof(UID));
  Serial.print("Module UID: ");
  Serial.println(UID);

  char address[9]; // 8 characters + null terminator
  uwb.getAddress(address, sizeof(address));
  Serial.print("Module address: ");
  Serial.println(address);

  char password[33]; // 32 characters + null terminator
  uwb.getPassword(password, sizeof(password));
  Serial.print("Module password: ");
  Serial.println(password);

  uint16_t enableTime, disableTime;
  uwb.getTagParameters(enableTime, disableTime);
  Serial.print("Module RF enable time: ");
  Serial.println(enableTime);
  Serial.print("Module RF disable time: ");
  Serial.println(disableTime);

  int8_t distance;
  uwb.getCalibrationDistance(distance);
  Serial.print("Module calibration distance: ");
  Serial.println(distance);

  char firmware[30];
  uwb.getFirmwareVersion(firmware, sizeof(firmware));
  Serial.print("Module firmware version: ");
  Serial.println(firmware);

  Serial.println();
}
