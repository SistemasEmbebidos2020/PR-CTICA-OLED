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

int sensor = 15;	//el pin DO del sensor hall conectar al d15 del esp32
int Led1 = 2;		//conectar a IN1 del L298n
int Led2 = 4;		//conectar a IN2 del L298n
int resolution = 8;	//resolución del PWM
int cnt = 0;		//contador para calculo de rpm
long timeIni;
int velreq =0;  //velocidad que se requiere
int vel=120;    //velocidad en pwm de 0-255

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

  // Se llama a la función que dibujará los ejes de coordenadas en la pantalla oled
}
void printoled(float v1, float v2, float v3) 
/*v1 = setpoint v2 = input v3 = pwm/control*/
{
  pantalla.clearDisplay();
  pantalla.setCursor(5, 5);
  pantalla.print("RPM");
  pantalla.setCursor(35, 5);
  pantalla.print(v1);
  pantalla.setCursor(5, 20);
  pantalla.print("Vreq");
  pantalla.setCursor(35, 20);
  pantalla.print(v2);
  pantalla.setCursor(5, 35);
  pantalla.print("Pwm");
  pantalla.setCursor(35, 35);
  pantalla.print(v3);
  pantalla.setCursor(5, 50);
  pantalla.print("volt");
  pantalla.setCursor(35, 50);
  pantalla.print(v3/255.0*3.3);
  pantalla.display();
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
    velreq = (analogRead(34)/4095.0*1020.0) + 540;
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
        Serial.print("requerida --> ");
        Serial.print(velreq);
        Serial.print("\t obtenida --> ");
        Serial.println(cnt*30);
        if (cnt*120 < velreq*0.98 ) vel++;    // si la velocidad actual es menor a la requerida se aumenta el pwm en 1
        else if (cnt*1200 > velreq *1.02) vel--; // si la velocidad actual es mayor a la requerida se disminuye el pwm en 1
        else Serial.println("velocidad obtenida = a la deseada");  //si está dentro del rando se imprime el mensaje
        printoled(cnt*120,velreq,vel);    // se imprime en la pantalla Oled los RPM

    }
    cnt = 0;
}
