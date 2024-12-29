#include "UARTProtocol.h"

UARTProtocol::UARTProtocol(HardwareSerial &serialPort, uint8_t headerByte, uint8_t maxPacketSize, unsigned long baudRate)
    : serial(serialPort), header(headerByte), maxPacketSize(maxPacketSize), baudRate(baudRate) {}

void UARTProtocol::begin()
{
    serial.begin(baudRate);
    PROTOCOL_DEBUG_PRINTLN("UARTProtocol initialized");
}

byte UARTProtocol::calculateChecksum(uint8_t commandType, byte *data, uint8_t length)
{
    uint8_t checksum = commandType;
    for (uint8_t i = 0; i < length; i++)
    {
        checksum ^= data[i];
    }
    return checksum;
}

void UARTProtocol::SendCommand(uint8_t commandType)
{
    PROTOCOL_DEBUG_PRINT("Sending packet with command type: ");
    PROTOCOL_DEBUG_PRINTLN(commandType, HEX);
    serial.write(header);
    serial.write(commandType);
    PROTOCOL_DEBUG_PRINT("Packet sent");
}

void UARTProtocol::SendData(byte *data, uint8_t length)
{
    PROTOCOL_DEBUG_PRINT("Sending data: ");
    serial.write(data, length);
    PROTOCOL_DEBUG_PRINTLN("Data sent");
}

void UARTProtocol::SendChecksum(uint8_t commandType, byte *data, uint8_t length)
{
    uint8_t checksum = calculateChecksum(commandType, data, length);
    serial.write(checksum);
    PROTOCOL_DEBUG_PRINT("Checksum sent: ");
    PROTOCOL_DEBUG_PRINTLN(checksum, HEX);
}

bool UARTProtocol::ReadCommand(uint8_t &commandType)
{
    PROTOCOL_DEBUG_PRINTLN("Waiting for packet...");
    serial.setTimeout(1);
    if (serial.find(header))
    {
        serial.setTimeout(1000);
        PROTOCOL_DEBUG_PRINTLN("Header found");
        if (serial.readBytes(&commandType, 1))
        {
            PROTOCOL_DEBUG_PRINT("Command type: ");
            PROTOCOL_DEBUG_PRINTLN(commandType, HEX);
            return true;
        }
        PROTOCOL_DEBUG_PRINTLN("Error: Command type not received");
        return false;
    }

    serial.setTimeout(1000);
    PROTOCOL_DEBUG_PRINTLN("Error: No valid header found");
    return false;
}

bool UARTProtocol::ReadData(byte *data, uint8_t length, int timeout)
{
    PROTOCOL_DEBUG_PRINTLN("Waiting for parameter...");
    serial.setTimeout(timeout);
    if (serial.readBytes(data, length) < length)
    {
        PROTOCOL_DEBUG_PRINTLN("Error: data length not enough");
        serial.setTimeout(1000);
        return false;
    }
    PROTOCOL_DEBUG_PRINTLN("Parameter received");
    serial.setTimeout(1000);
    return true;
}

bool UARTProtocol::ReadData(byte *data, uint8_t length)
{
    PROTOCOL_DEBUG_PRINTLN("Waiting for parameter...");
    if (serial.readBytes(data, length) < length)
    {
        PROTOCOL_DEBUG_PRINTLN("Error: data length not enough");
        return false;
    }
    PROTOCOL_DEBUG_PRINTLN("Parameter received");
    return true;
}

bool UARTProtocol::VerifyChecksum(uint8_t &commandType, byte *data, uint8_t dataLength)
{
    byte recievedChecksum;
    PROTOCOL_DEBUG_PRINTLN("Waiting for checksum...");
    if (serial.readBytes(&recievedChecksum, 1) == 1)
    {
        PROTOCOL_DEBUG_PRINTLN("Checksum received: ");
        PROTOCOL_DEBUG_PRINTLN(recievedChecksum, HEX);
    }
    else
    {
        PROTOCOL_DEBUG_PRINTLN("Error: checksum not received");
        return false;
    }

    byte calculatedChecksum;
    calculatedChecksum = calculateChecksum(commandType, data, dataLength);
    PROTOCOL_DEBUG_PRINTLN("Calculated checksum: ");
    PROTOCOL_DEBUG_PRINTLN(calculatedChecksum, HEX);
    if (recievedChecksum == calculatedChecksum)
    {
        PROTOCOL_DEBUG_PRINTLN("Checksum verified");
        return true;
    }
    else
    {
        PROTOCOL_DEBUG_PRINTLN("Checksum verification failed");
        return false;
    }
}
bool UARTProtocol::waitForHeader(unsigned long timeout)
{
    PROTOCOL_DEBUG_PRINTLN("Waiting for header with timeout...");

    unsigned long startTime = millis(); // Record the start time

    while (millis() - startTime < timeout)
    { // Check if timeout has been reached
        serial.setTimeout(1);
        if (serial.available() > 0)
        {                                 // Data available in the buffer
            int nextByte = serial.peek(); // Peek at the next byte without removing it
            if (nextByte == header)
            { // Check if the byte matches the header
                PROTOCOL_DEBUG_PRINTLN("Header byte found!");
                serial.setTimeout(1000);
                return true; // Exit without clearing the header byte
            }
            else
            {
                serial.read(); // Consume non-header byte
            }
        }
    }

    PROTOCOL_DEBUG_PRINTLN("Timeout: Header byte not found.");
    serial.setTimeout(1000);
    return false; // Timeout reached
}

// Check if there is data available to read
bool UARTProtocol::available()
{
    return serial.available();
}

/* bool UARTProtocol::receivePacket(uint8_t &commandType, uint8_t *parameters, uint8_t parameterCount, bool checkChecksum)
{
    PROTOCOL_DEBUG_PRINTLN("Waiting for packet...");

    while (serial.read() != header)
    {
        if (serial.available() < 1)
        {
            PROTOCOL_DEBUG_PRINTLN("Error: No valid header found");
            return false;
        }
    }

    PROTOCOL_DEBUG_PRINTLN("Header found");

    commandType = serial.read();
    PROTOCOL_DEBUG_PRINT("Command type: ");
    PROTOCOL_DEBUG_PRINTLN(commandType, HEX);

    if (parameterCount + 3 > maxPacketSize || serial.available() < parameterCount + (checkChecksum ? 1 : 0))
    {
        PROTOCOL_DEBUG_PRINTLN("Error: Invalid packet size or not enough data");
        return false;
    }

    for (uint8_t i = 0; i < parameterCount; i++)
    {
        parameters[i] = serial.read();
        PROTOCOL_DEBUG_PRINT("Parameter ");
        PROTOCOL_DEBUG_PRINT(i);
        PROTOCOL_DEBUG_PRINT(": ");
        PROTOCOL_DEBUG_PRINTLN(parameters[i], HEX);
    }

    if (checkChecksum)
    {
        uint8_t receivedChecksum = serial.read();
        PROTOCOL_DEBUG_PRINT("Received checksum: ");
        PROTOCOL_DEBUG_PRINTLN(receivedChecksum, HEX);

        uint8_t packet[maxPacketSize];
        packet[0] = header;
        packet[1] = commandType;

        for (uint8_t i = 0; i < parameterCount; i++)
        {
            packet[2 + i] = parameters[i];
        }

        if (calculateChecksum(packet, 2 + parameterCount) != receivedChecksum)
        {
            PROTOCOL_DEBUG_PRINTLN("Error: Checksum mismatch");
            return false;
        }

        PROTOCOL_DEBUG_PRINTLN("Checksum verified successfully");
    }

    PROTOCOL_DEBUG_PRINTLN("Packet received successfully");
    return true;
} */

/* bool UARTProtocol::sendPacket(uint8_t commandType, uint8_t *parameters, uint8_t parameterCount, bool checkChecksum)
{
    PROTOCOL_DEBUG_PRINT("Sending packet with command type: ");
    PROTOCOL_DEBUG_PRINTLN(commandType, HEX);

    if ((2 + parameterCount + 1) > maxPacketSize)
    {
        PROTOCOL_DEBUG_PRINTLN("Error: Packet size exceeds max limit");
        return false;
    }

    uint8_t packet[maxPacketSize];
    packet[0] = header;
    packet[1] = commandType;

    for (uint8_t i = 0; i < parameterCount; i++)
    {
        packet[2 + i] = parameters[i];
    }

    if (checkChecksum)
    {
        uint8_t checksum = calculateChecksum(packet, 2 + parameterCount);
        packet[2 + parameterCount] = checksum;
        PROTOCOL_DEBUG_PRINT("Calculated checksum: ");
        PROTOCOL_DEBUG_PRINTLN(checksum, HEX);
    }

    serial.write(packet, 2 + parameterCount + (checkChecksum ? 1 : 0));
    PROTOCOL_DEBUG_PRINTLN("Packet sent");
    return true;
} */
