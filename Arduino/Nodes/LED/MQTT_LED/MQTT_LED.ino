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
#include <printf.h>

RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);

#define LEDPIN 5
#define PIRPIN 4
#define NUMPIXELS 3
#define DELAYVAL 500
Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

IPAddress ip(10, 10, 2, 25);
IPAddress gateway(10, 10, 2, 2);  //Specify the gateway in case different from the server
IPAddress server(10, 10, 2, 2);   //The ip of the MQTT server


int r = 0, g = 0, b = 0;
int r1 = 0, g1 = 0, b1 = 0;
bool ledON = 0;

char clientID[] = { "arduinoClient   " };
uint32_t connectedTimer = 0;
char lastTopic = '2';

void changePixels() {

  pixels.clear();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(r1, g1, b1));
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

  for (i; i < length; i++) {
    if (isdigit(payload[i]) ) {
      rgbBuffer[i] = payload[i];
    } else {
      i++;
      break;
    }
  }
  r = atoi(rgbBuffer);
  memset(rgbBuffer, 0, 5);
  int j = 0;
  for (i; i < length; i++) {
    if (isdigit(payload[i]) ) {
      rgbBuffer[j] = payload[i];
    } else {
      i++;
      break;
    }
    j++;
  }
  g = atoi(rgbBuffer);
  memset(rgbBuffer, 0, 5);

  j = 0;
  for (i ; i < length; i++) {
    if (isdigit(payload[i])) {
      rgbBuffer[j] = payload[i];
    } else {
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

  if ( (millis() - connectedTimer < 2000 && topic[3] == lastTopic) || millis() - connectedTimer > 2000) {
    r1 = r; g1 = g; b1 = b;
    lastTopic = topic[3];
    if (ledON) {
      changePixels();
    }
  }

}

EthernetClient ethClient;
MQTTClient client;

void connect() {
  Serial.println("connecting...");
  uint32_t clTimeout = millis();
  if (!client.connect(clientID)) {
    return;
  }
  Serial.println("\nconnected!");
  client.subscribe("ledall", 2);
  client.subscribe("led25", 2);
}

uint32_t pixelTimer = 0;

void setup() {
  Serial.begin(115200);
  printf_begin();
  Ethernet.begin(ip, gateway);

  if (mesh.begin()) {
    Serial.println(" OK");
  } else {
    Serial.println(" Failed");
  }

  radio.printDetails();
  //Convert the last octet of the IP address to an identifier used
  char str[4];
  int test = ip[3];
  itoa(ip[3], str, 10);
  memcpy(&clientID[13], &str, strlen(str));
  Serial.println(clientID);

  pinMode(PIRPIN, INPUT);

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

  if (digitalRead(PIRPIN)) {
    if (!ledON) {
      changePixels();
    }
    ledON = 1;
    pixelTimer = millis();
  } else if (millis() - pixelTimer > 300000) {
    ledON = 0;
    pixels.clear();
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
    pixels.show();

  }
}
