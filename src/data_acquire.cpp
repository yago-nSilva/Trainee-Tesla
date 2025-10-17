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
        Serial.println("Falha ao encontrar chip MPU6050");
        while (1) {
        delay(10);
        }
    }

   // Range de medição em +- 2G 
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setFilterBandwidth(MPU6050_BAND_10_HZ); // Recomendado para reduzir ruído

    // Calibração do acelerômetro

    offsetCalibration();

}

void SensorMPU::offsetCalibration(){    // Determina qual o offset após fazer a média móvel
    
    enum Axis {
        xAxis = 0,
        yAxis = 1,
        zAxis = 2
    };

    const int nSamples = 100; // valor "alterável"
    float sum[3] = {0,0,0};
    float average[3] = {0,0,0};
    
    // lógica de "calibração automática"

    for(int i = 0; i < nSamples; i++){

        getRawAcceleration();

        sum[xAxis] += raw_acceleration.accx;
        sum[yAxis] += raw_acceleration.accy;
        sum[zAxis] += raw_acceleration.accz;

        getRawAcceleration();

        delay(1); // teoricamente isso serve para não sobrecarregar a CPU
    }

    for (int i = 0; i < 3; i++){
        average[i] = sum[i] / nSamples;
    }

    acc_offset.accx = average[xAxis];
    acc_offset.accy = average[yAxis];
    acc_offset.accz = average[zAxis] - 9.78;

}

void SensorMPU::getRawAcceleration(){
    
    sensors_event_t raw_acc_measure; 

    mpu.getEvent(&raw_acc_measure, nullptr, nullptr);

    raw_acceleration.accx = raw_acc_measure.acceleration.x;
    raw_acceleration.accy = raw_acc_measure.acceleration.y;
    raw_acceleration.accz = raw_acc_measure.acceleration.z;

}

// void SensorMPU::getFilteredAcc(){}