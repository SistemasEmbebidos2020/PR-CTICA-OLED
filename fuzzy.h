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
// Redefiniendo funciones MAX y MIN
#undef max
#define max(a,b) ((a)>(b)?(a):(b))
#undef min
#define min(a,b) ((a)<(b)?(a):(b))

#ifndef __FUZZY_H
#define __FUZZY_H

//Creamos la firma de la función
float triangular(float x,float a,float b,float c);
float myfuzzy(float e,float de,float me,float mde,float mu);

#endif
