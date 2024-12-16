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