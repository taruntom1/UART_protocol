#ifndef UARTPROTOCOL_H
#define UARTPROTOCOL_H

#include <Arduino.h>
#define PROTOCOL_DEBUG_LOGS 0
// Enable or disable debugging

#if PROTOCOL_DEBUG_LOGS
#define PROTOCOL_DEBUG_PRINT(...) do { Serial.print(__VA_ARGS__); } while (0)
#define PROTOCOL_DEBUG_PRINTLN(...) do { Serial.println(__VA_ARGS__); } while (0)
#else
#define PROTOCOL_DEBUG_PRINT(...)
#define PROTOCOL_DEBUG_PRINTLN(...)
#endif

class UARTProtocol {
private:
    uint8_t header;
    uint8_t maxPacketSize;
    unsigned long baudRate;
    HardwareSerial& serial;

public:
    UARTProtocol(HardwareSerial& serialPort, uint8_t headerByte, uint8_t maxPacketSize, unsigned long baudRate);
    void begin();
    void SendCommand(uint8_t commandType);
    void SendData(byte* data, uint8_t length);
    bool ReadCommand(uint8_t& commandType, uint32_t timeoutMs = 1000);
    bool ReadData(byte* data, uint8_t length, int timeout);
    bool ReadData(byte* data, uint8_t length);
    bool available();
};

#endif // UARTPROTOCOL_H