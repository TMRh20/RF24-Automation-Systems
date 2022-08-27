/*
 *************************************************************************
   RF24Ethernet Arduino library by TMRh20 - 2014-2022

   Automated (mesh) wireless networking and TCP/IP communication stack for RF24 radio modules

   RF24 -> RF24Network -> UIP(TCP/IP) -> RF24Ethernet
                       -> RF24Mesh

        Documentation: http://nRF24.github.io/RF24Ethernet/

 *************************************************************************

 **** EXAMPLE REQUIRES: Arduino MQTT library: https://github.com/256dpi/arduino-mqtt/ ***
   Shown in Arduino Library Manager as 'MQTT' by Joel Gaehwiler

 *************************************************************************

*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <RF24Ethernet.h>
#include <MQTT.h>
#include <dht.h>

dht DHT;
#define DHT22_PIN     6

RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);

IPAddress ip(10, 10, 2, 4);
IPAddress gateway(10, 10, 2, 2);  //Specify the gateway in case different from the server
IPAddress server(10, 10, 2, 2);   //The ip of the MQTT server
char clientID[] = { "arduinoClient   " };

EthernetClient ethClient;
MQTTClient client;

void connect() {
  Serial.print("connecting...");
  uint32_t clTimeout = millis();
  while (!client.connect(clientID)) {
    Serial.print(".");
    if (millis() - clTimeout > 5001) {
      Serial.println();
      return;
    }
    uint32_t timer = millis();
    //Instead of delay, keep the RF24 stack updating
    while (millis() - timer < 1000) {
      Ethernet.update();
    }
  }

  Serial.println("\nconnected!");
  client.publish("outTopic", "hello world");

}

void setup() {
  Serial.begin(115200);

  Ethernet.begin(ip, gateway);

  if (mesh.begin()) {
    Serial.println(" OK");
  } else {
    Serial.println(" Failed");
  }

  //Convert the last octet of the IP address to an identifier used
  char str[4];
  int test = ip[3];
  itoa(ip[3], str, 10);
  memcpy(&clientID[13], &str, strlen(str));
  Serial.println(clientID);

  client.begin(server, ethClient);
}

uint32_t mesh_timer = 0;
uint32_t pub_timer = 0;

void loop() {
  Ethernet.update();

  if (millis() - mesh_timer > 10000) {  //Every 30 seconds, test mesh connectivity
    mesh_timer = millis();
    if (!mesh.checkConnection()) {
      if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS) {
        mesh.begin();
      }
    }
    Serial.println();
  }
  if (!client.connected()) {
    connect();
  }

  client.loop();

  // Every 5 minutes, report to the MQTT server the temp and humidity
  if (client.connected() && millis() - pub_timer > 300000) {
    pub_timer = millis();
    char tempBuff[10];
    float temperature = 0;
    int chk = DHT.read22(DHT22_PIN);

    dtostrf(DHT.temperature, 5, 1, tempBuff);
    Serial.println(tempBuff);
    client.publish("temp4", tempBuff);
    memset(tempBuff,0,10);
    dtostrf(DHT.humidity, 5, 1, tempBuff);
    client.publish("hum4", tempBuff);

    
  }
}
