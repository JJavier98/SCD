#include <iostream>
#include <cassert>
#include <iomanip>
#include <random>
#include <chrono> 
#include "HoareMonitor.hpp"

using namespace std ;
using namespace HM ;

const 	int	NUM_FUMADORES 	= 10,
			NUM_ESTANQUEROS = 5,
			NUM_ITERACIONES = 10;
		int cont_fum = 0,
			cont_est = 0;		

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

int ProducirIngrediente(){
	chrono::milliseconds duracion_produccion( aleatorio<20,200>() );
	this_thread::sleep_for( duracion_produccion );
	return ( aleatorio<0,NUM_FUMADORES-1>() );
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

	void ponerIngrediente(int i, int est); 
	void EsperarRecogidaIngrediente(int i);
	void ObtenerIngrediente( int i );

	void SacarFumadores();
	int IngredienteActual();

};

MfumadoresSU::MfumadoresSU(){
   	ingrediente = -1;
	mostrador_vacio = true;

	for (int i = 0; i < NUM_FUMADORES; ++i)
	{
		fumadores[i] = newCondVar();
	}
}

void MfumadoresSU::ponerIngrediente(int ingr, int est){
	if(mostrador_vacio)
	{
		mostrador_vacio = false;
		ingrediente  = ingr ;
		cout << "++++++El estaquero " << est << " ha puesto el ingrediente: " << ingr << endl;
	    ++cont_est;
	}
}

void MfumadoresSU::EsperarRecogidaIngrediente(int i){
	
	if(!mostrador_vacio && ingrediente == i)
	{
		fumadores[i].signal();
	}
}

void MfumadoresSU::ObtenerIngrediente( int i ){

	fumadores[i].wait();

	if(cont_fum < NUM_ITERACIONES)
	{

		cout << "------El fumador " << i << " retira su ingrediente." << endl;

	    ++cont_fum;

		mostrador_vacio = true;
	}
	
}

int MfumadoresSU::IngredienteActual()
{
	return ingrediente;
}


void MfumadoresSU::SacarFumadores()
{
	for (int i = 0; i < NUM_FUMADORES; ++i)
	{
		while ( !fumadores[i].empty())
		{
			fumadores[i].signal();
		}
	}
}


void Estanquero(MRef<MfumadoresSU> Estanco, int i){

 	int ingrediente;

  	while (cont_est < NUM_ITERACIONES)
	{
	    ingrediente = ProducirIngrediente();
	    Estanco->ponerIngrediente( ingrediente, i );
	    Estanco->EsperarRecogidaIngrediente(ingrediente);
  	}
  	Estanco->SacarFumadores();
}


void fumador(MRef<MfumadoresSU> Estanco ,int i ){

   	while( cont_fum < NUM_ITERACIONES )
   	{
    	Estanco->ObtenerIngrediente( i );

    	if(Estanco->IngredienteActual() == i)
    	{
    		fumar(i);
    	}
   	}
}

int main(){

   	MRef<MfumadoresSU> Estanco = Create<MfumadoresSU>();


   	thread 	hebra_estanqueros[NUM_ESTANQUEROS],
   			hebra_fumadores[NUM_FUMADORES];


   	for (int i = 0; i < NUM_ESTANQUEROS; ++i)
   	{
   		hebra_estanqueros[i] = thread( Estanquero, Estanco, i);
   	}

	for (int i = 0; i < NUM_FUMADORES; ++i)
	{
		hebra_fumadores[i] = thread(fumador,Estanco,i);
	}



	for (int i = 0; i < NUM_ESTANQUEROS; ++i)
   	{
   		hebra_estanqueros[i].join();
   	}

	for (int i = 0; i < NUM_FUMADORES; ++i)
	{
		hebra_fumadores[i].join();
	}
}




   




  

   	
























