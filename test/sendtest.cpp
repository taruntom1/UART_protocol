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
  byte data[10] = {0x01, 0x02, 0x03};
  protocol.SendCommand(0x01);
  protocol.SendData(data, 3);
  protocol.SendChecksum(0x01, data, 3);


  delay(1000);
}
