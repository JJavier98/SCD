#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include "Semaphore.h"

using namespace std ;
using namespace SEM ;

Semaphore 	ingr_disp[3] = {0,0,0},
          	mostr_vacio = 1;



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

int producir_ingrediente()
{
	chrono::milliseconds duracion_producir( aleatorio<20,200>() );
	this_thread::sleep_for( duracion_producir );
   	return aleatorio<0,2>();
}

//----------------------------------------------------------------------
// función que ejecuta la hebra del estanquero

void funcion_hebra_estanquero(  )
{
	while( true )
	{
	    int ingrediente = producir_ingrediente();
	    mostr_vacio.sem_wait();
	    cout << "Banquero produce ingrediente: " << ingrediente << endl;
	    ingr_disp[ingrediente].sem_signal();
 	}
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

//----------------------------------------------------------------------
// función que ejecuta la hebra del fumador
void  funcion_hebra_fumador( int num_fumador )
{
   while( true )
   {
   	ingr_disp[num_fumador].sem_wait();
   	cout << "Retirado ingrediente: " << num_fumador << endl;
   	mostr_vacio.sem_signal();
   	fumar( num_fumador );

   }
}

//----------------------------------------------------------------------

int main()
{
   cout << "--------------------------" << endl
        << "Problema de los Fumadores." << endl
        << "--------------------------" << endl
        << flush ;

   	thread hebra_productora ( funcion_hebra_estanquero );
   	thread hebras_consumidoras[3];
    for(int i = 0 ; i < 3 ; ++i)
    {
    	hebras_consumidoras[i] = thread (funcion_hebra_fumador, i);
    }
   	
    for(int i = 0 ; i < 3 ; ++i)
    {
    	hebras_consumidoras[i].join();
    }
   	hebra_productora.join() ;

	cout << "FIN" << endl;
}
