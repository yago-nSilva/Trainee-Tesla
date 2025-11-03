#include <Arduino.h>
#include <WiFi.h> 

const char* ssid = "Telemetria";
const char* password = "12345678";
const int TELEMETRY_PORT = 80; // Porta do servidor (80 é padrão web)

WiFiServer telemetryServer(TELEMETRY_PORT);
WiFiClient client;

float fake_speed = 0.0;
float fake_distance = 0.0;

void setupWiFi_AP() {
    Serial.print("Criando Access Point: ");
    Serial.println(ssid);

    // Inicia o WiFi no modo Access Point
    if (!WiFi.softAP(ssid, password)) {
        Serial.println("Falha ao iniciar o AP!");
        while(1); 
    }

    Serial.println("Rede WiFi criada.");
    Serial.print("Endereço IP do ESP32 (Servidor): ");
    Serial.println(WiFi.softAPIP()); // O IP do ESP32 (geralmente 192.168.4.1)
}

void setupTelemetryServer() {
    telemetryServer.begin(); // Inicia o servidor
    Serial.print("Servidor de Telemetria iniciado na porta: ");
    Serial.println(TELEMETRY_PORT);
}

void clientConnection() {
    if (!client.connected()) {
        client.stop(); 

        client = telemetryServer.available(); 
        
        if (client) { // Se um novo cliente se conectou
            Serial.println("Novo cliente conectado!");
            client.println("Conectado Notebook. Enviando telemetria simulada...");
        }
    }
}

void sendTelemetryData(float speed, float distance) {
    // Só envia se houver um cliente conectado
    if (client.connected()) {
        
        String dataString = "Velocidade Instantânea (Simulada): " + String(speed, 2) + " m/s | Distancia Total percorrida: " + String(distance, 2) + " m";
        
        // Envia os dados pela rede
        client.println(dataString);
    }
}

void updateSimulation() {
    fake_speed += 0.05; 
    
    fake_distance += fake_speed * 0.1; 

    // Reseta a simulação se a velocidade ficar muito alta
    if (fake_speed > 10) {
        fake_speed = 0.0;
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial); // Espera a serial estar pronta
    Serial.println("\nInicializando Servidor de Telemetria (Simulação)...");

    // Inicializção da Rede
    setupWiFi_AP();

    // 2. Inicializa o Servidor
    setupTelemetryServer();

    Serial.println("Sistema pronto. Aguardando cliente...");
}

void loop() {
    clientConnection();

    updateSimulation();

    sendTelemetryData(fake_speed, fake_distance);

    // Print no serial para teste dos dados falsos
    Serial.print("Enviando dados simulados: V=");
    Serial.print(fake_speed);
    Serial.print(" m/s, D=");
    Serial.print(fake_distance);
    Serial.println(" m");

    delay(100); 
}