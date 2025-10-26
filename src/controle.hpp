void inicializar(const int RTDPin, const int ShutdownPin, const int MotorPin, const int NoisePin);

void debounce(int estado, const int Pin);

void MudarEstado(int &estado, const int Pin);

void AcionarShutdown();

void AcionarMotor();
