#include "DATA_ACQUIRE.H"

// =================================================================
// IMPLEMENTAÇÃO DA CLASSE SensorMPU
// =================================================================

SensorMPU::SensorMPU()
    : raw_acceleration({0,0,0}),
      filtered_acceleration({0,0,0}),
      acc_offset({0,0,0})
{}

bool SensorMPU::initializeSensor(){
    // A inicialização do sensor MPU (mpu.begin()) é feita no setup_mpu() do módulo de segurança
    // Aqui apenas definimos as configurações.
    
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);   // Range de medição em +- 2G 
    mpu.setFilterBandwidth(MPU6050_BAND_10_HZ);     // Recomendado para reduzir ruído

    offsetCalibration();    // Calibração do acelerômetro

    return true;
}

void SensorMPU::offsetCalibration(){
    enum Axis { xAxis = 0, yAxis = 1, zAxis = 2 };

    const int nSamples = 100; // valor de teste
    float sum[3] = {0,0,0};

    for(int i = 0; i < nSamples; i++){

        getRawAcceleration();

        sum[xAxis] += raw_acceleration.accx;
        sum[yAxis] += raw_acceleration.accy;
        sum[zAxis] += raw_acceleration.accz;

        delay(1);
    }

    acc_offset.accx = sum[xAxis] / nSamples;
    acc_offset.accy = sum[yAxis] / nSamples;
    // Subtrai 9.8 (gravidade) assumindo que o eixo Z está vertical
    acc_offset.accz = (sum[zAxis] / nSamples) - 9.8; 

}

void SensorMPU::getRawAcceleration(){
    sensors_event_t raw_acc_measure;
    mpu.getEvent(&raw_acc_measure, nullptr, nullptr);

    raw_acceleration.accx = raw_acc_measure.acceleration.x;
    raw_acceleration.accy = raw_acc_measure.acceleration.y;
    raw_acceleration.accz = raw_acc_measure.acceleration.z;
}

float SensorMPU::getFilteredAcc(){
    sensors_event_t raw_acc_measure;
    const int nSamples = 3;
 
    float sum_accx = 0; // Variável local para soma

    for (int i = 0; i < nSamples; i++){

        mpu.getEvent(&raw_acc_measure, nullptr, nullptr);
        sum_accx += (raw_acc_measure.acceleration.x - acc_offset.accx); // Aplica offset
        delay(1);
    }

    filtered_acceleration.accx = sum_accx / nSamples;

    return filtered_acceleration.accx;
}

// =================================================================
// IMPLEMENTAÇÃO DA CLASSE DataProcessing
// =================================================================

DataProcessing::DataProcessing(SensorMPU &sensor_aux)
    : _sensor(sensor_aux),
      instant_speed(0),
      total_distance(0),
      last_reading_time(0)
{}

float DataProcessing::getInstantSpeed(){
    return instant_speed;
}

float DataProcessing::getTotalDistance(){
    return total_distance;
}

void DataProcessing::updateSpeed(){
    unsigned long current_time = millis();

    if (last_reading_time == 0){
        last_reading_time = current_time;
        return;
    }

    float acceleration = _sensor.getFilteredAcc();
    // Calcula delta_time em segundos
    float delta_time = (current_time - last_reading_time) / 1000.0f; 

    // Cálculo de Integração (Velocidade)
    float speed_delta = acceleration * delta_time;
    instant_speed += speed_delta;
    
    // Cálculo de Integração (Distância) - Simplificado
    total_distance += instant_speed * delta_time; 

    last_reading_time = current_time;
}

unsigned long DataProcessing::timeAccIsZero(){
    float zero_acceleration_check;

    const float ZERO_TOLERANCE = 0.05;

    zero_acceleration_check = _sensor.getFilteredAcc();

    if (std::abs(zero_acceleration_check) > ZERO_TOLERANCE)
        return millis();

    else return 0;
}