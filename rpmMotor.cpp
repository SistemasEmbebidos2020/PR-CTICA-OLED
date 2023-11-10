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

int sensor = 15;	//el pin DO del sensor hall conectar al d15 del esp32
int Led1 = 2;		//conectar a IN1 del L298n
int Led2 = 4;		//conectar a IN2 del L298n
int resolution = 8;	//resolución del PWM
int cnt = 0;		//contador para calculo de rpm
long timeIni;

void oledinit()
{
  // se inicializa la pantalla Oled con dirección i2c = 3C
  pantalla.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // se limpia la pantalla
  pantalla.clearDisplay();
  // se establece el color del texto en la pantalla
  pantalla.setTextColor(SSD1306_WHITE);
  // se establece el tamaño del texto a mostrarse en la pantalla
  pantalla.setTextSize(1);
}
void printoled(String txto, float v1) 
/*txto es el mensaje que se desea mostrar, v1 el valor a mostrar para este caso rpm*/
{
  pantalla.clearDisplay();	// se limpia la pantalla
  pantalla.setCursor(5, 5);	// se establece que el cursor esté en 5,5
  pantalla.print(txto);		// se imprime el texto que se desea mostrar 
  pantalla.setCursor(35, 5);	// se establece que el cursor esté en 35,5
  pantalla.print(v1);		// se imprime el valor de rpm
  pantalla.display();		// se actualizar el contenido de la pantalla
}
void setup()
{
    pinMode(sensor, INPUT);	// se establece el pin sensor como entrada
    pinMode(Led1, OUTPUT);	// se establece el pin led1 como salida	
    pinMode(Led2, OUTPUT);	// se establece el pin led2 como salida
    Serial.begin(115200);	// se inicializa la comunicacion serial a 115200 baudios
    oledinit();			// se llama a la función que establece los parámetros iniciales de la oled
    ledcSetup(0, 5000, resolution); // Canal 0, frecuencia de 5 kHz con resolución de 8bits
    ledcAttachPin(Led1, 0);	//se enlaza el canal 0 con el pin led1
    ledcSetup(1, 5000, resolution); // Canal 1, frecuencia de 5 kHz con resolución de 8bits
    ledcAttachPin(Led2, 1);	//se enlaza el canal 1 con el pin led2
}
void loop()
{
    ledcWrite(1, 0);   		// Escribir el pwm de 0 para el pin led2 asociado al canal 1
    ledcWrite(0, 255); 		// Escribir el pwm de 255 para el pin led1 asociado al canal 0
    timeIni = millis();		// devuelve el tiempo en milisegundos
    if (digitalRead(sensor))	// pregunta si ha detectado por lo menos una vez el pulso del sensor para ingresar al conteo
    {
        while (millis() - timeIni < 500)	// espera a que pase medio segundo
        {
            if (digitalRead(sensor))		// pregunta si detecta el pulso
            {
                delay(10);			// sistema antirebote
                if (!(digitalRead(sensor)))	// espera a que deje de detectar el pulso

                {
                    cnt++;			// aumenta el contador en 1 para el calculo del rpm
                }
            }
        }
        Serial.print("Rpm --> ");		// se imprime en serial Rpm
        Serial.println(cnt*120);		// se multiplica por 120 para el cáluclo de RPM
        printoled("RPM" , cnt*120);		// se imprime en la pantalla Oled los RPM
    }
    cnt = 0;					// se reinicia el contador
}
