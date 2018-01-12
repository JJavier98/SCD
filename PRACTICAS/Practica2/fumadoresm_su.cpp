#include <iostream>
#include <cassert>
#include <iomanip>
#include <random>
#include <chrono> 
#include "HoareMonitor.hpp"

using namespace std ;
using namespace HM ;

const static int NUM_FUMADORES = 3;

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

int ProducirIngrediente(){
	chrono::milliseconds duracion_produccion( aleatorio<20,200>() );
	this_thread::sleep_for( duracion_produccion );
	return ( aleatorio<0,2>() );
}


void fumar( int num_fumador )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "###***Fumador " << num_fumador << ": empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "***###Fumador " << num_fumador << ": termina de fumar, comienza espera de ingrediente." << endl;

}



class MfumadoresSU : public HoareMonitor
{
private:
  	int ingrediente;
	bool mostrador_vacio;
	CondVar fumadores[NUM_FUMADORES];

public:
	MfumadoresSU();
	void ponerIngrediente(int i); 
	void EsperarRecogidaIngrediente(int i);
	void ObtenerIngrediente( int i );

};

MfumadoresSU::MfumadoresSU(){
   	ingrediente = -1;
	mostrador_vacio = true;

	for (int i = 0; i < NUM_FUMADORES; ++i)
	{
		fumadores[i] = newCondVar();
	}
}

void MfumadoresSU::ponerIngrediente(int i){
	if(mostrador_vacio)
	{
		ingrediente  = i ;
		cout << "++++++Se ha puesto el ingrediente: " << i << endl;
		mostrador_vacio = false;
	}
}

void MfumadoresSU::EsperarRecogidaIngrediente(int i){
	if(!mostrador_vacio && ingrediente == i)
		fumadores[i].signal();
}

void MfumadoresSU::ObtenerIngrediente( int i ){

	fumadores[i].wait();

	cout << "------El fumador " << i << " retira su ingrediente." << endl;

	mostrador_vacio = true;
}

void Estanquero(MRef<MfumadoresSU> Estanco){

  int i;

  while (true) {
    i = ProducirIngrediente();
    Estanco->ponerIngrediente( i );
    Estanco->EsperarRecogidaIngrediente(i);
  }
}


void fumador(MRef<MfumadoresSU> Estanco ,int i ){

   while( true ){
    Estanco->ObtenerIngrediente( i );
    fumar(i);
   }
}

int main(){

   	MRef<MfumadoresSU> Estanco = Create<MfumadoresSU>();


   	thread 	hebra_estanquero ( Estanquero, Estanco),
   			hebra_fumadores[NUM_FUMADORES];

	for (int i = 0; i < NUM_FUMADORES; ++i)
	{
		hebra_fumadores[i] = thread(fumador,Estanco,i);
	}

	for (int i = 0; i < NUM_FUMADORES; ++i)
	{
		hebra_fumadores[i].join();
	}
	hebra_estanquero.join() ;   
}




   




  

   	
























