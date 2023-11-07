#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definiciones4
#define ANCHO_PANTALLA 128
#define ALTO_PANTALLA 64

// se llama a la instancia de la oled para definir ancho alto
Adafruit_SSD1306 pantalla(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, -1);

const int sampleRate = 200;  // Tasa de muestreo en milisegundos
const int graphWidth = 105;  // Ancho de la gráfica en píxeles
const int graphHeight = 43;  // Altura de la gráfica en píxeles
const int Yinit = 16;        // se define el valor inicial en Y para la gráfica
static int xinit = 3;        // se define el valor inicial en X para la gráfica
static int prevX, prevY = 0; // se define valor en cero paraa utilizarse mas adelante

void drawejes()
{
  // se grafica una linea vertical desde xinit,Yinit-10  hasta  xinit,Yinit+35 y se establece un color de línea blanca
  pantalla.drawLine(xinit, Yinit - 10, xinit, Yinit + 35, SSD1306_WHITE);
  // se grafica una linea horizontal desde xinit,Yinit+35  hasta  xinit+105,Yinit+35 y se establece un color de línea blanca
  pantalla.drawLine(xinit, Yinit + 36, xinit + 104, Yinit + 36, SSD1306_WHITE);
  // se fija el cursor en la pantalla en 4,5 conociendo que el máximo es 128,64 por la resolucion de la pantalla oled
  pantalla.setCursor(6, 5);
  // se establece el tamaño del texto a escribri
  pantalla.setTextSize(1);
  // se escribe el texto en la posicion 4,5
  pantalla.print("Value: ");
  // se fija el cursor en la pantalla en 95,Yinit+40 conociendo que el máximo es 128,64 por la resolucion de la pantalla oled
  pantalla.setCursor(95, Yinit + 40);
  // se escribe el texto en la posicion 95, Yinit + 40
  pantalla.print("(t)");
  // se fija el cursor en la pantalla en 95,Yinit+40 conociendo que el máximo es 128,64 por la resolucion de la pantalla oled
  pantalla.setCursor(0, Yinit + 40);
  // se escribe el texto en la posicion 95, Yinit + 40
  pantalla.print("0");
  // se refresca la pantalla con lo valores establecidos previamente para todos los pixeles
  pantalla.display();
}

void oledinit()
{
  pantalla.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // se limpia la pantalla
  pantalla.clearDisplay();
  // se establece el color del texto en la pantalla
  pantalla.setTextColor(SSD1306_WHITE);
  // se establece el tamaño del texto a mostrarse en la pantalla
  pantalla.setTextSize(1);

  // Se llama a la función que dibujará los ejes de coordenadas en la pantalla oled
  drawejes();
}

void drawoled(float val2draw)
{
  // int val2draw = map(y, 20, 40, graphHeight+5, Yinit);
  pantalla.fillRoundRect(42, 4, 30, 11, 1, SSD1306_BLACK);
  // se setea el cursor en 85,6
  pantalla.setCursor(42, 6);
  // se muestra el valor del dato leído analógico
  //   Dibuja la lectura en la pantalla OLED
  pantalla.print(val2draw);
  // se verifica que el primer dato sea cero para graficar solo un pixel de inicio
  if (prevY == 0)
  {
    pantalla.drawPixel(xinit, val2draw, SSD1306_WHITE);
  }
  // caso contrario se grafica una línea desde el valor X,Y anterior hasta el nuevo valor X,Y
  else
  {
    pantalla.drawLine(prevX, prevY, xinit, val2draw, SSD1306_WHITE);
  }
  // se refresca la pantalla
  pantalla.display();
  // se establece que el valor anterior de Y sea el último leído
  prevY = val2draw;
  // se establece que el valor anterior de X sea el último leído
  prevX = xinit;
  // se autmenta de 1 en 1 el valor de x para poder graficar
  xinit++;
  // se verifica si el valor de x es mayor o igual al ancho máximo de mi "plotter"
  if (xinit >= graphWidth)
  {
    // se pone el valor de x inicial en el valor establecido originalmente
    xinit = 3;
    // se limpia la pantalla
    pantalla.clearDisplay();
    // se llama a la función para volvera graficar los ejes
    drawejes();
    // se establece el valor de prevY en cero
    prevY = 0;
  }
  // se establece el tiempo de espera entre una toma de datos y otra
  // delay(sampleRate);
}

void printoled(float v1, float v2, float v3) 
/*v1 = setpoint v2 = input v3 = pwm/control*/
{
  pantalla.clearDisplay();
  pantalla.setCursor(5, 5);
  pantalla.print("Sp");
  pantalla.setCursor(30, 5);
  pantalla.print(v1);
  pantalla.setCursor(5, 25);
  pantalla.print("PV");
  pantalla.setCursor(30, 25);
  pantalla.print(v2);
  pantalla.setCursor(5, 45);
  pantalla.print("Pwm");
  pantalla.setCursor(30, 45);
  pantalla.print(v3);
  pantalla.display();
}
