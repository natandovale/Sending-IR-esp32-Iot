#include <IRremoteESP8266.h> 
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include<DHT.h>
DHT dht(4, DHT11);
 
const int IR_PIN = 2; 
const int REC_IR = 13;
const int BUTTON = 12;
bool buttonState = HIGH;
bool lastButtonState = HIGH;
bool ledState = false;
unsigned long IR_1 = 0;
#define LED_RED 27
#define LED_GREEN 14
  
IRrecv irrecv(REC_IR);
IRsend irsend(IR_PIN); 
decode_results results;

void setup() {
  dht.begin();
  Serial.begin(115200);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  irrecv.enableIRIn();
  irsend.begin();
  delay(2000);
}

void loop() {
  float temp = dht.readTemperature();
  float humity = dht.readHumidity();

  buttonState = digitalRead(BUTTON);

  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      ledState = !ledState;  
      digitalWrite(LED_GREEN, ledState ? HIGH : LOW);  
      digitalWrite(LED_RED, LOW);
      Serial.println("Botão pressionado!"); 
      delay(50);  
    }
    lastButtonState = buttonState;
  } 
   
  if(temp < 22.00){
    unsigned long irCode = 0xBD7689; 
    irsend.sendNEC(IR_1, 32);  
    Serial.println("Sinal enviado: ");
  }
    
  if (irrecv.decode(&results)) {
    IR_1 = results.value;
    Serial.println(IR_1, HEX);
    digitalWrite(LED_GREEN, LOW);  
    digitalWrite(LED_RED, HIGH);
    ledState = false;  
    irrecv.resume();
  }
}
