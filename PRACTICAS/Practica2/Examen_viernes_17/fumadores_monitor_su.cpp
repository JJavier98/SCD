#include <iostream>
#include <random>
#include "HoareMonitor.hpp"
//Falta poner las condiciones de las colas.
using namespace std;
using namespace HM;

const int 	FUM_INGR_0 = 20,
			FUM_INGR_1 = 10,
			FUM_INGR_2 = 5,
			FUMADORES = FUM_INGR_0 + FUM_INGR_1 + FUM_INGR_2,
			INGREDIENTES = 3,
			LIMITE = 100,
			ESTANQUEROS = 1;
//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

// Función que simula la acción de producir, como un retardo aleatoria de la hebra

int ProducirIngrediente( )
{
	// Produce ingrediente aleatorio
	int producido = rand() % FUMADORES;
   // calcular milisegundos aleatorios de duración de la acción de producir)
   chrono::milliseconds duracion_producir( aleatorio<20,200>() );

 	// informa de que comienza a producir

    cout << "Se empieza a producir (" << duracion_producir.count() << " milisegundos)" << endl;
   // espera bloqueada un tiempo igual a 'duracion_producir' milisegundos
   this_thread::sleep_for( duracion_producir );

   // informa de que ha terminado de producirlo
   cout << "Producido ingrediente " << producido << endl;
	return producido;

}

//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

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

class Estanco : public HoareMonitor{
private:
	CondVar cola_fumador[FUMADORES];
	CondVar cola_estanqueros;
	int mostrador;
public:
	Estanco ();
	void ObtenerIngrediente( int ingrediente);
	void PonerIngrediente(int ingrediente);
	void EsperaRecogerIngrediente();
	
};

Estanco::Estanco (){;
	for (int i = 0; i < FUMADORES; ++i)
		cola_fumador[i] = newCondVar();
	cola_estanqueros = newCondVar();
	mostrador = -1;
}

void Estanco::ObtenerIngrediente ( int ingrediente){
	if ( mostrador != ingrediente)
		cola_fumador[ingrediente].wait();

	mostrador = -1;
	cout << "El fumador " << ingrediente << " recoge el ingrediente " << ingrediente << endl;
	cola_estanqueros.signal();
}

void Estanco::PonerIngrediente (int ingrediente){
	mostrador = ingrediente;
	cout << "El estanquero pone el ingrediente " << ingrediente << " en el mostrador." << endl;
	cola_fumador[ingrediente].signal();
}

void Estanco::EsperaRecogerIngrediente(){
	if (mostrador!=-1)
	{
		cola_estanqueros.wait();
	}
}

void funcion_hebra_fumador(MRef <Estanco> monitor, int num_fumador){
	while (true){
		monitor->ObtenerIngrediente (num_fumador);
		fumar(num_fumador);
	}
}

void funcion_hebra_estanquero(MRef <Estanco> monitor, int num_estanquero){
	int ingrediente;
	while (true){
		ingrediente = ProducirIngrediente();
		monitor->PonerIngrediente (ingrediente);
		monitor->EsperaRecogerIngrediente();	
	}
}

int main()
{
	cout << "--------------------------------------------------------" << endl
        << "Problema de los fumadores. 3 fumadores y 1 estanquero. Monitor SU." << endl
        << "--------------------------------------------------------" << endl
        << flush ;

	MRef <Estanco> monitor = Create <Estanco> ();
	thread hebras_estanqueros[ESTANQUEROS];
	thread hebras_fumadores[FUMADORES];

	for (int i=0; i<ESTANQUEROS; i++)
		hebras_estanqueros[i] = thread (funcion_hebra_estanquero,monitor,i);
	for (int i=0; i<FUMADORES; i++)
		hebras_fumadores[i] = thread (funcion_hebra_fumador,monitor,i);

	for (int i=0; i<ESTANQUEROS; i++)
		hebras_estanqueros[i].join();
	for (int i=0; i<FUMADORES; i++)
		hebras_fumadores[i].join();

	
}
