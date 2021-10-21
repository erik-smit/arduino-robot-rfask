// ask_transmitter.pde
// -*- mode: C++ -*-
// Simple example of how to use RadioHead to transmit messages
// with a simple ASK transmitter in a very simple way.
// Implements a simplex (one-way) transmitter with an TX-C1 module
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12

#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h> // Arduino pin i/o class header

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
hd44780_pinIO lcd( rs, en, d4, d5, d6, d7);

#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h> // Not actually used but needed to compile
#endif

const int VRxPin = A1;
const int VRyPin = A2;
const int SWPin = 3;
const int rfTxPin = 2;
const int rfRxPin = 0;

struct RFDataStruct {
  int speedLeft;
  int speedRight;
};

struct RFDataStruct RFData;
struct RFDataStruct RFDataPrev;

RH_ASK driver(2000, 0, rfTxPin);

void setup()
{
    lcd.begin(16, 2);
#ifdef RH_HAVE_SERIAL
    Serial.begin(9600);	  // Debugging only
#endif
    if (!driver.init())
#ifdef RH_HAVE_SERIAL
         Serial.println("init failed");
#else
	;
#endif
}

void loop()
{
    int VRx = analogRead(VRxPin);
    int VRy = analogRead(VRyPin);
    int SW = digitalRead(SWPin);

    int frontBack = map(VRx, 0, 1023, -255, 255);
    
    RFData.speedLeft = map(VRy, 0, 1023, -255, 255);
    RFData.speedRight = map(VRy, 0, 1023, 255, -255);

    RFData.speedLeft += frontBack;
    RFData.speedRight += frontBack;

    RFData.speedLeft = constrain(RFData.speedLeft, -255, 255);
    RFData.speedRight = constrain(RFData.speedRight, -255, 255);

    if (RFData.speedLeft != RFDataPrev.speedLeft ||
      RFData.speedRight != RFDataPrev.speedRight) {
      driver.send((uint8_t *)&RFData, sizeof(struct RFDataStruct));
      driver.waitPacketSent();
      memcpy(&RFDataPrev, &RFData, sizeof(struct RFDataStruct));
    }
}
