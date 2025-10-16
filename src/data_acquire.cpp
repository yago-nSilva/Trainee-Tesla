#include "data_acquire.hpp"

#include<cmath>

// Construtor
SensorMPU::SensorMPU()
    : raw_acceleration({0,0,0}),
      filtered_acceleration({0,0,0}),
      acc_offset({0,0,0})
{}

// Definição das funções da classe
bool SensorMPU::initializeSensor(){

    // inicialização do sensor
    if (!mpu.begin()) {
    Serial.println("Faalha ao encontrar chip MPU6050");
        while (1) {
        delay(10);
        }
    }

   // Range de medição em +- 2G 
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);

    // Calibração do acelerômetro

    while (!isSensorCalibrated()){
        sensorCalibration();
    }

    ////// existe mais algo para fazer?
}

bool SensorMPU::isSensorCalibrated(){

    const float zero_tolerance = 0.05;

    if(std::abs(raw_acceleration.accx) > zero_tolerance)
        return false;
    if(std::abs(raw_acceleration.accy) > zero_tolerance)
        return false;
    if(std::abs(raw_acceleration.accz) > zero_tolerance)
        return false;
    
        return true;
}

bool SensorMPU::sensorCalibration(){
    
    getRawAcceleration();
    // criar lógica de "calibração automática"
}

Acceleration SensorMPU::getRawAcceleration(){
    
    Acceleration raw_acceleration;

    raw
}