// ask_receiver.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to receive messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) receiver with an Rx-B1 module
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12

#include <ArduinoRobot.h>
#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

const unsigned long receiveTimeout = 1000;
unsigned long previousReceive = 0;
unsigned long currentTime = 0;

const int rfRxPin = A1;
RH_ASK driver(2000, rfRxPin, 0);

struct RFDataStruct {
  int speedLeft;
  int speedRight;
};

RFDataStruct RFData;
uint8_t RFDatalen = sizeof(RFData);


void setup()
{
#ifdef RH_HAVE_SERIAL
    Serial.begin(9600);	  // Debugging only
#endif
    if (!driver.init())
#ifdef RH_HAVE_SERIAL
         Serial.println("init failed");
#else
	;
#endif

  Robot.begin();
    // initialize the screen
  Robot.beginTFT();
}

void loop()
{
    currentTime = millis();
    
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);

    if (driver.recv((uint8_t*)&RFData, &RFDatalen))
    {
      previousReceive = currentTime;
      Robot.motorsWrite(
        RFData.speedLeft,
        RFData.speedRight
      );

      Robot.debugPrint(RFData.speedLeft, 0, 10);
      Robot.debugPrint(RFData.speedRight, 0, 20);
    }

    if (currentTime - previousReceive >= receiveTimeout) {
      Robot.motorsWrite(0,0);
    }
}
