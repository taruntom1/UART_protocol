#include "UARTProtocol.h"

UARTProtocol::UARTProtocol(HardwareSerial& serialPort, uint8_t headerByte, uint8_t maxPacketSize, unsigned long baudRate)
    : serial(serialPort), header(headerByte), maxPacketSize(maxPacketSize), baudRate(baudRate) {}

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

    if ((2 + parameterCount + 1) > maxPacketSize) {
        DEBUG_PRINTLN("Error: Packet size exceeds max limit");
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

void UARTProtocol::SendCommand(uint8_t commandType)
{
    DEBUG_PRINT("Sending packet with command type: ");
    DEBUG_PRINTLN(commandType, HEX);
    serial.write(header);
    serial.write(commandType);
    DEBUG_PRINT("Packet sent");
}

void UARTProtocol::SendData(byte* data, uint8_t length)
{
    DEBUG_PRINT("Sending data: ");
    for (int i = 0; i < length; i++)
    {
        serial.write(data[i]);
        DEBUG_PRINT(data[i], HEX);
    }
    DEBUG_PRINTLN("Data sent");
}

bool UARTProtocol::ReadCommand(uint8_t& commandType) {
    DEBUG_PRINTLN("Waiting for packet...");
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
    return true;
}

bool UARTProtocol::ReadData(byte* data, uint8_t length, int timeout) 
{
    DEBUG_PRINTLN("Waiting for parameter..."); 
    if (serial.readBytes(data, length) < length){
        DEBUG_PRINTLN("Error: data length not enough");
    }
    DEBUG_PRINTLN("Parameter received");
    return true;
}






bool UARTProtocol::receivePacket(uint8_t& commandType, uint8_t* parameters, uint8_t parameterCount, bool checkChecksum) {
    DEBUG_PRINTLN("Waiting for packet...");

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


bool UARTProtocol::waitForHeader(unsigned long timeout) {
    DEBUG_PRINTLN("Waiting for header with timeout...");

    unsigned long startTime = millis();  // Record the start time

    while (millis() - startTime < timeout) {  // Check if timeout has been reached
        if (serial.available() > 0) {  // Data available in the buffer
            int nextByte = serial.peek();  // Peek at the next byte without removing it
            if (nextByte == header) {      // Check if the byte matches the header
                DEBUG_PRINTLN("Header byte found!");
                return true;               // Exit without clearing the header byte
            } else {
                serial.read();  // Consume non-header byte
            }
        }
    }

    DEBUG_PRINTLN("Timeout: Header byte not found.");
    return false;  // Timeout reached
}

// Check if there is data available to read
bool UARTProtocol::available() {
    return serial.available();
}

