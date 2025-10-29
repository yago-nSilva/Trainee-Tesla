#include <Arduino.h> // Essencial no PlatformIO para usar funções como pinMode(), digitalWrite(), etc.
#include <Wire.h>
#include "DATA_ACQUIRE.H" // Inclui as classes SensorMPU e DataProcessing

// --- Variáveis de Classe (Instâncias) ---
// O objeto SensorMPU é necessário para a Segurança (temperatura) e Aquisição de Dados.
SensorMPU sensor_mpu; 
DataProcessing data_processor(sensor_mpu); // Instancia o processador usando a referência do sensor

// --- Funções de Segurança e Controle (Protótipos) ---
// Estas funções estão implementadas no seu arquivo principal de segurança/controle
void acionar_shutdown(); 
void set_led_rgb(int R, int G, int B);
void indicar_erro();
void indicar_pronto_rtd();
void indicar_aguardo();
void setup_mpu();
bool verificar_logica_seguranca();
void logica_rtd(); 
void controle_motores(bool habilitado);
float ler_tensao_bateria();
void enviar_dados_telemetria(float tensao, float temperatura);

// [Insira todas as suas constantes globais aqui, como SHUTDOWN_PIN, TEMP_MAX, etc.]
// ...
// ...

void setup() {
    Serial.begin(115200);

    // 1. Configuração de Pinos
    pinMode(SHUTDOWN_PIN, OUTPUT);
    digitalWrite(SHUTDOWN_PIN, HIGH); // Inicializa como PERMITIDO (Ativo Baixo)

    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(MOTOR_PIN, OUTPUT);
    
    // Configuração dos Pinos de ENTRADA
    pinMode(VOLTAGE_PIN, INPUT);
    pinMode(BUTTON_RTD_PIN, INPUT_PULLUP); // PULLUP para o botão RTD
    
    // 2. Inicialização dos Periféricos
    setup_mpu(); // Inicializa MPU6050 (inclui mpu.begin() e checagem de erro)
    
    // 3. Estado Inicial
    indicar_aguardo(); // LED Azul (Aguardo)
}

void loop() {
    
    // 1. VERIFICAÇÃO DE SEGURANÇA (VETO)
    // Se erro_ativo for true, o sistema trava o estado.
    bool erro_ativo = verificar_logica_seguranca(); 

    // 2. AQUISIÇÃO DE DADOS (Processa dados de aceleração)
    data_processor.updateSpeed(); 

    if (erro_ativo) {
        indicar_erro(); // LED Vermelho
        controle_motores(false); // Garante que o motor seja cortado
    } else {
        // 3. CONTROLE: Se a segurança está OK, roda a lógica RTD
        logica_rtd();
    }

    // 4. TELEMETRIA (Chamada contínua)
    enviar_dados_telemetria(ler_tensao_bateria(), sensor_mpu.getTemperature());
    
    delay(500); 
}