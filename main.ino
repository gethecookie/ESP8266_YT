#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <YoutubeApi.h>
#include <ArduinoJson.h>

// Parametri matrice Led
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN           12   // D6
#define DATA_PIN          15   // D8
#define CS_PIN            13   // D7

//Parametri per connessione a rete WiFi
char ssid[]              = "**********";  // SSID Rete Wi-Fi
char password[]          = "**********";  // Password di Wi-Fi

//Parametri per comunicazione con YouTube
#define idCanale          "**********"    // ID Canale YouTube
#define apiKey            "**********"    // Api Key Google

WiFiClientSecure client;
YoutubeApi api(apiKey, client);

MD_Parola displayLed = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

void setup(void) {

  client.setInsecure();
  
  displayLed.begin();  //inizializzo matrice led

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
/*
  Serial.begin(9600); //inizializzo scrittura di verifica a console
  Serial.print("Connessione in corso a: ");
  Serial.println(ssid);

  api._debug = true;  //stampa in automatico info di debug
*/

  WiFi.begin(ssid,password);  //mi connetto alla rete

  while(WiFi.status() != WL_CONNECTED){
      delay(500);
      //Serial.print(".");        
  }

/*
  Serial.println();

  Serial.println("[DEBUG] Connessione avvenuta con successo!");
  Serial.print("NodeMCU Indirizzo IP: ");
  Serial.println(WiFi.localIP());
*/

  displayLed.setTextAlignment(PA_CENTER);  //allineo testo al centro
}

void loop(void) {
  if(api.getChannelStatistics(idCanale)) {
    /*
      Serial.print("Numero Iscritti: ");
      Serial.println(api.channelStats.subscriberCount);
    */
      displayLed.print(api.channelStats.subscriberCount);
  } 
  else 
    displayLed.print("Error");
  
  
  delay(1000 * 60 * 30);  // aggiorno ogni 30min
}
