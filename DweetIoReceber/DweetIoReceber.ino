#include "keys.h"
#include <ESP8266WiFi.h> // Versão 2.6.0
#include <ArduinoJson.h> // Versão 6.13.0

String nomeDaCoisa = "Ocean";
const char* host = "dweet.io";
const int httpPort = 80;

int led = 2;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);

  // WiFi
  if (mudarMAC) {
    wifi_set_macaddr(STATION_IF, MAC); // Define novo endereço de MAC
  }
}

void loop() {
  // WiFi
  if (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }
  // Recebe o último valor enviado
  dweetDado();

  digitalWrite(led, LOW);
  delay(1000);
  digitalWrite(led, HIGH);
  delay(1000);
}

void conectarWiFi() {
  Serial.print("Conectando-se à rede: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());
}

// Conecta via TCP ao Dweet.io, inicia comunicação e finaliza a conexão
void dweetDado() {
  Serial.println("\nInício da conexão.");

  WiFiClient client; 
  client.setTimeout(10000);
  if (!client.connect(host, httpPort)) { 
    Serial.println("Falha de conexão com o Dweet.io!"); 
    return; 
  }

  Serial.println("Conectado!");
  
  // Prepara a mensagem e envia
  client.print(msgReceberHTTPDweet());

  // Decodifica a mensagem recebida
  decodificaRespostaDweet(client);
  
  // Desconectar
  client.stop();
  
  Serial.println();  
  Serial.println("Fim da conexão.");
  delay(1000); 
} 

// Prepara a mensagem de envio para o Dweet.io
// Segue o padrão: https://dweet.io/get/latest/dweet/for/my-thing-name
String msgReceberHTTPDweet() {
  String dweetHttpGet = "GET /get/latest/dweet/for/";
  dweetHttpGet = dweetHttpGet + String( nomeDaCoisa );
  dweetHttpGet = dweetHttpGet+" HTTP/1.1\r\n"+ 
                 "Host: " +  
                 host +  
                 "\r\n" +  
                 "Connection: close\r\n\r\n";
  Serial.println("\nTexto a enviar:");
  Serial.print(dweetHttpGet);
  return dweetHttpGet;
}

// Recebe o retorno em formato JSON e decodifica o dado lido
void decodificaRespostaDweet(WiFiClient client) {
  // Verifica resposta HTTP
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print("Resposta inesperada: ");
    Serial.println(status);
    return;
  }

  // Pula os cabecalhos HTTP
  char fimDosCabecalhos[] = "\r\n\r\n";
  if (!client.find(fimDosCabecalhos)) {
    Serial.println("Resposta inválida!");
    return;
  }

  // Salva o objeto JSON dinamicamente e imprime o dado na saída Serial
  // Use arduinojson.org/v6/assistant para definir esta capacidade
  const size_t capacity = JSON_ARRAY_SIZE(1) 
                        + JSON_OBJECT_SIZE(1) 
                        + JSON_OBJECT_SIZE(3) 
                        + JSON_OBJECT_SIZE(4) 
                        + 130;
  DynamicJsonDocument doc(capacity);
  DeserializationError erro = deserializeJson(doc, client);
  if (erro) {
    Serial.print("Falha em deserializeJson(): ");
    Serial.println(erro.c_str());
    return;
  }  
  JsonObject conteudo = doc["with"][0];
  const char* coisa = conteudo["thing"]; // "Ocean"
  const char* dataDeCriacao = conteudo["created"]; // "2019-11-04T13:22:33.516Z"  
  int dado = conteudo["content"]["dado"]; // valor do dado lido
  Serial.print("Dado lido: ");
  Serial.println(dado);
}
