#ifndef DATA_ACQUIRE_H
#define DATA_ACQUIRE_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h> 
#include <Adafruit_Sensor.h>
#include <iostream>

// def do subtipo aceleração
    struct Acceleration{
        float accx;
        float accy;
        float accz;
    };

    // classe responsável pela interação com o hardware MPU
    class SensorMPU{
        private:
            Acceleration raw_acceleration;
            Acceleration filtered_acceleration;
            Acceleration acc_offset; //importante para determinar a calibração 
            Adafruit_MPU6050 mpu;

            void sensorCalibration();
            void accelerationFilter();

        public:
            SensorMPU();
            Acceleration getFilteredAcc();
            bool initializeSensor();

    };


#endif