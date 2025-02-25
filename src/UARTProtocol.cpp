#include "UARTProtocol.h"

UARTProtocol::UARTProtocol(HardwareSerial &serialPort, uint8_t headerByte, uint8_t maxPacketSize, unsigned long baudRate)
    : serial(serialPort), header(headerByte), maxPacketSize(maxPacketSize), baudRate(baudRate) {}

void UARTProtocol::begin()
{
    serial.begin(baudRate);
    PROTOCOL_DEBUG_PRINTLN("UARTProtocol initialized");
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

bool UARTProtocol::ReadCommand(uint8_t &commandType, uint32_t timeoutMs)
{
    PROTOCOL_DEBUG_PRINTLN("Waiting for header with timeout...");

    unsigned long startTime = millis(); // Record the start time

    while (millis() - startTime < timeoutMs)
    { // Check if timeout has been reached
        serial.setTimeout(1);
        if (serial.available() > 0)
        {                                 // Data available in the buffer
            int nextByte = serial.peek(); // Peek at the next byte without removing it
            if (nextByte == header)
            { // Check if the byte matches the header
                PROTOCOL_DEBUG_PRINTLN("Header byte found!");
                serial.read(); // Consume header byte
                serial.setTimeout(1000);
                if (serial.readBytes(&commandType, 1))
                {
                    PROTOCOL_DEBUG_PRINT("Command type: ");
                    PROTOCOL_DEBUG_PRINTLN(commandType, HEX);
                    return true;
                }
                PROTOCOL_DEBUG_PRINTLN("Error: Command type not received");
                return false;
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

// Check if there is data available to read
bool UARTProtocol::available()
{
    return serial.available();
}
