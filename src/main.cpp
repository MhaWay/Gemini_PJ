/*
    SlimeVR Code is placed under the MIT license
    Copyright (c) 2021 Eiren Rain & SlimeVR contributors

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include "Wire.h"
#include "ota.h"
#include "sensors/SensorManager.h"
#include "configuration/Configuration.h"
#include "network/network.h"
#include "globals.h"
#include "credentials.h"
#include <i2cscan.h>
#include "serial/serialcommands.h"
#include "LEDManager.h"
#include "status/StatusManager.h"
#include "batterymonitor.h"
#include "logging/Logger.h"
#include "TCA9548A.h" // include la libreria TCA9548A
// Definisci il numero di sensori collegati al multiplexer
#define NUM_SENSORS 8

// Definisci l'indirizzo del multiplexer e il canale predefinito per il sensore selezionato
#define TCA_ADDR 0x70
#define DEFAULT_CHANNEL 0


SlimeVR::Logging::Logger logger("SlimeVR");
SlimeVR::Sensors::SensorManager sensorManager;
SlimeVR::LEDManager ledManager(LED_PIN);
SlimeVR::Status::StatusManager statusManager;
SlimeVR::Configuration::Configuration configuration;

int sensorToCalibrate = -1;
bool blinking = false;
unsigned long blinkStart = 0;
unsigned long loopTime = 0;
unsigned long lastStatePrint = 0;
bool secondImuActive = false;
BatteryMonitor battery;

void setup() {
  // Inizializzazione seriale
  Serial.begin(115200);
  delay(1000);
  
  // Inizializzazione del WiFi
  WiFiManager wifiManager;
  wifiManager.autoConnect("SlimeVR Tracker");
  
  // Inizializzazione del client MQTT
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(mqttCallback);

  // Inizializzazione del multiplexer
  mux.begin(TCA_ADDR);
  mux.select(DEFAULT_CHANNEL);
  
  // Inizializzazione dei sensori
  for (int i = 0; i < NUM_SENSORS; i++) {
    sensors[i].begin(BNO_ADDRS[i], &Wire, mux);
    sensors[i].setWire(&Wire);
    sensors[i].setAddr(BNO_ADDRS[i]);
    sensors[i].setMux(&mux);
    sensors[i].enableRotationVector();
    sensors[i].enableLinearAcceleration();
  }

  // Connessione al server MQTT
  while (!mqttClient.connected()) {
    if (mqttClient.connect(mqttClientId)) {
      Serial.println("Connesso al server MQTT");
      mqttClient.subscribe(mqttTopic);
    } else {
      Serial.println("Connessione al server MQTT fallita. Riprovo tra 5 secondi...");
      delay(5000);
    }
  }

  // Stampa un messaggio di avvio completato
  Serial.println("Avvio completato");
}

void loop() {
  // Loop attraverso tutti i sensori collegati al multiplexer
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    // Seleziona il canale corrispondente al sensore corrente
    mux.select(i);

    // Leggi i dati dal sensore corrente
    RotVecData data = rotvec_read();

    // Invia i dati tramite ESP-Now
    if (esp_now_send(BROADCAST_ADDRESS, (uint8_t*)&data, sizeof(data)) != ESP_OK) {
      Serial.println("Errore durante l'invio dei dati tramite ESP-Now");
    }
    delay(10);
  }

  // Aggiungi una funzione di ritardo tra le iterazioni del loop
  delay(100);
}

// Here we create a loop that will iterate through all 8 channels of the multiplexer
/*for (int i = 0; i < 8; i++) {
    // Select the current channel
    multiplexer.selectChannel(i);
    //Here we can read data from the sensor connected to the current channel.
    //We can use the sensorManager.update() function or a specific function for the sensor we are using.
    sensorManager.update();
}*/

#ifdef TARGET_LOOPTIME_MICROS
long elapsed = (micros() - loopTime);
if (elapsed < TARGET_LOOPTIME_MICROS)
{
    long sleepus = TARGET_LOOPTIME_MICROS - elapsed - 100;//µs to sleep
    long sleepms = sleepus / 1000;//ms to sleep
    if(sleepms > 0) // if >= 1 ms
    {
        delay(sleepms); // sleep ms = save power
        sleepus -= sleepms * 1000;
    }
    if(sleepus > 0) // if >= 1 µs
    {
        delayMicroseconds(sleepus);
    }
}
loopTime = micros();
#endif
    #if defined(PRINT_STATE_EVERY_MS) && PRINT_STATE_EVERY_MS > 0
        unsigned long now = millis();
//        if(lastStatePrint + PRINT_STATE_EVERY_MS < now) {
//            lastStatePrint = now;
//            SerialCommands::printState();
//}

    #endif

