# Documentation for Using the Custom UART Protocol Class

## Overview
This custom UART protocol class facilitates structured communication between two devices, such as Arduinos, over a UART interface. It supports sending and receiving packets with a fixed format and provides logging for debugging, which can be enabled or disabled at compile time.

The packet format is:

```
[Header][Command Type][Parameter 1][Parameter 2]...[Parameter n][Checksum (optional)]
```

The number of parameters is determined dynamically based on a command lookup table provided during class initialization.

---

## Features
1. **Dynamic Parameter Count:** Uses a lookup table to determine the number of parameters for a given command type.
2. **Checksum Validation:** Optionally calculate and verify a checksum for data integrity.
3. **Debug Logging:** Provides detailed logs for debugging, which can be enabled or disabled using the `DEBUG_LOGS` macro.
4. **Error Handling:** Ensures proper packet formatting and size validation.
5. **Header Synchronization:** Reads until the header byte is found to ensure proper packet alignment.

---

## Class Constructor
### Syntax
```cpp
UARTProtocol(HardwareSerial& serialPort, uint8_t headerByte, uint8_t maxPacketSize, unsigned long baudRate, uint8_t (*lookupTable)(uint8_t));
```

### Parameters
- **serialPort:** Reference to a hardware serial port (e.g., `Serial1`).
- **headerByte:** The header byte that marks the start of a packet.
- **maxPacketSize:** Maximum allowable packet size.
- **baudRate:** UART baud rate.
- **lookupTable:** Function pointer to a lookup table that returns the expected number of parameters for a given command type.

---

## Initialization
### Syntax
```cpp
void begin();
```
### Description
Initializes the serial port with the specified baud rate and logs initialization status if debugging is enabled.

---

## Sending Packets
### Syntax
```cpp
bool sendPacket(uint8_t commandType, uint8_t* parameters, uint8_t parameterCount, bool checkChecksum = true);
```

### Parameters
- **commandType:** The command type byte.
- **parameters:** Pointer to an array of parameter bytes.
- **parameterCount:** Number of parameters in the array.
- **checkChecksum (optional):** If `true`, appends a checksum to the packet.

### Returns
- **`true`:** Packet sent successfully.
- **`false`:** Invalid parameters or packet size exceeds the limit.

### Example
```cpp
uint8_t parameters[] = {0x10, 0x20};
protocol.sendPacket(0x01, parameters, 2);
```

---

## Receiving Packets
### Syntax
```cpp
bool receivePacket(uint8_t& commandType, uint8_t* parameters, uint8_t& parameterCount, bool checkChecksum = true);
```

### Parameters
- **commandType:** Reference to store the received command type.
- **parameters:** Pointer to an array to store the received parameters.
- **parameterCount:** Reference to store the number of received parameters.
- **checkChecksum (optional):** If `true`, verifies the checksum.

### Returns
- **`true`:** Packet received successfully.
- **`false`:** Error in packet reception (e.g., checksum mismatch, invalid size).

### Example
```cpp
uint8_t commandType;
uint8_t parameters[5];
uint8_t parameterCount;

if (protocol.receivePacket(commandType, parameters, parameterCount)) {
    Serial.println("Packet received successfully!");
} else {
    Serial.println("Error receiving packet.");
}
```

---

## Debug Logging
### Enabling/Disabling Logs
To enable or disable debugging logs, modify the `DEBUG_LOGS` macro at the top of the code:

```cpp
#define DEBUG_LOGS 1 // Enable logs
// or
#define DEBUG_LOGS 0 // Disable logs
```

### Debugging Statements
When enabled, logs provide insights into:
- Initialization
- Packet composition and checksum calculations
- Errors during packet transmission or reception

---

## Example Usage
### Complete Example
```cpp
#include <Arduino.h>

// Example parameter lookup table
uint8_t exampleLookupTable(uint8_t commandType) {
    switch (commandType) {
        case 0x01: return 2; // Command type 0x01 expects 2 parameters
        case 0x02: return 3; // Command type 0x02 expects 3 parameters
        case 0x03: return 1; // Command type 0x03 expects 1 parameter
        default: return 0; // Unknown command type
    }
}

// Initialize protocol instance
UARTProtocol protocol(Serial1, 0xAA, 10, 9600, exampleLookupTable);

void setup() {
    Serial.begin(9600);
    protocol.begin();

    // Example: Sending a packet
    uint8_t parameters[] = {0x10, 0x20};
    if (!protocol.sendPacket(0x01, parameters, 2)) {
        Serial.println("Failed to send packet");
    }
}

void loop() {
    // Example: Receiving a packet
    uint8_t commandType;
    uint8_t parameters[5];
    uint8_t parameterCount;

    if (protocol.receivePacket(commandType, parameters, parameterCount)) {
        Serial.println("Packet received successfully!");
    }
}
```

---

## Notes
1. Ensure the header byte matches on both sender and receiver.
2. The `maxPacketSize` should accommodate the header, command type, parameters, and optional checksum.
3. Adjust the `parameterLookup` function to handle your specific command types and expected parameters.

This document outlines the class's design and its usage, helping you implement robust UART communication in your projects.

