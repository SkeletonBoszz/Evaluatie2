// Definieer Blynk template ID en naam (vereist voor Blynk projectconfiguratie)
#define BLYNK_TEMPLATE_ID "user1"
#define BLYNK_TEMPLATE_NAME "user1@server.wyns.it"
#define BLYNK_PRINT Serial  // Stel Serial in voor Blynk debug output

// Inclusie van benodigde bibliotheken
#include "DHT.h"               // Bibliotheek voor DHT-sensor
#include <WiFi.h>              // Bibliotheek voor WiFi-verbinding
#include <WiFiClient.h>        // Bibliotheek voor WiFi-clientfunctionaliteit
#include <BlynkSimpleEsp32.h>  // Blynk-bibliotheek voor ESP32

// Definieer pinnen voor LEDs en DHT-sensor
#define red 2          // Pin voor rode LED
#define green 4        // Pin voor groene LED
#define yellow 16      // Pin voor gele LED
#define DHTPIN 5       // Pin waarop de DHT-sensor is aangesloten
#define DHTTYPE DHT11  // Type DHT-sensor (DHT11)

// Initialiseer DHT-sensor
DHT dht(DHTPIN, DHTTYPE);

// Blynk authenticatietoken (uniek voor jouw Blynk-project)
char auth[] = "erWN3cQq7De3e53zjp9kk2tNKMXN8sMd";

// WiFi-inloggegevens
char ssid[] = "Wifi-15";     // SSID van het WiFi-netwerk
char pass[] = "2800MECH15";  // Wachtwoord van het WiFi-netwerk

// Maak een BlynkTimer object aan voor periodieke taken
BlynkTimer timer;

// Variabelen voor temperatuur, luchtvochtigheid en temperatuurgrenzen
float temperature;  // Huidige temperatuur
float humidity;     // Huidige luchtvochtigheid
float temp_low;     // Onderste temperatuurgrens
float temp_high;    // Bovenste temperatuurgrens

// Functie om de bovenste temperatuurgrens aan te passen via Blynk slider (V3)
BLYNK_WRITE(V3) {
  int sliderValueHigh = param.asInt();  // Lees waarde van slider
  temp_high = sliderValueHigh;          // Update bovenste temperatuurgrens
  Serial.print("Maximum temperatuur aangepast: ");
  Serial.println(temp_high);  // Print nieuwe waarde naar Serial Monitor
}

// Functie om de onderste temperatuurgrens aan te passen via Blynk slider (V4)
BLYNK_WRITE(V4) {
  int slidervalueLow = param.asInt();  // Lees waarde van slider
  temp_low = slidervalueLow;           // Update onderste temperatuurgrens
  Serial.print("Minimum temperatuur aangepast: ");
  Serial.println(temp_low);  // Print nieuwe waarde naar Serial Monitor
}

// Functie om sensorwaarden te lezen en naar Blynk te sturen
void myTimer() {
  // Controleer of de sensorwaarden geldig zijn
  if (!isnan(temperature) && !isnan(humidity)) {
    // Stuur temperatuur naar virtuele pin V1 in Blynk
    Blynk.virtualWrite(V1, temperature);
    // Stuur luchtvochtigheid naar virtuele pin V2 in Blynk
    Blynk.virtualWrite(V2, humidity);

    // Print temperatuur en luchtvochtigheid naar Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  } else {
    Serial.println("Failed to read from DHT sensor!");  // Foutmelding bij ongeldige sensorwaarden
  }
}

// Setup-functie (wordt eenmaal uitgevoerd bij opstarten)
void setup() {
  // Stel LED-pinnen in als output
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);

  // Initialiseer Serial Monitor voor debugging
  Serial.begin(115200);

  // Wacht kort en print WiFi-verbindingsstatus
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Maak verbinding met WiFi
  WiFi.begin(ssid, pass);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");  // Bevestig WiFi-verbinding

  // Initialiseer DHT-sensor
  dht.begin();

  // Initialiseer Blynk met authenticatietoken, WiFi-gegevens en serverinstellingen
  Blynk.begin(auth, ssid, pass, "server.wyns.it", 8081);

  // Stel een timer in om elke 5 seconden myTimer() aan te roepen
  timer.setInterval(5000L, myTimer);
}

// Loop-functie (wordt continu uitgevoerd)
void loop() {
  Blynk.run();                          // Onderhoud Blynk-verbinding
  timer.run();                          // Voer timer-taken uit
  temperature = dht.readTemperature();  // Lees temperatuur van DHT-sensor
  humidity = dht.readHumidity();        // Lees luchtvochtigheid van DHT-sensor
  checkdata();                          // Controleer temperatuur en stuur LEDs aan
}

// Functie om LEDs aan te sturen op basis van temperatuur
void checkdata() {
  // Zet alle LEDs uit
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);

  // Controleer temperatuur en zet de juiste LED aan
  if (temperature > temp_high) {
    digitalWrite(red, HIGH);  // Rode LED aan als temperatuur te hoog is
  } else if (temperature < temp_low) {
    digitalWrite(yellow, HIGH);  // Gele LED aan als temperatuur te laag is
  } else {
    digitalWrite(green, HIGH);  // Groene LED aan als temperatuur binnen grenzen is
  }
}