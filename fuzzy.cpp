/* *****************************************************************************************************
                          CÓDIGO BASE DE CONTROL DE LÓGICA DIFUSA              
  Este es un código base por lógica difusa, puede modificar de acuerdo a sus especificaciones de diseño. 
  Esta función del Fuzzy-Mamdani tiene fundamentos de trabajos anteriores, no olvide referenciar:

  CITE AS:
    Giraldo, S. A. C., Gómez, D. S. H., & Blandón, J. H. G. (2013). Control y 
    monitoreo de temperatura para un horno de curado de prendas índigo utilizando 
    lógica difusa y controles pi. REVISTA POLITÉCNICA, 9(17), 69 - 81. 
    Recuperado a partir de https://revistas.elpoli.edu.co/index.php/pol/article/view/342
    
   **************************************************************************************************** */
#include "fuzzy.h" //Libreria de encabezados
/*===============================================================================
          CREACIÓN DE FUNCIÓN DE MEMBRESÍA (TRIANGULAR)
  ===============================================================================*/
float triangular(float x,float a,float b,float c)
{
   float y;
   y=max(min((x-a)/(b-a),(c-x)/(c-b)),0); //función membresía triangular
   return(y);
}
/*===============================================================================
        FUNCIÓN PRINCIPAL DE CONTROL FUZZY - MAMDANI
  ===============================================================================*/
float myfuzzy(float e,float de,float me,float mde,float mu)
{
  /*==========================================================
                  ETAPA DE FUSIFICACIÓN
    ==========================================================*/
   // me= univ. discurso max. error; mde= max. derivada del error
  // mu = universo salida o max. u  
 // DEF. VARIABLES LINGUISTICAS DE FUZZY-MAMDANI (7 Var. Linguisticas)
 float eNG,eNM,eNP,eZ,ePP,ePM,ePG;                 // Var. Ling. del Error
 float deNG,deNM,deNP,deZ,dePP,dePM,dePG;         // Var. Ling. de la Derivada del Error
 float uNG=0,uNM=0,uNP=0,uZ=0,uPP=0,uPM=0,uPG=0; //Salida Parcial
 float uFNG,uFNM,uFNP,uFZ,uFPP,uFPM,uFPG;       // Var. Ling. de la Salida Final
 float Uuniverse[3];                           // Vector Univ. del discurso desde -1*10 hasta 1*10 con incrementos de 0.1
 float r[49];                                 // Vector de Reglas de inferencia (7x7)
 float maxU=0;                               // Maximos de todas las salidas (E. Agregación)
 float cg;                                  //Centro de Gravedad
 float sumfx=0,sumUX=0;                    // Var. para cálculo de cg
 int i;                                   //Contadores
 float k;                                // Acumulador
 /*===================================================================
          DEF. DEL UNIVERSO DEL DISCURSO DE LEY DE CONTROL u
   ===================================================================*/
 //Para recorrer todas las funciones de membresia del discurso
 Uuniverse[0]=-1*mu;        // -10
 Uuniverse[1]=(mu*2)/200;  // Incrementos
 Uuniverse[2]=1*mu;       // 10
 /*===================================================================
          PERTENENCIA DE VARIABLES LINGUISTICAS
   ===================================================================*/
 // Ubicando variables linguisticas en funciones de membresia del error   
 eNG=triangular(e,-1*me,-1*0.75*me,-1*0.5*me);
 eNM=triangular(e,-1*0.75*me,-1*0.5*me, -1*0.25*me);
 eNP=triangular(e,-1*0.5*me, -1*0.25*me, 0);
 eZ=triangular(e,-1*0.25*me, 0, 0.25*me);
 ePP=triangular(e,0, 0.25*me, 0.5*me);
 ePM=triangular(e,0.25*me, 0.5*me, 0.75*me);
 ePG=triangular(e,0.5*me, 0.75*me, me);
 // Ubicando variables linguisticas en funciones de membresia de la derivada del error 
 deNG=triangular(de,-1*mde,-1*0.75*mde,-1*0.5*mde);
 deNM=triangular(de,-1*0.75*mde,-1*0.5*mde, -1*0.25*mde);
 deNP=triangular(de,-1*0.5*mde, -1*0.25*mde, 0);
 deZ=triangular(de,-1*0.25*mde, 0, 0.25*mde);
 dePP=triangular(de,0, 0.25*mde, 0.5*mde);
 dePM=triangular(de,0.25*mde, 0.5*mde, 0.75*mde);
 dePG=triangular(de,0.5*mde, 0.75*mde, mde);
/*===================================================================
               MECANISMO Y REGLAS DE INFERENCIA
  ===================================================================*/ 
 // DE ACUERDO A LA TABLA FAM
 r[0]=min(eNG,deNG);
 r[1]=min(eNG,deNM);
 r[2]=min(eNG,deNP);
 r[3]=min(eNG,deZ);
 r[4]=min(eNG,dePP);
 r[5]=min(eNG,dePM);
 r[6]=min(eNG,dePG);
 r[7]=min(eNM,deNG);
 r[8]=min(eNM,deNM);
 r[9]=min(eNM,deNP);
 r[10]=min(eNM,deZ);
 r[11]=min(eNM,dePP);
 r[12]=min(eNM,dePM);
 r[13]=min(eNM,dePG);
 r[14]=min(eNP,deNG);
 r[15]=min(eNP,deNM);
 r[16]=min(eNP,deNP);
 r[17]=min(eNP,deZ);
 r[18]=min(eNP,dePP);
 r[19]=min(eNP,dePM);
 r[20]=min(eNP,dePG);
 r[21]=min(eZ,deNG);
 r[22]=min(eZ,deNM);
 r[23]=min(eZ,deNP);
 r[24]=min(eZ,deZ);
 r[25]=min(eZ,dePP);
 r[26]=min(eZ,dePM);
 r[27]=min(eZ,dePG);
 r[28]=min(ePP,deNG);
 r[29]=min(ePP,deNM);
 r[30]=min(ePP,deNP);
 r[31]=min(ePP,deZ);
 r[32]=min(ePP,dePP);
 r[33]=min(ePP,dePM);
 r[34]=min(ePP,dePG);
 r[35]=min(ePM,deNG);
 r[36]=min(ePM,deNM);
 r[37]=min(ePM,deNP);
 r[38]=min(ePM,deZ);
 r[39]=min(ePM,dePP);
 r[40]=min(ePM,dePM);
 r[41]=min(ePM,dePG);
 r[42]=min(ePG,deNG);
 r[43]=min(ePG,deNM);
 r[44]=min(ePG,deNP);
 r[45]=min(ePG,deZ);
 r[46]=min(ePG,dePP);
 r[47]=min(ePG,dePM);
 r[48]=min(ePG,dePG);
 
 /*==================================================
          ETAPA DE AGREGACION POR EL MAXIMO
  ===================================================*/
 
  //Reglas Negativo Grande
    for(i=0;i<4;i++)
    {
     uNG = max(uNG,r[i]);
    }
    for(i=4;i<7;i++)
    {
     uNG = max(uNG,r[i+3]);
    }
    uNG = max(uNG,r[14]);
    uNG = max(uNG,r[15]);
    uNG = max(uNG,r[21]);

    //Reglas Negativo Mediano
    uNM = max(uNM,r[4]);
    uNM = max(uNM,r[10]);
    uNM = max(uNM,r[16]);
    uNM = max(uNM,r[22]);
    uNM = max(uNM,r[28]);
    
    //Reglas Negativo Pequeno
    uNP = max(uNP,r[5]);
    uNP = max(uNP,r[11]);
    uNP = max(uNP,r[17]);
    uNP = max(uNP,r[23]);
    uNP = max(uNP,r[29]);
    uNP = max(uNP,r[35]);
    
    //Reglas Zero
    uZ = max(uZ,r[6]);
    uZ = max(uZ,r[12]);
    uZ = max(uZ,r[18]);
    uZ = max(uZ,r[24]);
    uZ = max(uZ,r[30]);
    uZ = max(uZ,r[36]);
    uZ = max(uZ,r[42]);
    
    //Reglas Positivo Pequeno
    uPP = max(uPP,r[13]);
    uPP = max(uPP,r[19]);
    uPP = max(uPP,r[25]);
    uPP = max(uPP,r[31]);
    uPP = max(uPP,r[37]);
    uPP = max(uPP,r[43]);
    
    //Reglas Positivo Mediano
    uPM = max(uPM,r[20]);
    uPM = max(uPM,r[26]);
    uPM = max(uPM,r[32]);
    uPM = max(uPM,r[38]);
    uPM = max(uPM,r[44]);
    
    //Reglas Positivo Grande
    for(i=0;i<4;i++)
    {
     uPG = max(uPG, r[48-i]);
    }
    for(i=4;i<7;i++)
    {
     uPG = max(uPG, r[45-i]);
    }
    uPG = max(uPG, r[34]);
    uPG = max(uPG, r[33]);
    uPG = max(uPG, r[27]);
/*==================================================
          ETAPA DE DEFUSIFICACIÓN
  ===================================================*/
// OBTENCIÓN DE LA SALIDA DEL CONTROL DIFUSO
   k=Uuniverse[0];
   while(k<=Uuniverse[2])
   {
     uFNG=triangular(k,-1*mu,-1*0.75*mu,-1*0.5*mu);
     uFNM=triangular(k,-1*0.75*mu,-1*0.5*mu, -1*0.25*mu);
     uFNP=triangular(k,-1*0.5*mu, -1*0.25*mu, 0);
     uFZ=triangular(k,-1*0.25*mu, 0, 0.25*mu);
     uFPP=triangular(k,0, 0.25*mu, 0.5*mu);
     uFPM=triangular(k,0.25*mu, 0.5*mu, 0.75*mu);
     uFPG=triangular(k,0.5*mu, 0.75*mu, mu);
    /*==================================================
                ETAPA DE SATURACIÓN
      ===================================================*/  
    //Saturo la salida con los puntos maximos de la agregacion
    if(uFNG>=uNG)
     uFNG=uNG;
    if(uFNM>=uNM)
     uFNM=uNM;
    if(uFNP>=uNP)
     uFNP=uNP;
    if(uFZ>=uZ)
     uFZ=uZ;
    if(uFPP>=uPP)
     uFPP=uPP;
    if(uFPM>=uPM)
     uFPM=uPM;
    if(uFPG>=uPG)
     uFPG=uPG;
     
      
// SATURACIÓN: Agrupo los valores maximos hallados de las 7 funciones de membresía
    maxU = max(maxU,uFNG);
    maxU = max(maxU,uFNM);
    maxU = max(maxU,uFNP);
    maxU = max(maxU,uFZ);
    maxU = max(maxU,uFPP);
    maxU = max(maxU,uFPM);
    maxU = max(maxU,uFPG);
    /*==================================================
          CÁLCULO DEL CENTRO DE GRAVEDAD (cg)
                cg =Sumx*fx/Sumfx
    ===================================================*/
    //Guardo en el vector "FX" (maxU)
    sumUX = sumUX + maxU*k;
    sumfx = sumfx + maxU;
    
    k=k+Uuniverse[1];  // suma k cada periodo
    maxU = 0; 
   }
   
   //CALCULO DEL CENTRO DE GRAVEDAD
   cg=sumUX/sumfx;

  return(cg);

}
