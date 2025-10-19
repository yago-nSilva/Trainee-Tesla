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

            void offsetCalibration();
            void getRawAcceleration();

        public:
            SensorMPU();
          
            float getFilteredAcc();         
            bool initializeSensor();

    };

    class DataProcessing{
        private:
            SensorMPU &_sensor; // definição por composição

            float instant_speed;
            float total_distance;
            
            unsigned long last_reading_time;

            unsigned long timeAccIsZero();

        public:
            DataProcessing(SensorMPU &sensor_aux):_sensor(sensor_aux){}

            float getInstantSpeed();
            void updateSpeed();

            float getTotalDistance();

    };

#endif