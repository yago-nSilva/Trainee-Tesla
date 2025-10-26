const int RTDPin = 5, ShutdownPin = 0, MotorPin = 22, NoisePin = 33;
int state = HIGH, shutState = HIGH;
unsigned long tempoInicial = 0;
bool buzzerOn = false;

void inicializar(){
    pinMode(RTDPin, INPUT_PULLUP);
    pinMode(ShutdownPin, INPUT_PULLUP);
    pinMode(NoisePin, OUTPUT);
    pinMode(MotorPin, OUTPUT);

    digitalWrite(NoisePin, LOW);
    digitalWrite(MotorPin, LOW);
}

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
    if(debounce(estado, Pin) == LOW && state == HIGH){
        estado = LOW;
        return true;
    }
    else if(debounce(estado, Pin) == HIGH && state == LOW){
        estado = HIGH;
        return false;
    }
}

bool AcionarShutdown(){
    if(MudarEstado(shutState, ShutdownPin) || verificar_logica_seguranca()){
        return true;
    }
    else{
        return false;
    }
}

void AcionarMotor(){
    if(AcionarShutdown()){
        digitalWrite(MotorPin, LOW);
        digitalWrite(NoisePin, LOW);
        buzzerOn = false;
    }
    else{
        if(MudarEstado(state, RTDPin)){
            digitalWrite(NoisePin, HIGH);
            buzzerOn = true;
        }
        if(buzzerOn && millis() - tempoInicial >= 3000){
            digitalWrite(NoisePin, LOW);
            digitalWrite(MotorPin, HIGH);
        }
    }
}
