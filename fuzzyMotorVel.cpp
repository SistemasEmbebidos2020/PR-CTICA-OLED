#include <Arduino.h>
/* reemplazar el contenido de platformio.ini por:

[env:featheresp32]
platform = espressif32
board = featheresp32
framework = arduino
monitor_speed = 115200
lib_deps = 
	adafruit/Adafruit GFX Library@^1.11.5
	adafruit/Adafruit SSD1306@^2.5.7
*/

#include <Ticker.h>           // Librería de interrupciones
#include "fuzzy.h"

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

void actualiza_vectores(float v[],int kT){
   // float v: El vector que deseamos desplazar (Actualizar)
  // int kT:  Es la ultima posición del vector v
   int i;
   for(i=1;i<=kT;i++){
      v[i-1]=v[i];
   }
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


int sensor = 15;
int Led1 = 2;
int Led2 = 4;
int resolution = 8;
int cnt = 0;
long timeIni;
int velreq =0;
int vel=120;
Ticker interrupcion1;

/*============================================================
    VARIABLES GLOBALES PARA ERROR Y LEY DE CONTROL 
  ============================================================*/
float pwmOut =0.0;        // %PWM V_Base TIP31
float vrpm=0;            // velocidad en rpm
float w=0;              // Referencia (400 rpm a 2500 rpm)

float e[2]={0,0};      // Vector de error
float u[2]={0,0};     // Vector de Ley de Control
int kU = sizeof(u)/sizeof(float)-1;    // kU es la pos. de la ley de control actual 
int kE = sizeof(e)/sizeof(float)-1;   // e[kE] es el error actual

/*============================================================
    DEFINICIONES GLOBALES CONTROL FUZZY-MAMDANI
              UNIVERSO DEL DISCURSO
  ============================================================*/
float discurso_e = 1560, discurso_de = 1000, discurso_u = 10; //2500 en vez de 1560
float der;       // Almacenar derivada del  discreta
float lu;       // Almacenar ley de control
int Ts = 1;    // Periodo de Muestreo para calculo de derivada discreta

const int buffer_size =16;    // Tamaño del buffer
byte buf[buffer_size];     // Crea un byte buffer de 8 bytes

// Estructura para trabajar con datos tipo byte y float a la vez
union BtoF // Para variable rpm sensados
{
  byte brpm[16];
  float fvalrpm;
} rpmBorF; 
union BtoF2 // Para variable pwmOut
{
  byte bpwmOut[16];
  float fvalpwm;
} pwmBorF;
union BtoF3 // Para variable de referencia (w)
{
  byte bw[16];
  float fvalw;
} wBorF;
// Estructura para guardar parámetros
struct FloatValues {
  float v_w;       // w (ref)
  float v_rpm;    // vrpm
  float v_pwm;   // pwm
  };
FloatValues instancia; // instancia o objeto tipo estruct

/*============================================================
       INTERRUPCION CADA 1 SEGUNDO ACTUALIZACIÓN
  ============================================================*/
void interr_l_control()
{
  actualiza_vectores(u,kU);   //Actualiza vector u 
  actualiza_vectores(e,kE);  //Actualiza vector e 
  
  e[kE] = velreq - vrpm;      //Calcula el error actual: error=ref-realimentación -------- en vez de velreq estaba w
  
  // Satura el error para estar dentro del universo del discurso
  if(e[kE]<-1*discurso_e+0.1)    // limite inf discurso_e
    e[kE]=-1*discurso_e+0.1;    // Satura discurso_e
  if(e[kE]>discurso_e-0.1)     // limite sup discurso_e
    e[kE]=discurso_e-0.1;     // Satura discurso_e
         
  // Calcula la derivada del error discreta
  der=(e[kE]-e[kE-1])/Ts; //fórmula de aprox. de una derivada discreta
   
  // Satura la derivada del error para estar dentro del universo del discurso
  if(der<-1*discurso_de+0.01)
    der=-1*discurso_de+0.01;
  if(der>discurso_de-0.01)
    der=discurso_de-0.01;

  // Ley de control fuzzy  
  // Parámetros: (error_actual,derivada_error,Univ_disc_e,Univ_disc_de, Univ_disc_leycontrol)
  lu=myfuzzy(e[kE],der,discurso_e,discurso_de,discurso_u); 
   
  //Integral Colocada en salida de u
  u[kU] = lu + u[kU-1];   // Accion de control tiempo presente
  // Anti - Windup (Saturación de ley de control)
  if (u[kU] >= 100.0)        
      u[kU] = 100.0;          
  if (u[kU] <= 4.0)
      u[kU] = 0.0;
   pwmOut = u[kU];      // Actualiza el %PWM correspondiente
  //Aplica la acción de control PWM en pinEnable Driver L293D
   ledcWrite(0,map(pwmOut , 0,100, 0,255)); //Max= 100, Min=0
   // Solo gira en un semtido:
 //  digitalWrite(Led1, HIGH);   // Activa PinDriver 1
   digitalWrite(Led2, LOW);   //  Activa PinDriver 2
}
volatile unsigned long pulseCount = 0;
unsigned long lastTime = 0;
unsigned long deltaTime = 0;
unsigned long lastTime2 = 0;
unsigned long deltaTime2 = 0;
// Interrupción externa sensor óptico
void ICACHE_RAM_ATTR sensorInterrupt() {
  pulseCount++; // Incremento de contador en cada pulso
}


void setup()
{
    // Config. de la interrupción
    lastTime = millis();       // Recupera el tiempo actual en ms
    interrupcion1.attach(Ts, interr_l_control);// Llama a la interrupción cada Ts segundos
    pinMode(sensor, INPUT);
    pinMode(Led1, OUTPUT);
    pinMode(Led2, OUTPUT);
    Serial.begin(115200);
    ledcSetup(0, 5000, resolution); // Canal 0, frecuencia de 5 kHz
    ledcAttachPin(Led1, 0);
    ledcSetup(1, 5000, resolution); // Canal 0, frecuencia de 5 kHz
    ledcAttachPin(Led2, 1);
    oledinit();
  attachInterrupt(digitalPinToInterrupt(sensor), sensorInterrupt, RISING) ; 
}
void loop()
{
    velreq = (analogRead(34)/4095.0*1020.0) + 540;
 //   ledcWrite(1, 0);  //
 //   ledcWrite(0, pwmOut); // 
  /*==============================================================
                  Actualización de contador
    ==============================================================*/
  unsigned long currentTime = millis();  // Recupera el tiempo actual
  deltaTime = currentTime - lastTime;   // Intervalo transcurrido desde que inicia programa
  //  Se ejecuta una acción cada 1s:
  if (deltaTime >= 1000) {             // Calcula la velocidad cada segundo
    vrpm = (pulseCount * 60.0);       // Convierte a RPS a RPM

    pulseCount = 0;          // Reinicia contador para siguiente conteo de rpm
    lastTime = currentTime; // Actualiza el tiempo
  // ******** Enviar Datos por Puerto Serial ***************
    Serial.print(vrpm); // Escribir por serial vrpm y pwmOut
    Serial.print("  <<--rpm   pwm -->>  ");
    Serial.print(pwmOut/100.0*255);
    Serial.print("   relReq -->>  ");
    Serial.println(velreq);
    printoled(vrpm,velreq,pwmOut/100.0*255);
  }
}
