// =============================================
//   BRAS ROBOTIQUE - ESP32
//   C++ pur, sans bibliothèque
//   Contrôle servo par PWM direct (ledc ESP32)
//
//   Servo 1 (Base)     → GPIO 13
//   Servo 2 (Épaule)   → GPIO 14
//   Servo 3 (Coude)    → GPIO 15
//
//   Servo standard :
//   - Fréquence PWM : 50 Hz (période 20ms)
//   - 0°   = impulsion ~500µs
//   - 90°  = impulsion ~1500µs
//   - 180° = impulsion ~2500µs
// =============================================

// ----- Pins -----
#define PIN_BASE    13
#define PIN_EPAULE  14
#define PIN_COUDE   15

// ----- Canaux PWM ESP32 (0 à 15 disponibles) -----
#define CANAL_BASE    0
#define CANAL_EPAULE  1
#define CANAL_COUDE   2

// ----- Paramètres PWM -----
#define PWM_FREQ       50     // 50 Hz pour servos
#define PWM_RESOLUTION 16     // 16 bits → valeurs de 0 à 65535

// Conversion µs → valeur PWM 16 bits à 50Hz
// Période = 20 000 µs → 65535 unités
// 1 µs = 65535 / 20000 = 3.27675
#define US_VERS_PWM(us) ((uint32_t)((us) * 65535UL / 20000UL))

// Limites d'impulsion pour servos standards
#define PULSE_MIN_US  500    // 0°
#define PULSE_MAX_US  2500   // 180°

// ----- Vitesse de mouvement -----
#define VITESSE_MS 15  // ms entre chaque degré (plus grand = plus lent)

// ----- Angles actuels -----
int angleBase   = 90;
int angleEpaule = 90;
int angleCoude  = 90;

// =============================================
//   Écrire un angle sur un canal PWM
// =============================================
void servoEcrire(int canal, int angle) {
  // Clamp entre 0 et 180
  if (angle < 0)   angle = 0;
  if (angle > 180) angle = 180;

  // Interpolation linéaire entre PULSE_MIN et PULSE_MAX
  uint32_t pulseUs = PULSE_MIN_US + (uint32_t)(angle) * (PULSE_MAX_US - PULSE_MIN_US) / 180;
  uint32_t valPWM  = US_VERS_PWM(pulseUs);

  ledcWrite(canal, valPWM);
}

// =============================================
//   Initialiser un canal PWM pour un servo
// =============================================
void servoInit(int pin, int canal, int angleDepart) {
  ledcSetup(canal, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(pin, canal);
  servoEcrire(canal, angleDepart);
}

// =============================================
//   Afficher l'état temps réel des 3 servos
// =============================================
void afficherEtat() {
  Serial.println("-------------------------------");
  Serial.print("Base    : "); Serial.print(angleBase);   Serial.println(" deg");
  Serial.print("Epaule  : "); Serial.print(angleEpaule); Serial.println(" deg");
  Serial.print("Coude   : "); Serial.print(angleCoude);  Serial.println(" deg");
  Serial.println("-------------------------------");
}

// =============================================
//   Déplacer un servo en douceur vers un angle
//   Affichage temps réel degré par degré
// =============================================
void bouger(int canal, int &angleActuel, int angleCible, const char* nom) {
  Serial.print(">>> "); Serial.print(nom);
  Serial.print(" : "); Serial.print(angleActuel);
  Serial.print(" -> "); Serial.print(angleCible);
  Serial.println(" deg");

  int direction = (angleCible > angleActuel) ? 1 : -1;

  while (angleActuel != angleCible) {
    angleActuel += direction;
    servoEcrire(canal, angleActuel);
    delay(VITESSE_MS);

    // Retour temps réel
    Serial.print(nom); Serial.print(" : "); Serial.println(angleActuel);
  }

  Serial.print(">>> "); Serial.print(nom); Serial.println(" : position atteinte.");
  Serial.println();
}

// =============================================
//   Séquence de démonstration
// =============================================
void sequenceDemonstration() {
  Serial.println("=============================");
  Serial.println("  SEQUENCE DEMONSTRATION     ");
  Serial.println("=============================");
  delay(500);

  Serial.println("[1] Lever l epaule");
  bouger(CANAL_EPAULE, angleEpaule, 45, "Epaule");
  afficherEtat();
  delay(300);

  Serial.println("[2] Plier le coude");
  bouger(CANAL_COUDE, angleCoude, 135, "Coude");
  afficherEtat();
  delay(300);

  Serial.println("[3] Rotation base droite");
  bouger(CANAL_BASE, angleBase, 150, "Base");
  afficherEtat();
  delay(300);

  Serial.println("[4] Rotation base gauche");
  bouger(CANAL_BASE, angleBase, 30, "Base");
  afficherEtat();
  delay(300);

  Serial.println("[5] Retour position initiale");
  bouger(CANAL_BASE,   angleBase,   90, "Base");
  bouger(CANAL_EPAULE, angleEpaule, 90, "Epaule");
  bouger(CANAL_COUDE,  angleCoude,  90, "Coude");
  afficherEtat();

  Serial.println("=============================");
  Serial.println("  SEQUENCE TERMINEE          ");
  Serial.println("=============================");
}

// =============================================
//   SETUP
// =============================================
void setup() {
  Serial.begin(115200);
  delay(500);

  servoInit(PIN_BASE,   CANAL_BASE,   angleBase);
  servoInit(PIN_EPAULE, CANAL_EPAULE, angleEpaule);
  servoInit(PIN_COUDE,  CANAL_COUDE,  angleCoude);

  Serial.println("=============================");
  Serial.println("  BRAS ROBOTIQUE - ESP32     ");
  Serial.println("  Sans bibliotheque          ");
  Serial.println("  Initialisation OK          ");
  Serial.println("=============================");
  afficherEtat();
  delay(1000);

  sequenceDemonstration();
}

// =============================================
//   LOOP : commandes via Serial Monitor
//   1 → relancer la séquence
//   2 → position repos (90° partout)
// =============================================
void loop() {
  if (Serial.available() > 0) {
    char commande = Serial.read();

    if (commande == '1') {
      Serial.println(">>> COMMANDE : SEQUENCE");
      sequenceDemonstration();
    }
    else if (commande == '2') {
      Serial.println(">>> COMMANDE : REPOS");
      bouger(CANAL_BASE,   angleBase,   90, "Base");
      bouger(CANAL_EPAULE, angleEpaule, 90, "Epaule");
      bouger(CANAL_COUDE,  angleCoude,  90, "Coude");
      afficherEtat();
    }
    else {
      Serial.println("Commandes disponibles :");
      Serial.println("  1 -> Lancer la sequence");
      Serial.println("  2 -> Position repos");
    }
  }
}
