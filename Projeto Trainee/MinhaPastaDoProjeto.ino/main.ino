#include "DATA_ACQUIRE.H"
#include <Wire.h>


bool sistema_em_erro = false; 

// Pinos de Controle e Sinalização 
const int SHUTDOWN_PIN = 15;     
const int LED_R_PIN = 18;
const int LED_G_PIN = 19;
const int LED_B_PIN = 21;
const int VOLTAGE_PIN = 35;
const int BUTTON_RTD_PIN = 23;   
const int BUZZER_PIN = 5;        
const int MOTOR_PIN = 22;        // Pino de controle PWM do motor

// Constantes Técnicas e Limites
const int ON = LOW;
const int OFF = HIGH;
const float VREF = 3.3; 
const float ADC_MAX = 4095.0; 
const float FATOR_TENSAO_REAL = 2.6; // Fator do divisor 16k/10k

const float TEMP_MAX = 45.0; 
const float TENSAO_MAX = 8.4; 
const float TENSAO_MIN = 6.0; 
const float TENSAO_MIN_FALHA = 1.0; 
const unsigned long BUZZER_DURATION = 3000; // 3 segundos

// Variáveis de Estado de Controle ---
int rtd_state = HIGH; // Estado do botão RTD
int shut_state = HIGH; // Estado do botão Shutdown manual
unsigned long tempoInicial = 0; // Usado para debounce
bool buzzerOn = false;
unsigned long buzzer_tempo_inicio = 0; 
bool motor_habilitado = false;

// Objetos (Instâncias das Classes)
SensorMPU sensor_mpu;
DataProcessing data_processor(sensor_mpu);

//////////////// FUNÇÕES DE ATUAÇÃO

void acionar_shutdown() {
    sistema_em_erro = true;
    digitalWrite(SHUTDOWN_PIN, HIGH); // HIGH ativa o corte 
    Serial.println(">>> SHUTDOWN ACIONADO! Energia cortada. <<<");
}

void set_led_rgb(int R, int G, int B) {
    digitalWrite(LED_R_PIN, R);
    digitalWrite(LED_G_PIN, G);
    digitalWrite(LED_B_PIN, B); 
}

void indicar_erro() { set_led_rgb(ON, OFF, OFF); }
void indicar_pronto_rtd() { set_led_rgb(OFF, ON, OFF); }
void indicar_aguardo() { set_led_rgb(OFF, OFF, ON); }

////////////////// LEITURA E VALIDAÇÃO (SEGURANÇA)

float ler_tensao_bateria() {
    int leitura_bruta = analogRead(VOLTAGE_PIN); 
    float tensao_volts_adc = (float)leitura_bruta * (VREF / ADC_MAX);
    float tensao_real_bateria = tensao_volts_adc * FATOR_TENSAO_REAL; 
    return tensao_real_bateria;
}

// Lógica de Segurança
bool verificar_logica_seguranca() {
    if (sistema_em_erro) return true; 

    float temp_atual = sensor_mpu.getTemperature(); // Leitura da Temp. da classe SensorMPU
    float tensao_atual = ler_tensao_bateria();
    bool falha_detectada = false;

    // Lógica de monitoramento (Simplificada para brevity)
    if (temp_atual > TEMP_MAX || tensao_atual > TENSAO_MAX || tensao_atual < TENSAO_MIN || tensao_atual < TENSAO_MIN_FALHA) {
        falha_detectada = true;
        // (Adicionar Serial.println detalhada aqui)
     }

    // Adiciona o botão de Shutdown Manual (Controle) na verificação de segurança
    if (digitalRead(SHUTDOWN_PIN) == LOW || falha_detectada) {
        acionar_shutdown();
        return true;
    }

    return false;
}

//////// lÓGICA DE CONTROLE

int debounce(int estado, const int Pin){
    int stateNamoral = digitalRead(Pin);

    if(estado != stateNamoral){
        if(millis() - tempoInicial >= 50){
            stateNamoral = digitalRead(Pin);
            tempoInicial = millis();
        }
    }
    return stateNamoral;
}

bool MudarEstado(int &estado, const int Pin){
    // Simplificado e adaptado do seu debounce/MudarEstado original
    if(debounce(estado, Pin) == LOW && estado == HIGH){
        estado = LOW; // Estado LIGADO (RTD Acionado)
        return true;
     }
    else if(debounce(estado, Pin) == HIGH && estado == LOW){
        estado = HIGH; // Estado DESLIGADO (RTD Desligado)
        return false;
    }
    return false;
}

void AcionarMotor(){
    // Apenas habilita o pino do motor
    if (motor_habilitado) {
        digitalWrite(MOTOR_PIN, HIGH); // Assumindo LOW=OFF, HIGH=ON (Simples)
    } else {
        digitalWrite(MOTOR_PIN, LOW);
    }
}


void logica_rtd() {
    // 1. Verifica se o RTD está sendo solicitado (e se o botão foi apertado)
    if (MudarEstado(rtd_state, BUTTON_RTD_PIN) && rtd_state == LOW && !buzzerOn) {
        
        // Inicia o som do Buzzer (RTDS)
        acionar_buzzer(true);
        buzzerOn = true;
        buzzer_tempo_inicio = millis();
        indicar_aguardo(); // Azul durante o som
        Serial.println("RTD solicitado. Buzzer Ativo (3s).");
    }

    // 2. Verifica se o Buzzer terminou de tocar (3 segundos)
    if (buzzerOn) {
        if (millis() - buzzer_tempo_inicio >= BUZZER_DURATION) {
            
            // Fim do som: Habilita os motores
            acionar_buzzer(false);
            motor_habilitado = true;
            AcionarMotor();
            indicar_pronto_rtd(); // Verde
            Serial.println("RTD COMPLETO. Motores Habilitados.");

        } else {
            // Mantém o som durante a contagem
             // (Poderia ter lógica de piscar aqui)
        }
    }
}

// =================================================================
// 5. SETUP E LOOP PRINCIPAL
// =================================================================

void setup_mpu() {
    Wire.begin(); 
    
    if (!sensor_mpu.mpu.begin()) { // Usa o objeto mpu dentro da instância SensorMPU
        Serial.println("ERRO FATAL: MPU6050 não encontrado!");
        acionar_shutdown(); 
        while (1) delay(10);
    }
    sensor_mpu.initializeSensor(); // Inicializa e calibra a aceleração
}

void setup() {
    Serial.begin(115200);

    // Configuração de Pinos de SAÍDA
    pinMode(SHUTDOWN_PIN, OUTPUT);
    digitalWrite(SHUTDOWN_PIN, LOW); // LOW permite o motor

    pinMode(LED_R_PIN, OUTPUT);
    pinMode(LED_G_PIN, OUTPUT);
    pinMode(LED_B_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(MOTOR_PIN, OUTPUT);
    
    // Configuração dos Pinos de ENTRADA
    pinMode(VOLTAGE_PIN, INPUT);
    pinMode(BUTTON_RTD_PIN, INPUT_PULLUP); // PULLUP para o botão RTD (LOW = Pressionado)
    
    setup_mpu();
    indicar_aguardo(); // LED Azul
}

void loop() {
    
    // 1. VERIFICAÇÃO DE SEGURANÇA (VETO)
    // Se erro_ativo = true, o motor é cortado e o sistema trava.
    bool erro_ativo = verificar_logica_seguranca(); 

    // 2. AQUISIÇÃO DE DADOS (Atualiza Velocidade e Distância)
    data_processor.updateSpeed(); 

    if (erro_ativo) {
        indicar_erro(); 
        controle_motores(false); // Garante que o motor seja cortado
    } else {
        // 3. CONTROLE: Se a segurança está OK, roda a lógica RTD
        logica_rtd();
    }

    // 4. TELEMETRIA (Chamada contínua)
    enviar_dados_telemetria(ler_tensao_bateria(), sensor_mpu.getTemperature());
    
    delay(500); 
}