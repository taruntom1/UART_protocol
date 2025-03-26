# UARTProtocol Library

A lightweight Arduino library for handling UART communication with a structured protocol. This library allows sending and receiving commands and data using a header-based protocol with a configurable baud rate.

## Features
- Send and receive commands with a header byte
- Send and receive data with configurable packet sizes
- Timeout handling for reading commands and data
- Debug logging (configurable)

## Installation
Copy the `UARTProtocol.h` and `UARTProtocol.cpp` files into your Arduino project and include them in your sketch.

## Usage

### 1. Include the Library
```cpp
#include "UARTProtocol.h"
```

### 2. Create an Instance
```cpp
HardwareSerial &serialPort = Serial1; // Use Serial1 or any available UART port
UARTProtocol uart(serialPort, 0xAA, 64, 115200);
```

### 3. Initialize the Protocol
```cpp
void setup() {
    Serial.begin(115200); // Debugging output
    uart.begin();
}
```

### 4. Sending Commands
```cpp
uint8_t command = 0x01;
uart.SendCommand(command);
```

### 5. Sending Data
```cpp
byte data[] = {0x10, 0x20, 0x30};
uart.SendData(data, sizeof(data));
```

### 6. Receiving Commands
```cpp
uint8_t receivedCommand;
if (uart.ReadCommand(receivedCommand)) {
    Serial.print("Received command: ");
    Serial.println(receivedCommand, HEX);
}
```

### 7. Receiving Data
```cpp
byte receivedData[3];
if (uart.ReadData(receivedData, 3)) {
    Serial.println("Received Data: ");
    for (int i = 0; i < 3; i++) {
        Serial.print(receivedData[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}
```

### 8. Checking for Available Data
```cpp
if (uart.available()) {
    Serial.println("Data available in buffer");
}
```

## Debugging
Enable debugging by setting `PROTOCOL_DEBUG_LOGS` to `1` in `UARTProtocol.h`:
```cpp
#define PROTOCOL_DEBUG_LOGS 1
```
This will print debug logs to the serial monitor.

## License
This library is open-source and free to use under the MIT License.

