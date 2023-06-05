  //Import serial library
  #include "SerialTransfer.h"

  //The following libraries are used to read out the raw oxymeter data
  #include <Wire.h>
  #include "MAX30102.h"

  //Set up the sensor and the connection
  MAX30102 particleSensor;
  SerialTransfer myTransfer;

  const int samplerate = 100; //Sample rate in Hz
  volatile unsigned long timestamp = 0;
  volatile uint16_t ecg = 0;
  volatile uint16_t gsr = 0;
  volatile uint32_t red = 0;
  volatile uint32_t ir = 0;
  volatile unsigned long lastSampleTime = 0;
  const int analogPin1 = A0; // Analog input pin 1
  const int analogPin2 = A1; // Analog input pin 2

  struct __attribute__((packed)) STRUCT {
    unsigned long Timestamp;
    uint16_t ECG;
    uint16_t GSR;
    uint32_t Red;
    uint32_t IR;
  } testStruct;


  void setup()
  {
    //Initiate serial connection
    Serial.begin(115200);
    // while(!Serial);
    // Serial.println();
    // Serial.println("MAX30102");
    // Serial.println();
    myTransfer.begin(Serial);
    myTransfer.txObj(lastSampleTime);

    // Initialize sensor
    if (particleSensor.begin(Wire, I2C_SPEED_FAST) == false)
    {
      Serial.println("MAX30102 was not found. Please check wiring/power. ");
      while (1);
    }
    
    //Configure sensor
    byte powerLevel = 0x1F; //Options: 0=Off to 255=50mA. //Use 6.4mA for LED drive (default).
    byte sampleAverage = 1; //Options: 1, 2, 4, 8, 16, 32. //Use 4 (default).
    byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green (Green LED not available for MAX30102). //Use 2 (default is 3).
    int sampleRate = samplerate; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200. //Use same value as for other sensors defined earlier = 100 (default is 400).
    int pulseWidth = 411; //Options: 69, 118, 215, 411. //Use largest option = 411 (default)
    int adcRange = 4096; //Options: 2048, 4096, 8192, 16384. //Use 4096 (default)
    particleSensor.setup(powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); 
    // particleSensor.setup();

    //Set up timer to trigger interrupt at fixed interval
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = (16E6 / 256 / samplerate) -1; // Set compare match value
    TCCR1B |= (1 << WGM12); // CTC mode
    TCCR1B |= (1 << CS12); // 256 prescaler
    TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt
    interrupts();
  } 

  ISR(TIMER1_COMPA_vect) {
    // TestPrint();
    TransferData();
  }

  void TestPrint() {  
    // Serial.print(timestamp);
    Serial.print(millis());
    Serial.print(" ");
    Serial.print(ecg);
    // Serial.print(analogRead(analogPin1));
    Serial.print(" ");
    Serial.print(gsr);
    // Serial.print(analogRead(analogPin2));
    Serial.print(" ");
    Serial.print(red);
    Serial.print(" ");
    Serial.print(ir);
    Serial.println();
  }

  void TransferData() {
    //Read analog inputs and build the struct
    testStruct.Timestamp = millis();
    testStruct.ECG = ecg;
    testStruct.GSR = gsr;
    testStruct.Red = red;
    testStruct.IR = ir;

    // Transfer sampled data using serial
    uint16_t sendSize = 0;
    uint16_t length = 16; //4+2+2+(2*4)=16 bytes

    ///////////////////////////////////////// Stuff buffer with struct
    sendSize = myTransfer.txObj(testStruct, sendSize, length);
    ///////////////////////////////////////// Send buffer
    myTransfer.sendData(sendSize);

    //Update last sample time
    lastSampleTime += (1000 / samplerate);
    myTransfer.txObj(lastSampleTime);

  }

  void loop()
  {

    // timestamp = millis(); //Creating the timestamp. +1 to make it easier to read.
    ecg = analogRead(analogPin1);
    gsr = analogRead(analogPin2);
    red = particleSensor.getRed();
    ir = particleSensor.getIR();


    // particleSensor.check(); //Check the sensor
    // while (particleSensor.available()) {
    //   timestamp = millis();
    //   ecg = analogRead(analogPin1);
    //   gsr = analogRead(analogPin2);
    //   red = particleSensor.getFIFORed();
    //   ir = particleSensor.getFIFOIR();
    //   particleSensor.nextSample();      
    // }

  }
