/*
  RYUW122_UWB.cpp - Library for RYUW122 UWB module.
  Created by Bartosz Srebro, 06.07.2025
  Released into the public domain.
*/

#include "Arduino.h"
#include "RYUW122_UWB.h"

RYUW122_UWB::RYUW122_UWB(Stream &serial) : _serial(serial) {}

bool RYUW122_UWB::begin(int16_t resetPin, int16_t moduleResponseTimeout, int16_t distanceResponseTimeout)
{
    if (resetPin != -1)
    {
        this->resetPin = resetPin;
        pinMode(resetPin, OUTPUT);
        reset();
    }

    if (moduleResponseTimeout != -1)
    {
        this->moduleResponseTimeout = moduleResponseTimeout;
    }

    if (distanceResponseTimeout != -1)
    {
        this->distanceResponseTimeout = distanceResponseTimeout;
    }

    return isConnected();
}

bool RYUW122_UWB::isConnected()
{
    sendCommand("AT");
    return readResponse("OK\r\n", moduleResponseTimeout);
}

void RYUW122_UWB::reset()
{
    if (resetPin != -1)
    {
        digitalWrite(resetPin, LOW);
        delay(resetTimeDelay);
        digitalWrite(resetPin, HIGH);
        delay(resetTimeDelay);
    }
}

void RYUW122_UWB::setModuleResponseTimeout(uint16_t timeout)
{
    moduleResponseTimeout = timeout;
}

void RYUW122_UWB::setDistanceResponseTimeout(uint16_t timeout)
{
    distanceResponseTimeout = timeout;
}

uint16_t RYUW122_UWB::getModuleResponseTimeout() const
{
    return moduleResponseTimeout;
}

uint16_t RYUW122_UWB::getDistanceResponseTimeout() const
{
    return distanceResponseTimeout;
}

bool RYUW122_UWB::resetSW()
{
    sendCommand("AT+RESET");
    bool result = readResponse("READY\r\n", moduleResponseTimeout);
    delay(afterResponseDelay);
    return result;
}

bool RYUW122_UWB::setMode(RYUW122_Mode mode)
{
    switch (mode)
    {
    case MODE_TAG:
        sendCommandWithValue("AT+MODE=", "0");
        break;
    case MODE_ANCHOR:
        sendCommandWithValue("AT+MODE=", "1");
        break;
    case MODE_SLEEP:
        sendCommandWithValue("AT+MODE=", "2");
        break;
    default:
        return false;
    }
    bool result = readResponse("OK\r\n", moduleResponseTimeout);
    delay(afterResponseDelay);
    return result;
}

bool RYUW122_UWB::setBaudRate(RYUW122_BaudRate baudRate)
{
    switch (baudRate)
    {
    case BAUD_9600:
        sendCommandWithValue("AT+IPR=", "9600");
        break;
    case BAUD_57600:
        sendCommandWithValue("AT+IPR=", "57600");
        break;
    case BAUD_115200:
        sendCommandWithValue("AT+IPR=", "115200");
        break;
    default:
        return false;
    }
    bool result = readResponse("OK\r\n", moduleResponseTimeout);
    delay(afterResponseDelay);
    return result;
}

bool RYUW122_UWB::setChannel(RYUW122_Channel channel)
{
    switch (channel)
    {
    case CHANNEL_6489_6_MHz:
        sendCommandWithValue("AT+CHANNEL=", "5");
        break;
    case CHANNEL_7987_2_MHz:
        sendCommandWithValue("AT+CHANNEL=", "9");
        break;
    default:
        return false;
    }
    bool result = readResponse("OK\r\n", moduleResponseTimeout);
    delay(afterResponseDelay);
    return result;
}

bool RYUW122_UWB::setBandwidth(RYUW122_Bandwidth bandwidth)
{
    switch (bandwidth)
    {
    case BANDWIDTH_850_Kbps:
        sendCommandWithValue("AT+BANDWIDTH=", "0");
        break;
    case BANDWIDTH_6_8_Mbps:
        sendCommandWithValue("AT+BANDWIDTH=", "1");
        break;
    default:
        return false;
    }
    bool result = readResponse("OK\r\n", moduleResponseTimeout);
    delay(afterResponseDelay);
    return result;
}

bool RYUW122_UWB::setNetworkID(const char *networkID)
{
    size_t len = strlen(networkID);
    if (len > 8)
    {
        return false;
    }

    clearMessageBuffer();
    
    // Copy the actual network ID
    memcpy(messageBuffer, networkID, len);

    // Pad with spaces if it's shorter than 8 characters
    if (len < 8)
    {
        memset(messageBuffer + len, ' ', 8 - len);
    }

    // Send exactly 8 characters as the value
    sendCommandWithValue("AT+NETWORKID=", messageBuffer, 8);

    bool result = readResponse("OK\r\n", moduleResponseTimeout);
    delay(afterResponseDelay);
    return result;
}

bool RYUW122_UWB::setAddress(const char *address)
{
    size_t len = strlen(address);
    if (len > 8) {
        return false;  // Address must be max 8 characters
    }

    clearMessageBuffer();

    // Copy the address into the buffer
    memcpy(messageBuffer, address, len);

    // Pad with spaces if the address is shorter than 8 characters
    if (len < 8) {
        memset(messageBuffer + len, ' ', 8 - len);
    }

    // Send exactly 8 characters as required
    sendCommandWithValue("AT+ADDRESS=", messageBuffer, 8);

    // Wait for confirmation and delay for flash write
    bool result = readResponse("OK\r\n", moduleResponseTimeout);
    delay(afterResponseDelay);
    return result;
}

bool RYUW122_UWB::setPassword(const char *password)
{
    if (strlen(password) > 32)
    {
        return false;
    }

    sendCommandWithValue("AT+CPIN=", password);
    bool result = (readResponse("OK\r\n", moduleResponseTimeout));
    delay(afterResponseDelay);
    return result;
}

bool RYUW122_UWB::setTagParameters(uint16_t enableTime, uint16_t disableTime)
{
    if (enableTime > 28000 || disableTime > 28000)
    {
        return false;
    }
    clearMessageBuffer();
    snprintf(messageBuffer, sizeof(messageBuffer), "%d,%d", enableTime, disableTime);
    sendCommandWithValue("AT+TAGD=", messageBuffer);
    bool result = readResponse("OK\r\n", moduleResponseTimeout);
    delay(afterResponseDelay);
    return result;
}

bool RYUW122_UWB::sendMessage(const char *address, const char *message, bool padToMaxLength)
{
    // Check if the address is valid (must be max 8 characters)
    size_t addrLen = strlen(address);
    if (addrLen > 8) return false;

    // Check if the message is valid (must be max 12 characters)
    size_t msgLen = strlen(message);
    if (msgLen > 12 || msgLen == 0) return false; // Empty message is not allowed

    clearMessageBuffer();

    // Pad the address with spaces to ensure it is exactly 8 characters long
    memset(messageBuffer, ' ', 8);
    memcpy(messageBuffer, address, addrLen);

    char* ptr = messageBuffer + 8;
    int finalLen = padToMaxLength ? 12 : msgLen;

    // Add message length prefix (e.g., ",12,")
    int n = snprintf(ptr, sizeof(messageBuffer) - 8, ",%d,", finalLen);
    if (n < 0 || (size_t)n >= sizeof(messageBuffer) - 8) return false;

    ptr += n;

    // Copy message content
    memcpy(ptr, message, msgLen);

    // Pad the message with spaces if required
    if (padToMaxLength && msgLen < 12) {
        memset(ptr + msgLen, ' ', 12 - msgLen);
    }

    // Send the complete command
    sendCommandWithValue("AT+ANCHOR_SEND=", messageBuffer);
    return readResponse("OK\r\n", moduleResponseTimeout);
}

bool RYUW122_UWB::setTagResponseMessage(const char *message, bool restart, bool padToMaxLength)
{
    uint8_t len = strlen(message);
    if (len > 12) {
        return false;  // Reject messages longer than 12 characters
    }

    // Determine final message length
    uint8_t finalLen = padToMaxLength ? 12 : len;

    clearMessageBuffer();

    // Write "<len>," prefix
    int n = snprintf(messageBuffer, sizeof(messageBuffer), "%d,", finalLen);
    if (n < 0 || (size_t)n >= sizeof(messageBuffer)) {
        return false;
    }

    // Copy message content
    memcpy(messageBuffer + n, message, len);

    // Pad with spaces if needed
    if (padToMaxLength && len < 12) {
        memset(messageBuffer + n + len, ' ', 12 - len);
    }

    // Optional module reset
    if (restart) {
        reset();
    }

    sendCommandWithValue("AT+TAG_SEND=", messageBuffer);

    // Wait for confirmation
    return readResponse("OK\r\n", moduleResponseTimeout);
}

bool RYUW122_UWB::receiveMessage(RYUW122_MessageInfo &info, uint16_t timeout)
{
    if (timeout == 0)
    {
        timeout = moduleResponseTimeout;
    }

    if (readResponse("\r\n", timeout))
    {
        if (strstr(messageBuffer, "ANCHOR_RCV="))
        {
            return parseAnchorResponse(messageBuffer, info);
        }
        else if (strstr(messageBuffer, "TAG_RCV="))
        {
            return parseTagResponse(messageBuffer, info);
        }
    }
    return false;
}

bool RYUW122_UWB::setCalibrationDistance(int8_t distance)
{
    if (distance < -100 || distance > 100)
    {
        return false;
    }

    clearMessageBuffer();
    snprintf(messageBuffer, sizeof(messageBuffer), "%d", (int)distance);

    sendCommandWithValue("AT+CAL=", messageBuffer);

    bool result = readResponse("OK\r\n", moduleResponseTimeout);
    delay(afterResponseDelay);
    return result;
}

bool RYUW122_UWB::getMode(RYUW122_Mode &mode)
{
    sendCommand("AT+MODE?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char *ptr = strstr(messageBuffer, "=");
        if (ptr && strlen(ptr) >= 2)
        {
            char modeChar = ptr[1];
            switch (modeChar)
            {
                case '0':
                    mode = MODE_TAG;
                    return true;
                case '1':
                    mode = MODE_ANCHOR;
                    return true;
                case '2':
                    mode = MODE_SLEEP;
                    return true;
                default:
                    mode = MODE_UNKNOWN;
                    return false;
            }
        }
    }
    mode = MODE_UNKNOWN;
    return false;
}

bool RYUW122_UWB::getBaudRate(RYUW122_BaudRate &rate)
{
    sendCommand("AT+IPR?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char *ptr = strstr(messageBuffer, "=");
        if (ptr)
        {
            int value = atoi(ptr + 1);
            switch (value)
            {
                case 9600:
                    rate = BAUD_9600;
                    return true;
                case 57600:
                    rate = BAUD_57600;
                    return true;
                case 115200:
                    rate = BAUD_115200;
                    return true;
                default:
                    rate = BAUD_UNKNOWN;
                    return false;
            }
        }
    }
    rate = BAUD_UNKNOWN;
    return false;
}

bool RYUW122_UWB::getChannel(RYUW122_Channel &channel)
{
    sendCommand("AT+CHANNEL?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char *ptr = strstr(messageBuffer, "=");
        if (ptr && strlen(ptr) >= 2)
        {
            char chanChar = ptr[1];
            switch (chanChar)
            {
                case '5':
                    channel = CHANNEL_6489_6_MHz;
                    return true;
                case '9':
                    channel = CHANNEL_7987_2_MHz;
                    return true;
                default:
                    channel = CHANNEL_UNKNOWN;
                    return false;
            }
        }
    }
    channel = CHANNEL_UNKNOWN;
    return false;
}

bool RYUW122_UWB::getBandwidth(RYUW122_Bandwidth &bandwidth)
{
    sendCommand("AT+BANDWIDTH?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char *ptr = strstr(messageBuffer, "=");
        if (ptr && strlen(ptr) >= 2)
        {
            char modeChar = ptr[1];
            switch (modeChar)
            {
                case '0':
                    bandwidth = BANDWIDTH_850_Kbps;
                    return true;
                case '1':
                    bandwidth = BANDWIDTH_6_8_Mbps;
                    return true;
                default:
                    bandwidth = BANDWIDTH_UNKNOWN;
                    return false;
            }
        }
    }
    bandwidth = BANDWIDTH_UNKNOWN;
    return false;
}

bool RYUW122_UWB::getNetworkID(char* buffer, size_t bufferSize)
{
    if (bufferSize < 9)
        return false;

    sendCommand("AT+NETWORKID?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char* prefix = "+NETWORKID=";
        char* found = strstr(messageBuffer, prefix);
        if (found)
        {
            char* id = found + strlen(prefix);
            char* newline = strchr(id, '\n');
            if (newline)
                *newline = '\0';

            strncpy(buffer, id, bufferSize - 1);
            buffer[bufferSize - 1] = '\0';
            return true;
        }
    }
    return false;
}

bool RYUW122_UWB::getAddress(char* buffer, size_t bufferSize)
{
    if (bufferSize < 9)
        return false;

    sendCommand("AT+ADDRESS?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char* addressPrefix = "+ADDRESS=";
        char* found = strstr(messageBuffer, addressPrefix);
        if (found)
        {
            char* id = found + strlen(addressPrefix);
            char* newline = strchr(id, '\n');
            if (newline)
                *newline = '\0';

            strncpy(buffer, id, bufferSize - 1);
            buffer[bufferSize - 1] = '\0';
            return true;
        }
    }
    return false;
}

bool RYUW122_UWB::getUID(char* buffer, size_t bufferSize)
{
    if (bufferSize < 13)
        return false;

    sendCommand("AT+UID?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char* uidPrefix = "+UID=";
        char* found = strstr(messageBuffer, uidPrefix);
        if (found)
        {
            char* id = found + strlen(uidPrefix);
            char* newline = strchr(id, '\n');
            if (newline)
                *newline = '\0';

            strncpy(buffer, id, bufferSize - 1);
            buffer[bufferSize - 1] = '\0';
            return true;
        }
    }
    return false;
}

bool RYUW122_UWB::getPassword(char *buffer, size_t bufferSize)
{
    if (bufferSize < 33)
        return false;

    sendCommand("AT+CPIN?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char *passwordPrefix = "+CPIN=";
        char *found = strstr(messageBuffer, passwordPrefix);
        if (found)
        {
            char *pwd = found + strlen(passwordPrefix);
            char *newline = strchr(pwd, '\n');
            if (newline)
                *newline = '\0';

            strncpy(buffer, pwd, bufferSize - 1);
            buffer[bufferSize - 1] = '\0';
            return true;
        }
    }
    return false;
}

bool RYUW122_UWB::getTagParameters(uint16_t &enableTime, uint16_t &disableTime)
{
    sendCommand("AT+TAGD?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char *tagParamsPrefix = "+TAGD=";
        char *found = strstr(messageBuffer, tagParamsPrefix);
        if (found)
        {
            char *params = found + strlen(tagParamsPrefix);
            int enable = 0, disable = 0;
            if (sscanf(params, "%d,%d", &enable, &disable) == 2)
            {
                enableTime = (uint16_t)enable;
                disableTime = (uint16_t)disable;
                return true;
            }
        }
    }
    return false;
}

bool RYUW122_UWB::getCalibrationDistance(int8_t &distance)
{
    sendCommand("AT+CAL?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char *calPrefix = "+CAL=";
        char *found = strstr(messageBuffer, calPrefix);
        if (found)
        {
            char *valStr = found + strlen(calPrefix);
            int value = 0;
            if (sscanf(valStr, "%d", &value) == 1 && value >= -100 && value <= 100)
            {
                distance = (int8_t)value;
                return true;
            }
        }
    }
    return false;
}

bool RYUW122_UWB::getFirmwareVersion(char *buffer, size_t bufferSize)
{
    sendCommand("AT+VER?");
    if (readResponse("\r\n", moduleResponseTimeout))
    {
        const char *versionPrefix = "+VER=";
        char *found = strstr(messageBuffer, versionPrefix);
        if (found)
        {
            char *verStr = found + strlen(versionPrefix);
            char *newline = strchr(verStr, '\n');
            if (newline)
                *newline = '\0';

            strncpy(buffer, verStr, bufferSize - 1);
            buffer[bufferSize - 1] = '\0'; 
            return true;
        }
    }
    return false;
}

bool RYUW122_UWB::parseAnchorResponse(char *response, RYUW122_MessageInfo &info)
{
    const char *prefix = "+ANCHOR_RCV=";
    char *start = strstr(response, prefix);
    if (!start)
        return false;

    start += strlen(prefix);

    char *ptr1 = strchr(start, ',');
    if (!ptr1)
        return false;
    *ptr1 = '\0';
    strncpy(info.address, start, sizeof(info.address) - 1);
    info.address[sizeof(info.address) - 1] = '\0';

    char *ptr2 = strchr(ptr1 + 1, ',');
    if (!ptr2)
        return false;
    *ptr2 = '\0';
    info.payloadLength = atoi(ptr1 + 1);

    char *ptr3 = strchr(ptr2 + 1, ',');
    if (!ptr3)
        return false;
    *ptr3 = '\0';
    strncpy(info.payload, ptr2 + 1, sizeof(info.payload) - 1);
    info.payload[sizeof(info.payload) - 1] = '\0';

    char *distanceStr = ptr3 + 1;
    while (*distanceStr == ' ')
        distanceStr++;
    char *cmPtr = strstr(distanceStr, "cm");
    if (cmPtr)
        *cmPtr = '\0'; 
    info.distance = atoi(distanceStr);

    return true;
}

bool RYUW122_UWB::parseTagResponse(char* response, RYUW122_MessageInfo& info)
{
    const char* prefix = "+TAG_RCV=";
    char* start = strstr(response, prefix);
    if (!start)
        return false;

    start += strlen(prefix);

    char* ptr = strchr(start, ',');
    if (!ptr)
        return false;
    *ptr = '\0';

    info.payloadLength = atoi(start);
    strncpy(info.payload, ptr + 1, sizeof(info.payload) - 1);
    info.payload[sizeof(info.payload) - 1] = '\0';

    for (int i = 0; info.payload[i] != '\0'; ++i) {
        if (info.payload[i] == '\r' || info.payload[i] == '\n') {
            info.payload[i] = '\0';
            break;
        }
    }

    info.address[0] = '\0'; 
    info.distance = 0;

    return true;
}

void RYUW122_UWB::clearMessageBuffer()
{
    memset(messageBuffer, 0, sizeof(messageBuffer));
}

void RYUW122_UWB::sendCommand(const char *cmd)
{
    while (_serial.available())
        _serial.read();
    _serial.print(cmd);
    _serial.println();
}

void RYUW122_UWB::sendCommandWithValue(const char *cmd, const char *val, uint8_t valLength)
{
    while (_serial.available())
        _serial.read();
    _serial.print(cmd);
    if (valLength  == 0)
        _serial.print(val);
    else
        _serial.write(val, valLength);
    _serial.println();
}

bool RYUW122_UWB::readResponse(const char *expectedResponse, uint32_t timeout)
{
    clearMessageBuffer();

    size_t index = 0;
    uint32_t startTime = millis();

    while (millis() - startTime < timeout)
    {
        while (_serial.available())
        {
            char c = _serial.read();

            if (index < sizeof(messageBuffer) - 1)
            {
                messageBuffer[index++] = c;
                messageBuffer[index] = '\0';
            }

            if (strstr(messageBuffer, expectedResponse))
            {
                return true;
            }
        }
    }

    return strstr(messageBuffer, expectedResponse) != nullptr;
}

const char* toString(RYUW122_Mode mode) {
    switch (mode) {
        case MODE_TAG: return "TAG";
        case MODE_ANCHOR: return "ANCHOR";
        case MODE_SLEEP: return "SLEEP";
        case MODE_UNKNOWN: return "UNKNOWN";
        default: return "INVALID_MODE";
    }
}

const char* toString(RYUW122_BaudRate baudRate) {
    switch (baudRate) {
        case BAUD_9600: return "9600";
        case BAUD_57600: return "57600";
        case BAUD_115200: return "115200";
        case BAUD_UNKNOWN: return "UNKNOWN";
        default: return "INVALID_BAUD";
    }
}

const char* toString(RYUW122_Channel channel) {
    switch (channel) {
        case CHANNEL_6489_6_MHz: return "6489.6 MHz";
        case CHANNEL_7987_2_MHz: return "7987.2 MHz";
        case CHANNEL_UNKNOWN: return "UNKNOWN";
        default: return "INVALID_CHANNEL";
    }
}

const char* toString(RYUW122_Bandwidth bandwidth) {
    switch (bandwidth) {
        case BANDWIDTH_850_Kbps: return "850 Kbps";
        case BANDWIDTH_6_8_Mbps: return "6.8 Mbps";
        case BANDWIDTH_UNKNOWN: return "UNKNOWN";
        default: return "INVALID_BANDWIDTH";
    }
}