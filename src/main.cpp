/* #include <Arduino.h>

// Enable or disable debugging
#define DEBUG_LOGS 1

#if DEBUG_LOGS
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
    uint8_t (*parameterLookup)(uint8_t commandType);
    HardwareSerial& serial;

    uint8_t calculateChecksum(uint8_t* packet, uint8_t length) {
        uint8_t checksum = 0;
        for (uint8_t i = 0; i < length; i++) {
            checksum ^= packet[i];
        }
        return checksum;
    }

public:
    UARTProtocol(HardwareSerial& serialPort, uint8_t headerByte, uint8_t maxPacketSize, unsigned long baudRate, uint8_t (*lookupTable)(uint8_t))
        : serial(serialPort), header(headerByte), maxPacketSize(maxPacketSize), baudRate(baudRate), parameterLookup(lookupTable) {}

    void begin() {
        serial.begin(baudRate);
        DEBUG_PRINTLN("UARTProtocol initialized");
    }

    bool sendPacket(uint8_t commandType, uint8_t* parameters, uint8_t parameterCount, bool checkChecksum = true) {
        DEBUG_PRINT("Sending packet with command type: ");
        DEBUG_PRINTLN(commandType, HEX);

        if (parameterCount != parameterLookup(commandType) || (2 + parameterCount + 1) > maxPacketSize) {
            DEBUG_PRINTLN("Error: Invalid parameter count or packet size exceeds max limit");
            return false; // Invalid parameter count or packet size exceeds max limit
        }

        uint8_t packet[maxPacketSize];
        packet[0] = header;
        packet[1] = commandType;

        for (uint8_t i = 0; i < parameterCount; i++) {
            packet[2 + i] = parameters[i];
        }

        if (checkChecksum) {
            uint8_t checksum = calculateChecksum(packet, 2 + parameterCount);
            packet[2 + parameterCount] = checksum;
            DEBUG_PRINT("Calculated checksum: ");
            DEBUG_PRINTLN(checksum, HEX);
        }

        serial.write(packet, 2 + parameterCount + (checkChecksum ? 1 : 0));
        DEBUG_PRINTLN("Packet sent");
        return true;
    }

    bool receivePacket(uint8_t& commandType, uint8_t* parameters, uint8_t& parameterCount, bool checkChecksum = true) {
        DEBUG_PRINTLN("Waiting for packet...");

        if (serial.available() < 3) { // Minimum packet size
            DEBUG_PRINTLN("Error: Not enough data available for a valid packet");
            return false;
        }

        // Read until the header byte is found
        while (serial.read() != header) {
            if (serial.available() < 1) {
                DEBUG_PRINTLN("Error: No valid header found");
                return false; // No valid header found
            }
        }

        DEBUG_PRINTLN("Header found");

        commandType = serial.read();
        DEBUG_PRINT("Command type: ");
        DEBUG_PRINTLN(commandType, HEX);

        parameterCount = parameterLookup(commandType);
        DEBUG_PRINT("Expected parameter count: ");
        DEBUG_PRINTLN(parameterCount);

        if (parameterCount + 3 > maxPacketSize || serial.available() < parameterCount + (checkChecksum ? 1 : 0)) {
            DEBUG_PRINTLN("Error: Invalid packet size or not enough data");
            return false; // Invalid packet size or not enough data
        }

        for (uint8_t i = 0; i < parameterCount; i++) {
            parameters[i] = serial.read();
            DEBUG_PRINT("Parameter ");
            DEBUG_PRINT(i);
            DEBUG_PRINT(": ");
            DEBUG_PRINTLN(parameters[i], HEX);
        }

        if (checkChecksum) {
            uint8_t receivedChecksum = serial.read();
            DEBUG_PRINT("Received checksum: ");
            DEBUG_PRINTLN(receivedChecksum, HEX);

            uint8_t packet[maxPacketSize];
            packet[0] = header;
            packet[1] = commandType;

            for (uint8_t i = 0; i < parameterCount; i++) {
                packet[2 + i] = parameters[i];
            }

            if (calculateChecksum(packet, 2 + parameterCount) != receivedChecksum) {
                DEBUG_PRINTLN("Error: Checksum mismatch");
                return false; // Checksum mismatch
            }

            DEBUG_PRINTLN("Checksum verified successfully");
        }

        DEBUG_PRINTLN("Packet received successfully");
        return true;
    }
};

// Example parameter lookup table function
uint8_t exampleLookupTable(uint8_t commandType) {
    switch (commandType) {
        case 0x01: return 2; // Command type 0x01 expects 2 parameters
        case 0x02: return 3; // Command type 0x02 expects 3 parameters
        case 0x03: return 1; // Command type 0x03 expects 1 parameter
        default: return 0; // Unknown command type
    }
}

// Example usage
// UARTProtocol protocol(Serial1, 0xAA, 10, 9600, exampleLookupTable);
// protocol.begin();
 */

#include <Arduino.h>
#include <UARTProtocol.h>


uint8_t parameterLookup(uint8_t commandType) {
    switch (commandType) {
        case 0x01: return 2; // Command type 0x01 expects 2 parameters
        case 0x02: return 3; // Command type 0x02 expects 3 parameters
        case 0x03: return 1; // Command type 0x03 expects 1 parameter
        default: return 0; // Unknown command type
    }
}

// Example usage
// UARTProtocol protocol(Serial1, 0xAA, 10, 9600, exampleLookupTable);
// protocol.begin();





// Initialize UARTProtocol
UARTProtocol protocol(Serial, 0xAA, 10, 9600, parameterLookup);

void setup() {
    Serial.begin(9600); // Debugging
    protocol.begin();
}

void loop() {
    // Example: Send a packet with commandType 0x01 and two parameters
    uint8_t parameters[] = {0x10, 0x20};
    bool sent = protocol.sendPacket(0x01, parameters, 2, true);

    if (sent) {
        Serial.println("Packet sent successfully");
    } else {
        Serial.println("Failed to send packet");
    }

    delay(1000); // Delay to simulate periodic sending
}