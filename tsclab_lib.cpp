
#include "tsclab_lib.h"

void actualiza_vectores(float v[],int kT){
   // float v: El vector que deseamos desplazar (Actualizar)
  // int kT:  Es la ultima posición del vector v
   int i;
   for(i=1;i<=kT;i++){
      v[i-1]=v[i];
   }
}
