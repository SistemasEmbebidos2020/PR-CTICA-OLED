#include <Arduino.h>
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
  // se inicializa la pantalla Oled con dirección i2c = 3C
  pantalla.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // se limpia la pantalla
  pantalla.clearDisplay();
  // se establece el color del texto en la pantalla
  pantalla.setTextColor(SSD1306_WHITE);
  // se establece el tamaño del texto a mostrarse en la pantalla
  pantalla.setTextSize(1);
}
void printoled(String txt, float v1, float v2) 
/*v1 = velocidad v2 = pwm*/
{
  pantalla.clearDisplay();
  pantalla.setCursor(5, 5);
  pantalla.print(txt);
  pantalla.setCursor(35, 5);
  pantalla.print(v1);
  pantalla.setCursor(5, 20);
  pantalla.print("Pwm");
  pantalla.setCursor(35, 20);
  pantalla.print(v2);
  pantalla.setCursor(5, 35);
  pantalla.print("volt");
  pantalla.setCursor(35, 35);
  pantalla.print(v2/255.0*3.3);
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
    float pot = analogRead(34)/4095.0*255.0;
    ledcWrite(1, 0);   		// Escribir el pwm de 0 para el pin led2 asociado al canal 1
    ledcWrite(0, pot); 		// Escribir el pwm de 255 para el pin led1 asociado al canal 0
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
        printoled("RPM" , cnt*120 , pot);		// se imprime en la pantalla Oled los RPM
    }
    cnt = 0;
}
