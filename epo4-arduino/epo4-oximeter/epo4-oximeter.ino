  //Import serial library
  #include "SerialTransfer.h"

  //The following libraries are used to read out the raw oxymeter data
  #include <Wire.h>
  #include "MAX30105.h"

  //Set up the sensor
  MAX30105 particleSensor;

  SerialTransfer myTransfer;

  const int sampleRate = 100; //Sample rate in Hz
  uint16_t red;
  uint16_t ir;
  volatile unsigned long lastSampleTime = 0;
  const int analogPin1 = A0; // Analog input pin 1
  const int analogPin2 = A1; // Analog input pin 2

  //Create an instance of the serial interface

  struct __attribute__((packed)) STRUCT {
    unsigned long timestamp;
    uint16_t ECG;
    uint16_t GSR;
    uint32_t red;
    uint32_t IR;
  } testStruct;

  char arr[] = "hello";

  /**
  * @brief Function that is executed once, when the Arduino boots
  * 
  */

  void setup()
  {
    //Initiate serial connection
    Serial.begin(57600);
    myTransfer.begin(Serial);
    myTransfer.txObj(lastSampleTime);

    //Set up timer to trigger interrupt at fixed interval
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = (16E6 / 256 / sampleRate) -1; // Set compare match value
    TCCR1B |= (1 << WGM12); // CTC mode
    TCCR1B |= (1 << CS12); // 256 prescaler
    TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt
    interrupts();

    //Initialize the sensor
    //debug.begin(9600);
    //debug.println("MAX30105 Basic Readings Example");

    // Initialize sensor
    if (particleSensor.begin() == false)
    {
      //debug.println("MAX30105 was not found. Please check wiring/power. ");
      while (1);
     }
    
    particleSensor.setup(); //Configure sensor. Use 6.4mA for LED drive
  } 

  ISR(TIMER1_COMPA_vect) {


    //Read analog inputs and build the struct
    testStruct.timestamp = millis()+1; //Creating the timestamp. +1 to make it easier to read.
    testStruct.ECG = analogRead(analogPin1);
    testStruct.GSR = analogRead(analogPin2);
    testStruct.red = particleSensor.getRed();
    testStruct.IR = particleSensor.getIR();

    // Transfer sampled data using serial
    uint16_t sendSize = 0;
    uint16_t length = 16; //4+2+2+(2*2)=14 bytes

    ///////////////////////////////////////// Stuff buffer with struct
    sendSize = myTransfer.txObj(testStruct, sendSize, length);
    ///////////////////////////////////////// Send buffer
    myTransfer.sendData(sendSize);

    //Update last sample time
    lastSampleTime += (1000 / sampleRate);
    myTransfer.txObj(lastSampleTime);
    

    
  }

  void loop()
  {
    //red = particleSensor.getRed();
    //ir = particleSensor.getIR();
  }