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
  LED Control via MQTT

*/

#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>
#include <RF24Mesh.h>
#include <RF24Ethernet.h>
#include <MQTT.h>
#include <Adafruit_NeoPixel.h>

RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);

#define LEDPIN 5
#define NUMPIXELS 1
#define DELAYVAL 500
Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

IPAddress ip(10, 10, 2, 25);
IPAddress gateway(10, 10, 2, 2);  //Specify the gateway in case different from the server
IPAddress server(10, 10, 2, 2);   //The ip of the MQTT server

int r=0,g=0,b=0;

char clientID[] = { "arduinoClient   " };
uint32_t connectedTimer = 0;
char lastTopic = 'a';

void changePixels(){

    pixels.clear();
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(r,g,b));
    }
    pixels.show();
}

void messageReceived(MQTTClient* client, char topic[], char payload[], int length) {
  Serial.println("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.println(payload);

  char rgbBuffer[5];

  int i = 0;
  Serial.println(payload);
  for(i;i<length;i++){
    if(isdigit(payload[i]) ){
      rgbBuffer[i] = payload[i];
    }else{
      i++;
      break;
    }
  }
  r = atoi(rgbBuffer);
  memset(rgbBuffer,0,5);
  int j = 0;
  for(i; i<length;i++){
    if(isdigit(payload[i]) ){
      rgbBuffer[j] = payload[i];
    }else{
      i++;
      break;
    }
    j++;
  }
  g = atoi(rgbBuffer);
  memset(rgbBuffer,0,5);
  
  j = 0;
  for(i ; i<length;i++){
    if(isdigit(payload[i])){
      rgbBuffer[j] = payload[i];
    }else{
      break;
    }
    j++;
  }
  b = atoi(rgbBuffer);


  Serial.print("R ");
  Serial.print(r);
  Serial.print(" G ");
  Serial.print(g);
  Serial.print(" B ");
  Serial.println(b);

  if( (millis() - connectedTimer < 2000 && topic[3] == lastTopic) || millis() - connectedTimer > 2000){
    changePixels(); 
    lastTopic = topic[3];
  }
  
}

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
  connectedTimer = millis();
  Serial.println("\nconnected!");
  client.subscribe("ledall", 2);
  client.subscribe("led25", 2);
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

  pixels.begin();
  
  client.begin(server, ethClient);
  client.onMessageAdvanced(messageReceived);
}

uint32_t mesh_timer = 0;
uint32_t pub_timer = 0;

void loop() {
  Ethernet.update();

  if (millis() - mesh_timer > 10000) {  //Every 10 seconds, test mesh connectivity
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

}
