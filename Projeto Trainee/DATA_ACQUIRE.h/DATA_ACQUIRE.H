#ifndef DATA_ACQUIRE_H
#define DATA_ACQUIRE_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <cmath> // Adicionado para std::abs

// Definição do subtipo aceleração
struct Acceleration {
    float accx;
    float accy;
    float accz;
};

// Classe responsável pela interação com o hardware MPU
class SensorMPU {
private:
    Acceleration raw_acceleration;
    Acceleration filtered_acceleration;
    Acceleration acc_offset; // importante para determinar a calibração 
    Adafruit_MPU6050 mpu;

    void offsetCalibration();
    void getRawAcceleration();

public:
    SensorMPU();
 
    float getFilteredAcc();
    bool initializeSensor();

    // Funções de Temperatura (Expostas para o módulo de Segurança)
    float getTemperature() {
        sensors_event_t temp_event;
        // Obter apenas a temperatura
        mpu.getEvent(nullptr, nullptr, &temp_event); 
        return temp_event.temperature;
    }
};

class DataProcessing {
private:
    SensorMPU &_sensor; // definição por composição

    float instant_speed;
    float total_distance;

    unsigned long last_reading_time;

    unsigned long timeAccIsZero();

public:
    DataProcessing(SensorMPU &sensor_aux);

    float getInstantSpeed();
    void updateSpeed();

    float getTotalDistance(); // Função de distância total
};

#endif