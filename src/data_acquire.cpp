#include "data_acquire.hpp"

#include<cmath>

// Classe SensorMPU

SensorMPU::SensorMPU()  // Construtor
    : raw_acceleration({0,0,0}),
      filtered_acceleration({0,0,0}),
      acc_offset({0,0,0})
{}

// Definição das funções da classe
bool SensorMPU::initializeSensor(){

    // inicialização do sensor
    if (!mpu.begin()) {

        Serial.println("Falha ao encontrar chip MPU6050");
        delay(10);
        return false;
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);   // Range de medição em +- 2G 
    mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);     // Recomendado para reduzir ruído

    // Sample Rate default = 1kHz

    offsetCalibration();    // Calibração do acelerômetro

    return true;
}

void SensorMPU::offsetCalibration(){    // Determina qual o offset após fazer a média móvel
    
    enum Axis {
        xAxis = 0,
        yAxis = 1,
        zAxis = 2
    };

    const int nSamples = 100; // valor de teste (pode ser alterado)
                              // 100 samples em 0.1s
    float sum[3] = {0,0,0};
    float average[3] = {0,0,0};
    
    // lógica de "calibração automática"

    for(int i = 0; i < nSamples; i++){

        getRawAcceleration();

        sum[xAxis] += raw_acceleration.accx;
        sum[yAxis] += raw_acceleration.accy;
        sum[zAxis] += raw_acceleration.accz;

        delay(1); // teoricamente isso serve para não sobrecarregar a CPU
    }

    for (int i = 0; i < 3; i++){

        average[i] = sum[i] / nSamples;
        delay(1);
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

    Serial.print("Aceleração no eixo x:");
    Serial.println(raw_acceleration.accx);

    Serial.print("Aceleração no eixo y:");
    Serial.println(raw_acceleration.accy);

    Serial.print("Aceleração no eixo z:");
    Serial.println(raw_acceleration.accz);
}

float SensorMPU::getFilteredAcc(){

    sensors_event_t raw_acc_measure;
    const int nSamples = 3;  // Sample rate = 10hz --> 3 valores lidos = 333ms
    
    filtered_acceleration.accx = 0; // resetando filtered_accceleration.accx

    for (int i = 0; i < nSamples; i++){

        mpu.getEvent(&raw_acc_measure, nullptr, nullptr);  
        filtered_acceleration.accx += (raw_acc_measure.acceleration.x - acc_offset.accx);   // O carrinho anda reto --> acc em apenas um eixo (necessário alterar no momento dos testes)
        delay(1);
    }

    filtered_acceleration.accx = filtered_acceleration.accx / nSamples;

    Serial.print("Aceleração filtrada:");
    Serial.println(filtered_acceleration.accx);

    return filtered_acceleration.accx;

}

// Classe dataProcessing

DataProcessing::DataProcessing(SensorMPU& sensor_aux)
    : _sensor(sensor_aux),
      instant_speed(0),
      total_distance(0),
      last_reading_time(0)
{}

float DataProcessing::getInstantSpeed(){

    Serial.print("Distância total:");
    Serial.println(total_distance);
    return instant_speed;
}

void DataProcessing::updateKinematics(){

    unsigned long current_time = millis();
    const float ZERO_TOLERANCE = 0.05;

    if (last_reading_time == 0){

        last_reading_time = current_time; 
        return;
    }

    float acceleration = _sensor.getFilteredAcc();
    float delta_time = (current_time - last_reading_time) / 1000.0; // ms --> s

    if (std::abs(acceleration) < ZERO_TOLERANCE){
        
        instant_speed = 0;
    }

    else {
        total_distance += instant_speed*delta_time;
    
        instant_speed += acceleration*delta_time;
    }

    last_reading_time = current_time;
}

float DataProcessing::getTotalDistance(){
    
    Serial.print("Distância total:");
    Serial.println(total_distance);
    return total_distance;
}

