#include <WiFi.h>
#include <PubSubClient.h>

// Configurações de WiFi
const char* ssid = "";// adicione o nome da sua rede WiFi
const char* password = ""; // adicione a senha da sua rede WiFi

// Configurações de MQTT
const char* mqtt_server = "mqtt-dashboard.com"; // Broker público para teste
const char* topic_sensor = "bananup/umidade";
const char* topic_atuador = "bananup/bomba";

WiFiClient espClient;
PubSubClient client(espClient);

// Pinos
const int pinoPotenciometro = 34; // Simula sensor de umidade
const int pinoBomba = 2;          // Simula a bomba de água (LED)

void setup() {
  Serial.begin(115200);
  pinMode(pinoBomba, OUTPUT);
  pinMode(pinoPotenciometro, INPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando em ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    // ID do cliente aleatório
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leitura do Sensor (Simulação)
  int valorAnalogico = analogRead(pinoPotenciometro);
  // Mapeia 0-4095 para 0-100%
  int umidade = map(valorAnalogico, 0, 4095, 0, 100); 

  // Lógica de Controle (Borda)
  String statusBomba = "DESLIGADA";
  if (umidade < 30) { // Se umidade menor que 30%, liga a bomba
    digitalWrite(pinoBomba, HIGH);
    statusBomba = "LIGADA";
  } else {
    digitalWrite(pinoBomba, LOW);
  }

  // Publica no MQTT
  char msg[50];
  sprintf(msg, "%d", umidade);
  client.publish(topic_sensor, msg);
  
  // Debug Serial
  Serial.print("Umidade: ");
  Serial.print(umidade);
  Serial.print("% | Bomba: ");
  Serial.println(statusBomba);

  delay(2000); // Envia a cada 2 segundos
}