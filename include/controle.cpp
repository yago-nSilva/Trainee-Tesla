#include <Arduino.h>

void inicializar(const int RTDPin, const int MotorPin, const int NoisePin){
    pinMode(RTDPin, INPUT_PULLUP);
    pinMode(NoisePin, OUTPUT);
    pinMode(MotorPin, OUTPUT);

    noTone(NoisePin);
    digitalWrite(MotorPin, LOW);
    int state = HIGH;
}

int debounce(int state){
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

void AcionamentoMotor(const int MotorPin,  const int NoisePin){
    if(state == LOW){
        tone(NoisePin, 440, 3000);
        delay(3000);
        noTone(NoisePin);
        digitalWrite(MotorPin, HIGH);
    }
}
