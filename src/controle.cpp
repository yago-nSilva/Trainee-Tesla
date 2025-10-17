#include <Arduino.h>

void inicializar(const int RTDPin, const int MotorPin, const int NoisePin){
    pinMode(RTDPin, INPUT_PULLUP);
    pinMode(NoisePin, OUTPUT);
    pinMode(MotorPin, OUTPUT);

    digitalWrite(NoisePin, LOW);
    digitalWrite(MotorPin, LOW);
    int state = HIGH;
}

int debounce(int state, const int RTDPin){
    int stateNamoral = digitalRead(RTDPin);

    if(state != stateNamoral){
        delay(75);
        stateNamoral = digitalRead(RTDPin);
    }
    return stateNamoral;
}

int MudarEstado(int state){
    if(debounce(state) == LOW && state == HIGH){
        state = LOW;
    }
    else if(debounce(state) == HIGH && state == LOW){
        state = HIGH;
    }
}

void AcionamentoMotor(int state, const int MotorPin,  const int NoisePin){
    if(state == LOW){
        digitalWrite(NoisePin, HIGH);
        delay(3000);
        digitalWrite(NoisePin, LOW);
        delay(300);
        digitalWrite(MotorPin, HIGH);
    }
}
