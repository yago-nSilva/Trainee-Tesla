void inicializar(const int RTDPin, const int MotorPin, const int NoisePin);

void debounce(int state, const int RTDPin);

void MudarEstado(int state);

void AcionamentoMotor(int state, const int MotorPin,  const int NoisePin);
