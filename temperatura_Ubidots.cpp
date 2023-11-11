#include <Arduino.h>
#include "UbidotsEsp32Mqtt.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Ticker.h>
#include "drawOled.h"

/*reemplazar el contenido de platfomio.ini por:
[env:featheresp32]
platform = espressif32
board = featheresp32
framework = arduino
monitor_speed = 115200
lib_deps = 
    tomassantanave/Ubidots MQTT for ESP32@^1.0
    milesburton/DallasTemperature@^3.11.0
	adafruit/Adafruit GFX Library@^1.11.5
	adafruit/Adafruit SSD1306@^2.5.7
 */
/*Tener en cuenta que debe estar agregar a su carpeta src en VSC 
el archivo drawOled.h que se encuentra en este repositorio*/

// Objeto para generar la interrupción cada 1 segundo
Ticker timer;

#define ONE_WIRE_BUS 4 // Cambia esto al pin GPIO que estás utilizando
float temperatureC;
OneWire oneWire(ONE_WIRE_BUS);
// Inicializa el objeto DallasTemperature
DallasTemperature sensors(&oneWire);

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBFF-4ZN0fAigkVtNVUmZgPJ0oxHaKVN05r";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "XTRIM_TORRES";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "0921177333";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "Demo";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "Temperatura"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 2000; // Update rate in milliseconds

unsigned long timerpass;
uint8_t analogPin = 34; // Pin used to read data from GPIO34 ADC_CH6.

Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void readTemperature()
{
  // Realiza una lectura de temperatura
  sensors.requestTemperatures();
  temperatureC = sensors.getTempCByIndex(0); // Único sensor en el bus
  // Imprime la temperatura en el monitor serial
  Serial.println(temperatureC);
}
/****************************************
 * Main Functions
 ****************************************/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
   // Inicializa el sensor DS18B20
  sensors.begin();
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  timer.attach(2.0, readTemperature);
  timerpass = millis();
  oledinit();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if ((millis() - timerpass) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    ubidots.add(VARIABLE_LABEL, temperatureC); // Insert your variable Labels and the value to be sent
    drawoled(temperatureC);
    ubidots.publish(DEVICE_LABEL);
    timerpass = millis();
  }
  ubidots.loop();
}
