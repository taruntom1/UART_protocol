#include <Arduino.h>
#include "UARTProtocol.h"

// Define parameter lookup function
uint8_t parameterLookup(uint8_t commandType) {
    switch (commandType) {
        case 0x01: return 2; // Command type 0x01 expects 2 parameters
        case 0x02: return 3; // Command type 0x02 expects 3 parameters
        case 0x03: return 1; // Command type 0x03 expects 1 parameter
        default: return 0; // Unknown command type
    }
}

// Initialize UARTProtocol
UARTProtocol protocol(Serial, 0xAA, 10, 9600, parameterLookup);

void setup() {
    Serial.begin(9600); // Debugging
    protocol.begin();
}

void loop() {
    uint8_t commandType;
    uint8_t parameters[10];
    uint8_t parameterCount;

    // Attempt to receive a packet
    bool received = protocol.receivePacket(commandType, parameters, parameterCount, true);

    if (received) {
        Serial.println("Packet received:");
        Serial.print("Command type: ");
        Serial.println(commandType, HEX);
        Serial.print("Parameters: ");
        for (uint8_t i = 0; i < parameterCount; i++) {
            Serial.print(parameters[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    } else {
        Serial.println("Failed to receive packet");
    }

    delay(2000); // Delay to avoid flooding serial
}