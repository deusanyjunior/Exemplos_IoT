#include "keys.h"
#include <ESP8266WiFi.h>

String nomeDaCoisa = "HardwareLivreUSP";
const char* host = "dweet.io";
const int httpPort = 80;

int led = 2;

String nomeDoCampo = "dado";
int valorDoCampo = 0;

void setup() {
  Serial.begin(9600);

  // WiFi
  if (mudarMAC)
    wifi_set_macaddr(STATION_IF, MAC); // Define novo endereço de MAC
  conectarWiFi(); // Configura o Wifi
  
  pinMode(led, OUTPUT);
}

void loop() {
  // Define o valor do campo
  valorDoCampo = valorDoCampo + 1;
  // Envia o valor do campo
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
  if (!client.connect(host, httpPort)) { 
    Serial.println("Falha de conexão com o Dweet.io!"); 
    return; 
  }

  Serial.println("Conectado!");
  
  client.print(msgEnviarHTTPDweet()); 
  delay(10);
  
  Serial.println("Enviado!");
  
  // Desconectar
  client.stop();
  
  Serial.println();  
  Serial.println("Fim da conexão."); 
} 

// Prepara a mensagem de envio para o Dweet.io
// Segue o padrão: https://dweet.io/dweet/for/my-thing-name?hello=world&foo=bar
String msgEnviarHTTPDweet() {
  String dweetHttpGet = "GET /dweet/for/";
  dweetHttpGet = dweetHttpGet + String( nomeDaCoisa ) + "?";
  dweetHttpGet = dweetHttpGet + String( nomeDoCampo ) + "=" + String( valorDoCampo );
  dweetHttpGet = dweetHttpGet+" HTTP/1.1\r\n"+ 
                 "Host: " +  
                 host +  
                 "\r\n" +  
                 "Connection: close\r\n\r\n";
  Serial.println("\nTexto a enviar:");
  Serial.print(dweetHttpGet);
  return dweetHttpGet;
}
