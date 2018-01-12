#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;


Semaphore mostr_vacio(1);
Semaphore ingr_disp[2][3] = {0,0,0,0,0,0};


/*
	Suponiendo que el programa es:
	2 estanqueros, cada uno con sus 3 fumadores diferentes
	Los estanqueros esperan a que el fumador acabe de fumar 
	para poner otro ingrediente, no a que lo coja.
*/
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

int producir( )
{
	// Produce ingrediente aleatorio
	int producido = rand() % 3;
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
//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero(int num_estanquero  )
{
while( true )
   {
		int ingrediente = producir();
		mostr_vacio.sem_wait();
		cout << "El estanquero " << num_estanquero << " pone en mostrador ingrediente número " << ingrediente << endl;
		ingr_disp[num_estanquero][ingrediente].sem_signal();
   }
}

//-------------------------------------------------------------------------
// Función que simula la acción de fumar, como un retardo aleatoria de la hebra

void fumar( int num_fumador, int num_estanquero )
{

   // calcular milisegundos aleatorios de duración de la acción de fumar)
   chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

   // informa de que comienza a fumar

    cout << "Fumador " << num_fumador << "del estanquero " << num_estanquero << "  :"
          << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

   // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
   this_thread::sleep_for( duracion_fumar );

   // informa de que ha terminado de fumar

    cout << "Fumador " << num_fumador << " del estanquero " << num_estanquero << "  : termina de fumar, comienza espera de ingrediente." << endl;

}

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador(int num_fumador, int num_estanquero)
{
   while( true )
   {
		cout << "Esperando ingrediente número " << num_fumador << " del estanquero " << num_estanquero << endl; 
		ingr_disp[num_estanquero][num_fumador].sem_wait();
		cout << "Recogido ingrediente número " << num_fumador << " del estanquero " << num_estanquero << endl; 
		mostr_vacio.sem_signal();
		fumar (num_fumador, num_estanquero);		
   }
}

//----------------------------------------------------------------------

int main()
{
	cout << "--------------------------------------------------------" << endl
        << "Problema de los fumadores." << endl
        << "--------------------------------------------------------" << endl
        << flush ;
	thread hebras_estanqueros[2];
	thread hebras_fumadores[2][3];

	for (int i=0; i<2; i++){
		hebras_estanqueros[i] = thread (funcion_hebra_estanquero,i);
		for (int j=0; j<3; j++)
			hebras_fumadores[i][j] = thread (funcion_hebra_fumador,j,i);
	}
	
	for (int i=0; i<2; i++){
		hebras_estanqueros[i].join();
		for (int j=0; j<3; j++)
			hebras_fumadores[i][j].join();
	}
}