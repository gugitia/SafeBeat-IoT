#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

const int TAMANHO = 50;
const int ledVermelhoPin = 4;
const int ledInfravermelhoPin = 2;
const int fotodetetorPin = 34;

const char* ssid = "ambiente simulado";
const char* password = "sem necessidade";

const char* mqttServer = "test.mosquitto.org";
const int mqttPort = 1883; 

WiFiClient espClient;
PubSubClient client(espClient);

void connectToWiFi();
void connectToMQTT();

void setup() {

  Serial.begin(9600);
   pinMode(ledVermelhoPin, OUTPUT);
   pinMode(ledInfravermelhoPin, OUTPUT);

   connectToWiFi();
   connectToMQTT();
}

void loop() {

  StaticJsonDocument<TAMANHO> json;
  int somaDiferencas = 0;
  int diferencaIntensidades;

 // Tempo de simulação de um batimento cardíaco
  for (int i = 0; i < 15; ++i) {

    digitalWrite(ledInfravermelhoPin, HIGH);
    digitalWrite(ledVermelhoPin, LOW);


    delay(50);

    // Medir a intensidade da luz refletida 
    int intensidadeLuz1 = analogRead(fotodetetorPin);


    digitalWrite(ledInfravermelhoPin, LOW);

 
    delay(50);


    digitalWrite(ledInfravermelhoPin, LOW);
    digitalWrite(ledVermelhoPin, HIGH);


    delay(50);

    // Medir a intensidade da luz refletida 
    int intensidadeLuz2 = analogRead(fotodetetorPin);

    digitalWrite(ledVermelhoPin, LOW);

    // Calcular a diferença entre as intensidades de luz 
    int diferencaIntensidades = abs(intensidadeLuz1 - intensidadeLuz2);
    somaDiferencas += diferencaIntensidades;

    Serial.println("Diferença de Intensidades: " + String(diferencaIntensidades));

    delay(100);
  }

  // Calcular a média da diferença de intensidades para representar o batimento cardíaco 
  int mediaDiferencas = somaDiferencas / 10;

  // Calcular a frequência cardíaca 
  int frequenciaCardiaca = mapearMediaParaFrequencia(mediaDiferencas);

  Serial.println("Frequência Cardíaca: " + String(frequenciaCardiaca));
  json["batimento"] = frequenciaCardiaca;

  String jsonString;
  serializeJson(json, jsonString);

  Serial.println(jsonString);

  client.publish("user/cardio", jsonString.c_str());


  delay(5000);

}

int mapearMediaParaFrequencia(int media) {

  return map(media, 0, 1023, 60, 120); // Mapeia a média para uma frequência cardíaca entre 60 e 120 BPM
}

void connectToWiFi() {
  Serial.print("Conectando ao WiFi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\nConectado ao WiFi");
}

void connectToMQTT() {
  Serial.print("Conectando ao servidor MQTT");
  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    if (client.connect("esp32-client")) {
      Serial.println("\nConectado ao servidor MQTT");
    } else {
      Serial.print(".");
      delay(2000);
    }
  }
}