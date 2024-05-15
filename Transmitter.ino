#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t pipeOut = 0xF9E8F0F0E1LL;   
//IMPORTANT: The same as in the receiver 0xF9E8F0F0E1LL

// 0xF9E8F0F0E1LL is the pipe address for nrf24l01
RF24 radio(8, 9);
 // select CE,CSN pin nrf module by nano

struct PacketData 
{
  byte lxAxisValue;
  byte lyAxisValue;
  byte rxAxisValue;
  byte ryAxisValue;
  byte lPotValue;  
  byte rPotValue;    
  byte switch1Value;
  byte switch2Value;
  byte switch3Value;
  byte switch4Value;  
};
PacketData data;

// PaketData for stored 10 channel input value, tht is trnasmitted to nrf module

void setup()
{
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  radio.stopListening(); 
  //start the radio Transmitter 

  // we set some parameters openWritingPipe & stopListening

  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);      

  // this 4 pins for four switch button 
  // in case we don't use resisters for push button switch
}

//This function is used to map 0-1023 joystick value to 0-254.
// hence 127 is the center value which we send.
//It also adjust the deadband in joystick.
//Jotstick values range from 0-1023. But its center value is not always 511.
// It is little different.
//So we need to add some deadband to center value. in our case 500-530.
// Any value in this deadband range is mapped to center 127.

int mapAndAdjustJoystickDeadBandValues(int value, bool reverse)
{
  if (value >= 530)
  {
    value = map(value, 530, 1023, 127, 254);
  }
  else if (value <= 500)
  {
    value = map(value, 500, 0, 127, 0);  
  }
  else
  {
    value = 127;
  }

  if (reverse)
  {
    value = 254 - value;
  }
  return value;
}

// center value of joystick 127

void loop()
{
  data.lxAxisValue    = mapAndAdjustJoystickDeadBandValues(analogRead(A0), false);
  data.lyAxisValue    = mapAndAdjustJoystickDeadBandValues(analogRead(A1), false);
  data.rxAxisValue    = mapAndAdjustJoystickDeadBandValues(analogRead(A2), false);
  data.ryAxisValue    = mapAndAdjustJoystickDeadBandValues(analogRead(A3), false);
  // joystick value
  data.lPotValue      = map(analogRead(A4), 0, 1023, 0, 254); // this is for a potentiometer
  data.rPotValue      = map(analogRead(A5), 0, 1023, 0, 254); // this is for another potentiometer  
  data.switch1Value   = !digitalRead(2);
  data.switch2Value   = !digitalRead(3);
  data.switch3Value   = !digitalRead(4);
  data.switch4Value   = !digitalRead(5);
  // Push button switch

  radio.write(&data, sizeof(PacketData));
  // We use radio.write to send this 10 channel value
}