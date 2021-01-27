/**
 *	Librerie richieste: MD_Parola, YoutubeApi, ArduinoJson?, WiFiClientSecure
 **/
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <YoutubeApi.h>
#include <ArduinoJson.h>

// Impostazioni
#define DEBUG 				1
#define INSECURE_COMM 			1
#define HOSTNAME 			"YouTubeSubsCounter"

// Parametri Matrice Led 
#define HARDWARE_TYPE 		MD_MAX72XX::FC16_HW
#define MAX_DEVICES 		4  		// Numero matrici
#define PIN_CLK           	12 		// D6
#define PIN_DATA          	15 		// D8
#define CS_DATA           	13 		// D7

// Parametri per comunicazione con YouTube
#define idCanale          	"**********"    // ID Canale YouTube (Quello nell'URL)
#define apiKey            	"**********"    // Chiave API di Google

// Parametri per connessione a WiFi
#define SSID             	"**********"  	// SSID Rete Wi-Fi
#define PASSWORD          	"**********" 	// PASSWORD della Wi-Fi


WiFiClientSecure client;
YoutubeApi api(apiKey, client);

// Led matrix printer
MD_Parola displayLed = MD_Parola(HARDWARE_TYPE, PIN_DATA, PIN_CLK, CS_DATA, MAX_DEVICES);


void init_serial() {
	Serial.begin(9600);
}

/**
 * Abilita funzionalità e dettagli per il debugging
 */
void init_debug() {
	if (DEBUG) {
		api._debug = true;
		Serial.println("Debug build");
	}
}


void init_display() {
	displayLed.setTextAlignment(PA_CENTER);
}

/**
 * Abilita le comunicazioni in chiaro, necessarie per modelli di MTU precedenti al 2.5
 */
void init_client() {
	if (INSECURE_COMM) {
		client.setInsecure();
	}
}

/**
 * Funzione per inizializzare la libreria WiFi. Questa funzione fa alcune cose;
 * - Imposta la modalità del wifi a "WiFi station"
 * - Si disconnette da eventuali reti già collegate
 * - Mette il nome in rete della scheda al contenuto di HOSTNAME
 * - Abilita la riconnessione automatica
 */
void init_wifi() {
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);

	WiFi.hostname(HOSTNAME);
	WiFi.setAutoReconnect(true);
}

/**
 * Eseguiamo la connessione alla rete wifi descritta dall'SSID e dalla PASSWORD, con diagnostica degli errori.
 * Nel caso di successo il programma procede.
 * Nel caso di un errore recuperabile la scheda ritenta la connessione.
 * Nel caso di un errore fatale il programma si termina.
 **/
int start_wifi_connection() {
	if (WiFi.status() == WL_CONNECTED) {
		if (DEBUG) {
			Serial.printf("Gia connesso alla rete [%s]\n", SSID);
		}
		return 0;
	}

	if (DEBUG) {
		Serial.print("Tentativo di connessione alla rete [");
		Serial.print(SSID);
		Serial.println("]");
	}

	// Viene iniziata la procedura di connessione alla rete
	WiFi.begin(SSID, PASSWORD);


	// Attendiamo che il wifi si connetta
	while(WiFi.status() == WL_IDLE_STATUS) {
		if (DEBUG) {
	    	Serial.print("."); 
		}
	    delay(250);
	}

	delay(500);

	// Diagnostichiamo lo stato della rete
	switch (WiFi.status()) {
			
		case WL_CONNECTED:
			if (DEBUG) {
				Serial.print("!");
				Serial.println("Connessione stabilita con successo.");

				Serial.printf("Hostname: %s\n", 	WiFi.hostname().c_str());
				Serial.printf("Indirizzo IP: %s\n", 	WiFi.localIP().toString().c_str());
				Serial.printf("Subnet mask: %s\n", 	WiFi.subnetMask().toString().c_str());
				Serial.printf("Mac Address: %s\n", 	WiFi.macAddress().c_str());
				Serial.printf("Gataway IP: %s\n", 	WiFi.gatewayIP().toString().c_str());
				Serial.printf("RSSI: %d dBm\n", 	WiFi.RSSI());
				Serial.printf("BSSID: %s\n", 		WiFi.BSSIDstr().c_str());
			}
			return 0;
			
		case WL_NO_SSID_AVAIL:
			Serial.printf("[E] SSID [%s] non rilevato dalla scheda di rete.", SSID);
			return -1;
			
		case WL_CONNECT_FAILED:
			Serial.print("[E] Password rifiutata dall'SSID [%s].", SSID);
			Serial.println(SSID);
			return -1;
			
		case WL_IDLE_STATUS:
			if (DEBUG) {
				Serial.print("[W] Connessione a [%s] sospesa, ritento tra 10 secondi.", SSID);
			}
			delay(1000 * 10);
			return start_wifi_connection();
			
		case WL_DISCONNECTED:
			Serial.print("[E] Scheda di rete non in station mode (WIFI_STA)");
			return -1;
	}
}

void setup() {
  	init_serial();
  	init_debug();

	init_display();
	init_client();
	init_wifi();

	int wifiStatus = start_wifi_connection();

	if (wifiStatus != 0) {
		if (DEBUG) {
			Serial.println("Termino l'esecuzione.");
		}
		
		exit(-1);
	}
}

void loop() {
	if (api.getChannelStatistics(idCanale)) {
		displayLed.print(api.channelStats.subscriberCount);
	} else {
    		displayLed.print(":(");
	}
  
  	delay(1000 * 60 * 30);  // Aggiorno ogni 30 minuti
}
