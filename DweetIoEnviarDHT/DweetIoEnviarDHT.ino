/*
  Para verificar se está tudo indo bem, escreva a seguinte linha no seu terminal:
  curl -i https://dweet.io/listen/for/dweets/from/ocean 
*/

#include "keys.h"
#include <ESP8266WiFi.h> // Versão 2.6.0
#include "DHT.h" // v 1.4.0 DHT Sensor Library (Adafruit)

// WiFi
WiFiClient client;

// A Coisa
String nomeDaCoisa = "ocean";

// O Destino
const char* host = "dweet.io";
const int httpPort = 80;

// Umidade e temperatura aéreas (DHT22)
#define DHTPIN D3
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

// O Led
int led = BUILTIN_LED;

void setup(void)
{
  Serial.begin(9600);

  // WiFi
  conectarWiFi(); // Configura o Wifi
  
  // Umidade e temperatura aéreas (DHT22)
  dht.begin();

  // O Led
  pinMode(led, OUTPUT);

  delay(1000); // Aguarda um tempo antes de iniciar
}

void loop(void)
{
  // Verifica a conexão WiFi
  while (WiFi.status() != WL_CONNECTED) {
    conectarWiFi();
  }
  
  Serial.println("Olá, Internet!");

  // Umidade e temperatura aéreas (DHT22)
  int nDados = 2;
  String nomes[nDados];
  nomes[0] = "umidade";
  nomes[1] = "temperatura";
  float valores[nDados];
  valores[0] = dht.readHumidity();
  valores[1] = dht.readTemperature(); // Celsius
  //float dhtT = dht.reatTemperature(true); // Fahrenheit
  Serial.print("Umidade do ar:\t\t");
  Serial.print(valores[0], 1);
  Serial.println("%");
  Serial.print("Temperatura do ar:\t");
  Serial.print(valores[1], 1);
  Serial.println("C");

  // Envia os valores coletados para o Dweet.io
  dweetDado(nomes,valores,nDados);

  // Pisca O Led
  digitalWrite(led, LOW);
  delay(1000);
  digitalWrite(led, HIGH);
  delay(1000);
}

// Método para (re)conectar à rede WiFi
void conectarWiFi() {
  Serial.println("");
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
  Serial.println("");
}

// Conecta via TCP ao Dweet.io
//* Inicia comunicação, envia os dados recebidos e finaliza a conexão
void dweetDado(String nomes[], float valores[], int nDados) {
  Serial.println("\nInício da conexão com o Dweet.io");

  WiFiClient client;
  if (!client.connect(host, httpPort)) { 
    Serial.println("Falha de conexão com o Dweet.io!"); 
    return; 
  }
  Serial.println("Conectado!");

  // Prepara a mensagem e envia
  client.print( msgEnviarHTTPDweet(nomes, valores, nDados) ); 
  Serial.println("Enviado!");
  
  // Desconectar
  client.stop();
  
  Serial.println();  
  Serial.println("Fim da conexão com Dweet.io");
  delay(1000);
} 

// Prepara a mensagem de envio para o Dweet.io
// Segue o padrão: https://dweet.io/dweet/for/my-thing-name?hello=world
String msgEnviarHTTPDweet(String nomes[], float valores[], int nDados) {
  // Formatando a postagem
  String dweetHttpGet = "GET /dweet/for/";
  dweetHttpGet = dweetHttpGet + String( nomeDaCoisa ) + "?";

  // Adiciona todos os dados
  for (int i = 0; i < nDados; i++) {
    dweetHttpGet = dweetHttpGet + String( nomes[i] ) + "=" + String( valores[i] );
    if (i + 1 != nDados) {
      dweetHttpGet = dweetHttpGet + "&";
    }
  }
  
  dweetHttpGet = dweetHttpGet+" HTTP/1.1\r\n"+ 
                 "Host: " +  
                 host +  
                 "\r\n" +  
                 "Connection: close\r\n\r\n";
                 
  Serial.println("\nTexto a enviar:");
  Serial.print(dweetHttpGet);
  return dweetHttpGet;
}
