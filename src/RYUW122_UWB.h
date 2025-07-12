/*
  RYUW122_UWB.h - Library for RYUW122 UWB module.
  Created by Bartosz Srebro, 06.07.2025
  Released into the public domain.
*/

#ifndef RYUW122_UWB_H
#define RYUW122_UWB_H

#include <Arduino.h>

enum RYUW122_Mode : int8_t
{
    MODE_TAG = 0,
    MODE_ANCHOR = 1,
    MODE_SLEEP = 2,
    MODE_UNKNOWN = -1
};

enum RYUW122_BaudRate : int8_t
{
    BAUD_9600 = 0,
    BAUD_57600 = 1,
    BAUD_115200 = 2,
    BAUD_UNKNOWN = -1
};

enum RYUW122_Channel : int8_t
{
    CHANNEL_6489_6_MHz = 0,
    CHANNEL_7987_2_MHz = 1,
    CHANNEL_UNKNOWN = -1
};

enum RYUW122_Bandwidth : int8_t
{
    BANDWIDTH_850_Kbps = 0,
    BANDWIDTH_6_8_Mbps = 1,
    BANDWIDTH_UNKNOWN = -1
};

struct RYUW122_MessageInfo
{
    char address[9];        //8 chars + null terminator
    uint8_t payloadLength;
    char payload[13];       //12 chars + null terminator
    uint16_t distance;
};

const char *toString(RYUW122_Mode mode);
const char *toString(RYUW122_BaudRate rate);
const char *toString(RYUW122_Channel channel);
const char *toString(RYUW122_Bandwidth bandwidth);

class RYUW122_UWB
{
public:
    explicit RYUW122_UWB(Stream &serial);

    bool begin(int16_t resetPin = -1, int16_t moduleResponseTimeout = -1, int16_t distanceResponseTimeout = -1);
    bool isConnected();
    void reset();
    bool resetSW();
    void setModuleResponseTimeout(uint16_t timeout);
    void setDistanceResponseTimeout(uint16_t timeout);
    uint16_t getModuleResponseTimeout() const;
    uint16_t getDistanceResponseTimeout() const;

    bool setMode(RYUW122_Mode mode);
    bool setBaudRate(RYUW122_BaudRate baudRate);
    bool setChannel(RYUW122_Channel channel);
    bool setBandwidth(RYUW122_Bandwidth bandwidth);
    bool setNetworkID(const char *networkID, size_t len = 0);
    bool setAddress(const char *address, size_t len = 0);
    bool setPassword(const char *password, size_t len = 0);
    bool setTagParameters(uint16_t enableTime = 0, uint16_t disableTime = 0);
    bool sendMessage(const char *address, const char *message, size_t addressLen = 0, size_t messageLen = 0, bool padToMaxLength = false);
    bool sendMessage(const char *address, const char *message, bool padToMaxLength = false);
    bool setTagResponseMessage(const char *message, size_t messageLen = 0, bool restart = false, bool padToMaxLength = false);
    bool setTagResponseMessage(const char *message, bool restart = false, bool padToMaxLength = false);
    bool receiveMessage(RYUW122_MessageInfo &info, uint16_t timeout = 0);
    bool setCalibrationDistance(int8_t distance);

    bool getMode(RYUW122_Mode &mode);
    bool getBaudRate(RYUW122_BaudRate &rate);
    bool getChannel(RYUW122_Channel &channel);
    bool getBandwidth(RYUW122_Bandwidth &bandwidth);
    bool getNetworkID(char *buffer, size_t bufferSize);
    bool getUID(char *buffer, size_t bufferSize);
    bool getAddress(char *buffer, size_t bufferSize);
    bool getPassword(char *buffer, size_t bufferSize);
    bool getTagParameters(uint16_t &enableTime, uint16_t &disableTime);
    bool getCalibrationDistance(int8_t &distance);
    bool getFirmwareVersion(char *buffer, size_t bufferSize);

private:
    static constexpr size_t MessageBufferSize = 50;
    char messageBuffer[MessageBufferSize];
    const uint16_t resetTimeDelay = 5;      // Delay after waking up or reset the module
    const uint16_t afterResponseDelay = 5;  // Delay for commands thats save parameters in flash is required (module can be unresponsive for a while)
    uint16_t moduleResponseTimeout = 300;   // Timeout for module response
    uint16_t distanceResponseTimeout = 200; // Timeout for distance response from module
    int16_t resetPin = -1;                  // Pin for hardware reset, -1 means no reset pin used

    Stream &_serial;
    void sendCommandWithValue(const char *cmd, const char *val, uint8_t valLength = 0);
    void sendCommand(const char *cmd);
    bool readResponse(const char *expectedResponse, uint32_t timeout);
    bool parseAnchorResponse(char *response, RYUW122_MessageInfo &info);
    bool parseTagResponse(char *response, RYUW122_MessageInfo &info);
    void clearMessageBuffer();
};

#endif // RYUW122_UWB_H