#include <Arduino.h>
#include "UARTProtocol.h"



UARTProtocol protocol(Serial, 0xAA, 10, 9600);

void setup()
{
  protocol.begin();
  Serial.begin(9600);
}

void loop()
{
  uint8_t commandType;
  byte data[10];
  if (protocol.ReadCommand(commandType))
  {
    Serial.print("Received Command: ");
    Serial.println(commandType);
    if (protocol.ReadData(data, 3, 1000))
    {
      Serial.print("Received Data: ");
      for (int i = 0; i < 3; i++)
      {
        Serial.print(data[i], HEX);
      }

      if (protocol.VerifyChecksum(commandType, data, 3))
      {
        Serial.println("Checksum verified");
      }
      else
      {
        Serial.println("Checksum verification failed");
      }
    }
    else
    {
      Serial.println("Failed to read data");
    }
  }
  else
  {
    Serial.println("Failed to read command...");
  }
  delay(1000);
}
