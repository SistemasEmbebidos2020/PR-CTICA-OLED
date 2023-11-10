#include <Arduino.h>

/* en platformio.ini reemplazar su contenido por:
; PlatformIO Project Configuration File
;
;   Build options: build flags, source filter
;   Upload options: custom upload port, speed and extra flags
;   Library options: dependencies, extra library storages
;   Advanced options: extra scripting
;
; Please visit documentation for the other options and examples
; https://docs.platformio.org/page/projectconf.html

[env:featheresp32]
platform = espressif32
board = featheresp32
framework = arduino
monitor_speed = 115200
lib_deps = 
	milesburton/DallasTemperature@^3.11.0
	adafruit/Adafruit GFX Library@^1.11.5
	adafruit/Adafruit SSD1306@^2.5.7
*/


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definiciones4
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64

// se llama a la instancia de la oled para definir ancho alto
Adafruit_SSD1306 pantalla(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);

int sensor = 15;
int Led1 = 2;
int Led2 = 4;
int resolution = 8;
int cnt = 0;
long timeIni;

void oledinit()
{
  pantalla.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // se limpia la pantalla
  pantalla.clearDisplay();
  // se establece el color del texto en la pantalla
  pantalla.setTextColor(SSD1306_WHITE);
  // se establece el tamaño del texto a mostrarse en la pantalla
  pantalla.setTextSize(1);
}
void printoled(String txto, float v1) 
/*v1 = setpoint v2 = input v3 = pwm/control*/
{
  pantalla.clearDisplay();
  pantalla.setCursor(5, 5);
  pantalla.print(txto);
  pantalla.setCursor(35, 5);
  pantalla.print(v1);
  pantalla.display();
}
void setup()
{
    pinMode(sensor, INPUT);
    pinMode(Led1, OUTPUT);
    pinMode(Led2, OUTPUT);
    Serial.begin(115200);
    oledinit();
    ledcSetup(0, 5000, resolution); // Canal 0, frecuencia de 5 kHz
    ledcAttachPin(Led1, 0);
    ledcSetup(1, 5000, resolution); // Canal 0, frecuencia de 5 kHz
    ledcAttachPin(Led2, 1);
}
void loop()
{
    ledcWrite(1, 0);  // Escribir el ciclo de trabajo (0-2^resolution)
    ledcWrite(0, 255); // Escribir el ciclo de trabajo (0-2^resolution)
    timeIni = millis();
    if (digitalRead(sensor))
    {
        while (millis() - timeIni < 500)
        {
            if (digitalRead(sensor))
            {
                delay(10);
                if (!(digitalRead(sensor)))

                {
                    cnt++;
                }
            }
        }
        Serial.print("Rpm --> ");
        Serial.println(cnt*120);
        printoled("RPM" , cnt*120);
    }
    cnt = 0;
}
