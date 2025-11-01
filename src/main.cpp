#include "data_acquire.hpp"
#include <Arduino.h>

SensorMPU mpu;

DataProcessing data(mpu);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // inicialização MPU6050
  mpu.initializeSensor();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World");
  delay(2000);

  mpu.getFilteredAcc();
}

// put function definitions here: