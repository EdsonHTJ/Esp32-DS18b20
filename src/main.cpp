#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define WIFISSID "SSID"
#define PASSWORD "PASS"
#define TOKEN "TOKEN" 
#define MQTT_CLIENT_NAME "RandomName" 


#define VARIABLE_LABEL "Temperature" 
#define DEVICE_LABEL "esp32" 

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


char mqttBroker[]  = "things.ubidots.com";
char payload[100];
char topic[150];
char topicSubscribe[100];

char str_Temp[10];

float prevtmp;


WiFiClient ubidots;
PubSubClient client(ubidots);



void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
      client.subscribe(topicSubscribe);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  WiFi.begin(WIFISSID, PASSWORD);

  Serial.println();
  Serial.print("Waiting Connections...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);

}

void loop() {
  if (!client.connected()) {
    client.subscribe(topicSubscribe);   
    reconnect();
  }

  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload, "%s", ""); 
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL); 
  sensors.requestTemperatures(); 
  float Temp = sensors.getTempCByIndex(0);
  if(Temp+100>=0){
    prevtmp = Temp;
  }
  Serial.print("Valor de temperatura:- ");Serial.println(prevtmp);
    
  dtostrf(prevtmp, 4, 2, str_Temp);
    
  sprintf(payload, "%s {\"value\": %s}}", payload, str_Temp); 
  client.publish(topic, payload);
  client.loop();
  
  delay(1000);
}