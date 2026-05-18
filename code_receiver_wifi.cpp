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
const char* ssid     = "ESP32_RESEAU";   // même réseau que l'émetteur
const char* password = "12345678";

const int port_udp = 1234;

WiFiUDP udp;
char buffer[64];   // buffer pour recevoir le message

void afficherOLED(String ligne1, String ligne2 = "", String ligne3 = "") {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(ligne1);

  if (ligne2 != "") {
    display.setCursor(0, 20);
    display.setTextSize(2);
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

  afficherOLED("Connexion...", "", "ESP32 #2");

  // Se connecter au réseau créé par l'ESP32 #1
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnecté !");
  Serial.print("IP de cet ESP32 : ");
  Serial.println(WiFi.localIP());   // sera 192.168.4.2

  afficherOLED("WiFi OK", WiFi.localIP().toString(), "En attente...");

  udp.begin(port_udp);
}

void loop() {
  // Vérifier si un paquet UDP est arrivé
  int taille = udp.parsePacket();

  if (taille > 0) {
    // Lire le message reçu
    int lu = udp.read(buffer, sizeof(buffer) - 1);
    buffer[lu] = '\0';   // terminer la chaîne

    String message = String(buffer);
    Serial.println("Reçu : " + message);

    // Afficher sur OLED
    afficherOLED("Recu de #1 :", message, "OK");
  }
}