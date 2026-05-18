// =============================================
// 28BYJ-48 - Plateforme tournante
// Rotation continue sens horaire pendant 60s
// IN1 (D8)  = Coil 1 (Orange)
// IN2 (D9)  = Coil 3 (Jaune)  inversé
// IN3 (D10) = Coil 2 (Rose)   inversé
// IN4 (D11) = Coil 4 (Bleu)
// =============================================

#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// 3500µs = vitesse douce pour plateforme (~3 RPM)
// Diminue pour aller plus vite (min 1200µs)
// Augmente pour aller plus lentement
#define STEP_DELAY 3500

#define STEPS_PER_REV 2048
#define DUREE_MS 60000UL  // 60 secondes

const bool sequence[8][4] = {
  {1, 0, 0, 0},
  {1, 0, 1, 0},
  {0, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 0},
  {0, 1, 0, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1},
};

void setStep(int step) {
  digitalWrite(IN1, sequence[step][0]);
  digitalWrite(IN2, sequence[step][1]);
  digitalWrite(IN3, sequence[step][2]);
  digitalWrite(IN4, sequence[step][3]);
}

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopMotor();

  Serial.begin(9600);
  Serial.println("Plateforme : rotation 60s...");

  int currentStep = 0;
  unsigned long debut = millis();

  // Tourne en continu pendant 60 secondes
  while (millis() - debut < DUREE_MS) {
    currentStep = (currentStep + 1) % 8;  // +1 = horaire / -1+8 = anti-horaire
    setStep(currentStep);
    delayMicroseconds(STEP_DELAY);
  }

  stopMotor();
  Serial.println("Termine.");
}

void loop() {
  // Rien : tout se passe dans setup()
  // Le moteur s'arrête définitivement après 60s
}
