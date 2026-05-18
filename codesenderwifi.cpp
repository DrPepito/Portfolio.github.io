#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// ── OLED ──────────────────────────────────────────
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDR  0x3C
#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ── WIFI / UDP ─────────────────────────────────────
const char* ssid     = "ESP32_RESEAU";   // nom du réseau local créé
const char* password = "12345678";        // mot de passe

// IP fixe de l'ESP32 #2 (récepteur)
const char* ip_recepteur = "192.168.4.2";
const int   port_udp     = 1234;

WiFiUDP udp;

// ── CALCUL ────────────────────────────────────────
int monCalcul(int x, int y) {
  return x + y;   // remplacez par votre vrai calcul
}

void afficherOLED(String ligne1, String ligne2 = "", String ligne3 = "") {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(ligne1);

  if (ligne2 != "") {
    display.setCursor(0, 20);
    display.setTextSize(2);          // plus grand pour le résultat
    display.println(ligne2);
  }

  if (ligne3 != "") {
    display.setTextSize(1);
    display.setCursor(0, 50);
    display.println(ligne3);
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  // Démarrage OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDR)) {
    Serial.println("OLED non trouvé !");
    while (true);
  }

  afficherOLED("Demarrage...", "", "ESP32 #1");
  delay(1000);

  // Créer un point d'accès WiFi (réseau local entre les 2 ESP32)
  WiFi.softAP(ssid, password);
  Serial.println("Point d'accès WiFi créé");
  Serial.print("IP de cet ESP32 : ");
  Serial.println(WiFi.softAPIP());   // sera 192.168.4.1

  afficherOLED("WiFi OK", "192.168.4.1", "Emetteur pret");
  delay(1000);

  udp.begin(port_udp);
}

void loop() {
  // ── 1) Faire le calcul ──────────────────────────
  int a = 42;
  int b = 58;
  int resultat = monCalcul(a, b);

  String message = String(a) + "+" + String(b) + "=" + String(resultat);

  // ── 2) Afficher sur OLED ────────────────────────
  afficherOLED("Calcul :", String(resultat), "Envoi en cours...");

  // ── 3) Envoyer le résultat via UDP ──────────────
  udp.beginPacket(ip_recepteur, port_udp);
  udp.print(message);
  udp.endPacket();

  Serial.println("Envoyé : " + message);

  delay(3000);   // envoie toutes les 3 secondes
}