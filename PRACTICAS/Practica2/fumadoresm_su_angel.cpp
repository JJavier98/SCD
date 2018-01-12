#include <iostream>
#include <cassert>
#include <iomanip>
#include <random>
#include <chrono> 
#include "HoareMonitor.hpp"

using namespace std ;
using namespace HM ;

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

int Producir(){
   return ( aleatorio<0,2>() );
}


void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;

}





class MfumadoresSU : public HoareMonitor
{
   private:
   int ingrediente;
   bool vacio ;
   CondVar  recoge ;
   CondVar  produce ;
   CondVar  cola;

   public:
   MfumadoresSU();
   void PonerIngrediente(int i); 
   void EsperarRecogidaIngrediente();
   void ObtenerIngrediente( int i );

};

MfumadoresSU::MfumadoresSU(){
   vacio = true;
   recoge = newCondVar();
   produce = newCondVar();
   cola = newCondVar();
}

void MfumadoresSU::PonerIngrediente(int i){
   if(vacio == false )
     produce.wait();
   
   ingrediente  = i ;
   vacio = false;
   for(int i= 0 ; i<3 ; i++){
     cola.signal();
   }
   
}

void MfumadoresSU::EsperarRecogidaIngrediente(){
   while(vacio == false)
     recoge.wait();


   produce.signal();

}

void MfumadoresSU::ObtenerIngrediente( int i ){
   while(ingrediente != i || vacio == true){
     cola.wait();
   }

   vacio = true;
   recoge.signal();

}

void Estanquero(MRef<MfumadoresSU> Estanco){

  int i;

  while (true) {
    i = Producir();
    Estanco->PonerIngrediente( i );
    Estanco->EsperarRecogidaIngrediente();
  }
}


void fumador(MRef<MfumadoresSU> Estanco ,int i ){

   while( true ){
     Estanco->ObtenerIngrediente( i );
     cout << "comprado el ingrediente: " << i << "\n" ;
     fumar(i);


   }
}

int main(){



   MRef<MfumadoresSU> Estanco = Create<MfumadoresSU>();


   thread hebra_estanquero ( Estanquero, Estanco),
          hebra_fumador0(fumador,Estanco,0),
          hebra_fumador1(fumador,Estanco,1),
	  hebra_fumador2(fumador,Estanco,2);


   hebra_estanquero.join() ;
   hebra_fumador0.join() ;
   hebra_fumador1.join() ;
   hebra_fumador2.join() ;
   

   
}




   




  

   	
























