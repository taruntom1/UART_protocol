#ifndef UARTPROTOCOL_H
#define UARTPROTOCOL_H

#include <Arduino.h>

// Enable or disable debugging
#define PROTOCOL_DEBUG_LOGS 0

#if PROTOCOL_DEBUG_LOGS
#define DEBUG_PRINT(...) do { Serial.print(__VA_ARGS__); } while (0)
#define DEBUG_PRINTLN(...) do { Serial.println(__VA_ARGS__); } while (0)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

class UARTProtocol {
private:
    uint8_t header;
    uint8_t maxPacketSize;
    unsigned long baudRate;
    HardwareSerial& serial;

    uint8_t calculateChecksum(uint8_t* packet, uint8_t length);

public:
    UARTProtocol(HardwareSerial& serialPort, uint8_t headerByte, uint8_t maxPacketSize, unsigned long baudRate);
    void begin();
    bool sendPacket(uint8_t commandType, uint8_t* parameters, uint8_t parameterCount, bool checkChecksum = true);
    bool receivePacket(uint8_t& commandType, uint8_t* parameters, uint8_t parameterCount, bool checkChecksum = true);
    bool waitForHeader(unsigned long timeout);
    bool available();

};

#endif // UARTPROTOCOL_H