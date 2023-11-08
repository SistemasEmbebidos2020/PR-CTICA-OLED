#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Ticker.h>
#include "drawOled.h"

// Define el pin al que está conectado el sensor DS18B20
#define ONE_WIRE_BUS 4 // Cambia esto al pin GPIO que estás utilizando
#define base 5         // pin activacion transistor
// Inicializa el bus OneWire
OneWire oneWire(ONE_WIRE_BUS);
bool state = true;
// Inicializa el objeto DallasTemperature
DallasTemperature sensors(&oneWire);

// Objeto para generar la interrupción cada 1 segundo
Ticker timer;
Ticker timer2;
int t = 0;
int numero_aleatorio = 58545654;

float controloutval;
int pwmValue;
// Parámetros del PID
float Kp = 19.4;
float Ki = 0.257;
float Kd = 18.9;

// Variables del PID
float error_prev = 0;
float integral = 0;

// Valores de referencia y medidos
float setpoint = 29.5; // Valor de referencia
float input = 0;       // Valor medido
float outval = 0;      // Salida del controlador
// Parámetros del PID

unsigned long currentTime, previousTime;
double elapsedTime;

float calculatePID()
{

  currentTime = millis();
  elapsedTime = currentTime - previousTime;

  float error = setpoint - input;
  integral += error;
  float derivative = (error - error_prev);

  // Calcular la salida del PID
  outval = Kp * error + Ki * integral + Kd * derivative;
  // define Pim range between 0 and 255
  if (outval < 0)
    outval = 0;
  if (outval > 255)
    outval = 255;

  // Actualizar el valor previo del error
  error_prev = error;
  return outval;
}

void readTemperature()
{
  // Realiza una lectura de temperatura
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0); // Único sensor en el bus
  input = temperatureC;
  // Imprime la temperatura en el monitor serial

  Serial.print(temperatureC);
  Serial.print("  ,   ");
  Serial.print(setpoint);
  Serial.print("  ,   ");
  Serial.println(outval);
}

void setpointChange()
{
  setpoint = (analogRead(36) / 4095.0 * 5) + 29.0;
}

const int pwmResolution = 256; // Resolución PWM de 8 bits (0-255)

void setup()
{

  pinMode(base, OUTPUT);
  // Inicializa el pin de salida PWM
  ledcSetup(0, 8000, 8);  // Canal 0, frecuencia 1000 Hz, resolución 8 bits
  ledcAttachPin(base, 0); // Asocia el canal 0 al pin base

  oledinit();
  // Configura la comunicación serie para la salida de datos
  Serial.begin(115200);

  // Inicializa el sensor DS18B20
  sensors.begin();

  // Configura el temporizador para ejecutar la función readTemperature cada 1 segundo
  timer.attach(1.0, readTemperature);
  timer2.attach(0.5, setpointChange);
}

void loop()
{
  // Calcular la salida del controlador PID
  controloutval = calculatePID();
  printoled(setpoint,input,controloutval);
  // Aplicar la salida PWM al pin base
  ledcWrite(0, controloutval);
  analogWrite(base, controloutval);
  delay(500); // Pequeña pausa para el bucle
}
