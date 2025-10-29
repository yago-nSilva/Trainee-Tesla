#include <Wire.h>
#include <Adafruit_MPU6050.h> 
#include <Adafruit_Sensor.h>

bool sistema_em_erro = false; 

const int SHUTDOWN_PIN = 15
const int LED_R_PIN = 18;    
const int LED_G_PIN = 19;    
const int LED_B_PIN = 21;    
const int VOLTAGE_PIN = 35; 

const int ON = LOW;
const int OFF = HIGH;
const float VREF = 3.3; 
const float ADC_MAX = 4095.0; 
const float FATOR_TENSAO_REAL = 2.6;

const float TEMP_MAX = 45.0;     
const float TENSAO_MAX = 8.4;      
const float TENSAO_MIN = 6.0;      
const float TENSAO_MIN_FALHA = 1.0; 

Adafruit_MPU6050 mpu; 

void acionar_shutdown(){
    sistema_em_erro = true;
    digitalWrite(SHUTDOWN_PIN, HIGH);
    Serial.println("SHUTDOWN ACIONADO!");
}

void set_led_rgb(int R, int G, int B) {
    digitalWrite(LED_R_PIN, R);
    digitalWrite(LED_G_PIN, G);
    digitalWrite(LED_B_PIN, B); 
}

void indicar_erro() {
    set_led_rgb(ON, OFF, OFF); 
}

void indicar_pronto_rtd() {
    set_led_rgb(OFF, ON, OFF); 
}

void indicar_aguardo() {
    set_led_rgb(OFF, OFF, ON);
}

float ler_temperatura_mpu() {
    sensors_event_t a, g, temp_event;
    mpu.getEvent(&a, &g, &temp_event);
    return temp_event.temperature; 
}

float ler_tensao_bateria() {
    int leitura_bruta = analogRead(VOLTAGE_PIN); 
    float tensao_volts_adc = (float)leitura_bruta * (VREF / ADC_MAX);
    float tensao_real_bateria = tensao_volts_adc * FATOR_TENSAO_REAL; 
    return tensao_real_bateria;
}

bool verificar_logica_seguranca() {
    if (sistema_em_erro) return true; 

    float temp_atual = ler_temperatura_mpu();
    float tensao_atual = ler_tensao_bateria();
    bool falha_detectada = false;

    if (temp_atual > TEMP_MAX) {
        Serial.println("ERRO: Superaquecimento!");
        falha_detectada = true;
    }

    else if (tensao_atual > TENSAO_MAX) {
        Serial.println("ERRO: Sobretensão!");
        falha_detectada = true;
    }
    else if (tensao_atual < TENSAO_MIN) {
        Serial.println("ERRO: Subtensão!");
        falha_detectada = true;
    }
    else if (tensao_atual < TENSAO_MIN_FALHA) {
        Serial.println("ERRO: Falha no Chicote de Tensão.");
        falha_detectada = true;
    }

    if (falha_detectada) {
        acionar_shutdown();
        return true;
    }

    return false;
}

void setup_mpu() {
    Wire.begin(); 
    
    if (!mpu.begin()) {
        Serial.println("ERRO FATAL: MPU6050 não encontrado!");
        acionar_shutdown(); 
        while (1) delay(10);
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT);
    pinMode(VOLTAGE_PIN, INPUT);
    
    setup_mpu();

    indicar_aguardo();
}

void loop() {
    
    bool erro_ativo = verificar_logica_seguranca(); 

    if (erro_ativo) {
        indicar_erro();
    } else {
        acionar_rtd();
        indicar_pronto_rtd();
    }
    delay(500);
}

