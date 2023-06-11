
#include "globalConfig.h"

void setup() {
  Serial.begin(115200);
  irsend.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  ThingSpeak.begin(client);
  
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  
  Serial.println("Connected!!! ");
  
  dht.setup(pinDHT, DHTesp::DHT11);

  Serial.println(WiFi.localIP());
  
  delay(2000);
}


void loop() {
  http.begin("https://notifications-api-iot.com/send-notification");  

  http.addHeader("Content-Type", "application/json");  
  http.addHeader("x-api-key", "api-token");  
    
  String requestBody = "";

  TempAndHumidity data = dht.getTempAndHumidity();
  float temperature = data.temperature;
  float humity = data.humidity;
  Serial.println(temperature);
  
  Serial.println("Temperatura: " + String(data.temperature, 2) + "°C");
  Serial.println("Umidade: " + String(data.humidity, 1) + "%");
  
  Serial.println("---");

  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humity);
  
  ThingSpeak.writeFields(channelNumber, writeAPIKey);

  if(temperature > 22.00 && arCondicionadoDesligado){
            Serial.println("Enviando sinal de ligar");
            irsend.sendRaw(rawDataLig, 227, 38);  // Send a raw data capture at 38kHz.
            Serial.println("Sinal enviado: ");
            arCondicionadoDesligado = false;
            String message = "Foi enviado o sinal de ligar para o ar-conidionado. A temperatura atual eh: "+String(temperature)+" C°";
            requestBody = "{\"message\":\"" + message + "\"}";
            int httpResponseCode = http.POST(requestBody);
           String responseBody;
           Serial.println("Enviando requisicao");
           if (httpResponseCode == 201) {
                 responseBody = http.getString();
                Serial.println(responseBody);
           } else {
                Serial.println(httpResponseCode);
                Serial.println("Falha na requisição");
           }
            
            delay(150000);
  }
  
  if(temperature < 22.00 && !arCondicionadoDesligado){
           Serial.println("Enviando sinal de desligar");
           irsend.sendRaw(rawDataDesl, 227, 38);  // Send a raw data capture at 38kHz.
           Serial.println("Sinal enviado: ");
           arCondicionadoDesligado = true;
           String message = "Foi enviado o sinal de desligar para o ar-conidionado. A temperatura atual eh: "+String(temperature)+" C°";
           requestBody = "{\"message\":\"" + message + "\"}";

           Serial.println("Enviando requisicao");
           int httpResponseCode = http.POST(requestBody);

           if (httpResponseCode == 201) {
                String responseBody = http.getString();
                Serial.println(responseBody);
           } else {
                Serial.println(httpResponseCode);
                Serial.println("Falha na requisição");
           }
           delay(150000);
  }
  delay(10000);
}
