#include "UARTProtocol.h"

UARTProtocol::UARTProtocol(HardwareSerial& serialPort, uint8_t headerByte, uint8_t maxPacketSize, unsigned long baudRate, uint8_t (*lookupTable)(uint8_t))
    : serial(serialPort), header(headerByte), maxPacketSize(maxPacketSize), baudRate(baudRate), parameterLookup(lookupTable) {}

void UARTProtocol::begin() {
    serial.begin(baudRate);
    DEBUG_PRINTLN("UARTProtocol initialized");
}

uint8_t UARTProtocol::calculateChecksum(uint8_t* packet, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum ^= packet[i];
    }
    return checksum;
}

bool UARTProtocol::sendPacket(uint8_t commandType, uint8_t* parameters, uint8_t parameterCount, bool checkChecksum) {
    DEBUG_PRINT("Sending packet with command type: ");
    DEBUG_PRINTLN(commandType, HEX);

    if (parameterCount != parameterLookup(commandType) || (2 + parameterCount + 1) > maxPacketSize) {
        DEBUG_PRINTLN("Error: Invalid parameter count or packet size exceeds max limit");
        return false;
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

bool UARTProtocol::receivePacket(uint8_t& commandType, uint8_t* parameters, uint8_t& parameterCount, bool checkChecksum) {
    DEBUG_PRINTLN("Waiting for packet...");

    if (serial.available() < 3) {
        DEBUG_PRINTLN("Error: Not enough data available for a valid packet");
        return false;
    }

    while (serial.read() != header) {
        if (serial.available() < 1) {
            DEBUG_PRINTLN("Error: No valid header found");
            return false;
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
        return false;
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
            return false;
        }

        DEBUG_PRINTLN("Checksum verified successfully");
    }

    DEBUG_PRINTLN("Packet received successfully");
    return true;
}
