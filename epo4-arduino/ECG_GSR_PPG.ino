  
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
  * 
  * Author: Group C2
  * Date: 22-06-2023
  * Project: EPO-4 Biosensing; "Arduino-based Stress Detector"
  * 
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */



  // Import relevant libraries
  #include "SerialTransfer.h"
  #include <Wire.h>
  #include "MAX30102.h"

  // Debug print to Serial Monitor / Plotter
  #define DEBUG

  // Set up the sensor and the connection with python
  MAX30102 particleSensor;
  SerialTransfer myTransfer;

  // Define global variables
  const int samplerate = 400;     // Sample rate in Hz
  const int analogPin1 = A0;
  const int analogPin2 = A1;
  volatile uint16_t ecg = 0;
  volatile uint16_t gsr = 0;
  volatile uint32_t red = 0;
  volatile uint32_t ir = 0;

  // Initialize struct
  struct __attribute__((packed)) STRUCT {
    unsigned long Timestamp;
    uint16_t ECG;
    uint16_t GSR;
    uint32_t Red;
    uint32_t IR;
  } testStruct;

  void setup()
  {
    // Initiate Serial communication at 115200 baud
    Serial.begin(115200);

    #ifdef DEBUG
      // Print output via Serial
      Serial.println();
      Serial.println("MAX30102");
      Serial.println();
    #else
      // Use Serial Transfer to send output to python
      myTransfer.begin(Serial);
    #endif

    // Initialize sensor with a high speed connection
    if (particleSensor.begin(Wire, I2C_SPEED_FAST) == false)
    {
      Serial.println("MAX30102 was not found. Please check wiring/power. ");
      while (1);
    }
    
    // Configure sensor parameters for optimal resolution (18 bit)
    byte powerLevel = 0x7F;   // Options: 0=Off to 255=50mA
    byte sampleAverage = 1;   // Options: 1, 2, 4, 8, 16, 32
    byte ledMode = 2;         // Options: 1 = Red only, 2 = Red + IR
    int sampleRate = 400;     // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
    int pulseWidth = 411;     // Options: 69, 118, 215, 411
    int adcRange = 16384;     // Options: 2048, 4096, 8192, 16384
    particleSensor.setup(powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

    // Set up timer to trigger interrupts at a rate equal to the sample rate
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = (16E6 / 256 / samplerate) -1;   // Set compare match value
    TCCR1B |= (1 << WGM12);                 // CTC mode
    TCCR1B |= (1 << CS12);                  // 256 prescaler
    TIMSK1 |= (1 << OCIE1A);                // Enable timer compare interrupt
    interrupts();
  } 

  // Interrupt Service Routine function that runs every time an interrupt is done
  ISR(TIMER1_COMPA_vect) {
    #ifdef DEBUG
      // Forward to debugging function
      TestPrint();
    #else
      // Forward to regular operation function
      TransferData();
    #endif
  }

  // Function that prints output row by row to Serial Monitor / Plotter 
  void TestPrint() {
    Serial.print(millis());
    Serial.print(" ");
    Serial.print(ecg);
    Serial.print(" ");
    Serial.print(gsr);
    Serial.print(" ");
    Serial.print(red);
    Serial.print(" ");
    Serial.print(ir);
    Serial.println();
  }

  // Function that sends new data samples to python everytime its called
  void TransferData() {
    // Use most recent sensor values to fill the struct
    testStruct.Timestamp = millis();
    testStruct.ECG = ecg;
    testStruct.GSR = gsr;
    testStruct.Red = red;
    testStruct.IR = ir;

    // Define starting index and length of data to be send from buffer
    uint16_t sendSize = 0;
    uint16_t length = 16; //4+2+2+(2*4)=16 bytes

    ///////////////////////////////////////// Stuff buffer with struct
    sendSize = myTransfer.txObj(testStruct, sendSize, length);
    ///////////////////////////////////////// Send buffer
    myTransfer.sendData(sendSize);
  }

  void loop()
  {
    // Continuously read sensor values and store them in temporary variables
    ecg = analogRead(analogPin1);
    gsr = analogRead(analogPin2);
    red = particleSensor.getRed();
    ir = particleSensor.getIR();
  }
